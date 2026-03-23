#!/usr/bin/env bash
set -e

cd "$(dirname "$0")/.."
PROJECT_ROOT=$(pwd)

XRAY_DIR="third_party/xray-core"
CUSTOM_PROTO_DIR="proto"
OUT_DIR="generated"

if ! command -v protoc &> /dev/null; then
    echo "Error: protoc is not installed."
    exit 1
fi

GRPC_CPP_PLUGIN_PATH=$(which grpc_cpp_plugin)
if [ -z "$GRPC_CPP_PLUGIN_PATH" ]; then
    echo "Error: grpc_cpp_plugin is missing."
    exit 1
fi

if [ ! -d "$XRAY_DIR/app" ]; then
    git submodule update --init --recursive
fi

rm -rf "$OUT_DIR"
mkdir -p "$OUT_DIR"

# compile xray protos
mapfile -d $'\0' XRAY_PROTOS < <(find "$XRAY_DIR" -type f -name "*.proto" ! -path "*/vendor/*" ! -path "*/.*/*" -print0)

if [ ${#XRAY_PROTOS[@]} -gt 0 ]; then
    protoc -I="$XRAY_DIR" \
           --cpp_out="$OUT_DIR" \
           --grpc_out="$OUT_DIR" \
           --plugin=protoc-gen-grpc="$GRPC_CPP_PLUGIN_PATH" \
           "${XRAY_PROTOS[@]}"
fi

# compile custom protos
if [ -d "$CUSTOM_PROTO_DIR" ]; then
    mapfile -d $'\0' CUSTOM_PROTOS < <(find "$CUSTOM_PROTO_DIR" -type f -name "*.proto" -print0)

    if [ ${#CUSTOM_PROTOS[@]} -gt 0 ]; then
        protoc -I="$CUSTOM_PROTO_DIR" \
               -I="$XRAY_DIR" \
               --cpp_out="$OUT_DIR" \
               --grpc_out="$OUT_DIR" \
               --plugin=protoc-gen-grpc="$GRPC_CPP_PLUGIN_PATH" \
               "${CUSTOM_PROTOS[@]}"
    fi
fi

# patch protoc collision
ROUTER_CONFIG_HEADER="$OUT_DIR/app/router/config.pb.h"

if [ -f "$ROUTER_CONFIG_HEADER" ]; then
    echo "Patching C++ name collision in $ROUTER_CONFIG_HEADER..."
    # Safely replace the exact conflicting line
    sed -i 's/static constexpr Type Domain = Domain_Type_Domain;/static constexpr Type Domain_ALIAS = Domain_Type_Domain;/g' "$ROUTER_CONFIG_HEADER"
fi

echo "Protobuf generation complete!"
