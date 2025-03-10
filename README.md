<div align="center">
  <h1> Auto </h1>
</div>

<p align="center">
  <img src="https://img.shields.io/github/stars/marcuwynu23/Auto.svg" alt="Stars Badge"/>
  <img src="https://img.shields.io/github/forks/marcuwynu23/Auto.svg" alt="Forks Badge"/>
  <img src="https://img.shields.io/github/issues/marcuwynu23/Auto.svg" alt="Issues Badge"/>
  <img src="https://img.shields.io/github/license/marcuwynu23/Auto.svg" alt="License Badge"/>
</p>

AUTO (Automate YoUr Terminal Operation) is a powerful command-line tool designed to streamline your workflow by automating the execution of commands in separate instances of the terminal. With AUTO, you can easily define scripts containing sequences of commands, and the tool will run each command in its own terminal instance.

## How to Use

### Step 1: Create a `.autofile`

Create a file named `.autofile` and add commands as shown below:

```cmd
# This is a comment
# Define the dev block
dev {
  # Run taskkill single-use command
  $ taskkill /im node.exe

  # Run MongoDB server in the background
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

### Step 2: Run the `auto` command

Once your `.autofile` is ready, execute the following command in the terminal:

```sh
auto
```

## 📌 Note:

- **Do not rerun** the `auto` command while processes inside the `.autofile` are already running. Doing so may cause errors, especially if a specific command only supports **one running instance** at a time.

---

## 🚀 Building the Project

### **For C++ Version**

```sh
make
```

- This will compile the C++ version of the project using the `Makefile`.
- The compiled binary can be found inside the `bin/` directory.

### **For Rust Version**

```sh
make -f makefile.rust
```

- This will compile the Rust version of the project using `Cargo`.
- The compiled binary will be placed inside `bin/rust/auto`.

---

## 🛠 Contributing

### **Contributing to Auto**

We welcome contributions to the AutoCLI project! To get started, follow these steps:

1. **Fork** the project to your own GitHub account.
2. **Create a new branch** for your changes.
3. **Commit your changes** to your new branch.
4. **Push your branch** to your forked repository.
5. **Open a pull request** to the main project repository.

Our team will review your changes and merge them if they meet our standards. By contributing to AutoCLI, you'll be helping make the tool even better for everyone who uses it.

Thank you in advance for your contributions, and happy coding! 🚀
