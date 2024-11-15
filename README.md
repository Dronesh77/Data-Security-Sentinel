# Data Security Sentinel ( C++ )

## Overview
This project monitors both the clipboard and filesystem for specific activities within a specified root directory. It is designed to:

- Prevent users from copying files or directories within the root directory and outside the directory to the clipboard.


## Key Features
- Clipboard Monitoring: Monitors the clipboard for any file paths being copied, and if the path belongs to the restricted directory, the content is cleared.
- Cross-platform (Windows-only currently): This solution uses Windows-specific APIs (Windows.h, AclAPI.h) and is compatible with Windows operating systems.
## Table of Contents
1. Project Setup
2. Directory Structure
3. How It Works
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
    └── README.md           # This file.
```    
## How It Works
### Clipboard Monitoring (clipboard_monitor)
The ClipboardMonitor class monitors the clipboard for any copied content that may refer to the restricted root directory. If such content is found, the clipboard is cleared to prevent any attempt to paste restricted content.

Important functions:

- OpenClipboard(): Opens the clipboard for access.
- EmptyClipboard(): Clears the clipboard content.
- CloseClipboard(): Closes the clipboard access.

### Main Function (main.cpp)
The main() function initializes the monitoring by calling ClipboardMonitor::start() on a specified directory path. It then enters an infinite loop (while (true)) to keep the application running, periodically checking the clipboard and filesystem.

## Monitoring Details
### Clipboard Monitoring:
- The clipboard is continuously monitored. If any text is copied that contains a file path within the restricted directory, the text is removed from the clipboard to prevent its use.
- Only paths that contain the restricted directory path are blocked.

## Compilation Instructions
### Step 1: Clone the repository:
   ```bash
   git clone https://github.com/yourusername/Data-Security-Sentinel.git
   cd Data-Security-Sentinel
   ```

### Step 2: Compile the code
1. Open your C++ IDE (e.g., Visual Studio) and create a new C++ project.
2. Add all the source files (main.cpp, clipboard_monitor.cpp) and header files (clipboard_monitor.h) to your project.
3. Ensure that your project is configured to link against the Windows SDK libraries, especially for Windows-specific functions like AclAPI.h, windows.h, etc.
4. Compile the project using your IDE's build options.

Alternatively, if you're using a command-line interface like g++ (MinGW), you can use the following command:

```bash
cd path/to/your/directory
g++ main.cpp clipboard_monitor.cpp -o MonitorApp -lacl
```
### Step 3: Run the program
Once compiled, run the executable (MonitorApp.exe). The program will start monitoring the specified directory and clipboard for any restricted actions.
```bash
./MonitorApp.exe
```

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