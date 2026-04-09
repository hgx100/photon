/**
 *
 *  core_RequiresJsonBody.cpp
 *
 */

#include "core_RequiresJsonBody.hpp"
#include "../util/Helpers.hpp"

using namespace drogon;
using namespace core::filters;

void RequiresJsonBody::doFilter(const HttpRequestPtr &req,
                         FilterCallback &&fcb,
                         FilterChainCallback &&fccb)
{
  if (req->bodyLength() == 0) {
    fcb(core::helpers::makeErrorResponse(
      drogon::k400BadRequest,
      "Request body is missing"));
    return;
  }

  const auto payload = req->getJsonObject();
  if (!payload || payload->isNull() || !payload->isObject()) {
    fcb(core::helpers::makeErrorResponse(
      drogon::k400BadRequest,
      "Malformed JSON payload"));
    return;
  }

  fccb();
}
