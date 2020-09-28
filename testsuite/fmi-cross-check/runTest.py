import os
import sys
import platform
import subprocess
import time
from importFMUs import simulateWithOMSimulator 
from validateResults import validateGeneratedResults
from generateHTML import generateOverviewHTML 

#
# Start point for script
#

# Get Script arguments
if not len(sys.argv) == 3 or sys.argv[1].lower() == "--help":
    print("Usage: python3 runTest.py /path/to/fmi-cross-check/repo OMSimulator\n")
    quit()

currentWorkingDir = os.getcwd()
crossCheckDir = os.path.abspath(str(sys.argv[1]))
omsimulator = str(sys.argv[2])

omsVersion = subprocess.run([omsimulator, '--version'], stdout=subprocess.PIPE).stdout.decode()
omsVersion = omsVersion.replace("OMSimulator ", "").replace("\n", "")

if "dev" in omsVersion:
  omsVersionShort = omsVersion.split("dev")[0]
  omsVersionShort = omsVersionShort[0:-1]
else:
  omsVersionShort = omsVersion

# Get OS short string
if platform.system() == "Linux":
  if platform.architecture()[0] == "64bit":
    osShort = "linux64"
  elif platform.architecture()[0] == "32bit":
    osShort = "linux32"
  else:
    raise Exception("Unsupported Linux distribution.")
elif platform.system() == "Darwin":
  if platform.architecture()[0] == "64bit":
    osShort = "darwin64"
  else:
    raise Exception("Unsupported OSX distribution.")
elif platform.system() == "Windows":
  if platform.architecture()[0] == "64bit":
    osShort = "win64"
  elif platform.architecture()[0] == "32bit":
    osShort = "win32"
  else:
    raise Exception("Unsupported Linux distribution.")

# Get more system informations
sysInfo = platform.system() + " " + platform.release()

# Measure time
startTime = time.strftime("%Y-%m-%d %H:%M:%S", time.localtime())
t_sec = time.time()

# Run test
simulateWithOMSimulator(crossCheckDir, osShort, omsimulator, omsVersionShort)
validateGeneratedResults(crossCheckDir, osShort, omsVersionShort)

# Get total time
totalTime = (time.time() - t_sec)
print("Time needed for test: " + time.strftime("%M:%S", time.gmtime(totalTime)))

# Generate HTML
os.chdir( currentWorkingDir)
generateOverviewHTML(crossCheckDir, osShort, omsVersion, omsVersionShort, startTime, totalTime, sysInfo)
