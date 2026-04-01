#include <drogon/HttpAppFramework.h>
#include <drogon/HttpTypes.h>
#include <drogon/drogon.h>
#include <trantor/utils/Logger.h>

#include "config/AppConfig.hpp"

int main() {
  try {
    #ifdef DROGON_SPDLOG_SUPPORT
      trantor::Logger::enableSpdLog();
    #endif

    drogon::app().loadConfigFile("api/config/config.json");
    core::config::AppConfig::init();

    LOG_INFO << "Server started";

    drogon::app().run();

  } catch (const std::exception& e) {
    LOG_FATAL << "Startup failed: " << e.what();
    return 1;
  }

  return 0;

}
