#pragma once

#include <drogon/HttpController.h>

using namespace drogon;

namespace core
{
class StatsController : public drogon::HttpController<StatsController>
{
  public:
    METHOD_LIST_BEGIN
      ADD_METHOD_TO(core::StatsController::getDashboardStats, "/api/v1/dashboard/stats", Get);
    METHOD_LIST_END

    drogon::Task<HttpResponsePtr> getDashboardStats(HttpRequestPtr req);
};
}
