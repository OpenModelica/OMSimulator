# OMSimulator-testsuite

## Running the testsuite

The testsuite consists of test files (.lua and .py) and resources, such as reference files and FMUs.

1. `make`

   Will run all tests that currently should pass. Use this before checking in.

## Creating test files

To get a correct testfile, watch out for use of tab and space, can be hard to find.

`rtest` special directives added to help creating testcases:

* `status: correct`

* `teardown_command: rm -f ...`

  Will execute the provided command after running the test case, e.g. to remove generated files.
* `linux: no`

   Will disable a specific test case on Linux.
* `mingw: yes`

   Will enable a specific test case on Windows mingw.
