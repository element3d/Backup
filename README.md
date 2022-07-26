# Backup
#### A simple c++ application to generate a dummy sqlite3 database, dump and send dumped file to the ftp server.
#### The project uses cmdparser library for argument parsing and sqlite3 library for manipulating sql database which are placed under the libs directory.
#### For further improvements the project is separated to modules. Currently the only module SQLite3Cpp is placed under the modules directory and is a simple OOP wrapper aroudn sqlite3 c api.

### Build
#### The application supports only Windows platform. Cmake generated Visual Studio solution is placed under the vs directory.

### Run
#### Backup application has 5 required arguments:
- -d --db       | The database name to be generated, filled, dumped and uploaded
- -hh --host    | The ftp server host
- -pp --port    | The ftp server port
- -u --username | The ftp server username
- -p --password | The ftp server password

```text
$ Backup.exe -d ucl.db -hh 127.0.0.1 -pp 21 -u narek -p narek
```
