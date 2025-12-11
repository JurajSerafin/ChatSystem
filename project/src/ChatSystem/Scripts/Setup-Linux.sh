#!/bin/bash
set -e

verify_tool() {
    if ! command -v "$1" &> /dev/null; then
        echo "[ERROR] $1 still not found after installation!"
        return 1
    fi
    echo "[OK] $1 verified"
    return 0
}

# Install dependencies by category
install_core_build_tools() {
    echo "[1/10] Installing core build tools..."
    sudo apt-get install -y build-essential ninja-build pkg-config curl wget zip unzip tar gzip bzip2 xz-utils ca-certificates gnupg2 software-properties-common
}

install_autotools() {
    echo "[2/10] Installing autotools suite..."
    sudo apt-get install -y autoconf autoconf-archive automake autopoint libtool libtool-bin m4 gettext intltool
}

install_build_utilities() {
    echo "[3/10] Installing build utilities..."
    sudo apt-get install -y bison flex gperf nasm yasm texinfo help2man dos2unix rsync patch diffutils file
}

install_scripting_languages() {
    echo "[4/10] Installing scripting languages..."
    sudo apt-get install -y python3 python3-pip python3-setuptools python3-dev python3-distutils perl perl-base
}

install_compression_libs() {
    echo "[5/10] Installing compression and archive libraries..."
    sudo apt-get install -y libbz2-dev liblzma-dev zlib1g-dev libzip-dev libarchive-dev
}

install_ssl_db_libs() {
    echo "[6/10] Installing SSL/crypto and database libraries..."
    sudo apt-get install -y libssl-dev libcrypto++-dev libpq-dev libsqlite3-dev postgresql-server-dev-all libmysqlclient-dev || true
}

install_gui_libs() {
    echo "[7/10] Installing comprehensive X11/GUI libraries..."
    sudo apt-get install -y \
        libgtk-3-dev libgtk2.0-dev libwebkit2gtk-4.0-dev libnotify-dev \
        libglu1-mesa-dev libgl1-mesa-dev libegl1-mesa-dev libgles2-mesa-dev \
        libx11-dev libxext-dev libxrandr-dev libxinerama-dev libxi-dev libxxf86vm-dev \
        libxcursor-dev libxdamage-dev libxft-dev libxrender-dev libsm-dev libice-dev \
        libxtst-dev libxmu-dev libxpm-dev libxt-dev libxau-dev libxdmcp-dev libxss-dev x11proto-dev
}

install_media_libs() {
    echo "[8/10] Installing image, font, and media libraries..."
    sudo apt-get install -y libjpeg-dev libpng-dev libtiff-dev libgif-dev libfreetype6-dev libfontconfig1-dev \
        libexpat1-dev libcairo2-dev libpango1.0-dev librsvg2-dev libasound2-dev libpulse-dev libsndfile1-dev
}

install_wayland_input_libs() {
    echo "[9/10] Installing Wayland and input libraries..."
    sudo apt-get install -y libwayland-dev libxkbcommon-dev libibus-1.0-dev libdbus-1-dev libudev-dev libinput-dev libevdev-dev
}

install_additional_utils() {
    echo "[10/10] Installing additional utilities..."
    sudo apt-get install -y libicu-dev libtbb-dev libboost-all-dev ninja-build subversion git-lfs
}

# Install all system dependencies
install_system_dependencies() {
    echo ""
    echo "Installing ALL system dependencies..."


    sudo apt-get update

    # Call each group function
    install_core_build_tools
    install_autotools
    install_build_utilities
    install_scripting_languages
    install_compression_libs
    install_ssl_db_libs
    install_gui_libs
    install_media_libs
    install_wayland_input_libs
    install_additional_utils

    echo ""
    echo "[OK] All system dependencies installed and verified"
}

setup_vcpkg() {
    echo ""
    echo "Setting up vcpkg..."
    if [ ! -d "vcpkg" ]; then
        echo "Cloning vcpkg..."
        git clone https://github.com/microsoft/vcpkg.git
        cd vcpkg
        ./bootstrap-vcpkg.sh
        cd ..
    else
        echo "Updating existing vcpkg..."
        cd vcpkg
        git fetch
        git pull
        ./bootstrap-vcpkg.sh
        cd ..
    fi

    export VCPKG_ROOT="$(pwd)/vcpkg"
    echo "VCPKG_ROOT=$VCPKG_ROOT"

    if ! grep -q "VCPKG_ROOT" ~/.bashrc 2>/dev/null; then
        echo "" >> ~/.bashrc
        echo "# vcpkg" >> ~/.bashrc
        echo "export VCPKG_ROOT=\"$VCPKG_ROOT\"" >> ~/.bashrc
        echo "[INFO] Added VCPKG_ROOT to ~/.bashrc"
    fi
}

clean_previous_build() {
    if [ -d "Build/linux-debug" ]; then
        echo ""
        echo "Cleaning previous build directory..."
        rm -rf Build/linux-debug
    fi

    if [ -d "vcpkg_installed" ]; then
        echo "Cleaning previous vcpkg_installed..."
        rm -rf vcpkg_installed
    fi
}


configure_project() {
    echo ""
    echo "=========================================="
    echo "Configuring project..."
    echo "=========================================="
    echo "This will download and compile all dependencies."
    echo "First run takes 30-60 minutes depending on your system."
    echo ""

    if cmake --preset=linux-debug; then
        echo ""
        echo "=========================================="
        echo "Setup complete!"
        echo "=========================================="
        echo ""
        echo "Next steps:"
        echo "  Build:  cmake --build Build/linux-debug -j$(nproc)"
        echo "  Test:   ctest --test-dir Build/linux-debug"
        echo "  Run:    ./Build/linux-debug/App/ChatSystem"
        echo ""
        echo "Or use the build script:"
        echo "  ./Scripts/Build-Linux.sh"
    else
        EXIT_CODE=$?
        echo ""
        echo "=========================================="
        echo "[ERROR] Configuration failed!"
        echo "=========================================="
        exit $EXIT_CODE
    fi
}


install_system_dependencies
setup_vcpkg
clean_previous_build
configure_project
