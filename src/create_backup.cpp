#include "other.hpp"
#include "parsing.hpp"
#include "time.hpp"
#include <filesystem>
#include <fstream>
#include <spdlog/spdlog.h>
#include <string>

namespace fs = std::filesystem;

void saveMetadata(const std::string &targetPath, const std::string &backupListPath, const std::string &ignoreListPath, const std::string &ignoreDatesListPath, bool copyDates) {
    fs::create_directories(targetPath);

    std::ofstream metadataFile(targetPath + "/attributes.txt");
    if (!metadataFile.is_open())
        throw std::runtime_error("Could not open metadata file");

    metadataFile << "v1\n";
    metadataFile << "backups.list\n";
    metadataFile << "ignores.list\n";
    metadataFile << "ignore-dates.list\n";
    metadataFile << (copyDates ? "true" : "false") << '\n';
    metadataFile.close();

    fs::copy_file(backupListPath, targetPath + "/backups.list");
    fs::copy_file(ignoreListPath, targetPath + "/ignores.list");
    fs::copy_file(ignoreDatesListPath, targetPath + "/ignore-dates.list");
}

void recursiveCopy(const std::string &sourcePath, const std::string &destinationPath, const std::vector<std::string> &ignoreList, const std::vector<std::string> &ignoreDatesList, bool copyDates, bool quitOnSoftError, bool dryrun) {
    bool ignoreDates = false, ignore = false;

    for (const auto &entry : fs::directory_iterator(sourcePath)) {
        const std::string currentSourcePath = entry.path().string();

        for (const auto &ignorePattern : ignoreList) {
            if (std::regex_match(currentSourcePath, std::regex(ignorePattern)) || currentSourcePath.starts_with(ignorePattern)) {
                spdlog::info("Ignoring {}", currentSourcePath);
                ignore = true;
                break;
            }
        }

        if (ignore) {
            ignore = false;
            continue;
        }

        for (const auto &ignorePattern : ignoreDatesList) {
            if (std::regex_match(currentSourcePath, std::regex(ignorePattern)) || currentSourcePath.starts_with(ignorePattern)) {
                spdlog::info("Ignoring dates for {}", currentSourcePath);
                ignoreDates = true;
                break;
            }
        }

        if (entry.is_directory()) {
            spdlog::debug("Recursing into {}", currentSourcePath);

            std::string recursionTarget = destinationPath + "/" + entry.path().filename().string();

            if (copyDates && !ignoreDates && !dryrun)
                if (!setSystemTimeFromFileBirthTime(currentSourcePath))
                    spdlog::error("Failed to copy creation time of {} to {}", currentSourcePath, recursionTarget);

            try {
                if (!dryrun)
                    std::filesystem::create_directory(recursionTarget);
            } catch (const std::filesystem::filesystem_error &error) {
                if (error.code().value() == EINVAL) { // invalid path (for example: "hello: world" on ntfs)
                    if (quitOnSoftError)
                        throw error;

                    std::string convertedPath = convertInvalidPath(recursionTarget);
                    spdlog::warn("Invalid path name: '{}' Replacing with '{}'", recursionTarget, convertedPath);

                    std::filesystem::create_directory(convertedPath); // if this still errors, we have a bigger problem
                    recursionTarget = convertedPath;
                    break;
                }

                throw error; // not gonna handle anything else for now
            }

            if (copyDates && !ignoreDates && !dryrun)
                if (!copyTimes(currentSourcePath, recursionTarget))
                    spdlog::error("Failed to copy access and modification timestamps from {} to {}", currentSourcePath, recursionTarget);

            recursiveCopy(currentSourcePath, recursionTarget, ignoreList, ignoreDatesList, copyDates, quitOnSoftError, dryrun);
        } else { // here, it's a file
            std::string finalDestination = destinationPath + (currentSourcePath.c_str() + sourcePath.length());

            spdlog::info("Copying {} to {}", currentSourcePath, finalDestination);

            if (copyDates && !ignoreDates && !dryrun)
                if (!setSystemTimeFromFileBirthTime(currentSourcePath))
                    spdlog::error("Failed to copy creation time from {} to {}", currentSourcePath, finalDestination);

            if (!dryrun) {
                fs::copy_file(currentSourcePath, finalDestination, fs::copy_options::overwrite_existing);
                if (copyDates && !ignoreDates)
                    if (!copyTimes(currentSourcePath, finalDestination))
                        spdlog::error("Failed to copy timestamps from {} to {}", currentSourcePath, finalDestination);
            }
        }
    }
}

void runBackup(const std::string &destinationPath, const std::vector<BackupTarget> &backupList, const std::vector<std::string> &ignoreList, const std::vector<std::string> &ignoreDatesList, bool copyDates, bool quitOnSoftError, bool dryrun) {
    if (!dryrun && !destinationPath.empty())
        fs::create_directories(destinationPath);

    for (const auto &backupTarget : backupList) {
        std::string currentDestinationPath = destinationPath + "/" + backupTarget.destinationPath;
        while (currentDestinationPath.starts_with("//")) // little cleanup
            currentDestinationPath = currentDestinationPath.substr(1);

        spdlog::info("Backing up {} to {}", backupTarget.targetPath, currentDestinationPath);

        if (!dryrun)
            fs::create_directories(currentDestinationPath);
        recursiveCopy(backupTarget.targetPath, currentDestinationPath, ignoreList, ignoreDatesList, copyDates, quitOnSoftError, dryrun);
    }
}
