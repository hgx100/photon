#include <drogon/HttpAppFramework.h>
#include <drogon/HttpTypes.h>
#include <drogon/drogon.h>
#include <trantor/utils/Logger.h>

#include "util/util.hpp"

int main() {
  drogon::app().loadConfigFile("api/config/config.json");

  auto jwt_secret_result = core::util::getConfigVar("jwt_secret", "PHOTON_JWT_SECRET");
  if (!jwt_secret_result.has_value()) {
    LOG_FATAL << jwt_secret_result.error();
    return 1;
  }

  LOG_INFO << "Server started";
  drogon::app().run();
}
