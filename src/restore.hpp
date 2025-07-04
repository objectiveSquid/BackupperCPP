#pragma once

#include <string>

void loadMetadata(const std::string &backupDirectoryPath, std::string &backupListPath, std::string &ignoreListPath, std::string &ignoreDatesListPath, bool &copyDates);
