# ChatSystem

[Project proposal](../../docs/Proposal.md)


## Prerequisites

### Windows
- **Visual Studio 2022** with "Desktop development with C++" workload
- **C++ Clang tools for Windows** component (install via VS Installer)
- **Git**, **CMake 3.21+**
- All cmake commands must be run from: **x64 Native Tools Command Prompt for VS 2022**

### Linux (Ubuntu/Debian)

```bash
sudo apt-get install git cmake ninja-build clang
```

## Quick Guide on how to build and run the Application

### Windows

**Clone and Enter the Repository**
```powershell
git clone --recurse-submodules <repository-url>
cd ChatSystem
```

**Run the Setup (first time: ~25-35 minutes)**
```powershell
.\Scripts\Setup-Windows.bat
```

**Build the Project**
```powershell
.\Scripts\Build-Windows.bat
```

**Run the Application**
```powershell
.\Build\windows-debug\App\ChatSystem.exe
```

### Linux

**Clone and Enter the Repository**
```bash
git clone --recurse-submodules <repository-url>
cd ChatSystem
```
**Run the Setup (first time: ~25-35 minutes)**
```bash
chmod +x Scripts/*.sh
./Scripts/Setup-Linux.sh
```

**Build the Project**
```bash
./Scripts/Build-Linux.sh
```

**Run the Application**
```bash
./Build/linux-debug/App/ChatSystem
```

## Building

### Using Build Scripts (Recommended)

**Windows:**
```powershell
# Debug build (default)
.\Scripts\Build-Windows.bat

# Release build
.\Scripts\Build-Windows.bat windows-release Release

# Build specific target
.\Scripts\Build-Windows.bat windows-debug Debug ChatSystemApp
```

**Linux:**
```bash
# Debug build (default)
./Scripts/Build-Linux.sh

# Release build
./Scripts/Build-Linux.sh linux-release

# Build specific target
./Scripts/Build-Linux.sh linux-debug ChatSystemApp
```

### Using CMake Directly

```bash
# Configure
cmake --preset=windows-debug   # Windows
cmake --preset=linux-debug     # Linux

# Build
cmake --build Build/windows-debug --config Debug    # Windows
cmake --build Build/linux-debug                     # Linux
```

## Testing

### Run Tests

**Windows:**
```powershell
ctest --test-dir Build\windows-debug -C Debug --output-on-failure
```

**Linux:**
```bash
ctest --test-dir Build/linux-debug --output-on-failure
```

## Available Executables

After building, you'll have:

| Executable | Description | Location (Windows) | Location (Linux) |
|------------|-------------|-------------------|------------------|
| **ChatSystem** | Main GUI application | `Build\windows-debug\App\` | `Build/linux-debug/App/` |
| **ChatSystemServer** | Chat server | `Build\windows-debug\Server\` | `Build/linux-debug/Server/` |
| **CoreTests** | Unit tests | `Build\windows-debug\Tests\` | `Build/linux-debug/Tests/` |


## Build Configurations

### Windows Presets

- `windows-debug` - Debug build with ClangCL
- `windows-release` - Optimized release build with ClangCL

### Linux Presets

- `linux-debug` - Debug build with Clang 
- `linux-release` - Optimized release build with Clang
