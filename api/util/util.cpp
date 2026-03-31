#include <expected>
#include <string>
#include <drogon/drogon.h>
#include "util.hpp"

namespace core::util {
  std::expected<std::string, std::string> getConfigVar( const std::string& name, const std::string& env_name) {
    const auto& custom_config = drogon::app().getCustomConfig();
    if (custom_config.isMember(name) && custom_config[name].isString()) {
      if (auto val = custom_config[name].asString(); !val.empty()) {
        return val;
      }
    }

    if (!env_name.empty()) {
      if (const char* env_value = std::getenv(env_name.c_str()); env_value && *env_value != '\0'){
        return std::string(env_value);
      }
    }

    return std::unexpected(
        env_name.empty()
          ? std::format("Missing config variable: '{}'. Set custom_config.{} in config.", name, name)
          : std::format("Missing config variable: '{}'. Set custom_config.{} in config or {} env var.", name, name, env_name)
      );
  }
} // namespace core::util
