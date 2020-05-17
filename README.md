# Lift Simulator

### Anurag Singh

## Introduction

Lift Simulator is a solution to the Operating Systems (COMP2006) assignment in Semester 1 2020.

## Purpose

To demonstrate multi-threading, and inter process/thread communication, and solving the critical section problem.

## Building

Running `make` will build 2 executables, `lift_sim_A` and `lift_sim_B` which refer to threads and processes implementations respectively.

## Usage

Both lift_sim_A, and lift_sim_B are used the same way, the program will write to the sim_out file as it runs.

```
$ ./lift_sim_{A,B} [max buffer size] [time per sleep] [input file]
```

## Testing

Various test files are included in the `test/inputs` directory, with some sample outputs available in the `examples` directory.

Python scripts in the testing directory are for testing the program through the input files quickly.
