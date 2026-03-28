#include "core_NodeController.hpp"

using namespace core;

drogon::Task<HttpResponsePtr> core::NodeController::listNodes(HttpRequestPtr req){
  auto resp = HttpResponse::newHttpResponse();
  resp->setBody("stub");
  co_return resp;
}

drogon::Task<HttpResponsePtr> core::NodeController::getNode(HttpRequestPtr req, std::string nodeId) {
  auto resp = HttpResponse::newHttpResponse();
  resp->setBody("stub");
  co_return resp;
}

drogon::Task<HttpResponsePtr> core::NodeController::createNode(HttpRequestPtr req) {
  auto resp = HttpResponse::newHttpResponse();
  resp->setBody("stub");
  co_return resp;
}

drogon::Task<HttpResponsePtr> core::NodeController::updateNode(HttpRequestPtr req, std::string nodeId) {
  auto resp = HttpResponse::newHttpResponse();
  resp->setBody("stub");
  co_return resp;
}

drogon::Task<HttpResponsePtr> core::NodeController::deleteNode(HttpRequestPtr req, std::string nodeId) {
  auto resp = HttpResponse::newHttpResponse();
  resp->setBody("stub");
  co_return resp;
}

drogon::Task<HttpResponsePtr> core::NodeController::addInbound(HttpRequestPtr req, std::string nodeId) {
  auto resp = HttpResponse::newHttpResponse();
  resp->setBody("stub");
  co_return resp;
}

drogon::Task<HttpResponsePtr> core::NodeController::deleteInbound(HttpRequestPtr req, std::string nodeId, std::string inboundId) {
  auto resp = HttpResponse::newHttpResponse();
  resp->setBody("stub");
  co_return resp;
}
