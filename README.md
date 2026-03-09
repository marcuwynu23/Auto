<div align="center">

# Auto

**Automate YoUr Terminal Operation**

A command-line tool that automates workflows by running each script command in its own terminal instance.

[![GitHub stars](https://img.shields.io/github/stars/marcuwynu23/Auto.svg?style=for-the-badge&logo=github&logoColor=white)](https://github.com/marcuwynu23/Auto/stargazers)
[![GitHub forks](https://img.shields.io/github/forks/marcuwynu23/Auto.svg?style=for-the-badge&logo=github&logoColor=white)](https://github.com/marcuwynu23/Auto/network)
[![GitHub issues](https://img.shields.io/github/issues/marcuwynu23/Auto.svg?style=for-the-badge&logo=github&logoColor=white)](https://github.com/marcuwynu23/Auto/issues)
[![GitHub license](https://img.shields.io/github/license/marcuwynu23/Auto.svg?style=for-the-badge&logo=github&logoColor=white)](https://github.com/marcuwynu23/Auto/blob/main/LICENSE)

[Features](#-features) • [Installation](#-installation) • [Usage](#-usage) • [Building](#-building) • [Contributing](#-contributing)

</div>

---

## 📋 Table of Contents

- [Features](#-features)
- [Installation](#-installation)
- [Usage](#-usage)
- [Command Syntax](#-command-syntax)
- [Building](#-building)
- [Important Notes](#-important-notes)
- [Contributing](#-contributing)

---

## ✨ Features

- 🎯 **Block-based Configuration** - Organize commands into named blocks for easy management
- 🖥️ **Separate Terminal Instances** - Each command runs in its own terminal window (or Windows Terminal tab)
- ⚡ **Multiple Execution Modes** - Normal (`+`), minimized (`-`), background (`$`), and Windows Terminal with title (`&`)
- 🔄 **Background Processes** - Run commands in the background without blocking (`$` or async)
- 🪟 **Windows Terminal (wt) Support** - Use `wt` as the first token to pass the command through unchanged, or use `& "Title" command` to run in Windows Terminal with a custom tab title
- 📝 **Comment Support** - Empty lines and lines outside command rules are ignored; use `#`-style comments in your script
- 🎨 **Simple Syntax** - Easy-to-read and write configuration format
- 📎 **Quoted Arguments** - Commands support quoted parts as single arguments; surrounding quotes are stripped
- 🧪 **Test Mode** - Set `AUTO_TEST_MODE=1` to dry-run (print commands without executing), useful for CI
- 🔒 **Thread-safe Output** - Console output is serialized when mixing sync and async commands

---

## 📦 Installation

### Option 1: Download Pre-built Binary

1. Go to the [Releases](https://github.com/marcuwynu23/Auto/releases) page
2. Download the latest `auto.exe` or `AutoInstaller.*.exe`
3. For installer: Run the installer and follow the setup wizard
4. For standalone: Add `auto.exe` to your system PATH

### Option 2: Build from Source

See the [Building](#-building) section below.

---

## 🚀 Usage

### Step 1: Create a `.autofile`

Create a file named `.autofile` in your project directory and define your command blocks:

```cmd
# This is a comment
# Define the dev block
dev {
  # Run taskkill single-use command (background)
  $ taskkill /im node.exe

  # Run MongoDB server in minimized window
  - mongod.exe --dbpath C:\data\db

  # Run the web server normally
  + node server.js

  # Run the file server
  + python fileserver.py

  # Execute multiple commands in the same line
  + set PORT=4000 && npm start --prefix client

  # Run attendance frontend server
  + npm start --prefix attendance

  # Run HRIS backend server
  + npm run --prefix server dev
}
```

### Step 2: Run Auto

Execute the following command in your terminal:

```sh
auto dev
```

Or if using a custom file (file name must start with `.`):

```sh
auto -f .autofile dev
```

---

## 📖 Command Syntax

Auto supports four execution modes:

| Symbol | Mode             | Description                                                           |
| ------ | ---------------- | --------------------------------------------------------------------- |
| `+`    | Normal           | Runs command in a normal `cmd` window                                 |
| `-`    | Minimized        | Runs command in a minimized `cmd` window                              |
| `$`    | Background       | Runs command in the background (no window)                            |
| `&`    | Windows Terminal | Runs in Windows Terminal with a custom tab title: `& "Title" command` |

- Commands whose first token is `wt` are **not** wrapped; the line is passed through as-is to support full Windows Terminal options.
- Arguments can be quoted; quoted parts are treated as a single argument and surrounding quotes are removed.

### Example Configuration

```cmd
production {
  # Start services in background
  $ nginx.exe
  $ redis-server.exe

  # Start main application normally
  + node app.js

  # Start monitoring in minimized window
  - pm2 monit

  # Windows Terminal with custom tab title
  & "API" node server.js
  & "Worker" npm run worker

}
```

### Special Blocks

Blocks whose name starts with `.` (dot) are special blocks and can be run by name like any other block (e.g. `auto .init`):

```cmd
.init {
  + echo "Initializing environment..."
  + npm install
}
```


---

## 🔨 Building

### Prerequisites

- **C++ Compiler**: MinGW-w64, LLVM/Clang, or MSVC
- **Windows Resource Compiler**: `windres` (included with MinGW)
- **Make**: For using the Makefile (optional)

### Build Methods

#### Method 1: Using Make (MinGW - Default)

```sh
make
```

The compiled binary will be in the `bin/` directory.

#### Method 2: Using Make (LLVM/Clang)

```sh
make -f llvm.makefile
```

#### Method 3: Using CMake (Preferred)

```sh
cmake -B build -G "MinGW Makefiles"
cmake --build build
```

## ⚠️ Important Notes

### ⚡ Running Processes

- **Do not rerun** the `auto` command while processes inside the `.autofile` are already running
- Doing so may cause errors, especially if a specific command only supports **one running instance** at a time
- Always stop existing processes before running `auto` again

### 💡 Best Practices

- Use descriptive block names (e.g., `dev`, `production`, `test`)
- Add comments to document your automation scripts
- Group related commands in the same block
- Use background mode (`$`) for services that don't need user interaction
- Use minimized mode (`-`) for monitoring tools

---

## 🤝 Contributing

We welcome contributions to Auto! Here's how you can help:

### Getting Started

1. **Fork** the repository to your GitHub account
2. **Clone** your fork locally:
   ```sh
   git clone https://github.com/your-username/Auto.git
   cd Auto
   ```
3. **Create a new branch** for your changes:
   ```sh
   git checkout -b feature/your-feature-name
   ```
4. **Make your changes** and test them thoroughly
5. **Commit** your changes:
   ```sh
   git commit -m "Add: your feature description"
   ```
6. **Push** to your fork:
   ```sh
   git push origin feature/your-feature-name
   ```
7. **Open a Pull Request** on the main repository

### Contribution Guidelines

- Follow the existing code style
- Add comments for complex logic
- Test your changes before submitting
- Update documentation if needed
- Write clear commit messages

Our team will review your changes and merge them if they meet our standards. Thank you for helping make Auto better! 🎉

---

<div align="center">

**Made with ❤️ by [Mark Wayne Menorca](https://github.com/marcuwynu23)**

[⭐ Star this repo](https://github.com/marcuwynu23/Auto) • [🐛 Report Bug](https://github.com/marcuwynu23/Auto/issues) • [💡 Request Feature](https://github.com/marcuwynu23/Auto/issues)

</div>
