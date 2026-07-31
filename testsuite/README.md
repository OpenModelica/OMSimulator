# OMSimulator-testsuite

Each test file carries its own metadata header and its own expected result, embedded as a
`## Result: ... ## endResult` comment block at the end of the file.

```text
tests/        the test files (.py, .lua), grouped into one directory per topic
resources/    inputs shared by the tests: FMUs, SSPs and the sources they are packed from
references/   reference result files
difftool/     omc-diff, which compares actual against expected results
runtest.py    the CTest driver, running one test file
```

Tests refer to the shared directories relative to `tests/`, as `../resources/X.fmu` or
`../../../references/Y.mat`, depending on how deeply the test is nested.

## Running the testsuite

The tests run against an *installed* OMSimulator, because the Python bindings are only usable from
an install tree. Configure with `-DOMS_ENABLE_TESTSUITE=ON`, then build and install first:

```bash
cmake -S . -B build -DOMS_ENABLE_TESTSUITE=ON
cmake --build build
cmake --install build
```

```bash
ctest --test-dir build -j8               # run everything in parallel
ctest --test-dir build -j8 -L api        # run one directory (label = directory)
ctest --test-dir build -R SimpleSimulation1  # run tests matching a regex
ctest --test-dir build --rerun-failed --output-on-failure
ctest --test-dir build -j8 --output-junit result.xml   # for CI
```

Tests are named `<directory>.<file>`, e.g. `api.NewSSP` or
`reference-fmus.2.0.cs.BouncingBall-cs`. A test that is disabled on the current platform is
reported as *Skipped* rather than silently ignored.

Each test runs in its own working directory under `build/testsuite/work/<test name>`, which is
wiped at the start of every run. That directory keeps the log, the expected result, the actual
result and the omc-diff output of the last run, which is where to look when a test fails.

To test a simulator other than the one in the install prefix, configure with
`-DOMS_TESTSUITE_BIN_DIR=<dir>`, where `<dir>` contains `OMSimulator` and `OMSimulatorPython3`.

## Creating test files

To get a correct testfile, watch out for use of tab and space, can be hard to find.

Special directives in the header of a test file:

* `status: correct`

  `correct` expects the test to succeed, `incorrect` expects it to fail, and `erroneous` runs the
  test without checking anything.
* `linux: yes`, `mac: yes`, `win: yes`, `ucrt64: yes`

  Enable the test case on that platform. A test is skipped on every platform it does not name;
  `linux` defaults to `yes`, the others to `no`.
* `asan: yes`

  Also run the test case in an AddressSanitizer build (`-DOMS_TESTSUITE_ASAN=ON`), where all other
  tests are skipped. Such a run leak checks the `OMSimulatorPython3` wrapper and the interpreter
  as well; the leaks those report on their own are ignored through `lsan-suppressions.txt`.
* `env: KEY=VALUE ...`

  Extra environment variables for the test case.

## Updating expected results

Running a test with `OMS_TESTSUITE_BASELINE=1` records its actual output as the new expected
result, rewriting the `## Result:` block of the test file in place:

```bash
OMS_TESTSUITE_BASELINE=1 ctest --test-dir build -R api.NewSSP
cmake --build build --target testsuite-baseline   # rebaseline everything
```

Always review the resulting diff before committing it.
