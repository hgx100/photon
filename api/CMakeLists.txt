# API Target Definition

set(API_MODEL_SRCS
    models/AdminRefreshTokens.cpp
    models/Admins.cpp
    models/NodeInbounds.cpp
    models/Nodes.cpp
    models/Transactions.cpp
    models/UserBalances.cpp
    models/Users.cpp
    models/UserTrafficLogs.cpp
)

set(API_SRCS
    main.cpp
    config/AppConfig.cpp
    controllers/core_AdminAuthController.cpp
    controllers/core_NodeController.cpp
    controllers/core_StatsController.cpp
    controllers/core_SubController.cpp
    controllers/core_UserController.cpp
    filters/core_JwtAuth.cpp
    filters/core_LoginRateLimiter.cpp
    filters/core_RequiresJsonBody.cpp
    services/core_AuthService.cpp
    util/Crypto.cpp
    util/Helpers.cpp
    util/Util.cpp
    ${API_MODEL_SRCS}
)

if(TARGET photon_protos)
  add_executable(photon_api ${API_SRCS})

  if(PHOTON_ENABLE_CLANG_TIDY)
    set_target_properties(photon_api PROPERTIES CXX_CLANG_TIDY "${PHOTON_LINTER}")
  endif()

  if(API_MODEL_SRCS)
    set_source_files_properties(${API_MODEL_SRCS} PROPERTIES SKIP_LINTING ON)
  endif()

  target_include_directories(photon_api PRIVATE
      ${CMAKE_CURRENT_SOURCE_DIR}
  )

  target_link_libraries(photon_api PRIVATE
        photon_protos
        CLI11::CLI11
        nlohmann_json::nlohmann_json
        Threads::Threads
        drogon
        Botan::Botan
        jwt-cpp::jwt-cpp
        spdlog::spdlog
    )
endif()
