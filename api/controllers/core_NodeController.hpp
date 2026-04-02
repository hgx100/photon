#pragma once

#include <drogon/HttpController.h>
#include <drogon/HttpTypes.h>

using namespace drogon;

namespace core::controllers {
  class NodeController : public drogon::HttpController<NodeController>
  {
    public:
      METHOD_LIST_BEGIN
        ADD_METHOD_TO(core::controllers::NodeController::listNodes, "/api/v1/nodes", Get);
        ADD_METHOD_TO(core::controllers::NodeController::getNode, "/api/v1/nodes/{1}", Get);
        ADD_METHOD_TO(core::controllers::NodeController::createNode, "/api/v1/nodes", Post);
        ADD_METHOD_TO(core::controllers::NodeController::updateNode, "/api/v1/nodes/{1}", Put);
        ADD_METHOD_TO(core::controllers::NodeController::deleteNode, "/api/v1/nodes/{1}", Delete);
        ADD_METHOD_TO(core::controllers::NodeController::addInbound, "/api/v1/nodes/{1}/inbounds", Post);
        ADD_METHOD_TO(core::controllers::NodeController::deleteInbound, "/api/v1/nodes/{1}/inbounds/{2}", Delete);
      METHOD_LIST_END

      drogon::Task<HttpResponsePtr> listNodes(HttpRequestPtr req);
      drogon::Task<HttpResponsePtr> getNode(HttpRequestPtr req, std::string nodeId);
      drogon::Task<HttpResponsePtr> createNode(HttpRequestPtr req);
      drogon::Task<HttpResponsePtr> updateNode(HttpRequestPtr req, std::string nodeId);
      drogon::Task<HttpResponsePtr> deleteNode(HttpRequestPtr req, std::string nodeId);
      drogon::Task<HttpResponsePtr> addInbound(HttpRequestPtr req, std::string nodeId);
      drogon::Task<HttpResponsePtr> deleteInbound(HttpRequestPtr req, std::string nodeId, std::string inboundId);

  };
} // namespace core::controllers
