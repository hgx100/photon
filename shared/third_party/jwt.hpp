#pragma once

// Suppress clang-analyzer memory leak false positives originating from jwt-cpp/picojson
// NOLINTBEGIN(clang-analyzer-cplusplus.NewDeleteLeaks,clang-analyzer-cplusplus.NewDelete)
#include <jwt-cpp/jwt.h>
// NOLINTEND(clang-analyzer-cplusplus.NewDeleteLeaks,clang-analyzer-cplusplus.NewDelete)
