#include <util.hpp>

#ifdef _WIN32
#include <windows.h>
#else
#include <unistd.h>
#include <limits.h>
#endif

#include <lntc_cpp.hpp>
#include <cstdlib>

std::string get_environment_variable(const std::string& var, const std::string& fallback) {
#ifdef _WIN32
  const int wlen = MultiByteToWideChar(CP_UTF8, 0, var.c_str(), -1, nullptr, 0);
  if (wlen <= 0) return fallback;
  std::wstring wvar(wlen, L'\0');
  MultiByteToWideChar(CP_UTF8, 0, var.c_str(), -1, &wvar[0], wlen);
  wvar.resize(wlen - 1);
  const DWORD size_with_null = GetEnvironmentVariableW(wvar.c_str(), nullptr, 0);
  if (size_with_null == 0) return fallback;
  std::wstring wval(size_with_null, L'\0');
  const DWORD written = GetEnvironmentVariableW(wvar.c_str(), &wval[0], size_with_null);
  if (written == 0) return fallback;
  wval.resize(written);
  const int utf8len = WideCharToMultiByte(CP_UTF8, 0, wval.c_str(), static_cast<int>(wval.size()), nullptr, 0, nullptr, nullptr);
  if (utf8len <= 0) return fallback;
  std::string val(utf8len, '\0');
  WideCharToMultiByte(CP_UTF8, 0, wval.c_str(), static_cast<int>(wval.size()), &val[0], utf8len, nullptr, nullptr);
  return val;
#else
  CSTRING p = std::getenv(var.c_str());
  return p ? std::string(p) : fallback;
#endif
}

/**
 * Returns a REG_SZ (String) value from the Windows Registry.
 *
 * NOTE: On Non-Windows Systems, this will always return the fallback value, since the registry is windows-only.
 * @param root the root to search in. For example HKLM or HKEY_LOCAL_MACHINE. Case-Insensitive.
 * @param path the path to the key. For example "Software/Valve/Steam/SteamPath" - Forward-Slashes are automatically normalized to backslashes.
 * @param fallback the value to return if the key was not found. Defaults to an empty string.
 * @return the REG_SZ value found, or fallback if none was found.
 */
std::string get_reg_sz(const std::string& root, const std::string& path, const std::string& fallback) {
#ifdef _WIN32
  HKEY k0 = nullptr;
  if (const std::string s = lntc_cpp::str::to_lower_cpy(root); s == "hkcr" || s == "hkey_classes_root")
    k0 = HKEY_CLASSES_ROOT;
  else if (s == "hkcu" || s == "hkey_current_user")
    k0 = HKEY_CURRENT_USER;
  else if (s == "hklm" || s == "hkey_local_machine")
    k0 = HKEY_LOCAL_MACHINE;
  else if (s == "hku" || s == "hkey_users")
    k0 = HKEY_USERS;
  else if (s == "hkey_current_config")
    k0 = HKEY_CURRENT_CONFIG;
  if (!k0) return fallback;

  const auto path_normalized = lntc_cpp::str::replace_all_cpy(path, "/", "\\");

  const size_t i = path_normalized.find_last_of('\\');
  if (i == std::string::npos) return fallback;

  const std::string k1 = path_normalized.substr(0, i);
  const std::string k2 = path_normalized.substr(i + 1);

  HKEY k;
  if (RegOpenKeyExA(k0, k1.c_str(), 0, KEY_READ, &k) != ERROR_SUCCESS) return fallback;

  DWORD type = 0;
  DWORD buf_size = 0;
  LONG result = RegQueryValueExA(k, k2.c_str(), nullptr, &type, nullptr, &buf_size);
  if (result != ERROR_SUCCESS || type != REG_SZ) {
    RegCloseKey(k);
    return fallback;
  }

  std::string buf(buf_size, '\0');
  result = RegQueryValueExA(k, k2.c_str(), nullptr, &type, reinterpret_cast<LPBYTE>(buf.data()), &buf_size);
  RegCloseKey(k);

  if (result != ERROR_SUCCESS || type != REG_SZ) return fallback;

  buf.resize(std::strlen(buf.c_str()));
  return buf;
#else
  return fallback;
#endif
}

void measure_duration(const std::string& caller, const std::function<void()>& func) {
  const auto start = std::chrono::high_resolution_clock::now();
  func();
  const auto end = std::chrono::high_resolution_clock::now();
  const uint64_t duration = std::chrono::duration_cast<std::chrono::microseconds>(end - start).count();
  log_debug("util::measure_duration -> {} | [{}ms | {}\xE6s]", caller, duration / 1000, duration);
}

bool download_file(const std::string& url, const std::filesystem::path& destination) {
  if (url.empty()) return false;
  const std::filesystem::path abs_path = std::filesystem::absolute(destination);
  if (!std::filesystem::is_directory(abs_path.parent_path()) && !std::filesystem::create_directories(abs_path.parent_path())) {
    log_error("Failed to download file: Destination directory does not exist, and could not be created.");
    return false;
  }
  std::string dest_str = abs_path.string();
#if defined(_WIN32)
  for (auto& c : dest_str)
    if (c == '\\') c = '/';
#endif
  const std::string cmd = "curl -L \"" + url + "\" -o \"" + dest_str + "\" --silent --show-error";
  const int result = std::system(cmd.c_str());
  return result == 0;
}