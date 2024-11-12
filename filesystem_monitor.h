#ifndef FILESYSTEM_MONITOR_H
#define FILESYSTEM_MONITOR_H

#include <string>

class FileSystemMonitor {
public:
    static void start(const std::string& root_dir);
};

#endif // FILESYSTEM_MONITOR_H
