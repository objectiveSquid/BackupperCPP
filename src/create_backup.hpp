#pragma once

#include "parsing.hpp"
#include <regex>
#include <string>
#include <vector>

void saveMetadata(const std::string &targetPath, const std::string &backupListPath, const std::string &ignoreListPath, const std::string &ignoreDatesListPath, bool copyDates);
void runBackup(const std::string &destinationPath, const std::vector<BackupTarget> &backupList, const std::vector<std::string> &ignoreList, const std::vector<std::string> &ignoreDatesList, bool copyDates, bool quitOnSoftError, bool dryrun);
