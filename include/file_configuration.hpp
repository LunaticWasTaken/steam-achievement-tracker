#pragma once

#include <lntc_cpp.hpp>
#include <yaml-cpp/yaml.h>
#include <constants.hpp>

class FileConfiguration : public lntc_cpp::core::Singleton<FileConfiguration> {
public:
  void set(const std::string& path, const YAML::Node& value) {
    std::lock_guard lock(mutex_);
    const std::vector<std::string> keys = lntc_cpp::str::split(path, '/');
    if (keys.empty()) return;
    YAML::Node final_node;
    final_node[keys.back()] = value;
    for (int i = static_cast<int>(keys.size()) - 2; i >= 0; --i) {
      YAML::Node parent;
      parent[keys[i]] = final_node;
      final_node = parent;
    }
    if (!this->data_ || !this->data_.IsMap()) this->data_ = YAML::Node(YAML::NodeType::Map);
    for (auto it = final_node.begin(); it != final_node.end(); ++it) {
      const std::string key = it->first.as<std::string>();
      if (const YAML::Node& val = it->second; this->data_[key] && this->data_[key].IsMap() && val.IsMap()) {
        for (auto child = val.begin(); child != val.end(); ++child) this->data_[key][child->first.as<std::string>()] = child->second;
      } else {
        this->data_[key] = val;
      }
    }
  }

  template <class T>
  _NODISCARD T get(const std::string& path, const T& fallback = {}) const {
    std::lock_guard lock(mutex_);
    const auto keys = lntc_cpp::str::split(path, '/');
    if (keys.empty()) return fallback;
    YAML::Node cur = YAML::Clone(this->data_);
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

  _NODISCARD bool has(const std::string& path) const {
    std::lock_guard lock(mutex_);
    const std::vector<std::string> keys = lntc_cpp::str::split(path, '/');
    if (keys.empty()) return false;
    YAML::Node cur = data_;
    for (const auto& key : keys) {
      if (!cur || !cur.IsDefined() || !cur.IsMap() || !cur[key]) return false;
      cur = cur[key];
    }
    return cur && cur.IsDefined();
  }

  void load() {
    std::lock_guard lock(mutex_);
    const std::filesystem::path path = DATA_PATH / "config.yml";
    if (!std::filesystem::exists(path)) return;
    try {
      data_ = YAML::LoadFile(path.generic_string());
    } catch (...) {}
  }

  void save() const {
    std::lock_guard lock(mutex_);
    const std::filesystem::path path = DATA_PATH / "config.yml";
    if (std::ofstream out(path); out.is_open()) out << YAML::Dump(data_);
  }

private:
  YAML::Node data_;
  mutable std::mutex mutex_;
};