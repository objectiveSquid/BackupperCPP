#include "tarball.hpp"
#include <cstdlib>

int extractTarball(const std::string &tarballPath, const std::string &destinationPath) {
    std::string command = "tar -xf " + tarballPath + " -C " + destinationPath;

    return system(command.c_str());
}

int compressTarball(const std::string &sourcePath, const std::string &tarballPath) {
    std::string command = "tar -cJf " + tarballPath + " -C " + sourcePath + " .";

    return system(command.c_str());
}