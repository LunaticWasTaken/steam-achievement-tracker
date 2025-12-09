#pragma once

std::string get_environment_variable(const std::string& var, const std::string& fallback = "");

std::string get_reg_sz(const std::string& root, const std::string& path, const std::string& fallback = "");

void measure_duration(const std::string& caller, const std::function<void()>& func);

bool download_file(const std::string& url, const std::filesystem::path& destination);