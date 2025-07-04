#include "parsing.hpp"
#include <fstream>
#include <optional>
#include <regex>
#include <vector>

std::optional<std::vector<BackupTarget>> parseBackupList(const std::string &filepath) {
    std::ifstream backupListFile(filepath);
    if (!backupListFile.is_open())
        return std::nullopt;

    std::vector<BackupTarget> backupList;
    std::string line;
    while (std::getline(backupListFile, line)) {
        size_t delimiterPosition = line.find_first_of(':');
        if (delimiterPosition == std::string::npos)
            continue; // no ':' found

        backupList.emplace_back(
            line.substr(0, delimiterPosition),
            line.substr(delimiterPosition + 1, line.length() - 1));
    }

    if (backupList.empty())
        return std::nullopt;

    return backupList;
}

std::optional<std::vector<std::string>> parseIgnoreList(const std::string &filepath) {
    if (filepath.empty()) {
        std::vector<std::string> ignoreList;
        return ignoreList;
    }

    std::ifstream backupListFile(filepath);
    if (!backupListFile.is_open())
        return std::nullopt;

    std::vector<std::string> ignoreList;
    std::string line;

    while (std::getline(backupListFile, line)) {
        if (line.empty())
            continue;

        ignoreList.push_back(line);
    }

    if (ignoreList.empty())
        return std::nullopt;

    return ignoreList;
}
