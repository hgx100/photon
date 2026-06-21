cmake_minimum_required(VERSION 3.27)
project(Photon VERSION 0.1.0 LANGUAGES CXX C)

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
option(PHOTON_ENABLE_CLANG_TIDY "Enable clang-tidy on photon targets" ON)
option(PHOTON_BUILD_API "Build photon_api" ON)
option(PHOTON_BUILD_ADMIN_SEED "Build photon_admin_seed" ON)
option(PHOTON_BUILD_BOT "Build photon_bot" ON)

# add compile_commands.json symlink for clangd
if(NOT CMAKE_SOURCE_DIR STREQUAL CMAKE_BINARY_DIR)
  execute_process(COMMAND ${CMAKE_COMMAND} -E rm -f "${CMAKE_SOURCE_DIR}/compile_commands.json")

  execute_process(
        COMMAND ${CMAKE_COMMAND} -E create_symlink
        "${CMAKE_BINARY_DIR}/compile_commands.json"
        "${CMAKE_SOURCE_DIR}/compile_commands.json"
    )
endif()

# drogon:
set(BUILD_TESTING OFF CACHE BOOL "" FORCE)
set(BUILD_SHARED_LIBS OFF CACHE BOOL "" FORCE)
set(BUILD_EXAMPLES OFF CACHE BOOL "" FORCE)

# tgbotxx:
set(ENABLE_TESTS OFF CACHE BOOL "" FORCE)

# jwt-cpp
set(JWT_BUILD_TESTS OFF CACHE BOOL "" FORCE)
set(JWT_BUILD_EXAMPLES OFF CACHE BOOL "" FORCE)
set(JWT_EXTERNAL_NLOHMANN_JSON ON CACHE BOOL "" FORCE)

# cli11
set(CLI11_BUILD_TESTS OFF CACHE BOOL "" FORCE)
set(CLI11_BUILD_EXAMPLES OFF CACHE BOOL "" FORCE)
set(CLI11_BUILD_DOCS OFF CACHE BOOL "" FORCE)
set(CLI11_INSTALL OFF CACHE BOOL "" FORCE)

set(PHOTON_NEEDS_PROTO_TARGETS OFF)
if(PHOTON_BUILD_API OR PHOTON_BUILD_BOT)
  set(PHOTON_NEEDS_PROTO_TARGETS ON)
endif()

set(PHOTON_NEEDS_THREADS OFF)
if(PHOTON_BUILD_API OR PHOTON_BUILD_ADMIN_SEED)
  set(PHOTON_NEEDS_THREADS ON)
endif()

if(PHOTON_NEEDS_THREADS)
  find_package(Threads REQUIRED)
endif()

if(PHOTON_NEEDS_PROTO_TARGETS)
  find_package(gRPC REQUIRED)
endif()

if(PHOTON_BUILD_API OR PHOTON_BUILD_ADMIN_SEED)
  find_package(Botan REQUIRED)
endif()

if(PHOTON_BUILD_API)
  find_package(OpenSSL REQUIRED)
  find_package(PkgConfig REQUIRED)
  pkg_check_modules(CARES REQUIRED libcares)

  if(NOT TARGET c-ares_lib)
    if(TARGET c-ares::cares)
      add_library(c-ares_lib INTERFACE IMPORTED)
      target_link_libraries(c-ares_lib INTERFACE c-ares::cares)
    else()
      pkg_check_modules(CARES REQUIRED IMPORTED_TARGET libcares)
      add_library(c-ares_lib ALIAS PkgConfig::CARES)
    endif()
  endif()
endif()

add_subdirectory(third_party)

if(PHOTON_ENABLE_CLANG_TIDY)
  set(PHOTON_LINTER
      "clang-tidy;--use-color;--extra-arg-before=--driver-mode=g++;--exclude-header-filter=.*/api/models/.*")
endif()

# generate protos
if(PHOTON_NEEDS_PROTO_TARGETS)
  if(EXISTS "${CMAKE_SOURCE_DIR}/generated/protos.cmake")
    include("${CMAKE_SOURCE_DIR}/generated/protos.cmake")
  endif()

  if(NOT PROTO_SRCS)
    message(WARNING "No generated protobuf files found! Run ./scripts/generate_protos.sh")
  else()
    add_library(photon_protos STATIC ${PROTO_SRCS} ${PROTO_HDRS})

    target_include_directories(photon_protos PUBLIC
        ${CMAKE_SOURCE_DIR}/generated
        ${CMAKE_SOURCE_DIR}/shared
    )

    target_link_libraries(photon_protos PUBLIC
        protobuf::libprotobuf
        gRPC::grpc++
    )

    if(CMAKE_CXX_COMPILER_ID MATCHES "Clang|GNU")
      target_compile_options(photon_protos PRIVATE -Wno-unused-variable -Wno-deprecated-declarations)
    endif()
  endif()
endif()

# Subdirectories
if(PHOTON_BUILD_API)
  add_subdirectory(api)
endif()

if(PHOTON_BUILD_ADMIN_SEED)
  add_subdirectory(tools)
endif()

if(PHOTON_BUILD_BOT)
  add_subdirectory(bot)
endif()
