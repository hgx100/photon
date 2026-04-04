#pragma once

#include <drogon/HttpController.h>

using namespace drogon;

namespace core::controllers {
  class StatsController : public drogon::HttpController<StatsController>
  {
    public:
      METHOD_LIST_BEGIN
        ADD_METHOD_TO(core::controllers::StatsController::getDashboardStats, "/api/v1/dashboard/stats", Get, "core::filters::JwtAuth");
      METHOD_LIST_END

      drogon::Task<HttpResponsePtr> getDashboardStats(HttpRequestPtr req);
  };
} // namespace core::controllers
