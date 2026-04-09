#pragma once

#include <string>

#include <drogon/HttpTypes.h>

namespace core::types {

struct ServiceError {
    drogon::HttpStatusCode status;
    std::string message;
};

}  // namespace core::types
