#pragma once

#include <cstdint>
#include <string>

namespace core::types {

struct LoginResult {
    std::string access_token;
    std::string refresh_token;
    int64_t expires_in_sec;
};

}  // namespace core::types
