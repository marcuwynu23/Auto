
# Auto Installation Guide

## 1. Download the Latest Release Version:
- Visit the [Auto GitHub Releases Page](https://github.com/marcuwynu23/Auto/releases) and download the latest version of `auto.exe`.

## 2. Move the Executable to Program Files:
- Create a new folder named `Auto` in `C:\Program Files` (if not already created).
- Inside `C:\Program Files\Auto`, create another folder called `bin` (if not already created).
- Move the `auto.exe` file you downloaded into the `bin` folder.

## 3. Add `auto` to Your System Path:
- Open the **Start Menu** and search for **Environment Variables**.
- In the **System Properties** window, click on **Environment Variables**.
- In the **System variables** section, find and select the **Path** variable, then click **Edit**.
- Click **New** and paste the following path:
  ```
  C:\Program Files\Auto\bin
  ```
- Click **OK** to save your changes and close all windows.

## 4. Verify the Installation:
- Open **Command Prompt** and type the following:
  ```
  auto --version
  ```
- If `auto` is correctly installed and available in your system path, you should see the version number displayed.
  - If no error is shown and the version number appears, the installation was successful.
  - If you see an error like `"auto is not recognized as an internal or external command"`, it means `auto` is either not installed or not added to the system path properly.

---

You should now have `auto` installed and ready for use! If you have any issues, feel free to ask for further assistance.
