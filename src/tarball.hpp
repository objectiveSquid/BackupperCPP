#pragma once

#include <string>

int extractTarball(const std::string &tarballPath, const std::string &destinationPath);
int compressTarball(const std::string &sourcePath, const std::string &tarballPath);
