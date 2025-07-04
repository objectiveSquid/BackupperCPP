#pragma once

#include "parsing.hpp"
#include <string>
#include <vector>

void prepareBackupListForRestore(const std::string &backupFilesPath, std::vector<BackupTarget> &backupList);
std::string convertInvalidPath(const std::string &invalidPath);
