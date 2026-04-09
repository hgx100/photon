#include "core_AdminAuthController.hpp"
#include "../util/Helpers.hpp"
#include "../util/Util.hpp"
#include "../services/core_AuthService.hpp"
#include "../config/AppConfig.hpp"

#include <drogon/HttpTypes.h>
#include <drogon/drogon.h>
#include <nlohmann/json.hpp>
#include <string_view>

using namespace core;

drogon::Task<HttpResponsePtr> core::controllers::AdminAuthController::login(HttpRequestPtr req){
  const auto payload = req->getJsonObject();
  static constexpr std::array requiredFields{"username", "password"};

  for (const auto *const field : requiredFields) {
    if (!util::validateStringField(*payload, field)) {
      co_return core::helpers::makeErrorResponse(
        drogon::k400BadRequest,
        std::format("'{}' is required and must not be blank", field));
    }
  }

  const std::string username = (*payload)["username"].asString();
  const std::string password = (*payload)["password"].asString();

  auto *authService = drogon::app().getPlugin<core::services::AuthService>();
  auto login_result = co_await authService->login(username, password);
  if(!login_result.has_value()){
    co_return core::helpers::makeErrorResponse(
      login_result.error().status,
      login_result.error().message);
  }

  Json::Value retJson;
  retJson["success"] = true;
  retJson["access_token"] = login_result->access_token;
  retJson["expires_in"] = login_result->expires_in_sec;

  const auto& conf = core::config::AppConfig::get();
  auto res = drogon::HttpResponse::newHttpJsonResponse(retJson);
  drogon::Cookie refreshCookie("refresh_token", login_result->refresh_token);
  refreshCookie.setHttpOnly(true);
  refreshCookie.setSecure(true);
  refreshCookie.setSameSite(drogon::Cookie::SameSite::kStrict);
  refreshCookie.setPath("/api/v1/auth");
  refreshCookie.setMaxAge(conf.refresh_token_expires_in_days * 24 * 60 * 60);

  res->addCookie(std::move(refreshCookie));

  co_return res;
}

drogon::Task<HttpResponsePtr> core::controllers::AdminAuthController::refresh(HttpRequestPtr req){
  const std::string& refresh_token = req->getCookie("refresh_token");
  if (refresh_token.empty()) {
    co_return core::helpers::makeErrorResponse(drogon::k401Unauthorized);
  }

  auto *authService = drogon::app().getPlugin<core::services::AuthService>();
  auto refreshed = co_await authService->refresh(refresh_token);
  if (!refreshed.has_value()) {
    co_return core::helpers::makeErrorResponse(
      refreshed.error().status,
      refreshed.error().message);
  }

  Json::Value retJson;
  retJson["success"] = true;
  retJson["access_token"] = refreshed->access_token;
  retJson["expires_in"] = refreshed->expires_in_sec;

  auto res = drogon::HttpResponse::newHttpJsonResponse(retJson);

  const auto& conf = core::config::AppConfig::get();
  drogon::Cookie refreshCookie("refresh_token", refreshed->refresh_token);
  refreshCookie.setHttpOnly(true);
  refreshCookie.setSecure(true);
  refreshCookie.setSameSite(drogon::Cookie::SameSite::kStrict);
  refreshCookie.setPath("/api/v1/auth");
  refreshCookie.setMaxAge(conf.refresh_token_expires_in_days * 24 * 60 * 60);
  res->addCookie(std::move(refreshCookie));

  co_return res;
}

drogon::Task<HttpResponsePtr> core::controllers::AdminAuthController::logout(HttpRequestPtr req){
  const std::string& refresh_token = req->getCookie("refresh_token");
  auto *authService = drogon::app().getPlugin<core::services::AuthService>();
  co_await authService->logout(refresh_token);

  auto res = drogon::HttpResponse::newHttpResponse();
  res->setStatusCode(drogon::k200OK);

  drogon::Cookie clearCookie("refresh_token", "");
  clearCookie.setHttpOnly(true);
  clearCookie.setSecure(true);
  clearCookie.setSameSite(drogon::Cookie::SameSite::kStrict);
  clearCookie.setPath("/api/v1/auth");
  clearCookie.setMaxAge(0);
  res->addCookie(std::move(clearCookie));

  co_return res;
}
