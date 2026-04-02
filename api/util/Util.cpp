#include "Util.hpp"

namespace core::util {

  [[nodiscard]] bool isBlank(std::string_view sv) noexcept {
    return std::ranges::all_of(sv, [](unsigned char c) {
      return std::isspace(c);
    });
  }

  [[nodiscard]] std::optional<std::string>
  validateStringField(const Json::Value& payload, std::string_view field) {
    if (!payload.isMember(field.data(), field.data() + field.size())){
      return std::nullopt;
    }

    const std::string value = payload[std::string(field)].asString();
    if (value.empty() || isBlank(value)){
      return std::nullopt;
    }
    return value;
  }

} // namespace core::util
