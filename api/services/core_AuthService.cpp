/**
 *
 *  core_AuthService.cpp
 *
 */

#include "core_AuthService.hpp"
#include "../config/AppConfig.hpp"
#include "../util/Crypto.hpp"
#include "../util/Helpers.hpp"
#include <botan/system_rng.h>
#include <botan/base64.h>
#include <drogon/orm/CoroMapper.h>
#include <drogon/orm/Criteria.h>
#include <drogon/orm/Exception.h>
#include <drogon/utils/Utilities.h>
#include <drogon/utils/coroutine.h>
#include <drogon/drogon.h>
#include <algorithm>
#include <expected>
#include "../models/AdminRefreshTokens.h"
#include "../models/Admins.h"
#include <jwt-cpp/jwt.h>


using namespace drogon;
using namespace core::services;
using namespace drogon_model::photon_db;

std::string AuthService::generateAccessToken(const std::string& admin_id){
  const auto& conf = config::AppConfig::get();

  auto now = std::chrono::system_clock::now();
  auto expires_at = now + std::chrono::seconds(conf.jwt_expires_in_sec);

  return jwt::create()
    .set_type("JWS")
    .set_issuer("PhotonServer")
    .set_subject(admin_id)
    .set_issued_at(now)
    .set_expires_at(expires_at)
    .sign(jwt::algorithm::hs256{conf.jwt_secret});
}

std::string AuthService::generateRefreshToken(){
  auto& rng = Botan::system_rng();

  std::vector<uint8_t> buffer(32);
  rng.randomize(buffer.data(), buffer.size());
  std::string token = drogon::utils::base64Encode(buffer.data(), buffer.size(), true, false);

  return token;
}

void AuthService::initAndStart(const Json::Value &config) {
  LOG_INFO << "AuthService started";
}

void AuthService::shutdown() {
  LOG_INFO << "AuthService stopped";
}

drogon::Task<std::expected<LoginResult, ServiceError>> AuthService::login(const std::string& username, const std::string& password){
  auto db = drogon::app().getDbClient("PhotonServer");
  auto adminMapper = drogon::orm::CoroMapper<Admins>(db);
  auto criteria = drogon::orm::Criteria(Admins::Cols::_username, drogon::orm::CompareOperator::EQ, username);
  auto admin_list = co_await adminMapper.findBy(criteria);
  if (admin_list.empty()) {
    // dummy hash to avoid timing attacks
    core::crypto::verifyPassword(password, "$argon2id$v=19$m=65536,t=3,p=4$0gCDrx903KR8Fm9ZLe91dw$hks9CeG8HvWIEqdwYKj40ENfBqoPsCOoZBAFYwTSQSY");
    co_return std::unexpected(ServiceError{
      .status = drogon::k401Unauthorized,
      .message = "Invalid credentials"});
  }

  auto admin = admin_list.front();
  if (!core::crypto::verifyPassword(password, admin.getValueOfPasswordHash())) {
    co_return std::unexpected(ServiceError{
      .status = drogon::k401Unauthorized,
      .message = "Invalid credentials"});
  }
  std::string admin_id_str = admin.getValueOfId();

  std::string access_token = generateAccessToken(admin_id_str);
  std::string refresh_token = generateRefreshToken();

  try {
    // delete expired tokens
    co_await db->execSqlCoro("DELETE FROM admin_refresh_tokens WHERE admin_id = $1 AND expires_at < NOW()", admin_id_str);

    // cap the number of refresh tokens to 5
    co_await db->execSqlCoro(R"(
      DELETE FROM admin_refresh_tokens
      WHERE admin_id = $1
      AND token NOT IN (
        SELECT token FROM admin_refresh_tokens
        WHERE admin_id = $1
        ORDER BY created_at DESC
        LIMIT 4
      )
    )", admin_id_str);
  } catch (const std::exception& e) {
    LOG_ERROR << "Failed to enforce session limits: " << e.what();
  }

  const auto& conf = core::config::AppConfig::get();
  AdminRefreshTokens tokenModel;
  tokenModel.setToken(refresh_token);
  tokenModel.setAdminId(admin_id_str);
  tokenModel.setCreatedAt(trantor::Date::now());
  tokenModel.setExpiresAt(trantor::Date::now().after(static_cast<double>(conf.refresh_token_expires_in_days * 24 * 60 * 60)));
  try {
    drogon::orm::CoroMapper<AdminRefreshTokens> tokenMapper(db);
    co_await tokenMapper.insert(tokenModel);
  } catch (const drogon::orm::DrogonDbException& e) {
    LOG_ERROR << "Database error inserting refresh token: " << e.base().what();
    co_return std::unexpected(ServiceError{
      .status = drogon::k500InternalServerError,
      .message = std::string(core::helpers::defaultErrorMessage(drogon::k500InternalServerError))});
  }

  LoginResult result{
    .access_token = access_token,
    .refresh_token = refresh_token,
    .expires_in_sec = conf.jwt_expires_in_sec
  };

  co_return result;
}

drogon::Task<std::expected<LoginResult, ServiceError>> AuthService::refresh(const std::string& refresh_token) {
  if (refresh_token.empty()) {
    co_return std::unexpected(ServiceError{
      .status = drogon::k401Unauthorized,
      .message = std::string(core::helpers::defaultErrorMessage(drogon::k401Unauthorized))});
  }

  auto db = drogon::app().getDbClient("PhotonServer");
  try {
    auto trans = co_await db->newTransactionCoro();

    auto consumed = co_await trans->execSqlCoro(
      "DELETE FROM admin_refresh_tokens WHERE token = $1 AND expires_at > NOW() "
      "RETURNING admin_id",
      refresh_token);

    if (consumed.empty()) {
      auto expired = co_await trans->execSqlCoro(
        "SELECT 1 FROM admin_refresh_tokens WHERE token = $1 AND expires_at <= NOW() LIMIT 1",
        refresh_token);
      trans->rollback();
      if (!expired.empty()) {
        co_return std::unexpected(ServiceError{
          .status = drogon::k401Unauthorized,
          .message = std::string(core::helpers::defaultErrorMessage(drogon::k401Unauthorized))});
      }
      co_return std::unexpected(ServiceError{
        .status = drogon::k401Unauthorized,
        .message = std::string(core::helpers::defaultErrorMessage(drogon::k401Unauthorized))});
    }

    const std::string admin_id = consumed.front()["admin_id"].as<std::string>();

    auto admin = co_await trans->execSqlCoro(
        "SELECT 1 FROM admins WHERE id = $1 LIMIT 1",
        admin_id);
    if (admin.empty()) {
      trans->rollback();
      co_return std::unexpected(ServiceError{
        .status = drogon::k401Unauthorized,
        .message = std::string(core::helpers::defaultErrorMessage(drogon::k401Unauthorized))});
    }

    const auto& conf = core::config::AppConfig::get();
    const auto now = trantor::Date::now();
    const auto new_refresh = generateRefreshToken();
    const auto access_token = generateAccessToken(admin_id);
    const auto expires_new =
      now.after(static_cast<double>(conf.refresh_token_expires_in_days * 24 * 60 * 60));

    co_await trans->execSqlCoro(
      "DELETE FROM admin_refresh_tokens WHERE admin_id = $1 AND expires_at < NOW()",
    admin_id);
    co_await trans->execSqlCoro(R"(
      DELETE FROM admin_refresh_tokens
      WHERE admin_id = $1
      AND token NOT IN (
        SELECT token FROM admin_refresh_tokens
        WHERE admin_id = $1
        ORDER BY created_at DESC
        LIMIT 4
      )
    )", admin_id);
    co_await trans->execSqlCoro(
      "INSERT INTO admin_refresh_tokens (token, admin_id, created_at, expires_at) "
      "VALUES ($1, $2, $3, $4)",
      new_refresh,
      admin_id,
      now,
      expires_new);

    trans.reset();

    co_return LoginResult{
      .access_token = access_token,
      .refresh_token = new_refresh,
      .expires_in_sec = conf.jwt_expires_in_sec};
  } catch (const drogon::orm::DrogonDbException& e) {
    LOG_ERROR << "Refresh token rotation failed: " << e.base().what();
    co_return std::unexpected(ServiceError{
      .status = drogon::k500InternalServerError,
      .message = std::string(core::helpers::defaultErrorMessage(drogon::k500InternalServerError))});
  } catch (const std::exception& e) {
    LOG_ERROR << "Refresh token rotation failed: " << e.what();
    co_return std::unexpected(ServiceError{
      .status = drogon::k500InternalServerError,
      .message = std::string(core::helpers::defaultErrorMessage(drogon::k500InternalServerError))});
  }
}

drogon::Task<void> AuthService::logout(const std::string& refresh_token) {
  if (refresh_token.empty()) {
    co_return;
  }
  auto db = drogon::app().getDbClient("PhotonServer");
  drogon::orm::CoroMapper<AdminRefreshTokens> tokenMapper(db);
  try {
    co_await tokenMapper.deleteByPrimaryKey(refresh_token);
  } catch (const drogon::orm::DrogonDbException& e) {
    LOG_ERROR << "Failed to delete refresh token on logout: " << e.base().what();
  }
}
