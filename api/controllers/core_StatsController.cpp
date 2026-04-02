#include "core_StatsController.hpp"

drogon::Task<HttpResponsePtr> core::controllers::StatsController::getDashboardStats(HttpRequestPtr req){
  auto resp = HttpResponse::newHttpResponse();
  resp->setBody("stub");
  co_return resp;
}
