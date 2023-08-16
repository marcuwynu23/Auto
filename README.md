 # Auto
<p align="center">
  <img width="200" height="200" src="https://github.com/marcuwynu23/Auto/blob/main/docs/images/1.jpg" />
   <img width="200" height="200" src="https://github.com/marcuwynu23/Auto/blob/main/docs/images/2.jpg" />
</p>

AUTO (Automate YoUr Terminal Operation) is a powerful command-line tool designed to streamline your workflow by automating the execution of commands in separate instances of the terminal. With AUTO, you can easily define scripts containing sequences of commands, and the tool will run each command in its own terminal instance.
How to use:

Create a file .autofile then add commands like below in the .autofile:
```cmd
# This is a comment
# Run taskkill single use command
$ taskill /im node.exe

# Run MongoDB server in the background
- mongod.exe --dbpath C:\data\db

# Run the web server
+ node server.js

# Run the file server
+ python fileserver.py

```

then run in terminal:
```
auto
```

## NOTE:
Once the commands inside the .autofile is already RUNNING no need to rerun again the auto command sometimes it can cause error to the commands execution
specially if the a specify command can only run in ONE INSTANCE.


<!-- CONTRIBUTING -->
## Contributing
# Contributing to Ao

We welcome contributions to the AutoCLI project! To get started, simply follow these steps:

1. Fork the project to your own GitHub account
2. Create a new branch for your changes
3. Commit your changes to your new branch
4. Push your branch to your forked repository
5. Open a pull request to the main project repository

Our team will review your changes and merge them if they meet our standards. By contributing to AutoCLI, you'll be helping to make the tool even better for everyone who uses it. Thank you in advance for your contributions, and happy coding!

