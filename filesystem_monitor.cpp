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
        wcerr << L"Failed to open directory for monitoring. Error: " << GetLastError() << endl;
        return;
    }

    BYTE buffer[4096];
    DWORD bytesReturned;
    FILE_NOTIFY_INFORMATION* pNotify;

    while (true) {
        if (ReadDirectoryChangesW(
                hDir,
                buffer,
                sizeof(buffer),
                TRUE,
                FILE_NOTIFY_CHANGE_FILE_NAME | FILE_NOTIFY_CHANGE_CREATION,
                &bytesReturned,
                NULL,
                NULL)) {

            pNotify = (FILE_NOTIFY_INFORMATION*)buffer;
            do {
                wstring fileName(pNotify->FileName, pNotify->FileNameLength / sizeof(WCHAR));
                wcout << L"File change detected: " << fileName << endl;

                switch (pNotify->Action) {
                    case FILE_ACTION_ADDED:
                        wcout << L"Creation attempt detected and blocked." << endl;
                        break;
                     case FILE_ACTION_REMOVED:
                        std::wcout << L"Removed (Blocked)";
                        break;
                    case FILE_ACTION_MODIFIED:
                        std::wcout << L"Modified (Blocked)";
                        break;
                    case FILE_ACTION_RENAMED_OLD_NAME:
                        std::wcout << L"Renamed from (Blocked)";
                        break;
                    case FILE_ACTION_RENAMED_NEW_NAME:
                        std::wcout << L"Renamed to (Blocked)";
                        break;
                    default:
                        std::wcout << L"Unknown action";
                        break;
                }

                pNotify = pNotify->NextEntryOffset
                    ? (FILE_NOTIFY_INFORMATION*)((BYTE*)pNotify + pNotify->NextEntryOffset)
                    : NULL;
            } while (pNotify);
        } else {
            wcerr << L"ReadDirectoryChangesW failed. Error: " << GetLastError() << endl;
            break;
        }
    }

    CloseHandle(hDir);
}
