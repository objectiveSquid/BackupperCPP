#include "tarball.hpp"
#include <cstdlib>
#include <spdlog/spdlog.h>

int extractTarball(const std::string &tarballPath, const std::string &destinationPath) {
    std::string command = "tar -xf ";
    command += '"' + tarballPath + '"' + " -C " + '"' + destinationPath + '"';

    spdlog::debug("Executing tar command: {}", command);

    return system(command.c_str());
}

int compressTarball(const std::string &sourcePath, const std::string &tarballPath) {
    std::string command = "tar -cJf ";
    command += '"' + tarballPath + '"' + " -C " + '"' + sourcePath + '"' + " .";

    spdlog::debug("Executing tar command: {}", command);

    return system(command.c_str());
}