#include "core_UserController.hpp"

using namespace core;

drogon::Task<HttpResponsePtr> getUserList(HttpRequestPtr req)
{
  auto resp = drogon::HttpResponse::newHttpResponse();
  resp -> setBody("stub");
  co_return resp;
}

drogon::Task<HttpResponsePtr> getUser(HttpRequestPtr req, std::string id)
{
  auto resp = drogon::HttpResponse::newHttpResponse();
  resp -> setBody("stub");
  co_return resp;
}

drogon::Task<HttpResponsePtr> createUser(HttpRequestPtr req)
{
  auto resp = drogon::HttpResponse::newHttpResponse();
  resp -> setBody("stub");
  co_return resp;
}

drogon::Task<HttpResponsePtr> updateUser(HttpRequestPtr req, std::string id)
{
  auto resp = drogon::HttpResponse::newHttpResponse();
  resp -> setBody("stub");
  co_return resp;
}

drogon::Task<HttpResponsePtr> resetSubscription(HttpRequestPtr req, std::string id)
{
  auto resp = drogon::HttpResponse::newHttpResponse();
  resp -> setBody("stub");
  co_return resp;
}

drogon::Task<HttpResponsePtr> deleteUser(HttpRequestPtr req, std::string id)
{
  auto resp = drogon::HttpResponse::newHttpResponse();
  resp -> setBody("stub");
  co_return resp;
}
