#include "create_backup.hpp"
#include "other.hpp"
#include "parsing.hpp"
#include "restore.hpp"
#include "time.hpp"
#include <argparse/argparse.hpp>
#include <iostream>
#include <spdlog/spdlog.h>
#include <string>

int main(int argc, char **argv) {
    argparse::ArgumentParser program("BackupperCPP");

    // required arguments
    program.add_argument("backup-directory")
        .help("The backup directory");

    // semi-optional arguments
    program.add_argument("--backup-list")
        .help("Path to the file describing what to backup")
        .default_value("");
    program.add_argument("--ignore-list")
        .help("Path to the file describing what paths to ignore")
        .default_value("");
    program.add_argument("--ignore-dates-list")
        .help("Path to the file describing what paths to not copy dates from")
        .default_value("");

    // fully optional arguments
    auto &logGroup = program.add_mutually_exclusive_group();
    logGroup.add_argument("--quiet", "-q").flag().help("Only display warnings and errors");
    logGroup.add_argument("--verbose", "-v").flag().help("Display debug logs");

    program.add_argument("--dryrun").flag().help("Don't actually copy anything");
    program.add_argument("--dont-copy-dates").flag().help("Dont copy file dates");
    program.add_argument("--quit-on-soft-error").flag().help("Quit the program on soft errors, such as paths being converted");
    program.add_argument("--restore").flag().help("Restore an existing backup");

    // parse args
    try {
        program.parse_args(argc, argv);
    } catch (const std::exception &error) {
        spdlog::error(error.what());
        return 1;
    }

    // get args
    std::string backupDirectoryPath = program.get<std::string>("backup-directory");
    std::string backupListPath = program.get<std::string>("--backup-list");
    std::string ignoreListPath = program.get<std::string>("--ignore-list");
    std::string ignoreDatesListPath = program.get<std::string>("--ignore-dates-list");
    bool dryrun = program.is_used("--dryrun");
    bool restoring = program.is_used("--restore");
    bool quiet = program.is_used("--quiet") || program.is_used("-q");
    bool verbose = program.is_used("--verbose") || program.is_used("-v");
    bool copyDates = !program.is_used("--dont-copy-dates");
    bool quitOnSoftError = program.is_used("--quit-on-soft-error");

    // extra arg checks
    if (restoring) {
        if (program.is_used("--backup-list") || program.is_used("--ignore-list") || program.is_used("--ignore-dates-list")) {
            spdlog::critical("When using --restore, you cannot use --backup-list, --ignore-list, or --ignore-dates-list");
            return 1;
        }

        loadMetadata(backupDirectoryPath + "/meta", backupListPath, ignoreListPath, ignoreDatesListPath, copyDates);
    } else {
        if (!program.is_used("--backup-list")) {
            spdlog::critical("You must supply a backup list with --backup-list");
            return 1;
        }
    }

    // setup logging
    if (verbose)
        spdlog::set_level(spdlog::level::debug);
    else if (quiet)
        spdlog::set_level(spdlog::level::warn);
    else
        spdlog::set_level(spdlog::level::info);

    // parse input files
    auto backupList = parseBackupList(backupListPath);
    if (!backupList.has_value()) {
        spdlog::critical("Failed to parse backup list");
        return 1;
    }

    auto ignoreList = parseIgnoreList(ignoreListPath);
    if (!ignoreList.has_value()) {
        spdlog::critical("Failed to parse ignore list");
        return 1;
    }

    auto ignoreDatesList = parseIgnoreList(ignoreDatesListPath);
    if (!ignoreDatesList.has_value()) {
        spdlog::critical("Failed to parse ignore dates list");
        return 1;
    }

    // if restoring, we need to handle some stuff
    if (restoring) {
        prepareBackupListForRestore(backupDirectoryPath + "/files", backupList.value());
        ignoreList.value().clear(); // not used when restoring
    }

    // permissions check, we cannot set creation time if we dont have root permissions
    if (copyDates && !hasTimePermissions()) {
        spdlog::warn("Need root permissions to backup creation dates of files.");
        if (quitOnSoftError)
            return 1;
    }

    // remove time from logging setup, it is constantly modified when backing up
    spdlog::set_pattern("[%^%l%$] %v");

    // save system time. this is because we are settings creation date on the new files by changing the system time
    clock_t systemTimeBeforeBackup = 0, cpuTimeBeforeBackup = 0;
    if (copyDates) {
        systemTimeBeforeBackup = getSystemTimeMilliseconds();
        cpuTimeBeforeBackup = getCpuTimeMilliseconds();
    }

    // run backup
    spdlog::info("Starting {}", restoring ? "restore" : "backup");

    if (!restoring && !dryrun)
        saveMetadata(backupDirectoryPath + "/meta", backupListPath, ignoreListPath, ignoreDatesListPath, copyDates);

    if (restoring) // when restoring paths are already absolute
        runBackup("", backupList.value(), ignoreList.value(), ignoreDatesList.value(), copyDates, quitOnSoftError, dryrun);
    else
        runBackup(backupDirectoryPath + "/files", backupList.value(), ignoreList.value(), ignoreDatesList.value(), copyDates, quitOnSoftError, dryrun);

    // restore system time
    if (copyDates) {
        clock_t cpuTimeAfterBackup = getCpuTimeMilliseconds();
        // we dont care about (permission) errors, because if we get one, the time was never changed to begin with
        setSystemTime(systemTimeBeforeBackup + (cpuTimeAfterBackup - cpuTimeBeforeBackup));
    }

    spdlog::info("{} finished", restoring ? "Restore" : "Backup");

    return 0;
}
