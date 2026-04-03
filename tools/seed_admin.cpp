#include <CLI/CLI.hpp>
#include <drogon/orm/DbClient.h>
#include <trantor/utils/Logger.h>
#include <json/json.h>

#include <exception>
#include <fstream>
#include <iostream>
#include <sstream>
#include <string>

#include "../api/util/Crypto.hpp"

namespace {

  Json::Value loadConfigJson(const std::string& configPath) {
    std::ifstream input(configPath);
    if (!input) {
      throw std::runtime_error("Failed to open config file: " + configPath);
    }

    Json::Value root;
    Json::CharReaderBuilder builder;
    std::string errors;
    if (!Json::parseFromStream(builder, input, &root, &errors)) {
      throw std::runtime_error("Failed to parse config file: " + errors);
    }

    return root;
  }

  std::string requireString(const Json::Value& value,
                            const char* field,
                            const char* sectionName) {
    if (!value.isMember(field) || !value[field].isString()) {
      throw std::runtime_error(std::string("Missing or invalid string field '") +
                              field + "' in " + sectionName);
    }
    return value[field].asString();
  }

  std::string quoteConnValue(const std::string& value) {
    std::string escaped;
    escaped.reserve(value.size() + 2);
    escaped.push_back('\'');
    for (const char ch : value) {
      if (ch == '\\' || ch == '\'') {
        escaped.push_back('\\');
      }
      escaped.push_back(ch);
    }
    escaped.push_back('\'');
    return escaped;
  }

  unsigned short requirePort(const Json::Value& value, const char* field, const char* sectionName) {
    if (!value.isMember(field) || !value[field].isUInt()) {
      throw std::runtime_error(std::string("Missing or invalid port field '") +
                              field + "' in " + sectionName);
    }
    return static_cast<unsigned short>(value[field].asUInt());
  }

  std::shared_ptr<drogon::orm::DbClient> createDbClientFromConfig(const std::string& configPath) {
    const auto root = loadConfigJson(configPath);
    const auto& dbClients = root["db_clients"];
    if (!dbClients.isArray() || dbClients.empty()) {
      throw std::runtime_error("Config file does not define any db_clients");
    }

    const auto& db = dbClients[0];
    const auto rdbms = requireString(db, "rdbms", "db_clients[0]");
    if (rdbms != "postgresql") {
      throw std::runtime_error("Admin seeder currently supports only postgresql db_clients");
    }

    std::ostringstream connInfo;
    connInfo << "host=" << quoteConnValue(requireString(db, "host", "db_clients[0]"))
            << " port=" << requirePort(db, "port", "db_clients[0]")
            << " dbname=" << quoteConnValue(requireString(db, "dbname", "db_clients[0]"))
            << " user=" << quoteConnValue(requireString(db, "user", "db_clients[0]"))
            << " password=" << quoteConnValue(requireString(db, "password", "db_clients[0]"))
            << " connect_timeout=3";

    return drogon::orm::DbClient::newPgClient(connInfo.str(), 1);
  }

} // namespace


int main(int argc, char** argv) {
  try {
  #ifdef DROGON_SPDLOG_SUPPORT
    trantor::Logger::enableSpdLog();
  #endif

    std::string configPath = "config.json";
    std::string username;
    std::string password;
    bool resetPasswordOnly = false;

    CLI::App app{"Photon admin seeder"};
    app.set_help_flag("-h,--help", "Show this help");
    app.add_option("-c,--config", configPath, "Path to drogon config.json")
      ->default_val(configPath)
      ->envname("PHOTON_CONFIG_PATH");
    app.add_option("-u,--username", username, "Admin username")
      ->required();
    app.add_option("-p,--password", password, "Admin password")
      ->required();
    app.add_flag("--reset-password-only", resetPasswordOnly, "Fail if the admin does not already exist");

    try {
      app.parse(argc, argv);
    } catch (const CLI::ParseError& e) {
      return app.exit(e);
    }

    if (password.empty()) {
      std::cerr << "Password cannot be empty\n";
      return 1;
    }

    auto db = createDbClientFromConfig(configPath);
    const auto hash = core::crypto::hashPassword(password);

    const auto existing = db->execSqlSync(
      "SELECT id FROM admins WHERE username = $1 LIMIT 1", username);

    if (existing.empty()) {
      if (resetPasswordOnly) {
        std::cerr << "Admin '" << username << "' does not exist\n";
        return 1;
      }

      db->execSqlSync(
        "INSERT INTO admins (username, password_hash) VALUES ($1, $2)",
        username,
        hash);
      std::cout << "Created admin '" << username << "'\n";
      return 0;
    }

    db->execSqlSync(
      "UPDATE admins SET password_hash = $2, updated_at = NOW() WHERE username = $1",
      username,
      hash);
    std::cout << "Updated admin '" << username << "'\n";
    return 0;
  } catch (const std::exception& e) {
    LOG_FATAL << "Admin seeding failed: " << e.what();
    return 1;
  }
}
