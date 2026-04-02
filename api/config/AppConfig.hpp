#pragma once
#include <cstdint>
#include <string>

namespace core::config {

  struct AppConfig {
      std::string jwt_secret;
      int64_t jwt_expires_in_sec;
      int refresh_token_expires_in_days;
      uint32_t login_requests_per_window;
      size_t login_window_duration_sec;

      AppConfig(const AppConfig&) = delete;
      AppConfig& operator=(const AppConfig&) = delete;

      static const AppConfig& get();

      // must be called once in main()
      static void init();

    private:
      AppConfig() = default;
      static AppConfig instance;
  };

} // namespace core::config
