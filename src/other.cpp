#include "other.hpp"
#include "parsing.hpp"

void prepareBackupListForRestore(const std::string &backupFilesPath, std::vector<BackupTarget> &backupList) {
    for (auto &backupTarget : backupList) {
        std::swap(backupTarget.targetPath, backupTarget.destinationPath);
        backupTarget.targetPath = backupFilesPath + '/' + backupTarget.targetPath;
    }
}

std::string convertInvalidPath(const std::string &invalidPath) {
    std::string validPath = invalidPath;

    std::remove(validPath.begin(), validPath.end(), '<');
    std::remove(validPath.begin(), validPath.end(), '>');
    std::remove(validPath.begin(), validPath.end(), ':');
    std::remove(validPath.begin(), validPath.end(), '"');
    std::remove(validPath.begin(), validPath.end(), '|');
    std::remove(validPath.begin(), validPath.end(), '?');
    std::remove(validPath.begin(), validPath.end(), '*');

    if (validPath.at(validPath.length() - 1) == '.')
        validPath = validPath.substr(0, validPath.length() - 1);

    return validPath;
}
