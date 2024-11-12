#include "clipboard_monitor.h"
#include <windows.h>
#include <iostream>

using namespace std;

void ClipboardMonitor::start(const string& root_dir) {
    cout << "Monitoring clipboard for root folder: " << root_dir << endl;

    // Open the clipboard
    if (OpenClipboard(nullptr)) {
        HANDLE hClipboardData = GetClipboardData(CF_TEXT);
        if (hClipboardData) {
            char* clipboardText = static_cast<char*>(GlobalLock(hClipboardData));
            if (clipboardText) {
                // Check if clipboard content path is within root_dir
                string content(clipboardText);
                if (content.find(root_dir) != string::npos) {
                    cout << "Copy-paste attempt blocked from root directory." << endl;
                    EmptyClipboard();  // Clear clipboard if content is from the root directory
                }
                GlobalUnlock(hClipboardData);
            }
        }
        CloseClipboard();
    }
}
