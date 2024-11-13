#include <windows.h>
#include <AclAPI.h>
#include <Sddl.h>
#include <iostream>
#include <string>

class FileSystemMonitor {
public:
    void start(const std::string& root_dir);
};

void FileSystemMonitor::start(const std::string& root_dir) {
    std::wstring w_root_dir(root_dir.begin(), root_dir.end());

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
        std::wcerr << L"Failed to open directory for monitoring. Error: " << GetLastError() << std::endl;
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
                FILE_NOTIFY_CHANGE_FILE_NAME | FILE_NOTIFY_CHANGE_DIR_NAME |
                FILE_NOTIFY_CHANGE_ATTRIBUTES | FILE_NOTIFY_CHANGE_SIZE |
                FILE_NOTIFY_CHANGE_LAST_WRITE | FILE_NOTIFY_CHANGE_CREATION,
                &bytesReturned,
                NULL,
                NULL)) {

            pNotify = (FILE_NOTIFY_INFORMATION*)buffer;
            do {
                std::wstring fileName(pNotify->FileName, pNotify->FileNameLength / sizeof(WCHAR));
                std::wstring fullPath = w_root_dir + L"\\" + fileName;
                
                std::wcout << L"File change detected: " << fileName << L" - ";

                switch (pNotify->Action) {
                    case FILE_ACTION_ADDED:
                        std::wcout << L"Added (Blocked)";
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
                std::wcout << std::endl;

                pNotify = pNotify->NextEntryOffset
                    ? (FILE_NOTIFY_INFORMATION*)((BYTE*)pNotify + pNotify->NextEntryOffset)
                    : NULL;
            } while (pNotify);
        } else {
            std::wcerr << L"ReadDirectoryChangesW failed. Error: " << GetLastError() << std::endl;
            break;
        }
    }

    CloseHandle(hDir);
}