#include <CLI/CLI.hpp>
#include <drogon/HttpAppFramework.h>
#include <drogon/HttpTypes.h>
#include <drogon/drogon.h>
#include <trantor/utils/Logger.h>

#include "config/AppConfig.hpp"

#include <string>

int main(int argc, char** argv) {
  try {
  #ifdef DROGON_SPDLOG_SUPPORT
    trantor::Logger::enableSpdLog();
  #endif

    std::string configPath = "config.json";
    CLI::App app{"Photon API server"};
    app.set_help_flag("-h,--help", "Show this help");
    app.add_option("-c,--config", configPath, "Path to drogon config.json")
        ->default_val(configPath)
        ->envname("PHOTON_CONFIG_PATH");

    try {
      app.parse(argc, argv);
    } catch (const CLI::ParseError& e) {
      return app.exit(e);
    }

    drogon::app().loadConfigFile(configPath);
    core::config::AppConfig::init();

    LOG_INFO << "Server started";

    drogon::app().run();

  } catch (const std::exception& e) {
    LOG_FATAL << "Startup failed: " << e.what();
    return 1;
  }

  return 0;
}
