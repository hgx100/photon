#pragma once

#include <drogon/HttpController.h>

using namespace drogon;

namespace core
{
class UserController : public drogon::HttpController<UserController>
{
  public:
    METHOD_LIST_BEGIN
      ADD_METHOD_TO(core::UserController::getUserList,"/api/v1/users" ,Get);
      ADD_METHOD_TO(core::UserController::getUser, "/api/v1/users/{1}", Get);
      ADD_METHOD_TO(core::UserController::createUser, "/api/v1/users", Post);
      ADD_METHOD_TO(core::UserController::updateUser, "/api/v1/users/{1}", Put);
      ADD_METHOD_TO(core::UserController::resetSubscription, "/api/v1/users/{1}/reset-sub", Patch);
      ADD_METHOD_TO(core::UserController::deleteUser, "/api/v1/users/{1}", Delete);
    METHOD_LIST_END

    drogon::Task<HttpResponsePtr> getUserList(HttpRequestPtr req);
    drogon::Task<HttpResponsePtr> getUser(HttpRequestPtr req, std::string id);
    drogon::Task<HttpResponsePtr> createUser(HttpRequestPtr req);
    drogon::Task<HttpResponsePtr> updateUser(HttpRequestPtr req, std::string id);
    drogon::Task<HttpResponsePtr> resetSubscription(HttpRequestPtr req, std::string id);
    drogon::Task<HttpResponsePtr> deleteUser(HttpRequestPtr req, std::string id);
};
}
