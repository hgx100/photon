/**
 *
 *  core_RequiresJsonBody.hpp
 *
 */

#pragma once

#include <drogon/HttpFilter.h>
using namespace drogon;
namespace core::filters {
  class RequiresJsonBody : public HttpFilter<RequiresJsonBody>
  {
    public:
      RequiresJsonBody() {}
      void doFilter(const HttpRequestPtr &req,
                    FilterCallback &&fcb,
                    FilterChainCallback &&fccb) override;
  };
} // namespace core::filters
