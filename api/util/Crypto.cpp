#include "Crypto.hpp"

#include <botan/argon2fmt.h>
#include <botan/system_rng.h>

#include <cstddef>
#include <string>

namespace core::crypto {

  std::string hashPassword(const std::string& password) {
    auto& rng = Botan::system_rng();

    constexpr size_t parallelism = 4;
    constexpr size_t memory_kib = 65536;
    constexpr size_t iterations = 3;

    return Botan::argon2_generate_pwhash(password.c_str(), password.size(), rng, parallelism, memory_kib, iterations, 2);
  }

  bool verifyPassword(const std::string& password, const std::string& hash) {
    return Botan::argon2_check_pwhash(password.c_str(), password.size(), hash);
  }

}  // namespace core::crypto
