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

    // use METHOD_ADD to add your custom processing function here;
    // METHOD_ADD(AdminAuthController::get, "/{2}/{1}", Get); // path is /core/AdminAuthController/{arg2}/{arg1}
    // METHOD_ADD(AdminAuthController::your_method_name, "/{1}/{2}/list", Get); // path is /core/AdminAuthController/{arg1}/{arg2}/list
    // ADD_METHOD_TO(AdminAuthController::your_method_name, "/absolute/path/{1}/{2}/list", Get); // path is /absolute/path/{arg1}/{arg2}/list

    METHOD_LIST_END
    drogon::Task<HttpResponsePtr> login(HttpRequestPtr req);
    drogon::Task<HttpResponsePtr> refresh(HttpRequestPtr req);

    // your declaration of processing function maybe like this:
    // void get(const HttpRequestPtr& req, std::function<void (const HttpResponsePtr &)> &&callback, int p1, std::string p2);
    // void your_method_name(const HttpRequestPtr& req, std::function<void (const HttpResponsePtr &)> &&callback, double p1, int p2) const;
};
}
