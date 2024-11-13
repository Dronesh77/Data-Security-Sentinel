#include "clipboard_monitor.h"
#include <windows.h>
#include <iostream>
#include <string>
#include <shlobj.h>  // For CF_HDROP

using namespace std;

void ClipboardMonitor::start(const string& root_dir) {
    cout << "Monitoring clipboard for root folder: " << root_dir << endl;

    while (true) {
        if (OpenClipboard(nullptr)) {
            // Check if clipboard contains file paths (CF_HDROP format)
            HANDLE hClipboardData = GetClipboardData(CF_HDROP);
            if (hClipboardData) {
                HDROP hDrop = static_cast<HDROP>(hClipboardData);

                // Get the number of files in the clipboard
                UINT fileCount = DragQueryFile(hDrop, 0xFFFFFFFF, nullptr, 0);

                // Check each file path
                for (UINT i = 0; i < fileCount; ++i) {
                    char filePath[MAX_PATH];
                    DragQueryFileA(hDrop, i, filePath, MAX_PATH);

                    // Block if file path is within root_dir
                    string content(filePath);
                    if (content.find(root_dir) != string::npos) {
                        cout << "Copy-paste attempt blocked for file from root directory." << endl;
                        EmptyClipboard();  // Clear clipboard if content is from the root directory
                        break;
                    }
                }
                DragFinish(hDrop);
            }

            // Check for text content (CF_TEXT or CF_UNICODETEXT)
            hClipboardData = GetClipboardData(CF_TEXT);
            if (hClipboardData) {
                char* clipboardText = static_cast<char*>(GlobalLock(hClipboardData));
                if (clipboardText) {
                    string content(clipboardText);
                    if (content.find(root_dir) != string::npos) {
                        cout << "Copy-paste attempt blocked for text content from root directory." << endl;
                        EmptyClipboard();
                    }
                    GlobalUnlock(hClipboardData);
                }
            }

            // Unicode text content check (CF_UNICODETEXT)
            hClipboardData = GetClipboardData(CF_UNICODETEXT);
            if (hClipboardData) {
                wchar_t* clipboardText = static_cast<wchar_t*>(GlobalLock(hClipboardData));
                if (clipboardText) {
                    wstring content(clipboardText);
                    if (content.find(wstring(root_dir.begin(), root_dir.end())) != wstring::npos) {
                        cout << "Copy-paste attempt blocked for Unicode text content from root directory." << endl;
                        EmptyClipboard();
                    }
                    GlobalUnlock(hClipboardData);
                }
            }

            CloseClipboard();
        }
        Sleep(100);  
    }
}
