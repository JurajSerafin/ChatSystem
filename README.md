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
git clone --recurse-submodules https://github.com/JurajSerafin/ChatSystem.git
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
git clone --recurse-submodules https://github.com/JurajSerafin/ChatSystem.git
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


# Database Setup & Migrations

This project uses PostgreSQL hosted inside a Docker container for both development and testing.

Database schema evolution is managed through Flyway migrations to guarantee that all developers share an identical database structure:
- tables
- columns
- indexes
- foreign keys
- constraints

## Architecture Overview

### The Database (`postgres-db`)

Runs continuously in the background.

**Responsibilities:**
- Hosts the development database


### Flyway (`flyway`)

A short-lived migration container.

**Responsibilities:**
- Reads `.sql` migration scripts from:
  - `db/migrations`
- Applies migrations sequentially
- Terminates immediately after completion


## Prerequisites

Before starting, ensure the following tools are installed:

- **docker**
- **docker compose**

You must also create:
- `.env`

in the project root.


## Required Environment Variables

```env
DB_USER=chatsystem_admin
DB_PASSWORD=your_password
DB_NAME=db_name
DB_HOST=127.0.0.1
DB_PORT=5432
```


# Step-by-Step Setup

## 1. Boot the Database Engine

Start the PostgreSQL container in detached mode:

```bash
docker compose up -d postgres-db
```

This automatically creates the development database using values from `.env`.

> Wait approximately 5–10 seconds on first startup for PostgreSQL initialization.


## 2. Run Database Migrations (Flyway)

Once databases exist, execute Flyway migrations.


### Migrate the Development Database

```bash
docker-compose run --rm flyway
```


# Database Ready

After successful migration execution:

- PostgreSQL schema is fully initialized
- Development environment is operational

You may now:
- launch the ChatSystem server


# ⚠️ Troubleshooting: The "Nuclear Reset"

If:
- PostgreSQL becomes corrupted
- Docker caches invalid credentials
- `.env` password changes are ignored
- migrations become inconsistent

you can fully wipe and rebuild the database environment.


## Warning

This permanently deletes:
- all databases
- all stored data
- all Docker volumes


## 1. Destroy Containers & Volumes

```bash
docker-compose down -v
```


## 2. Rebuild PostgreSQL

```bash
docker-compose up -d postgres-db
```