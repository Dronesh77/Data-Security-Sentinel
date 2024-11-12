# Data Security Sentinel ( C++ )

## Overview
This project monitors both the clipboard and filesystem for specific activities within a specified root directory. It is designed to:

- Prevent users from copying files or directories within the root directory to the clipboard.
- Restrict write access to files in the root directory by setting them to read-only.
- Detect and block the creation or modification of files within the root directory.

## Key Features
- Clipboard Monitoring: Monitors the clipboard for any file paths being copied, and if the path belongs to the restricted directory, the content is cleared.
- Filesystem Monitoring: Monitors the filesystem for any changes (additions, deletions, modifications, renames) in the specified directory and ensures that files are set to read-only after modification or addition.
- Directory Protection: Prevents the creation of new files or directories within the specified directory by setting appropriate file permissions.
- Cross-platform (Windows-only currently): This solution uses Windows-specific APIs (Windows.h, AclAPI.h) and is compatible with Windows operating systems.
## Table of Contents
1. Project Setup
2. Directory Structure
3. How It Works
4. Monitoring Details
5. Compilation Instructions
6. Troubleshooting
7. Contributions

## Project Setup
### Prerequisites
To run this project, you will need:

- A Windows operating system (the project is specifically built for Windows).
- A C++ development environment (like Microsoft Visual Studio, or MinGW).
- Windows SDK (for access to Windows APIs like Windows.h, AclAPI.h).
## Directory Structure
```bash
/project-root
    ├── main.cpp            # Main entry point for the application.
    ├── clipboard_monitor.cpp # Clipboard monitoring implementation.
    ├── clipboard_monitor.h   # Header file for clipboard monitoring.
    ├── filesystem_monitor.cpp # Filesystem monitoring implementation.
    ├── filesystem_monitor.h   # Header file for filesystem monitoring.
    └── README.md           # This file.
```    
## How It Works
### Clipboard Monitoring (clipboard_monitor)
The ClipboardMonitor class monitors the clipboard for any copied content that may refer to the restricted root directory. If such content is found, the clipboard is cleared to prevent any attempt to paste restricted content.

Important functions:

- OpenClipboard(): Opens the clipboard for access.
- GetClipboardData(CF_TEXT): Retrieves text content from the clipboard.
- GlobalLock() and GlobalUnlock(): Locks and unlocks global memory (used for clipboard data).
- EmptyClipboard(): Clears the clipboard content.
- CloseClipboard(): Closes the clipboard access.
### Filesystem Monitoring (filesystem_monitor)
The FileSystemMonitor class monitors a specified directory for changes (e.g., file creation, deletion, modification). It uses the ReadDirectoryChangesW function to detect changes in real-time and takes the following actions:

- Block file creation: Sets permissions to deny the creation of new files in the monitored directory.
- Block file modifications: When files are modified or renamed, their permissions are set to read-only to prevent further modifications.
Important functions:

- CreateFileW(): Opens a directory for monitoring.
- ReadDirectoryChangesW(): Monitors changes in the specified directory.
- SetDirectoryProtection() and SetReadOnlyPermissions(): Modify directory and file permissions to block specific actions like write or delete.

### Main Function (main.cpp)
The main() function initializes the monitoring by calling ClipboardMonitor::start() and FileSystemMonitor::start() on a specified directory path. It then enters an infinite loop (while (true)) to keep the application running, periodically checking the clipboard and filesystem.

## Monitoring Details
### Clipboard Monitoring:
- The clipboard is continuously monitored. If any text is copied that contains a file path within the restricted directory, the text is removed from the clipboard to prevent its use.
- Only paths that contain the restricted directory path are blocked.
### Filesystem Monitoring:
- Filesystem monitoring starts immediately upon initialization of the program.
- Any file or directory creation, modification, or renaming is detected.
- Files that are modified or newly created in the monitored directory are set to read-only permissions immediately.
- New files and directories are blocked by denying write access at the filesystem level.

## Compilation Instructions
### Step 1: Set up your C++ development environment
You need to have a C++ development environment such as Microsoft Visual Studio or MinGW installed on your machine.

### Step 2: Compile the code
     1. Open your C++ IDE (e.g., Visual Studio) and create a new C++ project.
     2. Add all the source files (main.cpp, clipboard_monitor.cpp, filesystem_monitor.cpp) and header files (clipboard_monitor.h, filesystem_monitor.h) to your project.
     3. Ensure that your project is configured to link against the Windows SDK libraries, especially for Windows-specific functions like AclAPI.h, windows.h, etc.
     4. Compile the project using your IDE's build options.

Alternatively, if you're using a command-line interface like g++ (MinGW), you can use the following command:

```bash
g++ main.cpp clipboard_monitor.cpp filesystem_monitor.cpp -o MonitorApp -lacl
```
### Step 3: Run the program
Once compiled, run the executable (MonitorApp.exe). The program will start monitoring the specified directory and clipboard for any restricted actions.

## Troubleshooting
  1. Failed to open directory for monitoring:

  - Ensure the directory exists and the program has proper permissions to access it.
- You may need to run the program with administrator privileges to modify file system permissions.
2. Permission errors when setting file or directory protection:

- Make sure the program has the necessary administrative rights to modify security settings of files and directories.
3. Clipboard not clearing when paths are copied:

- Check that the clipboard contains a path that includes the root directory. If the path does not match, the clipboard content will not be cleared.


## Contributions
Feel free to contribute to this project by forking the repository, making improvements, and submitting pull requests.