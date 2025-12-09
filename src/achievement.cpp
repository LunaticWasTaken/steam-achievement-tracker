#include <achievement.hpp>

Achievement::Achievement(const uint64_t app_id, const uint8_t block_id, const uint8_t bit_id, const std::string& id, const std::unordered_map<std::string, std::string>& names,
                         const std::unordered_map<std::string, std::string>& descriptions, const std::string& icon_name, const std::string& icon_locked_name, const bool hidden)
  : app_id_(app_id), block_id_(block_id), bit_id_(bit_id), id_(id), names_(names), descriptions_(descriptions), icon_name_(icon_name), icon_locked_name_(icon_locked_name), hidden_(hidden) {}

uint64_t Achievement::app_id() const {
  return app_id_;
}

uint8_t Achievement::block_id() const {
  return block_id_;
}

uint8_t Achievement::bit_id() const {
  return bit_id_;
}

std::string Achievement::id() const {
  return id_;
}

std::unordered_map<std::string, std::string> Achievement::names() const {
  return names_;
}

std::unordered_map<std::string, std::string> Achievement::descriptions() const {
  return descriptions_;
}

std::string Achievement::icon_name() const {
  return icon_name_;
}

std::string Achievement::icon_locked_name() const {
  return icon_locked_name_;
}

bool Achievement::hidden() const {
  return hidden_;
}

nlohmann::json Achievement::dump() const {
  nlohmann::json obj = nlohmann::json::object();
  obj["app_id"] = this->app_id_;
  obj["block_id"] = this->block_id_;
  obj["bit_id"] = this->bit_id_;
  obj["id"] = this->id_;
  obj["names"] = this->names_;
  obj["descriptions"] = this->descriptions_;
  obj["icon_name"] = this->icon_name_;
  obj["icon_locked_name"] = this->icon_locked_name_;
  obj["hidden"] = this->hidden_;
  return obj;
}