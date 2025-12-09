#pragma once

#include <kvpp/KV1.h>
#include <kvpp/KV1Binary.h>

namespace vdf {
  /**
   * Reads the given vdf-file and parses it into a json object.
   *
   * Supports both text and binary vdf files.
   * @param path Path to the file
   * @param binary If true, reads the file as binary (https://developer.valvesoftware.com/wiki/Binary_VDF), otherwise as raw text.
   * @return the parsed vdf-data as json object.
   */
  inline nlohmann::json read_vdf(const std::filesystem::path& path, bool binary = false) {
    try {
      if (binary) {
        std::ifstream in(path, std::ios::binary | std::ios::ate);
        if (!in) return nlohmann::json();
        const std::streamsize size = in.tellg();
        in.seekg(0, std::ios::beg);
        std::vector<std::byte> buffer(static_cast<size_t>(size));
        if (!in.read(reinterpret_cast<char*>(buffer.data()), size)) return nlohmann::json();
        const kvpp::KV1Binary kv(std::span<const std::byte>(buffer.data(), buffer.size()));
        auto to_json = [](const kvpp::KV1BinaryElement& elem, auto&& self) -> nlohmann::json {
          if (elem.getChildCount() > 0) {
            nlohmann::json children = nlohmann::json::object();
            for (const auto& child : elem.getChildren()) children.emplace(child.getKey(), self(child, self));
            return children;
          }
          if (const auto s = elem.getValue<std::string>()) return *s;
          if (const auto i = elem.getValue<int32_t>()) return *i;
          if (const auto f = elem.getValue<float>()) return *f;
          if (const auto u = elem.getValue<uint64_t>()) return *u;
          if (const auto w = elem.getValue<std::wstring>()) {
            std::string s;
            s.reserve(w->size());
            for (const wchar_t wc : *w) s.push_back(static_cast<char>(wc));
            return s;
          }
          if (const auto c = elem.getValue<sourcepp::math::Vec4ui8>()) {
            return nlohmann::json::array({(*c)[0], (*c)[1], (*c)[2], (*c)[3]});
          }
          if (const auto p = elem.getValue<kvpp::KV1BinaryPointer>()) return *p;
          return nullptr;
        };

        nlohmann::json root = nlohmann::json::object();
        for (const auto& child : kv.getChildren()) {
          root.emplace(child.getKey(), to_json(child, to_json));
        }
        return root;
      }
      std::ifstream in(path);
      if (!in) return nlohmann::json();
      std::string contents((std::istreambuf_iterator(in)), std::istreambuf_iterator<char>());
      const kvpp::KV1 kv(contents, true);
      auto to_json = [](const kvpp::KV1ElementReadable<>& element, auto&& self) -> nlohmann::json {
        if (element.getChildCount() > 0) {
          nlohmann::json children = nlohmann::json::object();
          for (const auto& child : element.getChildren()) {
            children.emplace(std::string(child.getKey()), self(child, self));
          }
          return children;
        }
        try {
          return element.getValue<int32_t>();
        } catch (...) {
        }
        try {
          return element.getValue<int64_t>();
        } catch (...) {
        }
        try {
          return element.getValue<float>();
        } catch (...) {
        }
        try {
          return element.getValue<bool>();
        } catch (...) {
        }
        return std::string(element.getValue());
      };
      nlohmann::json root = nlohmann::json::object();
      for (const auto& child : kv.getChildren()) {
        root.emplace(std::string(child.getKey()), to_json(child, to_json));
      }
      return root;
    } catch (const std::exception& e) {
      std::cerr << "Error: " << e.what() << std::endl;
    }
    return nlohmann::json::object();
  }
}