#include "core_SubController.hpp"

using namespace core;

drogon::Task<HttpResponsePtr> core::SubController::getSubscription(HttpRequestPtr req, std::string subId){
  auto resp = HttpResponse::newHttpResponse();
  resp->setBody("stub");
  co_return resp;
}
