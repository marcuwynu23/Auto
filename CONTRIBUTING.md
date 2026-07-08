# Contributing to Auto

First off, thank you for considering contributing! We welcome all contributions
that align with the project's direction.

## Code of Conduct

This project and everyone participating in it is governed by the
[Code of Conduct](CODE_OF_CONDUCT.md). By participating, you are expected to
uphold this code.

## Prerequisites

| Tool          | Version   | Purpose                       |
|---------------|-----------|-------------------------------|
| C++ compiler  | C++11+    | Compile the source            |
| Clang/LLVM    | Latest    | Primary compiler (used in CI) |
| MinGW-w64     | Latest    | `windres` for icon resources  |
| Make          | Any       | Build automation              |
| CMake         | 3.16+     | Alternative build system      |

## Project Structure

```
Auto/
├── include/          # Public API headers
│   └── lib.h
├── src/              # Source code
│   ├── main.cpp      # CLI entry point / argument parser
│   └── lib.cpp       # Core engine (autofile parser + executor)
├── test/             # Test suite
│   ├── test_main.cpp # All test cases
│   └── test_lib.h    # Test helper header
├── resources/        # Icons and resource scripts
│   ├── auto.ico
│   └── icon.rc
├── docs/             # Website documentation (Cloudflare Pages)
├── installer-script/ # Inno Setup installer config
│   └── Auto.iss
├── .github/          # CI workflows, templates, funding
├── makefile          # GNU Make build (MinGW)
├── llvm.makefile     # GNU Make build (LLVM/Clang)
├── CMakeLists.txt    # CMake build (Clang-only)
└── CMakePresets.json # CMake presets (Debug / Release)
```

## Makefile Reference

| Target       | Description                          |
|--------------|--------------------------------------|
| `all`        | Build `bin/auto.exe` (default)       |
| `run`        | Run `bin/auto.exe`                   |
| `test`       | Compile test suite                   |
| `test-run`   | Compile and run tests                |
| `clean`      | Remove build artifacts               |

Use `make -f llvm.makefile <target>` for LLVM builds (matching CI).

### CMake Builds

```bash
# Release
cmake --preset=release
cmake --build build/release

# Debug
cmake --preset=default
cmake --build build/debug
```

### PowerShell Helper

```powershell
.\cmake-build.ps1 -Action release
.\cmake-build.ps1 -Action debug
.\cmake-build.ps1 -Action clean
```

## Development Workflow

1. Fork the repository
2. Clone your fork
3. Create a feature branch: `git checkout -b feat/my-feature`
4. Make your changes
5. Add or update tests in `test/`
6. Run the test suite: `make -f llvm.makefile test-run`
7. Commit using [conventional commits](#commit-conventions)
8. Push your branch and open a Pull Request

## Coding Standards

- **Language:** C++11 minimum (CI uses C++20)
- **Naming:** `camelCase` for functions and variables, `PascalCase` for classes
- **Formatting:** 2-space indentation, no tabs
- **Headers:** Use `#pragma once` or traditional `#ifndef` guards
- **Imports:** Group standard library includes first, then project headers
- **Errors:** Print errors to `std::cerr`, return non-zero exit codes
- **Windows-only:** Code targets Windows (uses `windows.h`, `GetFullPathNameA`, etc.)

## Testing

- **Coverage target:** All core functions and block parsing modes
- **Run tests:** `make -f llvm.makefile test-run`
- **Environment:** Tests respect `AUTO_TEST_MODE=1` (set automatically in CI)
- **Pattern:** Each feature gets a dedicated `void testFeature(TestRunner&)` function
- **Helpers:** Use `createTestFile()` / `deleteTestFile()` for temporary `.autofile` files
- **Assertions:** Use `runner.assertTrue()`, `runner.assertEquals()` from `TestRunner`

### Example

```cpp
void testMyFeature(TestRunner& runner) {
    std::string content = "myblock {\n  + echo hello\n}";
    std::string filename = createTestFile(content);
    if (!filename.empty()) {
        OutputCapture capture;
        autoRunner(filename, "myblock");
        std::string output = capture.getOutput();
        runner.assertTrue(output.find("End:") != std::string::npos, "myblock processed");
        deleteTestFile(filename);
    }
}
```

## Commit Conventions

We follow [Conventional Commits](https://www.conventionalcommits.org/):

```
<type>(<scope>): <description>
```

| Type       | Usage                          |
|------------|--------------------------------|
| `feat`     | A new feature                  |
| `fix`      | A bug fix                      |
| `docs`     | Documentation changes          |
| `test`     | Adding or updating tests       |
| `refactor` | Code refactoring               |
| `ci`       | CI/CD configuration changes    |
| `chore`    | Maintenance, build, tooling    |

### Scope

The scope should be the area affected: `parser`, `cli`, `test`, `docs`, `ci`, etc.

### Examples

```
feat(parser): add support for BREAKING CHANGE footer
fix(cli): handle missing block name gracefully
docs(readme): update installation instructions
test(parser): add edge case for empty blocks
ci(release): fix version extraction in workflow
```

## PR Process

1. All PRs must target the `main` branch
2. CI must pass (build + test suite)
3. At least one maintainer review required
4. PRs that introduce new features should include tests
5. PRs that fix bugs should include a regression test
6. Squash merge preferred for small changes

### PR Checklist

```markdown
- [ ] I have read the contributing guidelines
- [ ] My code follows the project's coding style
- [ ] I have added tests that prove my fix or feature works
- [ ] All existing tests pass
- [ ] I have updated documentation where necessary
```

## Release Process

1. Tag the commit with `v<major>.<minor>.<patch>` (e.g., `v2.0.3`)
2. Push the tag: `git push origin v2.0.3`
3. CI (`release.yml`) automatically:
   - Builds `auto.exe`
   - Packages the Inno Setup installer
   - Creates a GitHub Release with both artifacts

## Questions

Open a [discussion](https://github.com/marcuwynu23/Auto/issues) or
[start an issue](https://github.com/marcuwynu23/Auto/issues/new).
