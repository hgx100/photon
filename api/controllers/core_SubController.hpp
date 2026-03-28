#pragma once

#include <drogon/HttpController.h>

using namespace drogon;

namespace core
{
class SubController : public drogon::HttpController<SubController>
{
  public:
    METHOD_LIST_BEGIN
      ADD_METHOD_TO(core::SubController::getSubscription, "/sub/{1}", Get);
    METHOD_LIST_END

    drogon::Task<HttpResponsePtr> getSubscription(HttpRequestPtr req, std::string subId);
};
}
