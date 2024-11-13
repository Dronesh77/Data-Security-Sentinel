#include "clipboard_monitor.h"
#include "filesystem_monitor.h"
#include <windows.h>  

using namespace std;

int main() {
    string root_dir = "Y:\\Detrator_Task\\root";

    // Restrict write access on the root directory
    wstring w_root_dir(root_dir.begin(), root_dir.end());

    // Start clipboard and filesystem monitoring
    ClipboardMonitor::start(root_dir);
    FileSystemMonitor::start(root_dir);

    // Keep the application running to monitor
    while (true) {
        Sleep(1000);  
    }

    return 0;
}
