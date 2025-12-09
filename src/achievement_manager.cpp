#include <notification_widget.hpp>

#include <achievement_manager.hpp>
#include <constants.hpp>
#include <vdf.hpp>
#include <nlohmann/json.hpp>
#include <QFontDatabase>
#include <QTimer>
#include <QRegularExpressionMatchIterator>
#include <QThread>

void AchievementManager::init() {
  this->tick(true); // Run an initial tick, so that we can cache already unlocked achievements without sending dozens of notifications.

  // TODO: See if 250ms is fine, or too low.
  lntc_cpp::core::run_async([this] {
    // ReSharper disable once CppDFAEndlessLoop - False-Warning.
    while (true) {
      this->tick();
      std::this_thread::sleep_for(std::chrono::milliseconds(250));
    }
  });
  this->process_queue();
}

void AchievementManager::tick(const bool initial) {
  std::lock_guard lock(this->mutex_);
  for (const std::filesystem::path stats_path(STEAM_PATH / "appcache" / "stats"); const auto& entry : std::filesystem::directory_iterator(stats_path)) {
    const std::string filename = entry.path().filename().generic_string();
    const uint64_t timestamp_ms = std::chrono::duration_cast<std::chrono::milliseconds>(std::filesystem::last_write_time(entry).time_since_epoch()).count();
    if (std::smatch match; std::regex_match(filename, match, schema_re)) {
      // Schema
      const std::string app_id_str = match[2].str();
      const uint64_t app_id = std::stoull(app_id_str);
      if (this->global_achievements_[app_id].first == timestamp_ms) continue;
      const nlohmann::json schema = vdf::read_vdf(entry, true);
      if (!schema.contains(app_id_str)) continue;
      const nlohmann::json app_obj = schema[app_id_str];
      if (!app_obj.contains("stats")) continue;
      std::vector<Achievement> achievements;
      for (const nlohmann::json stats_obj = app_obj["stats"]; const auto& [block_id, block_obj] : stats_obj.items()) {
        if (!block_obj.contains("bits")) continue;
        for (const nlohmann::json bits_obj = block_obj["bits"]; const auto& [bit_id, bit_obj] : bits_obj.items()) {
          if (!bit_obj.contains("name")) continue; // All we need is the block-id, bit-id and the id. Therefore, for anything else we can also accept "invalid" values.
          const std::string id = bit_obj["name"];
          std::unordered_map<std::string, std::string> names;
          std::unordered_map<std::string, std::string> descriptions;
          std::string icon_name = "";
          std::string icon_name_locked = "";
          bool hidden = false;
          if (bit_obj.contains("display")) {
            const auto& display_obj = bit_obj["display"];
            if (display_obj.contains("name")) names = display_obj["name"].get<std::unordered_map<std::string, std::string>>();
            if (display_obj.contains("desc")) descriptions = display_obj["desc"].get<std::unordered_map<std::string, std::string>>();
            if (display_obj.contains("icon")) icon_name = display_obj["icon"].get<std::string>();
            if (display_obj.contains("icon_gray")) icon_name_locked = display_obj["icon_gray"].get<std::string>();
            if (display_obj.contains("hidden")) hidden = std::stoi(display_obj["hidden"].get<std::string>());
          }

          const std::filesystem::path icon_cache(DATA_PATH / "cache" / "icons");

          if (!icon_name.empty() && !std::filesystem::is_regular_file(icon_cache / app_id_str / icon_name)) {
            if (const std::string url = std::format("https://media.steampowered.com/steamcommunity/public/images/apps/{}/{}", app_id, icon_name);
                !download_file(url, icon_cache / app_id_str / icon_name)) {
              log_error("Failed to download icon: '{}'", url);
            } else {
              log_info("Successfully downloaded icon: '{}'", url);
            }
          }
          if (!icon_name_locked.empty() && !std::filesystem::is_regular_file(icon_cache / app_id_str / icon_name_locked)) {
            if (const std::string url = std::format("https://media.steampowered.com/steamcommunity/public/images/apps/{}/{}", app_id, icon_name_locked);
                !download_file(url, icon_cache / app_id_str / icon_name_locked)) {
              log_error("Failed to download icon: '{}'", url);
            } else {
              log_info("Successfully downloaded icon: '{}'", url);
            }
          }

          achievements.emplace_back(app_id, std::stoi(block_id), std::stoi(bit_id), id, names, descriptions, icon_name, icon_name_locked, hidden);
        }
      }
      this->global_achievements_[app_id] = std::make_pair(timestamp_ms, achievements);
      log_debug("Registered {} Achievements for app {}.", achievements.size(), app_id);
    } else if (std::regex_match(filename, match, stats_re)) {
      // Stats
      const std::string app_id_str = match[2].str();
      const uint64_t app_id = std::stoull(app_id_str);
      const nlohmann::json stats = vdf::read_vdf(entry, true);
      if (!stats.contains("cache")) continue;

      std::vector<std::string> achievement_ids;
      for (const nlohmann::json cache_obj = stats["cache"]; const auto& [block_id, block_obj] : cache_obj.items()) {
        if (!block_obj.contains("AchievementTimes")) continue;
        for (const nlohmann::json ach_obj = block_obj["AchievementTimes"]; const auto& [bit_id, unlock_time] : ach_obj.items()) {
          // TODO: Research if block/bit id's can be more than positive-255. I think steam's achievement limit is way too low to exceed this.
          const uint8_t block_id_int = std::stoi(block_id);
          const uint8_t bit_id_int = std::stoi(bit_id);

          auto& global_achievements = this->global_achievements_[app_id].second;
          if (const auto& achievement =
                lntc_cpp::collections::find_first(global_achievements, [block_id_int, bit_id_int](const Achievement& ach) { return ach.block_id() == block_id_int && ach.bit_id() == bit_id_int; })) {
            if (const std::string ach_id = achievement->id();
                !lntc_cpp::collections::find_first(this->unlocked_achievements_[app_id], [&ach_id](const std::string& id) { return id == ach_id; }).has_value()) {
              achievement_ids.push_back(ach_id);
            }
          }
        }
      }
      auto& existing = this->unlocked_achievements_[app_id];
      std::unordered_set<std::string> existing_set;
      existing_set.reserve(existing.size());
      existing_set.insert(existing.begin(), existing.end());

      std::vector<std::string> newly_unlocked;
      newly_unlocked.reserve(achievement_ids.size());
      for (const auto& id : achievement_ids) {
        if (existing_set.insert(id).second) {
          existing.push_back(id);
          newly_unlocked.push_back(id);
        }
      }

      if (!initial) {
        for (const auto& ach_id : newly_unlocked) {
          if (const auto ach = lntc_cpp::collections::find_first(global_achievements_[app_id].second, [&ach_id](const Achievement& a) { return a.id() == ach_id; })) {
            this->enqueue(*ach);
          }
        }
      }
    }
  }
}

std::unordered_map<uint64_t, std::pair<uint64_t, std::vector<Achievement>>> AchievementManager::global_achievements() const {
  std::lock_guard lock(this->mutex_);
  return global_achievements_;
}

std::pair<uint64_t, std::vector<Achievement>> AchievementManager::global_achievements(const uint64_t& app_id) const {
  std::lock_guard lock(this->mutex_);
  return global_achievements_.at(app_id);
}

std::unordered_map<uint64_t, std::vector<std::string>> AchievementManager::unlocked_achievements() const {
  std::lock_guard lock(this->mutex_);
  return unlocked_achievements_;
}

std::vector<std::string> AchievementManager::unlocked_achievements(const uint64_t& app_id) const {
  std::lock_guard lock(this->mutex_);
  return unlocked_achievements_.at(app_id);
}
void AchievementManager::enqueue(const Achievement& achievement) const {
  std::lock_guard lock(this->queue_mutex_);
  this->notification_queue_.push(achievement);
}

void AchievementManager::process_queue() const {
  std::optional<Achievement> achievement;
  {
    std::lock_guard lock(this->queue_mutex_);
    if (!this->notification_queue_.empty()) {
      achievement.emplace(this->notification_queue_.front());
      this->notification_queue_.pop();
    }
  }

  if (!achievement) {
    QTimer::singleShot(250, [this] { process_queue(); });
    return;
  }

  NotificationWidget* notification_widget = NotificationWidget::get_singleton();

  notification_widget->open(*achievement);

  QTimer::singleShot(5000, [this, notification_widget] {
    notification_widget->close();
    QTimer::singleShot(1000, [this] { process_queue(); });
  });
}
