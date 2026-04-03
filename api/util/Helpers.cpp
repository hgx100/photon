#include "Helpers.hpp"
#include <drogon/HttpResponse.h>
#include <nlohmann/json.hpp>

namespace core::helpers {
  drogon::HttpResponsePtr makeJsonResponse(const nlohmann::json& payload, drogon::HttpStatusCode status) {
    auto resp = drogon::HttpResponse::newHttpResponse();
    resp->setStatusCode(status);
    resp->setContentTypeCode(drogon::CT_APPLICATION_JSON);
    resp->setBody(payload.dump());
    return resp;
  }
} // namespace core::helpers
