# CDT316 Distributed Systems Project
This is a solution to the group assignment for Mälardalen University's Distributed Systems course.
Developed by Mitchell Hentges and Stefan Danielsson

## Using Code::Blocks

### Setting up

1. Open a terminal in this `project` folder
2. `mkdir build`
3. `cd build`
4. `cmake -G “CodeBlocks – Unix Makefiles” ..`
5. Open Code::Blocks
6. File>Open, choose `project/build/distributed-db.cbp`
7. Project>Properties>Build targets, change “Type” to “Console application” and “Output filename” to src/distributed-db

## Using the command line

### Setting up

1. Open a terminal in this `project` folder
2. `mkdir build`
3. `cd build`
4. `cmake ..`

### Building and running

1. Open a terminal in the `project/build` folder
2. `make`
3. Run the program with `./src/distributed-db`

## Usage

`distributed-db config-path [address port]`

If address and port aren't specified, then the address will be read from the system, and port will be determined via
the config file.

Specifying the address and port manually is helpful when running multiple instances of `distributed-db` on the same
machine.

### Config file

In the following example's format, separated by spaces:
```
0 193.11.112.192 9001
1 192.168.123.333 9002
2 193.11.112.177 9003
```
Where the first column is "id", second column is "address" and third column is "port"
