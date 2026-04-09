# Photon

Photon is a high-performance VPN control plane and management system for Xray-core. It features a modular architecture with a dedicated REST API (C++23/Drogon) and a Telegram bot interface.

## Prerequisites

Ensure your system has the following components installed:

* **Compiler**: A C++23 capable compiler (Clang 19+ or GCC 14+ recommended).
* **Build Tools**: CMake (3.25+), Ninja, and `protoc` (Protobuf compiler with gRPC plugins).
* **System Libraries**:
    * OpenSSL
    * gRPC & Protobuf (Development headers)
    * Botan (v3)
    * c-ares
    * libsystemd
    * zlib

## Build Instructions

### 1. Clone the repository
The project uses several git submodules for its dependencies. Clone recursively:

```bash
git clone --recursive https://github.com/hgx100/photon.git
cd photon
```

### 2. Generate Protobuf files
Before configuring the project, generate the C++ headers from the Xray-core and internal proto definitions:

```bash
chmod +x scripts/generate_protos.sh
./scripts/generate_protos.sh
```

### 3. Choose a build preset
The project uses CMake presets to manage different build configurations. List available options:

```bash
cmake --list-presets
```

Unix-like presets are the maintained and tested presets, and they contain the project's fuller build configuration. Windows presets exist, but they are currently minimal, bare-bones, and not tested.

Available configure presets:
* `windows-msvc-debug-developer-mode` - msvc Debug (Developer Mode)
* `windows-msvc-release-developer-mode` - msvc Release (Developer Mode)
* `windows-msvc-debug-user-mode` - msvc Debug (User Mode)
* `windows-msvc-release-user-mode` - msvc Release (User Mode)
* `windows-clang-debug` - clang Debug
* `windows-clang-release` - clang Release
* `unixlike-gcc-debug` - gcc Debug
* `unixlike-gcc-release` - gcc Release
* `unixlike-clang-debug` - clang Debug
* `unixlike-clang-release` - clang Release
* `unixlike-gcc-asan` - gcc ASan+UBSan
* `unixlike-clang-asan` - clang ASan+UBSan

### 4. Configure
Run the configuration using your preferred preset (e.g., clang release):

```bash
cmake --preset unixlike-clang-release
```

### 5. Build
Compile the specific target you need (`photon_api` or `photon_bot`):

```bash
cmake --build --parallel --preset unixlike-clang-release --target photon_api
```

The resulting binaries will be located in the build directory defined by the preset.

## Runtime Configuration

`photon_api` and the admin seeder look for `./config.json` by default. Use `--config /path/to/config.json` to override it.

## Seed an admin

Build the seeder target:

```bash
cmake --build --parallel --preset unixlike-clang-release --target photon_admin_seed
```

Then create or update an admin:

```bash
./scripts/seed_admin.sh --username admin --password 'secure_password'
```

The wrapper looks for the built binary in the common preset output directories under `out/build/` and `build/`.
