#pragma once

#include <filesystem>
#include <util.hpp>

#ifdef _WIN32
#include <windows.h>
#else
#include <unistd.h>
#include <limits.h>
#endif

#ifdef _WIN32
inline static std::filesystem::path DATA_PATH = std::filesystem::path(get_environment_variable("USERPROFILE")) / ".config" / "steam-achievement-tracker";
#else
inline static std::filesystem::path DATA_PATH = std::filesystem::path(get_environment_variable("HOME")) / ".config" / "steam-achievement-tracker";
#endif

#ifdef _WIN32
inline static std::filesystem::path STEAM_PATH = std::filesystem::path(get_reg_sz("HKCU", "Software/Valve/Steam/SteamPath"));
#else
inline static std::filesystem::path STEAM_PATH = std::filesystem::path(get_environment_variable("HOME")) / ".steam" / "steam";
#endif

inline static std::filesystem::path APP_PATH = [] {
#ifdef _WIN32
  wchar_t path[MAX_PATH];
  if (const HMODULE module = GetModuleHandleW(nullptr)) {
    if (const DWORD length = GetModuleFileNameW(module, path, MAX_PATH); length != 0) {
      const std::wstring full_path(path, length);
      return std::filesystem::path(full_path).parent_path();
    }
  }
#else
  char result[PATH_MAX];
  if (const ssize_t count = readlink("/proc/self/exe", result, PATH_MAX); count != -1) {
    return std::filesystem::path(std::string(result, count)).parent_path();
  }
#endif
  return std::filesystem::path("./");
}();