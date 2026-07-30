#!/usr/bin/env python3
"""Run a single OMSimulator testsuite case.

This is the CTest driver. It replaces the per-file part of the `rtest` Perl
script: it reads the metadata header of a test file, runs the test in a private
working directory, normalizes the output and compares it against the expected
result that is embedded in the test file itself (the `## Result: ... ##
endResult` block), using omc-diff.

Test discovery, parallelization and reporting are handled by CTest, so none of
that is duplicated here.

Exit codes:
  0   test passed
  1   test failed
  77  test skipped (CTest's SKIP_RETURN_CODE)
"""

import argparse
import os
import re
import shutil
import subprocess
import sys
import time
from pathlib import Path

# Same tolerance rtest uses when calling omc-diff.
DEFAULT_EPSILON = "5e-5"

SKIP_EXIT_CODE = 77

# Metadata keys understood in the test file header, with their defaults.
DEFAULT_INFO = {
    "status": "unknown",
    "keywords": "unknown",
    "env": "",
    "linux": "yes",
    "win": "",
    "ucrt64": "",
    "mac": "",
    "asan": "",
}

COMMENT_PREFIX = {
  ".py": "##",
  ".lua": "--"
}
DEFAULT_COMMENT_PREFIX = "//"

# Lines that are noise from the environment rather than from OMSimulator.
IGNORED_LINE_PATTERNS = (
    # sometimes comes from libhwloc when running docker
    re.compile(r"^Unexpected end of /proc/mounts line"),
    # comes from libgc
    re.compile(r"^GC Warning:"),
)


def comment_prefix(test_file: Path) -> str:
  return COMMENT_PREFIX.get(test_file.suffix, DEFAULT_COMMENT_PREFIX)


PLATFORMS = ("linux", "mac", "win", "ucrt64")

# The platform whose metadata key decides whether a test is enabled. Set from
# the command line in main(); detected automatically when not given.
PLATFORM = None


def detect_platform() -> str:
  """The metadata key describing the platform we are running on."""
  # MSYS2/MinGW python reports win32 too, so MSYSTEM decides; a native Windows
  # build (MSVC) has to be selected explicitly with --platform=win.
  if os.environ.get("MSYSTEM") == "UCRT64":
    return "ucrt64"
  if sys.platform == "win32":
    return "win"
  if sys.platform == "darwin":
    return "mac"
  return "linux"


def simulator_executable(test_file: Path, bin_dir: Path) -> Path:
  """The OMSimulator front end used to run a test of this type."""
  # Both "win" and "ucrt64" run the tests through a native Windows Python
  # (subprocess uses CreateProcess, not a POSIX exec), which cannot run the
  # shebang-based shell script directly, so both need the .bat/.exe wrapper.
  windows = PLATFORM in ("win", "ucrt64")
  if test_file.suffix == ".py":
    name = "OMSimulatorPython3.bat" if windows else "OMSimulatorPython3"
  else:
    name = "OMSimulator.exe" if windows else "OMSimulator"
  return bin_dir / name


def parse_test_file(test_file: Path):
  """Split a test file into its metadata header and its expected output.

  Returns (info, expected). `expected` is normalized the same way the actual
  output is, so that the two can be compared line by line.
  """
  info = dict(DEFAULT_INFO)
  expected = []
  prefix = comment_prefix(test_file)
  header_re = re.compile(r"^" + re.escape(prefix) + r"[ \\|]*([a-z_0-9]*):[ \\|]*([^\r\n]*)")
  in_result = False

  with test_file.open("r", encoding="utf-8", errors="replace") as f:
    for line in f:
      if not in_result and line.startswith(prefix + " Result:"):
        in_result = True
        continue
      if in_result:
        if line.startswith(prefix + " endResult"):
          in_result = False
          continue
        expected.append(strip_result_marker(line, prefix))
        continue
      match = header_re.match(line)
      if match:
        info[match.group(1)] = match.group(2)

  return info, expected


def strip_result_marker(line: str, prefix: str) -> str:
  """Turn one line of a `## Result:` block into the raw expected output line."""
  line = line.rstrip("\r\n")
  line = line.lstrip(" \t")
  line = collapse_first_whitespace_run(line)
  if line.startswith(prefix + " "):
    return line[len(prefix) + 1:] + "\n"
  if line == prefix:
    return "\n"
  return line + "\n"


def collapse_first_whitespace_run(line: str) -> str:
  """Replace the first run of spaces/tabs by a single space.

  This mirrors rtest's `s/[ \\t]+/ /` (no /g), and has to stay bug-compatible
  with it: the expected results checked into the test files were generated with
  exactly this normalization.
  """
  return re.sub(r"[ \t]+", " ", line, count=1)


def normalize_output_line(line: str) -> str:
  line = line.rstrip("\r\n")
  line = line.lstrip(" \t")
  line = collapse_first_whitespace_run(line)
  if PLATFORM == "ucrt64":
    # fix generated files on windows: /X.mo?_tempNNNN/ -> /
    line = re.sub(r"/[^/]+\.mos?_temp\d+/", "/", line)
  return line + "\n"


def prepare_work_dir(work_dir: Path, testsuite_root: Path, tests_root: Path,
                     test_file: Path) -> Path:
  """Create a private copy of the tests layout and return the run directory.

  Each test runs in its own directory so that tests can run in parallel and
  cannot see each other's output files. The directory tree mirrors the layout
  below tests/, with resources/ and references/ linked in at its root, so the
  relative paths the tests use (`../resources/X.fmu`, `../../../references/Y.mat`)
  keep working unchanged.
  """
  if work_dir.exists():
    shutil.rmtree(work_dir)

  rel_dir = test_file.parent.relative_to(tests_root)
  run_dir = work_dir / rel_dir
  run_dir.mkdir(parents=True)

  # Shared, read-only inputs are linked, not copied: resources/ alone is >100 MB.
  for shared in ("resources", "references"):
    source = testsuite_root / shared
    if source.is_dir():
      link_or_copy(source, work_dir / shared)

  link_or_copy(test_file, run_dir / test_file.name)
  return run_dir


def link_or_copy(source: Path, target: Path) -> None:
  """Symlink source to target, falling back to a copy where that is not allowed."""
  try:
    os.symlink(source, target, target_is_directory=source.is_dir())
    return
  except (OSError, NotImplementedError):
    pass

  if sys.platform == "win32" and source.is_dir():
    # Directory junctions do not need the privilege that symlinks need.
    result = subprocess.run(["cmd", "/c", "mklink", "/J", str(target), str(source)],
                            capture_output=True)
    if result.returncode == 0:
      return

  if source.is_dir():
    shutil.copytree(source, target)
  else:
    shutil.copy2(source, target)


def build_env(info, asan: bool) -> dict:
  env = dict(os.environ)
  # Windows (MinGW) prints 3 digit exponents (vs 2 on *nix) by default, which
  # shows up as spurious mismatches.
  env["PRINTF_EXPONENT_DIGITS"] = "2"
  if asan:
    # The tests are started through python3, which is not instrumented, so
    # libasan has to be preloaded for the instrumented library to work.
    env["LD_PRELOAD"] = subprocess.check_output(
        ["gcc", "-print-file-name=libasan.so"]).decode().strip()
  for assignment in info["env"].split():
    if "=" in assignment:
      key, value = assignment.split("=", 1)
      env[key] = value
  return env


def run_test(test_file: Path, run_dir: Path, exe: Path, info, asan: bool):
  """Run the test and return its exit code and the path of the captured log."""
  log_file = run_dir / (test_file.name + ".log")

  with log_file.open("wb") as log:
    returncode = subprocess.call([str(exe), test_file.name], cwd=str(run_dir),
                                 env=build_env(info, asan),
                                 stdout=log, stderr=subprocess.STDOUT)

  return returncode, log_file


def read_log(log_file: Path) -> list:
  with log_file.open("r", encoding="utf-8", errors="replace") as log:
    return log.readlines()


def extract_result(log_lines) -> list:
  """Normalize the captured output into the form the expected result has."""
  result = []
  for line in log_lines:
    if any(pattern.match(line.lstrip(" \t")) for pattern in IGNORED_LINE_PATTERNS):
      continue
    result.append(normalize_output_line(line))
  return result


def write_lines(path: Path, lines) -> None:
  with path.open("w", encoding="utf-8", newline="\n") as f:
    f.writelines(lines)


def compare(omc_diff: Path, epsilon: str, expected_file: Path, got_file: Path, diff_file: Path) -> bool:
  with diff_file.open("wb") as diff:
    returncode = subprocess.call([str(omc_diff), epsilon, str(expected_file), str(got_file)],
                                 stdout=diff, stderr=subprocess.STDOUT)
  return returncode == 0


def dump(title: str, path: Path) -> None:
  print("==== %s (%s)" % (title, path))
  try:
    sys.stdout.write(path.read_text(encoding="utf-8", errors="replace"))
  except OSError as error:
    print("could not read %s: %s" % (path, error))
  print()


def set_baseline(test_file: Path, log_lines, prefix: str) -> None:
  """Rewrite the `## Result:` block of the test file with the actual output.

  The output is recorded verbatim, without the normalization that comparing
  applies; both sides of a comparison get normalized, so the two agree either
  way, and this keeps the recorded baselines identical to what rtest produced.
  """
  body = []
  in_result = False
  with test_file.open("r", encoding="utf-8", errors="replace") as f:
    for line in f:
      if line.startswith(prefix + " Result:"):
        in_result = True
      if not in_result:
        body.append(line.rstrip("\r\n") + "\n")
      if line.startswith(prefix + " endResult"):
        in_result = False

  body.append("%s Result:\n" % prefix)
  for line in log_lines:
    line = line.rstrip("\r\n")
    body.append("%s\n" % prefix if line == "" else "%s %s\n" % (prefix, line))
  body.append("%s endResult\n" % prefix)

  # Write in binary mode to force LF instead of CRLF on Windows.
  with test_file.open("w", encoding="utf-8", newline="\n") as f:
    f.writelines(body)


def main() -> int:
  parser = argparse.ArgumentParser(description=__doc__)
  parser.add_argument("--test", required=True, type=Path,
                      help="test file to run, inside the testsuite source tree")
  parser.add_argument("--testsuite-root", required=True, type=Path,
                      help="the testsuite source directory, holding resources/ and references/")
  parser.add_argument("--tests-root", required=True, type=Path,
                      help="the directory the test files are grouped under; the working "
                           "directory mirrors the layout below it")
  parser.add_argument("--work-dir", required=True, type=Path,
                      help="private working directory for this test; it is wiped on every run")
  parser.add_argument("--bin-dir", required=True, type=Path,
                      help="directory holding OMSimulator / OMSimulatorPython3")
  parser.add_argument("--omc-diff", required=True, type=Path,
                      help="the omc-diff executable used to compare results")
  parser.add_argument("--epsilon", default=DEFAULT_EPSILON,
                      help="numeric tolerance passed to omc-diff (default: %(default)s)")
  parser.add_argument("--asan", action="store_true",
                      help="running under AddressSanitizer; skips tests not marked 'asan: yes'")
  parser.add_argument("--platform", choices=PLATFORMS, default=detect_platform(),
                      help="platform the tests are enabled for (default: %(default)s); a native "
                           "Windows build run from an MSYS2 shell has to select 'win' explicitly")
  parser.add_argument("--baseline", action="store_true",
                      default=os.environ.get("OMS_TESTSUITE_BASELINE", "") not in ("", "0"),
                      help="update the expected result in the test file instead of comparing")
  args = parser.parse_args()

  global PLATFORM
  PLATFORM = args.platform

  test_file = args.test.resolve()
  testsuite_root = args.testsuite_root.resolve()
  tests_root = args.tests_root.resolve()
  prefix = comment_prefix(test_file)
  info, expected = parse_test_file(test_file)

  if info["status"] not in ("correct", "incorrect", "erroneous"):
    print("unknown testcase status: %r" % info["status"])
    return 1

  # Also applies when setting a baseline: a test that is disabled here would
  # otherwise get an expected result recorded from the wrong platform.
  if args.asan and info["asan"] != "yes":
    print("skipped: not enabled for AddressSanitizer")
    return SKIP_EXIT_CODE
  if info.get(PLATFORM, "") != "yes":
    print("skipped: not enabled for platform %r" % PLATFORM)
    return SKIP_EXIT_CODE

  exe = simulator_executable(test_file, args.bin_dir.resolve())
  if not exe.exists():
    print("%s not found; build and install OMSimulator before running the testsuite" % exe)
    return 1

  run_dir = prepare_work_dir(args.work_dir.resolve(), testsuite_root, tests_root,
                             test_file)

  start = time.time()
  returncode, log_file = run_test(test_file, run_dir, exe, info, args.asan)
  duration = time.time() - start

  log_lines = read_log(log_file)

  if args.baseline:
    set_baseline(test_file, log_lines, prefix)
    print("set baseline for %s" % test_file.name)
    return 0

  if info["status"] == "erroneous":
    print("erroneous [time: %.1fs]" % duration)
    return 0

  if returncode != 0 and info["status"] == "correct":
    print("execution failed with exit code %d [time: %.1fs]" % (returncode, duration))
    dump("Log", log_file)
    return 1
  if returncode == 0 and info["status"] != "correct":
    print("this test should have failed [time: %.1fs]" % duration)
    dump("Log", log_file)
    return 1

  expected_file = run_dir / (test_file.name + ".expected")
  got_file = run_dir / (test_file.name + ".got")
  diff_file = run_dir / (test_file.name + ".diff")
  write_lines(expected_file, expected)
  write_lines(got_file, extract_result(log_lines))

  if not compare(args.omc_diff, args.epsilon, expected_file, got_file, diff_file):
    print("result mismatch [time: %.1fs]" % duration)
    dump("Log", log_file)
    dump("omc-diff", diff_file)
    return 1

  print("ok [time: %.1fs]" % duration)
  return 0


if __name__ == "__main__":
  sys.exit(main())
