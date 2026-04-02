#pragma once

#include <drogon/HttpController.h>
#include <drogon/HttpTypes.h>
#include "../services/core_AuthService.hpp"

using namespace drogon;

namespace core::controllers {
  class AdminAuthController : public drogon::HttpController<AdminAuthController>
  {
    public:

      METHOD_LIST_BEGIN
        ADD_METHOD_TO(core::controllers::AdminAuthController::login, "/api/v1/auth/login", Post, "core::filters::RequiresJsonBody",  "core::filters::LoginRateLimiter");
        ADD_METHOD_TO(core::controllers::AdminAuthController::refresh, "/api/v1/auth/refresh", Post);
        ADD_METHOD_TO(core::controllers::AdminAuthController::logout, "/api/v1/auth/logout", Post);
      METHOD_LIST_END

      drogon::Task<HttpResponsePtr> login(HttpRequestPtr req);
      drogon::Task<HttpResponsePtr> refresh(HttpRequestPtr req);
      drogon::Task<HttpResponsePtr> logout(HttpRequestPtr req);

    private:
      ::core::services::AuthService *authService_ = drogon::app().getPlugin<::core::services::AuthService>();

  };

} // namespace core
