#pragma once

#include <drogon/HttpController.h>

using namespace drogon;

namespace core::controllers {
  class UserController : public drogon::HttpController<UserController>
  {
    public:
      METHOD_LIST_BEGIN
        ADD_METHOD_TO(core::controllers::UserController::getUserList,"/api/v1/users" ,Get);
        ADD_METHOD_TO(core::controllers::UserController::getUser, "/api/v1/users/{1}", Get);
        ADD_METHOD_TO(core::controllers::UserController::createUser, "/api/v1/users", Post);
        ADD_METHOD_TO(core::controllers::UserController::updateUser, "/api/v1/users/{1}", Put);
        ADD_METHOD_TO(core::controllers::UserController::resetSubscription, "/api/v1/users/{1}/reset-sub", Patch);
        ADD_METHOD_TO(core::controllers::UserController::deleteUser, "/api/v1/users/{1}", Delete);
      METHOD_LIST_END

      drogon::Task<HttpResponsePtr> getUserList(HttpRequestPtr req);
      drogon::Task<HttpResponsePtr> getUser(HttpRequestPtr req, std::string id);
      drogon::Task<HttpResponsePtr> createUser(HttpRequestPtr req);
      drogon::Task<HttpResponsePtr> updateUser(HttpRequestPtr req, std::string id);
      drogon::Task<HttpResponsePtr> resetSubscription(HttpRequestPtr req, std::string id);
      drogon::Task<HttpResponsePtr> deleteUser(HttpRequestPtr req, std::string id);
  };
} // namespace core::controllers
