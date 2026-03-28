#include <drogon/HttpAppFramework.h>
#include <drogon/HttpTypes.h>
#include <drogon/drogon.h>
#include <trantor/utils/Logger.h>

int main() {
  // drogon::app().loadConfigFile("config/config.json");
  drogon::app().addListener("127.0.0.1", 8080);
  LOG_INFO << "Server started on 127.0.0.1:8080";
  drogon::app().run();

}
