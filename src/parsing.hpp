#pragma once

#include <optional>
#include <regex>
#include <string>
#include <vector>

typedef struct {
    std::string targetPath;
    std::string destinationPath;
} BackupTarget;

std::optional<std::vector<BackupTarget>> parseBackupList(const std::string &filepath);
std::optional<std::vector<std::string>> parseIgnoreList(const std::string &filepath);
