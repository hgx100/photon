# Bot Target Definition

if(EXISTS "${CMAKE_CURRENT_SOURCE_DIR}/main.cpp")
  add_executable(photon_bot main.cpp)

  if(PHOTON_ENABLE_CLANG_TIDY)
    set_target_properties(photon_bot PROPERTIES CXX_CLANG_TIDY "${PHOTON_LINTER}")
  endif()

  target_link_libraries(photon_bot PRIVATE
        photon_protos
        tgbotxx
        spdlog::spdlog
  )
else()
  message(STATUS "bot/main.cpp not found. Skipping photon_bot target.")
endif()
