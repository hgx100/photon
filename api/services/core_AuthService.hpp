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

#include "../types/AuthTypes.hpp"
#include "../types/ServiceTypes.hpp"

namespace core::services {
  using core::types::LoginResult;
  using core::types::ServiceError;

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
