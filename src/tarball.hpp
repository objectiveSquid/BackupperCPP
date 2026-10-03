#pragma once

#include <string>

int extractTarball(const std::string &tarballPath, const std::string &destinationPath, bool verbose);
int createTarball(const std::string &sourcePath, const std::string &tarballPath, bool useXz, bool verbose);
