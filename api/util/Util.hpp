#pragma once

#include <cstdlib>
#include <expected>
#include <format>
#include <optional>
#include <string>
#include <string_view>
#include <drogon/drogon.h>

#include "Helpers.hpp"

namespace core::util {

  [[nodiscard]] bool isBlank(std::string_view sv) noexcept;
  [[nodiscard]] std::optional<std::string> validateStringField(const Json::Value& payload, std::string_view field);

  template<typename T>
  std::expected<T, std::string> getConfigVar(const std::string& name, const std::string& env_name = {}) {
    const auto& custom_config = drogon::app().getCustomConfig();
    if (custom_config.isMember(name) && custom_config[name].is<T>()) {
      if (auto val = custom_config[name].as<T>(); helpers::isNonEmpty(val)) {
        return val;
      }
    }

    if (!env_name.empty()) {
      if (const char* raw = std::getenv(env_name.c_str()); raw && *raw != '\0') {
        if (auto val = helpers::fromEnvString<T>(raw)) {
          return *val;
        }
        return std::unexpected(std::format(
          "Config variable '{}': env var {} could not be parsed as {}.",
          name, env_name, helpers::typeName<T>()));
      }
    }

    return std::unexpected(
      env_name.empty()
        ? std::format("Missing config variable: '{}' (expected {}). Set custom_config.{} in config.",
          name, helpers::typeName<T>(), name)
        : std::format("Missing config variable: '{}' (expected {}). Set custom_config.{} in config or {} env var.",
          name, helpers::typeName<T>(), name, env_name)
    );
  }

} // namespace core::util
