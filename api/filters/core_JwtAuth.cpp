/**
 *
 *  core_JwtAuth.cpp
 *
 */

#include "core_JwtAuth.hpp"
#include "../config/AppConfig.hpp"
#include "../util/Helpers.hpp"
#include <jwt-cpp/jwt.h>
#include <string_view>

using namespace drogon;
using namespace core::filters;

namespace {
  HttpResponsePtr unauthorizedResponse()
  {
    return core::helpers::makeJsonResponse(
      {{"success", false}, {"message", "Unauthorized"}},
      drogon::k401Unauthorized);
  }

  std::string_view extractBearerToken(const HttpRequestPtr &req)
  {
    const std::string_view authHeader = req->getHeader("Authorization");
    static constexpr std::string_view kBearerPrefix = "Bearer ";
    if (!authHeader.starts_with(kBearerPrefix)) {
      return {};
    }

    const auto token = authHeader.substr(kBearerPrefix.size());
    if (token.empty() || token.find_first_not_of(' ') == std::string_view::npos) {
      return {};
    }
    return token;
  }
} // namespace

void JwtAuth::doFilter(const HttpRequestPtr &req,
                       FilterCallback &&fcb,
                       FilterChainCallback &&fccb)
{
  const std::string_view token = extractBearerToken(req);
  if (token.empty()) {
    fcb(unauthorizedResponse());
    return;
  }

  const auto &conf = core::config::AppConfig::get();

  try {
    const auto decoded = jwt::decode(std::string(token));
    const auto verifier =
      jwt::verify()
        .allow_algorithm(jwt::algorithm::hs256{conf.jwt_secret})
        .with_issuer("PhotonServer");

    verifier.verify(decoded);

    const auto adminId = decoded.get_subject();
    if (adminId.empty()) {
      fcb(unauthorizedResponse());
      return;
    }

    req->attributes()->insert(std::string("admin_id"), adminId);
    fccb();
  } catch (const std::exception &) {
    fcb(unauthorizedResponse());
  }
}
