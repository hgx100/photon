cmake_minimum_required(VERSION 4.2.3)
project(Photon VERSION 0.1.0 LANGUAGES CXX C)

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

# add compile_commands.json symlink for clangd
if(NOT CMAKE_SOURCE_DIR STREQUAL CMAKE_BINARY_DIR)
  execute_process(COMMAND ${CMAKE_COMMAND} -E rm -f "${CMAKE_SOURCE_DIR}/compile_commands.json")

  execute_process(
        COMMAND ${CMAKE_COMMAND} -E create_symlink
        "${CMAKE_BINARY_DIR}/compile_commands.json"
        "${CMAKE_SOURCE_DIR}/compile_commands.json"
    )
endif()

set(BUILD_TESTING OFF CACHE BOOL "" FORCE)
set(BUILD_SHARED_LIBS OFF CACHE BOOL "" FORCE)
set(BUILD_EXAMPLES OFF CACHE BOOL "" FORCE) # drogon
set(ENABLE_TESTS OFF CACHE BOOL "" FORCE) # tgbotxx
set(JWT_BUILD_TESTS OFF CACHE BOOL "" FORCE)
set(JWT_BUILD_EXAMPLES OFF CACHE BOOL "" FORCE)
set(JWT_EXTERNAL_NLOHMANN_JSON ON CACHE BOOL "" FORCE)

find_package(Threads REQUIRED)
find_package(gRPC REQUIRED)
find_package(OpenSSL REQUIRED)
find_package(Botan REQUIRED)

find_package(PkgConfig REQUIRED)
pkg_check_modules(pqxx REQUIRED IMPORTED_TARGET libpqxx)
add_library(pqxx::pqxx ALIAS PkgConfig::pqxx)

pkg_check_modules(CARES REQUIRED libcares)

if(NOT TARGET c-ares_lib)
  if(TARGET c-ares::cares)
    add_library(c-ares_lib INTERFACE IMPORTED)
    target_link_libraries(c-ares_lib INTERFACE c-ares::cares)
  else()
    find_package(PkgConfig REQUIRED)
    pkg_check_modules(CARES REQUIRED IMPORTED_TARGET libcares)
    add_library(c-ares_lib ALIAS PkgConfig::CARES)
  endif()
endif()

add_subdirectory(third_party/nlohmann_json SYSTEM)
add_subdirectory(third_party/drogon SYSTEM)
add_subdirectory(third_party/tgbotxx SYSTEM)
add_subdirectory(third_party/jwt-cpp SYSTEM)
add_subdirectory(third_party/spdlog SYSTEM)

set(PHOTON_LINTER "clang-tidy;--extra-arg-before=--driver-mode=g++")

# generate protos
include_directories(${CMAKE_SOURCE_DIR}/generated)
include_directories(${CMAKE_SOURCE_DIR}/shared)

file(GLOB_RECURSE PROTO_SRCS "${CMAKE_SOURCE_DIR}/generated/*.pb.cc")
file(GLOB_RECURSE PROTO_HDRS "${CMAKE_SOURCE_DIR}/generated/*.pb.h")

if(NOT PROTO_SRCS)
  message(WARNING "No generated protobuf files found! Run ./scripts/generate_protos.sh")
else()
  add_library(photon_protos STATIC ${PROTO_SRCS} ${PROTO_HDRS})

  target_link_libraries(photon_protos PUBLIC
        protobuf::libprotobuf
        gRPC::grpc++
    )

  if(CMAKE_CXX_COMPILER_ID MATCHES "Clang|GNU")
    target_compile_options(photon_protos PRIVATE -Wno-unused-variable -Wno-deprecated-declarations)
  endif()
endif()

# photon core
file(GLOB_RECURSE API_SRCS "api/*.cpp" "shared/*.cpp")

if(API_SRCS AND TARGET photon_protos)
  add_executable(photon_api ${API_SRCS})

  set_target_properties(photon_api PROPERTIES CXX_CLANG_TIDY "${PHOTON_LINTER}")

  target_link_libraries(photon_api PRIVATE
        photon_protos
        pqxx::pqxx
        nlohmann_json::nlohmann_json
        Threads::Threads
        drogon
        Botan::Botan
        jwt-cpp::jwt-cpp
        spdlog::spdlog
    )
endif()

# photon bot
file(GLOB_RECURSE BOT_SRCS "bot/*.cpp" "shared/*.cpp")

if(BOT_SRCS AND TARGET photon_protos)
  add_executable(photon_bot ${BOT_SRCS})

  set_target_properties(photon_bot PROPERTIES CXX_CLANG_TIDY "${PHOTON_LINTER}")

  target_link_libraries(photon_bot PRIVATE
        photon_protos
        tgbotxx
        spdlog::spdlog
    )
endif()
