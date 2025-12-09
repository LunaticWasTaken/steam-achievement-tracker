#pragma once

#include <nlohmann/json.hpp>

class Achievement final {
public:
  Achievement(const std::string& name, const std::string& description, const std::string& icon_name = "")
    : app_id_(0), block_id_(0), bit_id_(0), id_(""), names_{{"english", name}}, descriptions_{{"english", description}}, icon_name_(icon_name), icon_locked_name_(""), hidden_(false) {}

  Achievement(
    uint64_t app_id,
    uint8_t block_id,
    uint8_t bit_id,
    const std::string& id,
    const std::unordered_map<std::string, std::string>& names,
    const std::unordered_map<std::string, std::string>& descriptions,
    const std::string& icon_name,
    const std::string& icon_locked_name,
    bool hidden
  );

  _NODISCARD uint64_t app_id() const;

  _NODISCARD uint8_t block_id() const;

  _NODISCARD uint8_t bit_id() const;

  _NODISCARD std::string id() const;

  _NODISCARD std::unordered_map<std::string, std::string> names() const;

  _NODISCARD std::unordered_map<std::string, std::string> descriptions() const;

  _NODISCARD std::string icon_name() const;

  _NODISCARD std::string icon_locked_name() const;

  _NODISCARD bool hidden() const;

  nlohmann::json dump() const;

private:
  uint64_t app_id_;
  uint8_t block_id_;
  uint8_t bit_id_;
  std::string id_;
  std::unordered_map<std::string, std::string> names_;
  std::unordered_map<std::string, std::string> descriptions_;
  std::string icon_name_;
  std::string icon_locked_name_;
  bool hidden_;
};