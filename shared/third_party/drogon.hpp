#pragma once

// Suppress clang-analyzer memory leak false positives originating from drogon templates
// NOLINTBEGIN(clang-analyzer-cplusplus.NewDeleteLeaks,clang-analyzer-cplusplus.NewDelete)
#include <drogon/orm/CoroMapper.h>
#include <drogon/orm/Criteria.h>
#include <drogon/orm/Exception.h>
#include <drogon/utils/Utilities.h>
#include <drogon/utils/coroutine.h>
#include <drogon/drogon.h>
// NOLINTEND(clang-analyzer-cplusplus.NewDeleteLeaks,clang-analyzer-cplusplus.NewDelete)
