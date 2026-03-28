#include "core_AdminAuthController.hpp"

using namespace core;

drogon::Task<HttpResponsePtr> core::AdminAuthController::login(HttpRequestPtr req){
  auto resp = HttpResponse::newHttpResponse();
  resp->setBody("stub");
  co_return resp;
}

drogon::Task<HttpResponsePtr> core::AdminAuthController::refresh(HttpRequestPtr req){
  auto resp = HttpResponse::newHttpResponse();
  resp->setBody("stub");
  co_return resp;
}
