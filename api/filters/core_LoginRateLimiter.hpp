/**
 *
 *  core_LoginRateLimiter.hpp
 *
 */

#pragma once

#include <drogon/HttpFilter.h>
using namespace drogon;
namespace core::filters {
  class LoginRateLimiter : public HttpFilter<LoginRateLimiter>
  {
    public:
      LoginRateLimiter() = default;
      void doFilter(const HttpRequestPtr &req,
                    FilterCallback &&fcb,
                    FilterChainCallback &&fccb) override;
  };
} // namespace core::filters
