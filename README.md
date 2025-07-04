# Backupper CPP
Simple backup program written in C++.<br>
I made this because no other solutions existed for my problem (*cliché*), which was that I wanted to back up specific folders with the ability to ignore some things based on regex expressions. And I also wanted dates to be copied as well.

## Features
- Create and restore backups (of course)
- Ignore files based on regex
- Copy file dates (creation time (*if supported by the underlying driver*), modification time and access time)
- Not copy dates of files based on regex
- In my humble opinion, quite easy to use.

## Building
You will need `cmake`, `make` and `g++` for building:
```bash
sudo apt-get install cmake make g++ -y
```

Then you can build the program:
```bash
mkdir build
cd build
cmake ..
make
```

## Example uses
### DISCLAIMER: If you want to copy file creation dates, you will need root permissions!
### Creating a backup
To create a simple backup, you can use the following command:
```bash
./BackupperCPP /home/user/Backups/backup2025 --backup-list backups.list
```
You can use the `--ignore-list` parameter to ignore some files.
```bash
./BackupperCPP /home/user/Backups/backup2025 --backup-list backups.list --ignore-list ignores.list
```
You can use the `--ignore-dates-list` parameter to ignore the dates of some files.
```bash
./BackupperCPP /home/user/Backups/backup2025 --backup-list backups.list --ignore-dates-list ignore-dates.list
```
### Restoring a backup
To restore a backup, you can simply run the following command:
```bash
./BackupperCPP /home/user/Backups/backup2025
```

## Formats
### Backup lists
A backup list could look like this, with the colon seperating the source path and the destination in the backup:
```
/home/user/Downloads:Downloads
/home/user/Scripts:Scripts
/etc:etc
/lib:libraries
```

### Ignore lists (and dates ignore list)
An ignore list is a list of regex expressions (or paths), which if matched with a file path, the path is not backed up.<br>
They should look like this:
```
.*__pycache__.*
/etc/unimportant_directory
/home/user/Scripts/unimportant_script.sh
```
