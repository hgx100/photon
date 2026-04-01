#include "AppConfig.hpp"
#include "../util/Util.hpp"
#include <stdexcept>
#include <iostream>

namespace core::config {

AppConfig AppConfig::instance{};

const AppConfig& AppConfig::get() {
    return instance;
}

void AppConfig::init() {
    auto secret_result = core::util::getConfigVar<std::string>("jwt_secret", "PHOTON_JWT_SECRET");
    if (!secret_result.has_value()) {
        throw std::runtime_error(secret_result.error());
    }
    instance.jwt_secret = secret_result.value();

    auto expiry_result = core::util::getConfigVar<int64_t>("jwt_expires_in", "PHOTON_JWT_EXPIRES_IN");
    if (expiry_result.has_value()) {
        instance.jwt_expires_in_sec = expiry_result.value();
    } else {
        instance.jwt_expires_in_sec = 900;
        LOG_INFO << "Using default JWT expiry: 900s";
    }
}

} // namespace core::config
