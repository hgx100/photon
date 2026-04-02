#include "AppConfig.hpp"
#include "../util/Util.hpp"
#include <cstddef>
#include <cstdint>
#include <stdexcept>
#include <iostream>

namespace core::config {

  AppConfig AppConfig::instance{};

  const AppConfig& AppConfig::get() {
      return instance;
  }

  void AppConfig::init() {
      auto jwt_secret_result = core::util::getConfigVar<std::string>("jwt_secret", "PHOTON_JWT_SECRET");
      if (!jwt_secret_result.has_value()) {
          throw std::runtime_error(jwt_secret_result.error());
      }
      instance.jwt_secret = jwt_secret_result.value();

      auto jwt_expiry_result = core::util::getConfigVar<int64_t>("jwt_expires_in", "PHOTON_JWT_EXPIRES_IN");
      if (jwt_expiry_result.has_value()) {
          instance.jwt_expires_in_sec = jwt_expiry_result.value();
      } else {
          instance.jwt_expires_in_sec = 900;
          LOG_INFO << "Using default JWT expiry: 900s";
      }

      auto refresh_token_expiry_result = core::util::getConfigVar<int>("refresh_token_expires_in_days", "PHOTON_REFRESH_TOKEN_EXPIRES_IN_DAYS");
      if (refresh_token_expiry_result.has_value()) {
          instance.refresh_token_expires_in_days = refresh_token_expiry_result.value();
      } else {
          instance.refresh_token_expires_in_days = 7;
          LOG_INFO << "Using default refresh token expiry: 7 days";
      }

      auto login_requests_per_window_result = core::util::getConfigVar<uint32_t>("login_requests_per_window", "PHOTON_LOGIN_REQUESTS_PER_WINDOW");
      if (login_requests_per_window_result.has_value()) {
          instance.login_requests_per_window = login_requests_per_window_result.value();
      } else {
          instance.login_requests_per_window = 5;
          LOG_INFO << "Using default login requests per window: 5 requests";
      }

      auto login_window_duration_result = core::util::getConfigVar<size_t>("login_window_duration_sec", "PHOTON_LOGIN_WINDOW_DURATION_SEC");
      if (login_window_duration_result.has_value()) {
          instance.login_window_duration_sec = login_window_duration_result.value();
      } else {
          instance.login_window_duration_sec = 60;
          LOG_INFO << "Using default login window duration: 60s";
      }
  }

} // namespace core::config
