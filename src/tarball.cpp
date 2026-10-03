#include "tarball.hpp"
#include <cstdlib>
#include <spdlog/spdlog.h>

int extractTarball(const std::string &tarballPath, const std::string &destinationPath, bool verbose) {
    std::string command = "tar -x ";

    if (verbose)
        command += 'v';

    command += "f \"" + tarballPath + "\" -C \"" + destinationPath + '"';

    spdlog::debug("Executing tar command: {}", command);

    return system(command.c_str());
}

int createTarball(const std::string &sourcePath, const std::string &tarballPath, bool useXz, bool verbose) {
    std::string command = "tar -c";

    if (useXz)
        command += 'J';
    if (verbose)
        command += 'v';

    command += "f \"" + tarballPath + "\" -C \"" + sourcePath + "\" .";

    spdlog::debug("Executing tar command: {}", command);

    return system(command.c_str());
}