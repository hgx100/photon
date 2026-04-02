#include "core_NodeController.hpp"

drogon::Task<HttpResponsePtr> core::controllers::NodeController::listNodes(HttpRequestPtr req){
  auto resp = HttpResponse::newHttpResponse();
  resp->setBody("stub");
  co_return resp;
}

drogon::Task<HttpResponsePtr> core::controllers::NodeController::getNode(HttpRequestPtr req, std::string nodeId) {
  auto resp = HttpResponse::newHttpResponse();
  resp->setBody("stub");
  co_return resp;
}

drogon::Task<HttpResponsePtr> core::controllers::NodeController::createNode(HttpRequestPtr req) {
  auto resp = HttpResponse::newHttpResponse();
  resp->setBody("stub");
  co_return resp;
}

drogon::Task<HttpResponsePtr> core::controllers::NodeController::updateNode(HttpRequestPtr req, std::string nodeId) {
  auto resp = HttpResponse::newHttpResponse();
  resp->setBody("stub");
  co_return resp;
}

drogon::Task<HttpResponsePtr> core::controllers::NodeController::deleteNode(HttpRequestPtr req, std::string nodeId) {
  auto resp = HttpResponse::newHttpResponse();
  resp->setBody("stub");
  co_return resp;
}

drogon::Task<HttpResponsePtr> core::controllers::NodeController::addInbound(HttpRequestPtr req, std::string nodeId) {
  auto resp = HttpResponse::newHttpResponse();
  resp->setBody("stub");
  co_return resp;
}

drogon::Task<HttpResponsePtr> core::controllers::NodeController::deleteInbound(HttpRequestPtr req, std::string nodeId, std::string inboundId) {
  auto resp = HttpResponse::newHttpResponse();
  resp->setBody("stub");
  co_return resp;
}
