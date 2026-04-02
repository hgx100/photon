/**
 *
 *  core_LoginRateLimiter.cpp
 *
 */

#include "core_LoginRateLimiter.hpp"
#include <drogon/CacheMap.h>
#include <drogon/drogon.h>
#include <nlohmann/json_fwd.hpp>
#include "../util/Helpers.hpp"
#include "../config/AppConfig.hpp"

using namespace drogon;
using namespace core::filters;

namespace {
  struct RateLimitData {
      int count = 0;
      time_t start_time = 0;
  };
}

void LoginRateLimiter::doFilter(const HttpRequestPtr &req,
                         FilterCallback &&fcb,
                         FilterChainCallback &&fccb)
{
  const auto& conf = core::config::AppConfig::get();

  std::string ip = req->peerAddr().toIp();

  static drogon::CacheMap<std::string, RateLimitData> ip_attempts(
    drogon::app().getLoop()
  );

  time_t now = std::time(nullptr);
  RateLimitData data;

  bool exists = ip_attempts.findAndFetch(ip, data);

  size_t cache_ttl = conf.login_window_duration_sec * 2;

  if (!exists) {
    data.start_time = now;
    data.count = 1;

    ip_attempts.insert(ip, data, cache_ttl);
  } else {
    if (now - data.start_time >= conf.login_window_duration_sec) {
      data.count = 1;
      data.start_time = now;
    } else {
      if (data.count >= conf.login_requests_per_window) {
        nlohmann::json response;
        response["success"] = false;
        response["message"] = "Too many login attempts. Please try again later.";
        fcb(core::util::helpers::makeJsonResponse(response));
        return;
      }
      data.count++;
    }

    ip_attempts.modify(ip, [&data](RateLimitData &d) {
        d.count = data.count;
        d.start_time = data.start_time;
    }, cache_ttl);
  }

  fccb();
}
