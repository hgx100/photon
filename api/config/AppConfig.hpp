#pragma once
#include <string>

namespace core::config {

struct AppConfig {
    std::string jwt_secret;
    int64_t jwt_expires_in_sec;

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
