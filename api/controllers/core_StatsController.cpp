#include "core_StatsController.hpp"

using namespace core;

drogon::Task<HttpResponsePtr> core::StatsController::getDashboardStats(HttpRequestPtr req){
  auto resp = HttpResponse::newHttpResponse();
  resp->setBody("stub");
  co_return resp;
}
