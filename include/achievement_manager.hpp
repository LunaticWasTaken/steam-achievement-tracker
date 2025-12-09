#pragma once

#include <lntc_cpp.hpp>
#include <achievement.hpp>
#include <regex>
#include <queue>

class AchievementManager : public lntc_cpp::core::Singleton<AchievementManager> {
  friend class Singleton;

public:
  void init();

  void tick(bool initial = false);

  _NODISCARD std::unordered_map<uint64_t, std::pair<uint64_t, std::vector<Achievement>>> global_achievements() const;

  _NODISCARD std::pair<uint64_t, std::vector<Achievement>> global_achievements(const uint64_t& app_id) const;

  _NODISCARD std::unordered_map<uint64_t, std::vector<std::string>> unlocked_achievements() const;

  _NODISCARD std::vector<std::string> unlocked_achievements(const uint64_t& app_id) const;

  void enqueue(const Achievement& achievement) const;

  void process_queue() const;

private:
  mutable std::mutex mutex_;
  const std::regex schema_re = std::regex(R"(^(UserGameStatsSchema_(\d+)\.bin)$)");
  const std::regex stats_re = std::regex(R"(^(UserGameStats_\d+_(\d+)\.bin)$)");
  std::unordered_map<uint64_t, std::pair<uint64_t, std::vector<Achievement>>> global_achievements_;
  std::unordered_map<uint64_t, std::vector<std::string>> unlocked_achievements_;
  mutable std::queue<Achievement> notification_queue_;
  mutable std::mutex queue_mutex_;
};
