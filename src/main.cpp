#include <achievement_manager.hpp>
#include <constants.hpp>
#include <file_configuration.hpp>
#include <theme_manager.hpp>

#include <QApplication>
#include <QTimer>

int main(int argc, char* argv[]) {
  spdlog::set_level(spdlog::level::debug);
  if (!std::filesystem::is_directory(STEAM_PATH)) {
    log_error("Failed to find steam install path...");
    return 1;
  }
  log_info("Found Steam-Directory at '{}'.", STEAM_PATH.generic_string());

  if (!std::filesystem::is_directory(DATA_PATH) && !std::filesystem::create_directories(DATA_PATH)) {
    log_error("Data directory does not exist and could not be created.");
    return 1;
  }
  log_info("Found Data-Directory at '{}'.", DATA_PATH.generic_string());

  QApplication app(argc, argv);

  FileConfiguration* config = FileConfiguration::get_singleton();
  config->load();
  log_info("Configuration loaded.");

  ThemeManager* theme_manager = ThemeManager::get_singleton();
  theme_manager->load();

  AchievementManager* achievement_manager = AchievementManager::get_singleton();

  achievement_manager->enqueue(Achievement("Steam Achievement Tracker", "If you see this, the app is now successfully running!"));

  achievement_manager->init();

  return app.exec();
}