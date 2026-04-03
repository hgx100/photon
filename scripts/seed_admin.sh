#!/usr/bin/env bash

set -euo pipefail

ROOT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")/.." && pwd)"
declare -a CANDIDATES=(
  "$ROOT_DIR/out/build/unixlike-clang-release/photon_admin_seed"
  "$ROOT_DIR/out/build/unixlike-gcc-release/photon_admin_seed"
  "$ROOT_DIR/out/build/unixlike-clang-debug/photon_admin_seed"
  "$ROOT_DIR/out/build/unixlike-gcc-debug/photon_admin_seed"
  "$ROOT_DIR/build/photon_admin_seed"
)

for binary in "${CANDIDATES[@]}"; do
  if [[ -x "$binary" ]]; then
    exec "$binary" "$@"
  fi
done

echo "Could not find a built photon_admin_seed binary." >&2
echo "Build it first, for example:" >&2
echo "  cmake --build --preset unixlike-clang-release --target photon_admin_seed" >&2
exit 1
