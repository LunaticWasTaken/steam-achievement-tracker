#pragma once

#include <achievement.hpp>
#include <theme_manager.hpp>

#include <QWidget>

class NotificationWidget final : public lntc_cpp::core::Singleton<NotificationWidget> {
  friend class Singleton;

public:
  void open(const Achievement& achievement);

  void close();

  [[nodiscard]] QWidget* overlay() const {
    return overlay_;
  }

protected:
  NotificationWidget();

private:
  QWidget* overlay_ = nullptr;
  QRect banner_geometry_;
  const struct {
  private:
    static std::string expand_vars(const std::string& str) {
      std::string result = str;
      const std::string theme_dir = lntc_cpp::str::replace_all_cpy(ThemeManager::get_singleton()->active_theme_dir().generic_string(), "\\", "/");
      lntc_cpp::str::replace_all(result, "{THEME_DIR}", theme_dir);
      return result;
    }

  public:
    const std::string root_anchor = lntc_cpp::str::to_lower_cpy(ThemeManager::get_singleton()->get<std::string>("root/anchor", "bottomright"));
    const int32_t root_offset_x = ThemeManager::get_singleton()->get<int32_t>("root/offset_x", -8);
    const int32_t root_offset_y = ThemeManager::get_singleton()->get<int32_t>("root/offset_y", -8);
    const uint32_t root_width = ThemeManager::get_singleton()->get<int32_t>("root/width", 300);
    const uint32_t root_height = ThemeManager::get_singleton()->get<int32_t>("root/height", 100);
    const std::string root_background = expand_vars(ThemeManager::get_singleton()->get<std::string>("root/background", "rgba(25,25,25,1.0)"));
    const uint8_t root_rounding = ThemeManager::get_singleton()->get<uint8_t>("root/rounding", 12);

    const bool icon_enabled = ThemeManager::get_singleton()->get<bool>("icon/enabled", true);
    const std::string icon_anchor = lntc_cpp::str::to_lower_cpy(ThemeManager::get_singleton()->get<std::string>("icon/anchor", "topleft"));
    const int32_t icon_offset_x = ThemeManager::get_singleton()->get<int32_t>("icon/offset_x", 10);
    const int32_t icon_offset_y = ThemeManager::get_singleton()->get<int32_t>("icon/offset_y", 10);
    const uint32_t icon_width = ThemeManager::get_singleton()->get<int32_t>("icon/width", 80);
    const uint32_t icon_height = ThemeManager::get_singleton()->get<int32_t>("icon/height", 80);
    const uint8_t icon_rounding = ThemeManager::get_singleton()->get<uint8_t>("icon/rounding", 12);

    const bool title_enabled = ThemeManager::get_singleton()->get<bool>("title/enabled", true);
    const std::string title_anchor = lntc_cpp::str::to_lower_cpy(ThemeManager::get_singleton()->get<std::string>("title/anchor", "topleft"));
    const int32_t title_offset_x = ThemeManager::get_singleton()->get<int32_t>("title/offset_x", 100);
    const int32_t title_offset_y = ThemeManager::get_singleton()->get<int32_t>("title/offset_y", 20);
    const std::string title_font = ThemeManager::get_singleton()->get<std::string>("title/font", "Arial");
    const bool title_font_bold = ThemeManager::get_singleton()->get<bool>("title/font_bold", false);
    const bool title_font_italic = ThemeManager::get_singleton()->get<bool>("title/font_italic", false);
    const uint32_t title_font_size = ThemeManager::get_singleton()->get<int32_t>("title/font_size", 12);
    const std::string title_font_color = ThemeManager::get_singleton()->get<std::string>("title/font_color", "white");
    const bool title_font_wrapping = ThemeManager::get_singleton()->get<bool>("title/font_wrapping", true);

    const bool description_enabled = ThemeManager::get_singleton()->get<bool>("description/enabled", true);
    const std::string description_anchor = lntc_cpp::str::to_lower_cpy(ThemeManager::get_singleton()->get<std::string>("description/anchor", "topleft"));
    const int32_t description_offset_x = ThemeManager::get_singleton()->get<int32_t>("description/offset_x", 100);
    const int32_t description_offset_y = ThemeManager::get_singleton()->get<int32_t>("description/offset_y", 60);
    const std::string description_font = ThemeManager::get_singleton()->get<std::string>("description/font", "Arial");
    const bool description_font_bold = ThemeManager::get_singleton()->get<bool>("description/font_bold", false);
    const bool description_font_italic = ThemeManager::get_singleton()->get<bool>("description/font_italic", false);
    const uint32_t description_font_size = ThemeManager::get_singleton()->get<int32_t>("description/font_size", 10);
    const std::string description_font_color = ThemeManager::get_singleton()->get<std::string>("description/font_color", "white");
    const bool description_font_wrapping = ThemeManager::get_singleton()->get<bool>("description/font_wrapping", true);

    const std::string misc_raw_qss = expand_vars(ThemeManager::get_singleton()->get<std::string>("misc/raw_qss", ""));
  } theme_;

  static QFont load_font(const std::string& font, int size);

  void apply_stylesheet();

  void apply_geometries();
};