<div align="center">

# Auto

**Automate Your Terminal Operation**

A command-line tool that automates workflows by running each script command in its own terminal instance.

[GitHub stars](https://github.com/marcuwynu23/Auto/stargazers) • [GitHub forks](https://github.com/marcuwynu23/Auto/network) • [GitHub issues](https://github.com/marcuwynu23/Auto/issues) • [License](https://github.com/marcuwynu23/Auto/blob/main/LICENSE)

[Features](#-features) • [Installation](#-installation) • [Usage](#-usage) • [Building](#-building) • [Contributing](#-contributing)

</div>

---

## Table of Contents

* [Features](#-features)
* [Installation](#-installation)
* [Usage](#-usage)
* [Command Syntax](#-command-syntax)
* [Building](#-building)
* [Important Notes](#-important-notes)
* [Contributing](#-contributing)

---

## Features

* **Block-based Configuration**: Organize commands into named blocks for easy management
* **Separate Terminal Instances**: Each command runs in its own terminal window or tab
* **Multiple Execution Modes**: Normal (`+`), minimized (`-`), background (`$`), and Windows Terminal with title (`&`)
* **Background Processes**: Run commands without blocking the terminal
* **Windows Terminal Support**: Pass commands directly or run with custom tab titles
* **Comment Support**: Use comments and empty lines in scripts for readability
* **Simple Syntax**: Easy-to-read configuration format
* **Quoted Arguments**: Commands support quoted arguments as single units
* **Test Mode**: Dry-run commands for testing without execution
* **Thread-safe Output**: Console output is serialized for mixed sync and async commands

---

## Installation

### Option 1: Download Pre-built Binary

1. Go to the [Releases](https://github.com/marcuwynu23/Auto/releases) page
2. Download the latest `auto.exe` or `AutoInstaller.*.exe`
3. Run the installer or add `auto.exe` to your system PATH

### Option 2: Build from Source

See the [Building](#-building) section below.

---

## Usage

### Step 1: Create a `.autofile`

Define command blocks in `.autofile` in your project directory:

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

### Step 2: Run Auto

```sh
auto dev
```

Or with a custom file:

```sh
auto -f .autofile dev
```

---

## Command Syntax

| Symbol | Mode             | Description                                              |
| ------ | ---------------- | -------------------------------------------------------- |
| `+`    | Normal           | Runs command in a normal terminal window                 |
| `-`    | Minimized        | Runs command in a minimized window                       |
| `$`    | Background       | Runs command in the background (no window)               |
| `&`    | Windows Terminal | Runs command in Windows Terminal with a custom tab title |

* Commands starting with `wt` are passed through as-is to support full Windows Terminal options.
* Arguments can be quoted; quotes are stripped when passing to commands.

### Example Configuration

```cmd
production {
  $ nginx.exe
  $ redis-server.exe
  + node app.js
  - pm2 monit
  & "API" node server.js
  & "Worker" npm run worker
}
```

### Special Blocks

Blocks starting with `.` (dot) are special and can be executed like any other block:

```cmd
.init {
  + echo "Initializing environment..."
  + npm install
}
```

---

## Building

### Prerequisites

* C++ Compiler (MinGW-w64, LLVM/Clang, or MSVC)
* Windows Resource Compiler (`windres`)
* Make (optional)

### Build Methods

#### Method 1: Using Make (MinGW)

```sh
make
```

Binary will be in `bin/` directory.

#### Method 2: Using Make (LLVM/Clang)

```sh
make -f llvm.makefile
```

#### Method 3: Using CMake

```sh
cmake -B build -G "MinGW Makefiles"
cmake --build build
```

---

## Important Notes

* Do not rerun `auto` if commands from the same `.autofile` are still running.
* Use descriptive block names and comments.
* Group related commands together.
* Use background mode (`$`) for non-interactive services.
* Use minimized mode (`-`) for monitoring tools.

---

## Contributing

1. Fork the repository
2. Clone your fork
3. Create a branch for your changes
4. Make changes and test
5. Commit and push
6. Open a Pull Request

Follow existing code style, add comments for complex logic, and update documentation if needed.

---

<div align="center">

**Made with ❤️ by [Mark Wayne Menorca](https://github.com/marcuwynu23)**
[Star this repo](https://github.com/marcuwynu23/Auto) • [Report Bug](https://github.com/marcuwynu23/Auto/issues) • [Request Feature](https://github.com/marcuwynu23/Auto/issues)

</div>
