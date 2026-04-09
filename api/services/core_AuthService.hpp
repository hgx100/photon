/**
 *
 *  core_AuthService.hpp
 *
 */

#pragma once

#include <drogon/HttpResponse.h>
#include <drogon/plugins/Plugin.h>
#include <drogon/utils/coroutine.h>
#include <expected>

namespace core::services {

  struct ServiceError {
      drogon::HttpStatusCode status;
      std::string message;
  };

  struct LoginResult {
      std::string access_token;
      std::string refresh_token;
      int64_t expires_in_sec;
  };

  class AuthService : public drogon::Plugin<AuthService>
  {
    public:
      AuthService() = default;
      /// This method must be called by drogon to initialize and start the plugin.
      /// It must be implemented by the user.
      void initAndStart(const Json::Value &config) override;

      /// This method must be called by drogon to shutdown the plugin.
      /// It must be implemented by the user.
      void shutdown() override;

      drogon::Task<std::expected<LoginResult, ServiceError>> login(const std::string& username, const std::string& password);

      drogon::Task<std::expected<LoginResult, ServiceError>> refresh(const std::string& refresh_token);

      drogon::Task<void> logout(const std::string& refresh_token);

    private:
      static std::string generateAccessToken(const std::string& admin_id);
      static std::string generateRefreshToken();

  };

} // namespace core::services
