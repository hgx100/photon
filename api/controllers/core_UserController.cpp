#include "core_UserController.hpp"

using namespace core;

drogon::Task<HttpResponsePtr> core::UserController::getUserList(HttpRequestPtr req)
{
  auto resp = drogon::HttpResponse::newHttpResponse();
  resp -> setBody("stub");
  co_return resp;
}

drogon::Task<HttpResponsePtr> core::UserController::getUser(HttpRequestPtr req, std::string id)
{
  auto resp = drogon::HttpResponse::newHttpResponse();
  resp -> setBody("stub");
  co_return resp;
}

drogon::Task<HttpResponsePtr> core::UserController::createUser(HttpRequestPtr req)
{
  auto resp = drogon::HttpResponse::newHttpResponse();
  resp -> setBody("stub");
  co_return resp;
}

drogon::Task<HttpResponsePtr> core::UserController::updateUser(HttpRequestPtr req, std::string id)
{
  auto resp = drogon::HttpResponse::newHttpResponse();
  resp -> setBody("stub");
  co_return resp;
}

drogon::Task<HttpResponsePtr> core::UserController::resetSubscription(HttpRequestPtr req, std::string id)
{
  auto resp = drogon::HttpResponse::newHttpResponse();
  resp -> setBody("stub");
  co_return resp;
}

drogon::Task<HttpResponsePtr> core::UserController::deleteUser(HttpRequestPtr req, std::string id)
{
  auto resp = drogon::HttpResponse::newHttpResponse();
  resp -> setBody("stub");
  co_return resp;
}
