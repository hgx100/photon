#!/usr/bin/env bash
set -e

pin() {
    local path=$1
    local tag=$2
    echo "Pinning $path to $tag..."
    cd "$path"
    git fetch --tags --quiet
    git checkout "$tag" --quiet
    cd - > /dev/null
    git add "$path"
}

pin "third_party/xray-core"      "v26.2.6"
pin "third_party/drogon"         "v1.9.12"
pin "third_party/spdlog"         "v1.17.0"
pin "third_party/nlohmann_json"  "v3.12.0"
pin "third_party/jwt-cpp"        "v0.7.2"
pin "third_party/cli11"          "v2.6.2"


pin "third_party/tgbotxx"        "v1.2.9.5"

echo "All submodules pinned. Please commit these changes."
