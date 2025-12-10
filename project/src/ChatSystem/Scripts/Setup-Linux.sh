#!/bin/bash
set -e

echo "=========================================="
echo "ChatSystem - Linux Setup"
echo "=========================================="
echo ""

echo "Checking prerequisites..."

check_tool() {
    if ! command -v $1 &> /dev/null; then
        echo "[ERROR] $1 not found"
        return 1
    fi
    echo "[OK] $1 found"
    return 0
}

MISSING=0
check_tool ninja || MISSING=1

if [ $MISSING -eq 1 ]; then
    echo ""
    echo "Install missing tools:"
    echo "  sudo apt-get update"
    echo "  sudo apt-get install -y ninja-build pkg-config"
    exit 1
fi

# System dependencies for wxWidgets
echo ""
echo "Installing system dependencies..."
sudo apt-get update
sudo apt-get install -y \
    libgtk-3-dev \
    libwebkit2gtk-4.0-dev \
    libpq-dev \
    libssl-dev \
    pkg-config \
    curl zip unzip tar

# Install vcpkg
echo ""
echo "Setting up vcpkg..."
if [ ! -d "vcpkg" ]; then
    git clone https://github.com/microsoft/vcpkg.git
    ./vcpkg/bootstrap-vcpkg.sh
else
    cd vcpkg && git pull && ./bootstrap-vcpkg.sh && cd ..
fi

export VCPKG_ROOT="$(pwd)/vcpkg"
echo ""
echo "Add to ~/.bashrc:"
echo "  export VCPKG_ROOT=\"$VCPKG_ROOT\""
echo ""

# Configure
echo "Configuring project..."
cmake --preset=debug

echo ""
echo "=========================================="
echo "Setup complete!"
echo "=========================================="
echo ""
echo "Build:  cmake --build Build/debug"
echo "Test:   ctest --test-dir Build/debug"
echo "Run:    ./Build/debug/App/ChatSystem"