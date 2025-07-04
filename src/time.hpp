#pragma once

#include <string>

bool hasTimePermissions();
bool setSystemTime(const clock_t systemTimeMillis);
clock_t getSystemTimeMilliseconds();
clock_t getCpuTimeMilliseconds();

bool setSystemTimeFromFileBirthTime(const std::string &path);
bool copyTimes(const std::string &originalPath, const std::string &targetPath);
