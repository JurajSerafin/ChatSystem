#!/bin/bash
set -e

# Output colors
GREEN='\033[0;32m'
YELLOW='\033[1;33m'
RED='\033[0;31m'
NC='\033[0m'

PRESET=${1:-debug}
TARGET=${2:-all}

SCRIPT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
PROJECT_ROOT="$(dirname "$SCRIPT_DIR")"

cd "$PROJECT_ROOT"

# Check if build directory exists
if [ ! -d "Build/$PRESET" ]; then
    echo -e "${YELLOW}Build directory doesn't exist. Running configuration...${NC}"
    cmake --preset=$PRESET
fi

# Get number of CPU cores
if [[ "$OSTYPE" == "darwin"* ]]; then
    CORES=$(sysctl -n hw.ncpu)
else
    CORES=$(nproc)
fi

echo -e "${GREEN}Building $TARGET with $CORES cores (preset: $PRESET)...${NC}"

# Build
if [ "$TARGET" = "all" ]; then
    cmake --build Build/$PRESET --parallel $CORES
else
    cmake --build Build/$PRESET --target $TARGET --parallel $CORES
fi

if [ $? -eq 0 ]; then
    echo ""
    echo -e "${GREEN}✓ Build completed successfully!${NC}"
    echo ""
    echo "Run application: ./Build/$PRESET/App/ChatSystem"
    echo "Run tests:       ctest --test-dir Build/$PRESET"
    echo "Run server:      ./Build/$PRESET/Server/ChatSystemServer"
else
    echo ""
    echo -e "${RED}✗ Build failed!${NC}"
    exit 1
fi