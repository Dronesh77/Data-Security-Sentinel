#ifndef CLIPBOARD_MONITOR_H
#define CLIPBOARD_MONITOR_H

#include <string>
using namespace std;
class ClipboardMonitor {
public:
    static void start(const string& root_dir);
};

#endif // CLIPBOARD_MONITOR_H
