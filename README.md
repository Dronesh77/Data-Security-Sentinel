# File and Clipboard Monitoring Application

## Overview
This application monitors the clipboard and file system to restrict copy-paste operations from a specific root directory. It is designed for Windows 11 and written in C++.

## Setup
### Prerequisites
1. **Windows 11**: This program requires Windows 11 for compatibility.
2. **Visual Studio** or **G++**: Required for compilation.

### Compilation
To compile, use the following command:
```sh
g++ main.cpp clipboard_monitor.cpp filesystem_monitor.cpp -o MonitorApp
