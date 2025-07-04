#include "restore.hpp"
#include <fstream>

void loadMetadata(const std::string &backupDirectoryMetaPath, std::string &backupListPath, std::string &ignoreListPath, std::string &ignoreDatesListPath, bool &copyDates) {
    std::ifstream metadataFile(backupDirectoryMetaPath + "/attributes.txt");
    if (!metadataFile.is_open())
        throw std::runtime_error("Could not open metadata file");

    std::string line;
    std::getline(metadataFile, line);
    if (line != "v1")
        throw std::runtime_error("Invalid metadata version");

    std::getline(metadataFile, line);
    backupListPath = backupDirectoryMetaPath + '/' + line;

    std::getline(metadataFile, line);
    ignoreListPath = backupDirectoryMetaPath + '/' + line;

    std::getline(metadataFile, line);
    ignoreDatesListPath = backupDirectoryMetaPath + '/' + line;

    std::getline(metadataFile, line);
    copyDates &= (line == "true");

    metadataFile.close();
}
