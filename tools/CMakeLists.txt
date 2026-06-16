# Tools Target Definition

add_executable(photon_admin_seed
    seed_admin.cpp
    ../api/util/Crypto.cpp
)

if(PHOTON_ENABLE_CLANG_TIDY)
  set_target_properties(photon_admin_seed PROPERTIES CXX_CLANG_TIDY "${PHOTON_LINTER}")
endif()

target_include_directories(photon_admin_seed PRIVATE
    ${CMAKE_SOURCE_DIR}/api
)

target_link_libraries(photon_admin_seed PRIVATE
    CLI11::CLI11
    Threads::Threads
    drogon
    Botan::Botan
)
