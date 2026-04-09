#pragma once
#include <charconv>
#include <cstdint>
#include <string_view>
#include <optional>
#include <string>
#include <drogon/HttpResponse.h>
#include <drogon/HttpTypes.h>
#include <nlohmann/json.hpp>

namespace core::helpers {

  drogon::HttpResponsePtr makeJsonResponse(const nlohmann::json& payload, drogon::HttpStatusCode status = drogon::k200OK);
  std::string_view defaultErrorMessage(drogon::HttpStatusCode status) noexcept;
  drogon::HttpResponsePtr makeErrorResponse(
    drogon::HttpStatusCode status,
    std::string_view message = {});

  template<typename T>
  std::optional<T> fromEnvString(std::string_view s) {
    if constexpr (std::is_same_v<T, std::string>) {
      return std::string(s);
    } else if constexpr (std::is_arithmetic_v<T>) {
      T val{};
      auto [ptr, ec] = std::from_chars(s.data(), s.data() + s.size(), val);
      if (ec == std::errc{}) { return val; }
      return std::nullopt;
    } else {
      static_assert(false, "No fromEnvString specialization for this type");
    }
  }

  template<typename T>
  bool isNonEmpty(const T& val) {
    if constexpr (requires { val.empty(); }) {
      return !val.empty();
    } else {
      return true;
    }
  }

  template<typename T>
  consteval std::string_view typeName() {
    if constexpr (std::is_same_v<T, std::string>) {
      return "string";
    } else if constexpr (std::is_same_v<T, int>) {
      return "int";
    } else if constexpr (std::is_same_v<T, long>) {
      return "long";
    } else if constexpr (std::is_same_v<T, float>) {
      return "float";
    } else if constexpr (std::is_same_v<T, double>) {
      return "double";
    } else if constexpr (std::is_same_v<T, bool>) {
      return "bool";
    } else if constexpr (std::is_same_v<T, uint32_t>) {
      return "uint32";
    } else if constexpr (std::is_same_v<T, uint64_t>) {
      return "uint64";
    } else {
      return "unknown";
    }
  }

} // namespace core::helpers
