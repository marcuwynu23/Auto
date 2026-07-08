# Auto User Guide

**Auto** is a Windows command-line tool that automates multi-terminal workflows.
It reads a configuration file (`.autofile`) and runs each command in its own
terminal instance, with support for normal, minimized, background, and
Windows Terminal modes.

---

## Table of Contents

- [Installation](#installation)
- [Quick Start](#quick-start)
- [Command Reference](#command-reference)
- [Configuration](#configuration)
- [Templates](#templates)
- [Concepts](#concepts)
- [CI/CD Integration](#cicd-integration)
- [Workflows](#workflows)
- [Troubleshooting](#troubleshooting)
- [FAQ](#faq)

---

## Installation

### Prerequisites

- Windows 10 or later
- Windows Terminal (optional, for `&` mode)

### Option 1: Download Pre-built Binary

1. Go to the [Releases page](https://github.com/marcuwynu23/Auto/releases)
2. Download `AutoInstaller.<version>.exe` (recommended) or `auto.exe`
3. Run the installer or add `auto.exe` to your system PATH

### Option 2: Build from Source

| Tool         | Requirement            |
|--------------|------------------------|
| C++ compiler | C++11 or later         |
| Make         | Any version            |
| windres      | MinGW-w64 or llvm-rc   |

```bash
# Using make (MinGW)
make

# Using make (LLVM/Clang)
make -f llvm.makefile all

# Using CMake
cmake -B build -G "MinGW Makefiles"
cmake --build build
```

### Verify Installation

```bash
auto --help
```

Expected output:

```
Usage: auto.exe [-f <file_path>] <block_name>
```

---

## Quick Start

### Step 1: Create a `.autofile`

In your project directory, create a file named `.autofile`:

```cmd
dev {
  $ taskkill /im node.exe
  - mongod.exe --dbpath C:\data\db
  + node server.js
  + python fileserver.py
  & "API" node server.js
}
```

### Step 2: Run Auto

```bash
auto dev
```

Each command in the `dev` block opens in its own terminal instance.

---

## Command Reference

### `auto <block_name>`

Execute all commands in the named block from the default `.autofile` file.

| Argument     | Description                    |
|--------------|--------------------------------|
| `block_name` | Name of the block to execute   |

### `auto -f <file_path> <block_name>`

Execute commands from a custom hidden file.

| Flag         | Default      | Description                        |
|--------------|--------------|------------------------------------|
| `-f`         | —            | Flag to specify a custom file      |
| `file_path`  | —            | Path to a hidden file (starts with `.`) |
| `block_name` | —            | Name of the block to execute       |

#### Examples

**Basic usage:**
```bash
auto dev
```

**Custom file:**
```bash
auto -f .mycommands prod
```

**Special block (dot-prefixed):**
```bash
auto .init
```

---

## Configuration

### The `.autofile` Format

`.autofile` is a plain-text file that organizes commands into named blocks.

```
blockname {
  <mode> <command>
}
```

### Block Syntax

| Element     | Description                        | Example                    |
|-------------|------------------------------------|----------------------------|
| `blockname` | A unique name for the block        | `dev`, `prod`, `.init`     |
| `{` / `}`   | Curly braces enclose the block     |                            |
| `<mode>`    | Execution mode symbol              | `+`, `-`, `$`, `&`        |
| `<command>` | The shell command to execute       | `node server.js`           |

### Execution Modes

| Symbol | Mode             | Description                                |
|--------|------------------|--------------------------------------------|
| `+`    | Normal           | Opens a normal terminal window             |
| `-`    | Minimized        | Opens a minimized window                   |
| `$`    | Background       | Runs without showing any window            |
| `&`    | Windows Terminal | Opens a Windows Terminal tab with a title  |

### Mode Details

#### Normal (`+`)

Opens a new `cmd` window and runs the command.

```
+ node server.js
```

#### Minimized (`-`)

Opens a minimized `cmd` window.

```
- mongod.exe --dbpath C:\data\db
```

#### Background (`$`)

Runs the command silently in the background. No window appears.

```
$ nginx.exe
$ redis-server.exe
```

#### Windows Terminal (`&`)

Opens a new tab in Windows Terminal with a custom title. The syntax requires
a quoted title followed by the command.

```
& "API" node server.js
& "Worker" npm run worker
```

#### Raw `wt` passthrough

Commands starting with `wt` are passed through directly without wrapping,
allowing full Windows Terminal options.

```
wt -w 0 new-tab cmd /K nslookup google.com
```

### Special Blocks

Blocks whose names start with `.` (dot) are special blocks. They behave
identically to regular blocks but use a naming convention for organization.

```cmd
.init {
  + echo "Initializing environment..."
  + npm install
}

.cleanup {
  + echo "Cleaning up..."
}
```

### Comments and Whitespace

- Lines starting with `//` or `#` are treated as comments (blank lines are skipped)
- Empty lines within blocks are ignored
- Tabs are replaced with 4 spaces

### Configuration Precedence

1. CLI arguments (`-f <file_path> <block_name>`)
2. Default file (`.autofile` in current directory)

---

## Templates

### Built-in Use Cases

**Development server setup:**
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

**Production deployment:**
```cmd
production {
  $ nginx.exe
  $ redis-server.exe
  + node app.js
  - pm2 monit
  & "API" node server.js
}
```

**CI/CD test runner:**
```cmd
test {
  + npm test
  + python -m pytest
  + go test ./...
}
```

---

## Concepts

### How Auto Works

1. Auto reads the `.autofile` configuration file
2. It locates the named block (e.g., `dev { ... }`)
3. For each command line inside the block, it determines the execution mode
   from the first character (`+`, `-`, `$`, or `&`)
4. It constructs the appropriate Windows `start` command
5. Each command runs in its own terminal instance
6. Commands in the current block are executed in order
7. The working directory for all commands is the directory containing the
   `.autofile` file

### Thread Safety

Console output is serialized using a mutex, ensuring that mixed synchronous
and asynchronous commands produce clean, non-interleaved output.

### Execution Order

Commands within a block execute sequentially. However:
- Background (`$`) and Windows Terminal (`&`) commands are fire-and-forget
  (the tool continues to the next command immediately)
- Synchronous commands (`+`, `-`) wait for the terminal window to open
  but do not wait for the command inside to complete

---

## CI/CD Integration

### GitHub Actions

```yaml
- name: Run Auto smoke test
  shell: pwsh
  env:
    AUTO_TEST_MODE: "1"
  run: |
    "@test {`n  + echo hello`n}" | Out-File -FilePath ".autofile" -Encoding utf8
    auto test
```

The `AUTO_TEST_MODE=1` environment variable prevents Auto from actually
executing commands (for safe CI testing).

### Testing

Auto's own test suite runs on `windows-latest` in GitHub Actions:

```yaml
- name: Build Tests
  run: make -B -f llvm.makefile test

- name: Run Tests
  env:
    AUTO_TEST_MODE: "1"
  run: make -f llvm.makefile test-run
```

---

## Workflows

### Monorepo Workflow

Create separate blocks for each service:

```cmd
api {
  & "API" npm run --prefix api dev
}

web {
  & "Web" npm run --prefix web dev
}

worker {
  $ npm run --prefix worker start
}

all {
  .api
  .web
  .worker
}
```

### Single Command Quick Run

```cmd
quick {
  + echo "Running single task"
}
```

### Multi-service Local Development

```cmd
infra {
  $ docker-compose up -d
  $ nginx.exe
}

services {
  & "Auth" npm run --prefix auth dev
  & "Gateway" npm run --prefix gateway dev
}
```

---

## Troubleshooting

| Problem                          | Cause                                  | Fix                                                 |
|----------------------------------|----------------------------------------|-----------------------------------------------------|
| `Failed to open file`            | `.autofile` not found                  | Create `.autofile` or use `-f` to specify the file  |
| `Block "X" not found`            | Typo in block name                     | Check block names in your `.autofile`               |
| Commands run in wrong directory  | `.autofile` is not in project root     | Place `.autofile` in the project root directory     |
| Windows Terminal not opening     | Windows Terminal not installed         | Install from Microsoft Store or use `+` mode instead |
| `auto` not recognized            | Not in system PATH                     | Add `auto.exe` directory to PATH                    |
| Commands still running           | Re-ran `auto` with same block          | Close existing terminals or use different block names |
| Quoted arguments not working     | Incorrect quoting                      | Use double quotes: `echo "hello world"`             |

---

## FAQ

**Q: What operating systems are supported?**

A: Windows only. Auto uses Windows-specific APIs (`start`, `cmd`, `windows.h`).

**Q: Can I use Auto on macOS or Linux?**

A: No. Auto is designed exclusively for Windows and relies on Windows shell
commands.

**Q: What is the `.autofile` format?**

A: It is a plain-text file using a block-based syntax. Each block has a name
and contains commands prefixed with a mode symbol.

**Q: How do I stop background processes?**

A: Background processes run in their own windows. Use Task Manager or
`taskkill /f /im <process-name>.exe` to terminate them.

**Q: Can I run the same block twice?**

A: It is not recommended. If commands from the same `.autofile` are still
running, re-running Auto may cause conflicts.

**Q: What does the `&` mode require?**

A: Windows Terminal must be installed. The command will not work in plain
`cmd.exe` or PowerShell without Windows Terminal.

**Q: How do I debug a `.autofile`?**

A: Start with a simple block like `test { + echo hello }` and verify it works
before adding complexity.

**Q: Does Auto support environment variables?**

A: Yes. Environment variables in commands are expanded by `cmd.exe` when the
command runs.

**Q: Can I pass arguments to blocks?**

A: Not directly. Each block is a static set of commands. Use environment
variables or separate blocks for parameterization.

**Q: How do I contribute?**

A: See [CONTRIBUTING.md](CONTRIBUTING.md) for guidelines.

**Q: Where do I report bugs?**

A: Open an issue at https://github.com/marcuwynu23/Auto/issues.

**Q: What license is this under?**

A: Apache 2.0. See [LICENSE](LICENSE).
