#pragma once

#include <yaml-cpp/yaml.h>
#include <file_configuration.hpp>

class ThemeManager : public lntc_cpp::core::Singleton<ThemeManager> {
public:
  void load() {
    const std::filesystem::path theme_path = std::filesystem::path(DATA_PATH / "themes");
    if (!std::filesystem::is_directory(theme_path)) {
      std::filesystem::create_directory(theme_path);
      log_debug("Created theme directory.");
    }
    for (const auto& entry : std::filesystem::directory_iterator(theme_path)) {
      if (!std::filesystem::is_directory(entry)) continue;
      std::string theme_name = entry.path().filename().generic_string();
      if (this->themes_.contains(theme_name)) continue;
      const std::filesystem::path theme_file = entry.path() / "theme.yml";
      if (!std::filesystem::is_regular_file(theme_file)) {
        log_warn("Could not load theme '{}': Missing 'theme.yml' file.", theme_name);
        continue;
      }
      try {
        if (const YAML::Node node = YAML::LoadFile(theme_file.string()); node.IsMap() && node["theme"].IsDefined()) {
          this->themes_[theme_name] = std::make_pair(entry.path(), node["theme"]);
          log_info("Loaded theme '{}'", theme_name);
          continue;
        }
        log_warn("Could not load theme '{}': Missing 'theme' node in theme file.", theme_name);
      } catch (const YAML::Exception& e) {
        log_error("Failed to load theme '{}': {}", theme_file.generic_string(), e.what());
      }
    }
    if (std::string active_theme = FileConfiguration::get_singleton()->get<std::string>("active_theme"); !active_theme.empty() && this->themes_.contains(active_theme)) {
      this->active_theme_ = active_theme;
      log_info("Using theme '{}'.", active_theme);
      return;
    }
    log_info("Using default theme.");
    this->active_theme_ = "";
  }

  _NODISCARD std::filesystem::path active_theme_dir() const {
    if (!this->active_theme_.empty()) {
      return this->themes_.at(active_theme_).first;
    }
    return std::filesystem::path();
  }

  _NODISCARD YAML::Node active_theme_node() const {
    if (!this->active_theme_.empty()) {
      return this->themes_.at(active_theme_).second;
    }
    return YAML::Node(YAML::NodeType::Map);
  }

  template <class T>
  _NODISCARD T get(const std::string& path, const T& fallback = {}) const {
    const auto keys = lntc_cpp::str::split(path, '/');
    if (keys.empty()) return fallback;
    YAML::Node cur = YAML::Clone(this->active_theme_node());
    if (keys.size() == 1) return cur[keys[0]].as<T>(fallback);
    for (const auto& key : keys) {
      if (!cur || !cur.IsDefined() || !cur.IsMap() || !cur[key].IsDefined()) return fallback;
      cur = cur[key];
    }
    if (cur && cur.IsDefined()) {
      try {
        return cur.as<T>(fallback);
      } catch (const std::exception& e) {
        log_error(e.what());
      }
    }
    return fallback;
  }

private:
  std::unordered_map<std::string, std::pair<std::filesystem::path, YAML::Node>> themes_;
  std::string active_theme_;
};
