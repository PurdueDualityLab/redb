
# redb - Regex Reuse Database

## About
The goal of this tool is to make regular expressions easier to reuse by allowing developers to
find an existing regular expression that fits their needs rather than writing one from scratch.

## Project Structure
This project has three main components: a C++ library that provides tools for setting up and
querying a regular expression database (librereuse/), an API for querying a regular expression
database (server/), and a frontend that provides a web interface for querying the database
(frontend/). There are also some unit tests (test/) and some experiments
(clustering_experiment/).

## Building
Building requires the following:
- A C++ Compiler that supports C++17 (g++ >= 6.x/7.x or clang++ >= 4)
- CMake
- git

Steps to build are:
```shell
$ git clone --recurse-submodules https://github.com/PurdueDualityLab/redb.git  # Note: the --recurse-submodules flag is very important
$ cd redb 
$ mkdir build && cd build
$ cmake ..  # Or, to skip tests and experiments, cmake .. -DSKIP_TESTS=ON -DSKIP_EXPERIMENTS=ON
$ make
```
I would not recommend running `make install` as I have not really implemented it yet.

## Running a server locally
To start running a server locally, you first need a file that contains seeds for the database.
The seed file is a json file with the following format:
```json
[
  [ // Patterns in cluster 1
    "[asdf]*",
    "[a-z]+"
  ],
  [ // patterns in cluster 2
    ...
  ],
  ... // rest of clusters
]
```

With this file, you can start the server. You can start the server two ways: set the
environment variable `CLUSTER_DB_PATH` to the absolute path to your seed file, or pass the path
in as the command line argument `-f` to the server. These two ways would like:
```shell
$ cd redb/build
$ ./redb-server -f /path/to/seed/file
# or
$ export CLUSTER_DB_PATH=/path/to/seed/file
$ ./redb-server
```
