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
    fcb(core::util::helpers::makeJsonResponse(
      {{"success", false}, {"message", "Request body is missing"}},
      drogon::k400BadRequest));
    return;
  }

  const auto payload = req->getJsonObject();
  if (!payload || payload->isNull() || !payload->isObject()) {
    fcb(core::util::helpers::makeJsonResponse(
      {{"success", false}, {"message", "Malformed JSON payload"}},
      drogon::k400BadRequest));
    return;
  }

  fccb();
}
