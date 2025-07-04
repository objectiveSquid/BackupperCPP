#include "time.hpp"
#include <errno.h>
#include <fcntl.h>
#include <string>
#include <sys/stat.h>
#include <sys/types.h>
#include <time.h>

bool hasTimePermissions() {
    auto systemTime = getSystemTimeMilliseconds();

    return setSystemTime(systemTime);
}

bool setSystemTime(const clock_t systemTimeMillis) {
    struct timespec time = {systemTimeMillis / 1000, (systemTimeMillis % 1000) * 1000000};

    return clock_settime(CLOCK_REALTIME, &time) == 0;
}

clock_t getSystemTimeMilliseconds() {
    struct timespec time;

    if (clock_gettime(CLOCK_REALTIME, &time) == -1)
        return -1;

    return time.tv_sec * 1000 + time.tv_nsec / 1000000;
}

clock_t getCpuTimeMilliseconds() {
    return clock() / CLOCKS_PER_SEC * 1000;
}

// this is the only way to affect creating time of files in linux, im sorry...
bool setSystemTimeFromFileBirthTime(const std::string &path) {
    // get creation time
    struct statx statsExtended;
    if (statx(AT_FDCWD, path.c_str(), 0, STATX_BTIME, &statsExtended) == -1)
        return false;

    struct timespec fileBirthTime = {statsExtended.stx_btime.tv_sec, statsExtended.stx_btime.tv_nsec};

    // set the time
    if (clock_settime(CLOCK_REALTIME, &fileBirthTime) == -1) {
        if (errno == EACCES || errno == EPERM)
            return true; // dont tell the user about permission errors
        return false;
    }

    return true;
}

bool copyTimes(const std::string &originalPath, const std::string &targetPath) {
    // get file info
    struct stat attributes;
    if (stat(originalPath.c_str(), &attributes) == -1)
        return false;

    struct timespec times[2];

    // access time
    times[0].tv_sec = attributes.st_atim.tv_sec;
    times[0].tv_nsec = attributes.st_atim.tv_nsec;

    // modify time
    times[1].tv_sec = attributes.st_mtim.tv_sec;
    times[1].tv_nsec = attributes.st_mtim.tv_nsec;

    // set time to new file
    return utimensat(AT_FDCWD, targetPath.c_str(), times, 0) == 0;
}
