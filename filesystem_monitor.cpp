#include <windows.h>
#include <AclAPI.h>
#include <Sddl.h>
#include <iostream>
#include <string>

// Function to set directory permissions to prevent new file creation
void SetDirectoryProtection(const std::wstring& dirPath) {
    PSECURITY_DESCRIPTOR pSD = NULL;
    PACL pOldDACL = NULL, pNewDACL = NULL;
    PSID pSid = NULL;
    SID_IDENTIFIER_AUTHORITY SIDAuth = SECURITY_NT_AUTHORITY;
    EXPLICIT_ACCESS_W ea[2];

    if (AllocateAndInitializeSid(&SIDAuth, 1, SECURITY_BUILTIN_DOMAIN_RID, DOMAIN_ALIAS_RID_USERS,
                                0, 0, 0, 0, 0, 0, &pSid)) {

        if (GetNamedSecurityInfoW((LPWSTR)dirPath.c_str(), SE_FILE_OBJECT, DACL_SECURITY_INFORMATION,
                               NULL, NULL, &pOldDACL, NULL, &pSD) == ERROR_SUCCESS) {
            
            // Grant LIST_DIRECTORY and READ access
            ZeroMemory(&ea[0], sizeof(EXPLICIT_ACCESS_W));
            ea[0].grfAccessPermissions = FILE_LIST_DIRECTORY | FILE_READ_ATTRIBUTES | FILE_READ_EA | READ_CONTROL;
            ea[0].grfAccessMode = GRANT_ACCESS;
            ea[0].grfInheritance = NO_INHERITANCE;
            ea[0].Trustee.TrusteeForm = TRUSTEE_IS_SID;
            ea[0].Trustee.TrusteeType = TRUSTEE_IS_USER;
            ea[0].Trustee.ptstrName = (LPWSTR)pSid;

            // Deny file creation and modification
            ZeroMemory(&ea[1], sizeof(EXPLICIT_ACCESS_W));
            ea[1].grfAccessPermissions = FILE_ADD_FILE | FILE_ADD_SUBDIRECTORY | FILE_WRITE_ATTRIBUTES | 
                                       FILE_WRITE_EA | FILE_DELETE_CHILD;
            ea[1].grfAccessMode = DENY_ACCESS;
            ea[1].grfInheritance = NO_INHERITANCE;
            ea[1].Trustee.TrusteeForm = TRUSTEE_IS_SID;
            ea[1].Trustee.TrusteeType = TRUSTEE_IS_USER;
            ea[1].Trustee.ptstrName = (LPWSTR)pSid;

            if (SetEntriesInAclW(2, ea, pOldDACL, &pNewDACL) == ERROR_SUCCESS) {
                DWORD dwRes = SetNamedSecurityInfoW(
                    (LPWSTR)dirPath.c_str(),
                    SE_FILE_OBJECT,
                    DACL_SECURITY_INFORMATION,
                    NULL,
                    NULL,
                    pNewDACL,
                    NULL
                );
                if (dwRes != ERROR_SUCCESS) {
                    std::wcerr << L"Failed to set directory protection. Error: " << dwRes << std::endl;
                }
            }
        }
    }

    if (pSid) FreeSid(pSid);
    if (pSD) LocalFree(pSD);
    if (pNewDACL) LocalFree(pNewDACL);
}

// Function to set file as read-only
void SetReadOnlyPermissions(const std::wstring& filePath) {
    PSECURITY_DESCRIPTOR pSD = NULL;
    PACL pOldDACL = NULL, pNewDACL = NULL;
    PSID pSid = NULL;
    SID_IDENTIFIER_AUTHORITY SIDAuth = SECURITY_NT_AUTHORITY;
    EXPLICIT_ACCESS_W ea[2];

    if (AllocateAndInitializeSid(&SIDAuth, 1, SECURITY_BUILTIN_DOMAIN_RID, DOMAIN_ALIAS_RID_USERS,
                                0, 0, 0, 0, 0, 0, &pSid)) {

        if (GetNamedSecurityInfoW((LPWSTR)filePath.c_str(), SE_FILE_OBJECT, DACL_SECURITY_INFORMATION,
                               NULL, NULL, &pOldDACL, NULL, &pSD) == ERROR_SUCCESS) {
            
            // Grant READ access
            ZeroMemory(&ea[0], sizeof(EXPLICIT_ACCESS_W));
            ea[0].grfAccessPermissions = GENERIC_READ;
            ea[0].grfAccessMode = GRANT_ACCESS;
            ea[0].grfInheritance = NO_INHERITANCE;
            ea[0].Trustee.TrusteeForm = TRUSTEE_IS_SID;
            ea[0].Trustee.TrusteeType = TRUSTEE_IS_USER;
            ea[0].Trustee.ptstrName = (LPWSTR)pSid;

            // Deny WRITE and DELETE
            ZeroMemory(&ea[1], sizeof(EXPLICIT_ACCESS_W));
            ea[1].grfAccessPermissions = GENERIC_WRITE | DELETE;
            ea[1].grfAccessMode = DENY_ACCESS;
            ea[1].grfInheritance = NO_INHERITANCE;
            ea[1].Trustee.TrusteeForm = TRUSTEE_IS_SID;
            ea[1].Trustee.TrusteeType = TRUSTEE_IS_USER;
            ea[1].Trustee.ptstrName = (LPWSTR)pSid;

            if (SetEntriesInAclW(2, ea, pOldDACL, &pNewDACL) == ERROR_SUCCESS) {
                DWORD dwRes = SetNamedSecurityInfoW(
                    (LPWSTR)filePath.c_str(),
                    SE_FILE_OBJECT,
                    DACL_SECURITY_INFORMATION,
                    NULL,
                    NULL,
                    pNewDACL,
                    NULL
                );
                if (dwRes != ERROR_SUCCESS) {
                    std::wcerr << L"Failed to set file protection. Error: " << dwRes << std::endl;
                }
            }
        }
    }

    if (pSid) FreeSid(pSid);
    if (pSD) LocalFree(pSD);
    if (pNewDACL) LocalFree(pNewDACL);
}

class FileSystemMonitor {
public:
    void start(const std::string& root_dir);
};

void FileSystemMonitor::start(const std::string& root_dir) {
    std::wstring w_root_dir(root_dir.begin(), root_dir.end());

    // First, protect the directory itself
    SetDirectoryProtection(w_root_dir);

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
                        // Even if a file somehow gets created, make it read-only immediately
                        SetReadOnlyPermissions(fullPath);
                        break;
                    case FILE_ACTION_REMOVED:
                        std::wcout << L"Removed (Blocked)";
                        break;
                    case FILE_ACTION_MODIFIED:
                        std::wcout << L"Modified (Blocked)";
                        SetReadOnlyPermissions(fullPath);
                        break;
                    case FILE_ACTION_RENAMED_OLD_NAME:
                        std::wcout << L"Renamed from (Blocked)";
                        break;
                    case FILE_ACTION_RENAMED_NEW_NAME:
                        std::wcout << L"Renamed to (Blocked)";
                        SetReadOnlyPermissions(fullPath);
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