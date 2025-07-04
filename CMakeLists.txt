cmake_minimum_required(VERSION 3.22.1)

# project configuration
project(BackupperCPP VERSION 1.0)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED True)

# executable
add_executable(BackupperCPP
    src/main.cpp
    src/parsing.cpp
    src/create_backup.cpp
    src/restore.cpp
    src/time.cpp
    src/other.cpp
)
target_compile_options(BackupperCPP PRIVATE -Wall -O2)

# spdlog
include(FetchContent)
FetchContent_Declare(
    spdlog
    GIT_REPOSITORY https://github.com/gabime/spdlog.git
    GIT_TAG v1.15.3         # known good release
)
FetchContent_MakeAvailable(spdlog)
target_link_libraries(BackupperCPP PRIVATE spdlog::spdlog)

# spdlog uses fmt
include(FetchContent)
FetchContent_Declare(
    fmt
    GIT_REPOSITORY https://github.com/fmtlib/fmt.git
    GIT_TAG 11.2.0          # known good release
)
FetchContent_MakeAvailable(fmt)
target_link_libraries(BackupperCPP PRIVATE fmt::fmt)

# argparse
include(FetchContent)
FetchContent_Declare(
    argparse
    GIT_REPOSITORY https://github.com/p-ranav/argparse.git
    GIT_TAG v3.2            # known good release
)
FetchContent_MakeAvailable(argparse)
target_link_libraries(BackupperCPP PRIVATE argparse)
