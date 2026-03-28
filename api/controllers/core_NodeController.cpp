#include "core_NodeController.hpp"

using namespace core;

drogon::Task<HttpResponsePtr> core::NodeController::listNodes(HttpRequestPtr req){
  auto resp = HttpResponse::newHttpResponse();
  resp->setBody("stub");
  co_return resp;

}

drogon::Task<HttpResponsePtr> getNode(HttpRequestPtr req, std::string nodeId) {
  auto resp = HttpResponse::newHttpResponse();
  resp->setBody("stub");
  co_return resp;
}

drogon::Task<HttpResponsePtr> createNode(HttpRequestPtr req) {
  auto resp = HttpResponse::newHttpResponse();
  resp->setBody("stub");
  co_return resp;
}

drogon::Task<HttpResponsePtr> updateNode(HttpRequestPtr req, std::string nodeId) {
  auto resp = HttpResponse::newHttpResponse();
  resp->setBody("stub");
  co_return resp;
}

drogon::Task<HttpResponsePtr> deleteNode(HttpRequestPtr req, std::string nodeId) {
  auto resp = HttpResponse::newHttpResponse();
  resp->setBody("stub");
  co_return resp;
}

drogon::Task<HttpResponsePtr> addInbound(HttpRequestPtr req, std::string nodeId) {
  auto resp = HttpResponse::newHttpResponse();
  resp->setBody("stub");
  co_return resp;
}

drogon::Task<HttpResponsePtr> deleteInbound(HttpRequestPtr req, std::string nodeId, std::string inboundId) {
  auto resp = HttpResponse::newHttpResponse();
  resp->setBody("stub");
  co_return resp;
}
