#include "core_SubController.hpp"

drogon::Task<HttpResponsePtr> core::controllers::SubController::getSubscription(HttpRequestPtr req, std::string subId){
  auto resp = HttpResponse::newHttpResponse();
  resp->setBody("stub");
  co_return resp;
}
