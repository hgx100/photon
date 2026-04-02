/**
 *
 *  core_AuthService.cpp
 *
 */

#include "core_AuthService.hpp"
#include "../config/AppConfig.hpp"
#include <botan/system_rng.h>
#include <botan/base64.h>
#include <drogon/orm/Criteria.h>
#include <drogon/orm/Exception.h>
#include <drogon/utils/coroutine.h>
#include <drogon/drogon.h>
#include <algorithm>
#include <algorithm>
#include <expected>
#include "../models/AdminRefreshTokens.h"
#include "../models/Admins.h"
#include <botan-3/botan/argon2.h>
#include <jwt-cpp/jwt.h>


using namespace drogon;
using namespace core::services;
using namespace drogon_model::photon_db;

std::string AuthService::hash_password(const std::string& password) {
  auto& rng = Botan::system_rng();

    // Argon2id parameters
    size_t p = 4;      // Parallelism (threads)
    size_t M = 65536;  // Memory cost in KB (64 MB)
    size_t t = 3;      // Time cost (iterations)

    return Botan::argon2_generate_pwhash(password.c_str(), password.size(), rng, p, M, t, 2);
}

bool AuthService::verify_password(const std::string& password, const std::string& hash) {
  return Botan::argon2_check_pwhash(password.c_str(), password.size(), hash);
}

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
  std::string token = Botan::base64_encode(buffer.data(), buffer.size());

  std::erase(token, '=');
  std::ranges::replace(token, '+', '-');
  std::ranges::replace(token, '/', '_');

  return token;
}

void AuthService::initAndStart(const Json::Value &config) {
  LOG_INFO << "AuthService started";
}

void AuthService::shutdown() {
  LOG_INFO << "AuthService stopped";
}

drogon::Task<std::expected<LoginResult, std::string>> AuthService::login(const std::string& username, const std::string& password){
  auto db = drogon::app().getDbClient("PhotonServer");
  auto adminMapper = drogon::orm::Mapper<Admins>(db);
  auto criteria = drogon::orm::Criteria(Admins::Cols::_username, drogon::orm::CompareOperator::EQ, username);
  auto admin_list = adminMapper.findBy(criteria);
  if (admin_list.empty()) {
    // dummy hash to avoid timing attacks
    verify_password(password, "$argon2id$v=19$m=65536,t=3,p=4$0gCDrx903KR8Fm9ZLe91dw$hks9CeG8HvWIEqdwYKj40ENfBqoPsCOoZBAFYwTSQSY");
    co_return std::unexpected("Invalid credentials");
  }

  auto admin = admin_list.front();
  if (!verify_password(password, admin.getValueOfPasswordHash())) {
    co_return std::unexpected("Invalid credentials");
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
    drogon::orm::Mapper<AdminRefreshTokens> tokenMapper(db);
    tokenMapper.insert(tokenModel);
  } catch (const drogon::orm::DrogonDbException& e) {
    LOG_ERROR << "Database error inserting refresh token: " << e.base().what();
    co_return std::unexpected("An internal server error occurred.");
  }

  LoginResult result{
    .access_token = access_token,
    .refresh_token = refresh_token,
    .expires_in_sec = conf.jwt_expires_in_sec
  };

  co_return result;
}

drogon::Task<std::expected<LoginResult, std::string>> AuthService::refresh(const std::string& refresh_token) {
  if (refresh_token.empty()) {
    co_return std::unexpected("Missing refresh token");
  }

  auto db = drogon::app().getDbClient("PhotonServer");
  drogon::orm::Mapper<AdminRefreshTokens> tokenMapper(db);

  AdminRefreshTokens stored;
  try {
    stored = tokenMapper.findByPrimaryKey(refresh_token);
  } catch (const drogon::orm::UnexpectedRows&) {
    co_return std::unexpected("Invalid refresh token");
  }

  if (stored.getValueOfExpiresAt() <= trantor::Date::now()) {
    try {
      tokenMapper.deleteByPrimaryKey(refresh_token);
    } catch (const drogon::orm::DrogonDbException& e) {
      LOG_ERROR << "Failed to delete expired refresh token: " << e.base().what();
    }
    co_return std::unexpected("Refresh token expired");
  }

  const std::string admin_id = stored.getValueOfAdminId();

  drogon::orm::Mapper<Admins> adminMapper(db);
  try {
    adminMapper.findByPrimaryKey(admin_id);
  } catch (const drogon::orm::UnexpectedRows&) {
    co_return std::unexpected("Invalid refresh token");
  }

  std::string access_token = generateAccessToken(admin_id);
  std::string new_refresh = generateRefreshToken();

  try {
    tokenMapper.deleteByPrimaryKey(refresh_token);
  } catch (const drogon::orm::DrogonDbException& e) {
    LOG_ERROR << "Failed to revoke old refresh token: " << e.base().what();
    co_return std::unexpected("Internal server error");
  }

  try {
    co_await db->execSqlCoro("DELETE FROM admin_refresh_tokens WHERE admin_id = $1 AND expires_at < NOW()", admin_id);
    co_await db->execSqlCoro(R"(
      DELETE FROM admin_refresh_tokens
      WHERE admin_id = $1
      AND token NOT IN (
        SELECT token FROM admin_refresh_tokens
        WHERE admin_id = $1
        ORDER BY created_at DESC
        LIMIT 4
      )
    )", admin_id);
  } catch (const std::exception& e) {
    LOG_ERROR << "Failed to enforce session limits: " << e.what();
  }

  const auto& conf = core::config::AppConfig::get();
  AdminRefreshTokens newRow;
  newRow.setToken(new_refresh);
  newRow.setAdminId(admin_id);
  newRow.setCreatedAt(trantor::Date::now());
  newRow.setExpiresAt(trantor::Date::now().after(static_cast<double>(conf.refresh_token_expires_in_days * 24 * 60 * 60)));

  try {
    tokenMapper.insert(newRow);
  } catch (const drogon::orm::DrogonDbException& e) {
    LOG_ERROR << "Database error inserting rotated refresh token: " << e.base().what();
    co_return std::unexpected("An internal server error occurred.");
  }

  co_return LoginResult{
    .access_token = std::move(access_token),
    .refresh_token = std::move(new_refresh),
    .expires_in_sec = conf.jwt_expires_in_sec};
}

drogon::Task<void> AuthService::logout(const std::string& refresh_token) {
  if (refresh_token.empty()) {
    co_return;
  }
  auto db = drogon::app().getDbClient("PhotonServer");
  drogon::orm::Mapper<AdminRefreshTokens> tokenMapper(db);
  try {
    tokenMapper.deleteByPrimaryKey(refresh_token);
  } catch (const drogon::orm::DrogonDbException& e) {
    LOG_ERROR << "Failed to delete refresh token on logout: " << e.base().what();
  }
}
