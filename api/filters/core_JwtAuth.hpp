/**
 *
 *  core_JwtAuth.hpp
 *
 */

#pragma once

#include <drogon/HttpFilter.h>
#include <string_view>

using namespace drogon;

namespace core::filters {

  class JwtAuth : public HttpFilter<JwtAuth>
  {
    public:
      JwtAuth() = default;
      void doFilter(const HttpRequestPtr &req,
                    FilterCallback &&fcb,
                    FilterChainCallback &&fccb) override;
  };

} // namespace core::filters
