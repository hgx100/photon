#pragma once

#include <drogon/HttpController.h>
#include <drogon/HttpTypes.h>

using namespace drogon;

namespace core
{
class AdminAuthController : public drogon::HttpController<AdminAuthController>
{
  public:

    METHOD_LIST_BEGIN
      ADD_METHOD_TO(core::AdminAuthController::login, "/api/v1/auth/login", Post);
      ADD_METHOD_TO(core::AdminAuthController::refresh, "/api/v1/auth/refresh", Post);
    METHOD_LIST_END

    drogon::Task<HttpResponsePtr> login(HttpRequestPtr req);
    drogon::Task<HttpResponsePtr> refresh(HttpRequestPtr req);
};
}
