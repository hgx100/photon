#include "Helpers.hpp"
#include <drogon/HttpResponse.h>
#include <drogon/HttpTypes.h>
#include <nlohmann/json.hpp>

namespace core::helpers {
  drogon::HttpResponsePtr makeJsonResponse(const nlohmann::json& payload, drogon::HttpStatusCode status) {
    auto resp = drogon::HttpResponse::newHttpResponse();
    resp->setStatusCode(status);
    resp->setContentTypeCode(drogon::CT_APPLICATION_JSON);
    resp->setBody(payload.dump());
    return resp;
  }

  std::string_view defaultErrorMessage(drogon::HttpStatusCode status) noexcept
  {
    switch (status) {
      case drogon::k400BadRequest:
        return "Bad Request";
      case drogon::k401Unauthorized:
        return "Unauthorized";
      case drogon::k403Forbidden:
        return "Forbidden";
      case drogon::k404NotFound:
        return "Not Found";
      case drogon::k409Conflict:
        return "Conflict";
      case drogon::k429TooManyRequests:
        return "Too Many Requests";
      case drogon::k500InternalServerError:
      default:
        return "Internal Server Error";
    }
  }

  drogon::HttpResponsePtr makeErrorResponse(
    drogon::HttpStatusCode status,
    std::string_view message)
  {
    if (message.empty()) {
      message = defaultErrorMessage(status);
    }

    return makeJsonResponse(
      {{"success", false}, {"message", std::string(message)}},
      status);
  }
} // namespace core::helpers
