if(CMAKE_CXX_COMPILER_ID MATCHES "Clang|GNU")
  add_compile_options(-w)
elseif(CMAKE_CXX_COMPILER_ID MATCHES "MSVC")
  add_compile_options(/w)
endif()

if(PHOTON_BUILD_API)
  add_subdirectory(nlohmann_json SYSTEM)
  add_subdirectory(jwt-cpp SYSTEM)
endif()

if(PHOTON_BUILD_API OR PHOTON_BUILD_ADMIN_SEED)
  add_subdirectory(cli11 SYSTEM)
  add_subdirectory(drogon SYSTEM)
  if(TARGET drogon)
    # Drogon adds -Werror for GCC; demote to warnings so dependency updates do not break the tree.
    target_compile_options(drogon PRIVATE $<$<CXX_COMPILER_ID:GNU>:-Wno-error>)
  endif()
endif()

if(PHOTON_BUILD_API OR PHOTON_BUILD_BOT)
  add_subdirectory(spdlog SYSTEM)
endif()

if(PHOTON_BUILD_BOT)
  # cpr (inside tgbotxx) appends -Wall -Wextra -Wpedantic -Werror to CMAKE_CXX_FLAGS (global). Restore
  # after add_subdirectory so that does not stick for photon_* or later deps; cpr may still add per-target flags.
  set(_PHOTON_CXX_FLAGS_PRE_TGBOTXX "${CMAKE_CXX_FLAGS}")
  add_subdirectory(tgbotxx SYSTEM)
  set(CMAKE_CXX_FLAGS "${_PHOTON_CXX_FLAGS_PRE_TGBOTXX}"
      CACHE STRING "Flags used by the compiler during all build types." FORCE)
  unset(_PHOTON_CXX_FLAGS_PRE_TGBOTXX)
endif()
