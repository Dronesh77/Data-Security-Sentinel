#include "filesystem_monitor.h"
#include <windows.h>
#include <iostream>
#include <string>

using namespace std;

void FileSystemMonitor::start(const string& root_dir) {
    wstring w_root_dir(root_dir.begin(), root_dir.end());

   
HANDLE hDir = CreateFileW(
    w_root_dir.c_str(),
    FILE_LIST_DIRECTORY,
    FILE_SHARE_READ | FILE_SHARE_WRITE | FILE_SHARE_DELETE,
    NULL,
    OPEN_EXISTING,
    FILE_FLAG_BACKUP_SEMANTICS,
    NULL
);


    if (hDir == INVALID_HANDLE_VALUE) {
        cerr << "Failed to open directory for monitoring. Error: " << GetLastError() << endl;
        return;
    }

    char buffer[1024];
    DWORD bytesReturned;
    FILE_NOTIFY_INFORMATION* pNotify;

    // Continuous monitoring loop
    while (true) {
        // Watch for specific changes in the directory
        if (ReadDirectoryChangesW(
                hDir,
                buffer,
                sizeof(buffer),
                TRUE,  // TRUE to monitor subdirectories as well
                FILE_NOTIFY_CHANGE_FILE_NAME | FILE_NOTIFY_CHANGE_DIR_NAME |
                FILE_NOTIFY_CHANGE_ATTRIBUTES | FILE_NOTIFY_CHANGE_SIZE |
                FILE_NOTIFY_CHANGE_LAST_WRITE | FILE_NOTIFY_CHANGE_CREATION,
                &bytesReturned,
                NULL,
                NULL)) {

            // Parse the notification information
            pNotify = (FILE_NOTIFY_INFORMATION*)buffer;
            do {
                // Extract the filename and action type
                wstring fileName(pNotify->FileName, pNotify->FileNameLength / sizeof(WCHAR));
                wcout << L"File change detected: " << fileName << L" - ";

                switch (pNotify->Action) {
                    case FILE_ACTION_ADDED:
                        wcout << L"Added";
                        break;
                    case FILE_ACTION_REMOVED:
                        wcout << L"Removed";
                        break;
                    case FILE_ACTION_MODIFIED:
                        wcout << L"Modified";
                        break;
                    case FILE_ACTION_RENAMED_OLD_NAME:
                        wcout << L"Renamed from";
                        break;
                    case FILE_ACTION_RENAMED_NEW_NAME:
                        wcout << L"Renamed to";
                        break;
                    default:
                        wcout << L"Unknown action";
                        break;
                }
                wcout << endl;

                // Move to the next entry in the buffer, if any
                pNotify = pNotify->NextEntryOffset
                    ? (FILE_NOTIFY_INFORMATION*)((char*)pNotify + pNotify->NextEntryOffset)
                    : NULL;
            } while (pNotify);
        } else {
            cerr << "ReadDirectoryChangesW failed. Error: " << GetLastError() << endl;
            break;
        }
    }

    // Close the directory handle
    CloseHandle(hDir);
}
