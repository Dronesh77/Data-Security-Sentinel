#include "clipboard_monitor.h"
#include <windows.h>
#include <iostream>
#include <string>
#include <shlobj.h> // For CF_HDROP

using namespace std;

void ClipboardMonitor::start(const string &root_dir)
{
    cout << "Monitoring clipboard for root folder: " << root_dir << endl;

    while (true)
    {
        if (OpenClipboard(nullptr))
        {
            bool clearClipboard = false;

            // Check if clipboard contains file paths (CF_HDROP format)
            HANDLE hClipboardData = GetClipboardData(CF_HDROP);
            if (hClipboardData)
            {
                HDROP hDrop = static_cast<HDROP>(hClipboardData);

                UINT fileCount = DragQueryFile(hDrop, 0xFFFFFFFF, nullptr, 0);

                for (UINT i = 0; i < fileCount; ++i)
                {
                    char filePath[MAX_PATH];
                    DragQueryFileA(hDrop, i, filePath, MAX_PATH);

                    string content(filePath);
                    if (content.find(root_dir) != string::npos)
                    {
                        cout << "Copy-paste attempt blocked for file from root directory: " << content << endl;
                        clearClipboard = true;
                        break;
                    }
                }

                if (clearClipboard)
                {
                    EmptyClipboard();
                    cout << "Clipboard cleared due to file from root directory." << endl;
                }

                DragFinish(hDrop);
            }

            CloseClipboard();
        }

        Sleep(100);
    }
}




























