<div align="center">

# Auto

<a href="https://github.com/marcuwynu23/Auto/releases"><img src="https://img.shields.io/github/v/release/marcuwynu23/Auto" alt="Release"></a>
<a href="LICENSE"><img src="https://img.shields.io/github/license/marcuwynu23/Auto?logo=github" alt="License"></a>
<a href="https://github.com/marcuwynu23/Auto/stargazers"><img src="https://img.shields.io/github/stars/marcuwynu23/Auto" alt="Stars"></a>
<img src="https://img.shields.io/badge/C%2B%2B-11%2F20-blue" alt="C++11/20">

**Automate your terminal operations.** Run each script command in its own terminal instance with control over window mode, title, and visibility.

➡️ **[Read the full user guide →](USER-GUIDE.md)**

</div>

---

## Table of Contents

- [What Is Auto?](#what-is-auto)
- [Use Cases](#use-cases)
- [Benefits](#benefits)
- [Advantages Over Other Tools](#advantages-over-other-tools)
- [User Guide](USER-GUIDE.md)
- [Installation](#installation)
- [Quick Start](#quick-start)
- [CLI Reference](#cli-reference)
- [Configuration](#configuration)
- [Examples](#examples)
- [CI/CD Integration](#cicd-integration)
- [Development](#development)
- [Architecture](#architecture)

---

## What Is Auto?

**Auto** is a Windows CLI tool that reads a block-based configuration file
(`.autofile`) and runs each command in its own terminal instance. It supports
normal, minimized, background, and Windows Terminal modes, giving you precise
control over how multi-service workflows launch on your machine.

### What It Does

- **Execute block-based workflows** — Organize commands into named blocks and run them with a single command
- **Launch separate terminal instances** — Each command gets its own window or tab
- **Support multiple execution modes** — Normal, minimized, background, and Windows Terminal with custom titles
- **Handle quoted arguments** — Preserve argument boundaries when splitting commands
- **Run from any directory** — Commands execute relative to the `.autofile` location
- **Thread-safe output** — Console output is serialized for mixed sync and async commands

### Why Use It?

| Problem                                                                  | How Auto Solves It                                              |
| ------------------------------------------------------------------------ | --------------------------------------------------------------- |
| Starting 5+ services for local dev requires opening N terminals manually | **One command** — `auto dev` launches everything                |
| Background processes clutter your taskbar                                | **Background mode** (`$`) runs silently with no window          |
| You need monitoring tools visible but not intrusive                      | **Minimized mode** (`-`) opens a reduced window                 |
| Windows Terminal tabs lose context                                       | **Named tabs** (`& "title"`) give each tab a meaningful name    |
| Team onboarding requires explaining which services to start              | **`.autofile` in the repo** — documented, reproducible workflow |

### The Philosophy

1. **Minimal setup, maximum value.** A single plain-text file replaces dozens of terminal windows and manual startup rituals.
2. **Your process stays yours.** Auto wraps your existing commands — it doesn't replace your shell, your scripts, or your tools.
3. **Windows-native, Windows-first.** Built specifically for the Windows ecosystem using `start`, `cmd`, and Windows Terminal APIs.

---

## Use Cases

| Scenario                    | How Auto Helps                                                                  |
| --------------------------- | ------------------------------------------------------------------------------- |
| **Local microservices dev** | Start API, web app, database, and worker in separate terminals with one command |
| **CI/CD debugging**         | Reproduce workflow steps locally with the same command structure                |
| **Project onboarding**      | New devs run `auto setup` to install deps, start services, and open monitoring  |
| **Multi-service testing**   | Launch test runners, mock servers, and watchers in parallel                     |
| **Production adjacents**    | Start nginx, redis, app server, and monitoring side by side                     |

---

## Benefits

- **One command, many terminals** — `auto dev` replaces 7 manual window opens
- **Reproducible workflows** — Commit `.autofile` to the repo; every dev runs the same setup
- **Four execution modes** — Choose visibility per command: normal, minimized, background, or named Windows Terminal tab
- **Zero dependencies** — A single `.exe`, no runtime, no package manager required
- **No configuration language** — Plain text with simple syntax; no YAML, JSON, or DSL to learn
- **Portable** — Drop `auto.exe` anywhere, add to PATH, and you're done
- **CI-safe testing** — `AUTO_TEST_MODE=1` skips actual execution for pipeline validation
- **Thread-safe** — Mutex-guarded output prevents interleaved console logs

---

## Advantages Over Other Tools

| Aspect                             | Auto                 | Manual (cmd windows) | Docker Compose            | tmux / screen            |
| ---------------------------------- | -------------------- | -------------------- | ------------------------- | ------------------------ |
| **Setup time**                     | ~10 seconds          | Ongoing effort       | Minutes (requires Docker) | Not available on Windows |
| **Windows native**                 | Yes                  | Yes                  | Partial (WSL)             | No                       |
| **Multiple terminal windows**      | Yes                  | Manual               | No (logs only)            | No (panes only)          |
| **Per-command visibility control** | Yes                  | Manual               | No                        | Partial                  |
| **Windows Terminal integration**   | Yes                  | Manual               | No                        | No                       |
| **Custom tab titles**              | Yes                  | No                   | No                        | No                       |
| **Background execution**           | Yes                  | Manual               | Yes                       | No                       |
| **Configuration file**             | Single `.autofile`   | None                 | `docker-compose.yml`      | `.tmux.conf`             |
| **Learning curve**                 | 2 minutes            | None                 | Moderate                  | Steep                    |
| **CI integration**                 | Via `AUTO_TEST_MODE` | Scripts              | Native                    | No                       |
| **License**                        | Apache 2.0           | N/A                  | Apache 2.0                | BSD                      |
| **Runtime dependency**             | None                 | Shell                | Docker daemon             | tmux package             |

---

## Installation

### Download Binary

1. Go to the [Releases page](https://github.com/marcuwynu23/Auto/releases)
2. Download `AutoInstaller.<version>.exe` (installer) or `auto.exe` (standalone)
3. Run the installer, or place `auto.exe` in a directory on your `PATH`

### Build from Source

See the [Development](#development) section below.

### Verify

```bash
auto --help
```

Expected output:

```
Usage: auto.exe [-f <file_path>] <block_name>
```

---

## Quick Start

```bash
# 1. Create a .autofile
echo dev { + echo hello world } > .autofile

# 2. Run the block
auto dev
```

That's it. Each command in the block opens in its own terminal window.

For a more realistic example, create a `.autofile` with:

```cmd
dev {
  $ taskkill /im node.exe
  - mongod.exe --dbpath C:\data\db
  + node server.js
  & "API" node server.js
}
```

Then:

```bash
auto dev
```

---

## CLI Reference

### `auto <block_name>`

Execute a block from the default `.autofile` file.

| Argument     | Description                  |
| ------------ | ---------------------------- |
| `block_name` | Name of the block to execute |

### `auto -f <file_path> <block_name>`

Execute a block from a custom hidden file.

| Flag         | Default | Description                                 |
| ------------ | ------- | ------------------------------------------- |
| `-f`         | —       | Flag to specify a custom file path          |
| `file_path`  | —       | Path to a hidden file (must start with `.`) |
| `block_name` | —       | Name of the block to execute                |

#### Examples

```bash
# Run the "dev" block from .autofile
auto dev

# Run the "prod" block from a custom file
auto -f .myconfig prod

# Run a special (dot-prefixed) block
auto .init
```

---

## Configuration

### The `.autofile` Format

```
blockname {
  <mode> <command>
}
```

| Symbol | Mode             | Description                               |
| ------ | ---------------- | ----------------------------------------- |
| `+`    | Normal           | Opens a normal terminal window            |
| `-`    | Minimized        | Opens a minimized window                  |
| `$`    | Background       | Runs silently in the background           |
| `&`    | Windows Terminal | Opens a Windows Terminal tab with a title |

### Example

```cmd
production {
  $ nginx.exe
  $ redis-server.exe
  + node app.js
  - pm2 monit
  & "API" node server.js
}
```

Commands starting with `wt` are passed through to Windows Terminal directly.
Blocks starting with `.` (dot) are special blocks and work like any other block.

---

## Examples

### Multi-service Development

```cmd
dev {
  $ taskkill /im node.exe
  - mongod.exe --dbpath C:\data\db
  + node server.js
  + python fileserver.py
  + set PORT=4000 && npm start --prefix client
  + npm start --prefix attendance
  + npm run --prefix server dev
}
```

```bash
auto dev
```

### Project Initialization

```cmd
.init {
  + echo "Initializing environment..."
  + npm install
  + python -m venv .venv
}
```

```bash
auto .init
```

---

## CI/CD Integration

### GitHub Actions (safe mode)

```yaml
- name: Smoke test
  shell: pwsh
  env:
    AUTO_TEST_MODE: "1"
  run: |
    "@test {`n  + echo hello`n}" | Out-File -FilePath ".autofile" -Encoding utf8
    .\auto.exe test
```

Set `AUTO_TEST_MODE=1` to prevent actual command execution in CI pipelines.

---

## Development

### Prerequisites

| Tool         | Version | Purpose                      |
| ------------ | ------- | ---------------------------- |
| C++ compiler | C++11+  | Compile source               |
| Clang/LLVM   | Latest  | Primary compiler (CI)        |
| MinGW-w64    | Latest  | `windres` for icon resources |
| Make         | Any     | Build automation             |
| CMake        | 3.16+   | Alternative build system     |

### Build

```bash
# Make (MinGW)
make

# Make (LLVM/Clang)
make -f llvm.makefile all

# CMake
cmake --preset=release
cmake --build build/release

# PowerShell helper
.\cmake-build.ps1 -Action release
```

### Test

```bash
make -f llvm.makefile test
make -f llvm.makefile test-run
```

### Project Structure

```
Auto/
├── src/main.cpp       # CLI entry point
├── src/lib.cpp        # Core engine
├── include/lib.h      # Public API header
├── test/test_main.cpp # Test suite (10 suites, 30+ tests)
├── resources/         # Icons and resource scripts
├── makefile           # GNU Make (MinGW)
├── llvm.makefile      # GNU Make (LLVM/Clang)
├── CMakeLists.txt     # CMake build
└── CMakePresets.json  # CMake presets
```

---

## Architecture

- **`main.cpp`** — Parses CLI arguments (`auto <block>` or `auto -f <file> <block>`), calls `autoRunner()`
- **`lib.cpp`** — Core engine: reads `.autofile`, tokenizes blocks, maps mode symbols to `start` commands, dispatches execution
- **Execution modes** — `cmd /K` for interactive windows, `cmd /C` for background, `wt -w 0 new-tab` for Windows Terminal
- **Thread safety** — A `std::mutex` serializes console output across sync and async commands
- **Test mode** — `AUTO_TEST_MODE=1` env var suppresses `system()` calls for safe CI execution
- **Working directory** — Resolved to the `.autofile`'s parent directory using `GetFullPathNameA`

---

<div align="center">

Made by [Mark Wayne Menorca](https://github.com/marcuwynu23)

[Star this repo](https://github.com/marcuwynu23/Auto) •
[Report Bug](https://github.com/marcuwynu23/Auto/issues) •
[Request Feature](https://github.com/marcuwynu23/Auto/issues) •
[Contribute](CONTRIBUTING.md)

</div>
