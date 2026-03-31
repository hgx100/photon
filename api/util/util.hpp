#pragma once

#include <expected>
#include <string>

namespace core::util {
  std::expected<std::string, std::string> getConfigVar(const std::string& name, const std::string& env_name = {} );
} // namespace core::util
