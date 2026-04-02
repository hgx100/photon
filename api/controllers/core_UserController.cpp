#include "core_UserController.hpp"

drogon::Task<HttpResponsePtr> core::controllers::UserController::getUserList(HttpRequestPtr req)
{
  auto resp = drogon::HttpResponse::newHttpResponse();
  resp -> setBody("stub");
  co_return resp;
}

drogon::Task<HttpResponsePtr> core::controllers::UserController::getUser(HttpRequestPtr req, std::string id)
{
  auto resp = drogon::HttpResponse::newHttpResponse();
  resp -> setBody("stub");
  co_return resp;
}

drogon::Task<HttpResponsePtr> core::controllers::UserController::createUser(HttpRequestPtr req)
{
  auto resp = drogon::HttpResponse::newHttpResponse();
  resp -> setBody("stub");
  co_return resp;
}

drogon::Task<HttpResponsePtr> core::controllers::UserController::updateUser(HttpRequestPtr req, std::string id)
{
  auto resp = drogon::HttpResponse::newHttpResponse();
  resp -> setBody("stub");
  co_return resp;
}

drogon::Task<HttpResponsePtr> core::controllers::UserController::resetSubscription(HttpRequestPtr req, std::string id)
{
  auto resp = drogon::HttpResponse::newHttpResponse();
  resp -> setBody("stub");
  co_return resp;
}

drogon::Task<HttpResponsePtr> core::controllers::UserController::deleteUser(HttpRequestPtr req, std::string id)
{
  auto resp = drogon::HttpResponse::newHttpResponse();
  resp -> setBody("stub");
  co_return resp;
}
