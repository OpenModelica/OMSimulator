import os
import platform
import sys
import pandas as pd
import shutil
from tempfile import NamedTemporaryFile
import subprocess
import sys
import time
#sys.path.append("/opt/OMSimulator-dev-181/lib")

from OMPython import OMCSessionZMQ
omc = OMCSessionZMQ()

from generateHTML import generateOverviewHTML

debugPrint = False
ignoreNotCompliantWithLatestRules = True
generateHTML = True

ulimitOMSimulator = 60
default_tolerance = 1e-8
reltolDiffMinMax = 1e-4
rangeDelta = 0.002

class HideOutput(object):
  '''
  A context manager that block stdout for its scope, usage:

  with HideOutput():
      os.system('ls -l')
  '''

  def __init__(self, *args, **kw):
      sys.stdout.flush()
      self._origstdout = sys.stdout
      self._oldstdout_fno = os.dup(sys.stdout.fileno())
      self._devnull = os.open(os.devnull, os.O_WRONLY)

  def __enter__(self):
      self._newstdout = os.dup(1)
      os.dup2(self._devnull, 1)
      os.close(self._devnull)
      sys.stdout = os.fdopen(self._newstdout, 'w')

  def __exit__(self, exc_type, exc_val, exc_tb):
      sys.stdout = self._origstdout
      sys.stdout.flush()
      os.dup2(self._oldstdout_fno, 1)


def simulate(testFMUDir, resultDir, modelName, fmiType, luaFile):
  tempDir = os.path.join(resultDir,"temp")
  intervals = "500"

  # Run lua file with OMSimulator via shell
  cmd = "OMSimulator \\\n"                                                     \
      + "    --stripRoot=true \\\n"                                            \
      + "    --skipCSVHeader=true \\\n"                                        \
      + "    --addParametersToCSV=true \\\n"                                   \
      + "    --intervals=" + intervals + " \\\n"                               \
      + "    --suppressPath=true \\\n"                                         \
      + "    --timeout=" + str(ulimitOMSimulator) +  "\\\n"                    \
      + "    " + os.path.relpath(luaFile,resultDir)

  if sys.platform == "win32":
    proc = subprocess.Popen(cmd.split(" \\\n    "), cwd=resultDir, stdout=subprocess.PIPE, stderr=subprocess.PIPE, shell=True)
  else:
    proc = subprocess.Popen([cmd], cwd=resultDir, stdout=subprocess.PIPE, stderr=subprocess.PIPE, shell=True)
  (out, err) = proc.communicate()
  exitCode = proc.returncode

  outStr = out.decode()
  errStr = err.decode()

  if debugPrint:
    print("cmd:\n"+cmd)
    print("stdout:")
    print(outStr)
    print("stderr:")
    print(errStr)
    print("\n\n")

  # Check if result file was generated
  if (not os.path.isfile(os.path.join(resultDir,  modelName+"_out.csv"))) and (exitCode == 0):
    exitCode = 1

  # Delete temp files
  shutil.rmtree(tempDir, ignore_errors=True)

  return (exitCode, cmd, outStr, errStr)


def filterResultFile(resultFile, referenceFile, resultDir):
  # Only save columns from fmu and remove ".fmu" and "\"" from variable names
  csv_res = pd.read_csv(resultFile)
  fmu_vars = [s for s in list(csv_res.columns.values) if(s.startswith(" \"fmu.") or s == "time")]
  csv_res = csv_res[fmu_vars]

  fmu_vars = list(map(lambda x: x.replace('fmu.','').replace('"','').replace(' ',''),fmu_vars))
  csv_res.columns = fmu_vars

  # Save only variables from reference file
  csv_ref = pd.read_csv(referenceFile)
  refVariables = list(csv_ref.columns.values)

  resultDiff = ""
  toRemove = []
  for var in refVariables:
    if not var in csv_res:
      if debugPrint:
        print("Result Diff: Variable \"" + var + "\" not found in result file but is in reference file.")
        open(os.path.join(resultDir,"failed"), "w").close()
      resultDiff = resultDiff + "Result Diff: Variable \"" + var + "\" not found in result file but is in reference file.\\n"
      toRemove.append(var)
  refVariables = [elem for elem in refVariables if elem not in toRemove]

  try:
    csv_res = csv_res[refVariables]
  except:
    raise

  # Overwrite file
  csv_res.to_csv(resultFile, index = False, header=True)
  return resultDiff


def diffSimulationResults(resultFile, referenceFile, modelName, testFMUDir, resultDir, relTol, absTol):
  """Check results with OpenModelica's diffSimulationResults function."""

  omc.sendExpression('cd("%s")' % resultDir)
  (resultsCorrect, failVars) =  omc.sendExpression('diffSimulationResults("%s","%s","%s",relTol=%g,relTolDiffMinMax=%g,rangeDelta=%g)' %
                                                  (resultFile,       \
                                                  referenceFile,     \
                                                  modelName+".diff", \
                                                  relTol,            \
                                                  reltolDiffMinMax,  \
                                                  rangeDelta))

  if len(failVars) > 0:
    resultsCorrect = False

  return (resultsCorrect, failVars)


def generateLua(modelName, testFMUDir, resultDir, fmiType):

  # Get some paths
  testFMU = os.path.relpath(os.path.join(testFMUDir, modelName + ".fmu"), resultDir)
  inputCSV = os.path.relpath(os.path.join(testFMUDir, modelName + "_in.csv"), resultDir)
  luaFilePath = os.path.join(resultDir, modelName + ".lua")

  # Set OMSimulator settings
  tempDir = "temp"
  startTime = "0.0"
  stopTime = "1.0"
  relTol = str(default_tolerance)
  interval = 500
  inputCSV = ""
  refOptFile = os.path.join(testFMUDir, modelName + "_ref.opt")
  df = pd.read_csv(refOptFile, delimiter=',', index_col=0, header=None)

  if "StartTime" in df.index:
    startTime = str(df.loc["StartTime", 1])

  if "StopTime" in df.index:
    stopTime = str(df.loc["StopTime", 1])

  if "RelTol" in df.index:
    if not df.loc["RelTol", 1] == 0:
      relTol = str(df.loc["RelTol", 1])

  maximumStepSize = str((float(stopTime) - float(startTime))/interval)
  if "StepSize" in df.index:
    if not df.loc["StepSize", 1] == 0:
      maximumStepSize = str(df.loc["StepSize", 1])

  # Check for input file
  inputs = []
  if os.path.isfile(inputCSV):
    df = pd.read_csv(inputCSV, delimiter=',')
    inputs = list(df.columns.values[1:])
    if debugPrint:
      print("Model " + testFMU + " has inputs\n")

  # System type
  if fmiType == "me":
    systemType = "oms_system_sc"
  elif fmiType == "cs":
    systemType = "oms_system_wc"
  else:
    raise Exception("Unsupportet FMU type \"" + fmiType + "\"\n")

  f = open(luaFilePath, "w")

  f.write("-- Lua file for " + modelName + ".fmu\n")
  f.write("oms_setTempDirectory(\""+tempDir+"\")\n")
  f.write("oms_newModel(\"model\")\n")
  f.write("oms_addSystem(\"model.root\", " + systemType + ")\n")

  f.write("\n-- instantiate FMU\n")
  if os.path.isfile(inputCSV):
    f.write("oms_addSubModel(\"model.root.input\", \"" + inputCSV + "\")\n")
  f.write("oms_addSubModel(\"model.root.fmu\", \"" + testFMU + "\")\n")

  if len(inputs) > 0:
    f.write("\n-- Connect inputs to FMU\n")
    for inp in inputs:
      f.write("oms_addConnection(\"model.root.input." + inp + "\", \"model.root.fmu." + inp + "\")\n")

  f.write("\n-- Simulation settings\n")
  f.write("oms_setSignalFilter(\"model\", \".*\")\n")
  f.write("oms_setResultFile(\"model\", \"" + modelName + "_out.csv\")\n")
  f.write("oms_setStartTime(\"model\", " + startTime + ")\n")
  f.write("oms_setStopTime(\"model\", " + stopTime + ")\n")
  f.write("oms_setTolerance(\"model\", " + relTol + ")\n")
  f.write("initialStepSize, minimumStepSize, maximumStepSize, status = oms_getVariableStepSize(\"model\")\n")
  f.write("oms_setVariableStepSize(\"model\", " + maximumStepSize + ", minimumStepSize, " + maximumStepSize + ")\n")
  f.write("oms_setFixedStepSize(\"model\", " + maximumStepSize +")\n")

  f.write("\n-- Instantiate, initialize and simulate\n")
  f.write("oms_instantiate(\"model\")\n")
  f.write("oms_initialize(\"model\")\n")
  f.write("oms_simulate(\"model\")\n")
  f.write("oms_terminate(\"model\")\n")
  f.write("oms_delete(\"model\")\n")

  f.close()

  return (luaFilePath, float(relTol), float(startTime), float(stopTime))


def createREADME(resultDir, modelName, cmd, luaFile):
  luaFile = open(luaFile, "r")

  resFile = open(os.path.join(resultDir,"README.md"), "w")
  resFile.write("To simulate FMU `"+ modelName +".fmu` with OMSimulator run\n")
  resFile.write("```bash\n$ "+ cmd +"\n")
  resFile.write("```\n\n")
  resFile.write("Lua file:\n```lua\n")
  resFile.write(luaFile.read())
  resFile.write("```\n\n")
  resFile.write("See the [OMSimulator documentation](https://openmodelica.org/doc/OMSimulator/master/html/index.html) for more information.")
  resFile.close()


def addResult(crossCheckDir, fmiVersion, fmiType, platform, importingToolID,
              importingToolVersion, exportingToolID, exportingToolVersion,
              modelName, df):

  testFMUDir = os.path.join(crossCheckDir, "fmus", fmiVersion, fmiType, platform,         \
                            exportingToolID, exportingToolVersion, modelName)
  resultDir = os.path.join(crossCheckDir, "results", fmiVersion, fmiType,                 \
                           platform, importingToolID, importingToolVersion,               \
                           exportingToolID, exportingToolVersion, modelName)

  testFMU = os.path.join(testFMUDir, modelName + ".fmu")
  print("Testing " + os.path.relpath(testFMU, crossCheckDir))

  if not os.path.isfile(testFMU):
    raise Exception("FMU \"" + testFMU + "\" not found!")

  os.makedirs(resultDir, exist_ok = True)

  # Generate lua file
  (luaFile, relTol, startTime, stopTime) = generateLua(modelName, testFMUDir, resultDir, fmiType)

  # Simulate FMU with OMSimulator
  simTimeStart = time.time()
  (exitCode, cmd, out, error) = simulate(testFMUDir, resultDir, modelName, fmiType, luaFile)
  simTime = time.time() - simTimeStart

  # Generate README
  createREADME(resultDir, modelName, cmd, luaFile)

  # Return if simulation failed
  if not exitCode==0:
    if os.path.isfile(os.path.join(testFMUDir,"notCompliantWithLatestRules")):
      open(os.path.join(resultDir,"rejected"), "w").close()
    else:
      open(os.path.join(resultDir,"failed"), "w").close()
    canSimulate = False
  else:
    canSimulate = True

  # Filter result file and check if all variables are in output file
  resultFile = os.path.join(resultDir, modelName + "_out.csv")
  referenceResult = os.path.join(testFMUDir, modelName + "_ref.csv")
  resultDiff = ""
  if os.path.isfile(resultFile):
    resultDiff = filterResultFile(resultFile, referenceResult, resultDir)
  else:
    open(os.path.join(resultDir,"failed"), "w").close()

  csv_ref = pd.read_csv(referenceResult)
  numRefVars = len(list(csv_ref.columns.values))-1

  # Check results
  resultsCorrect = False
  failedVars = []
  if os.path.isfile(resultFile):
    (resultsCorrect, failedVars) = diffSimulationResults(resultFile, referenceResult, modelName, testFMUDir, resultDir, 0.1, 0.1)
    if resultsCorrect and len(resultDiff) > 0:
      raise Exception("That was not supposed to happen!")

  if not (os.path.isfile(os.path.join(resultDir,"failed")) or os.path.isfile(os.path.join(resultDir,"rejected"))):
    open(os.path.join(resultDir,"passed"), "w").close()

  df = df.append({
    "FMI Version": fmiVersion,
    "FMI Type": fmiType,
    "Exporting Tool": exportingToolID,
    "Exporting Tool Version": exportingToolVersion,
    "Model Name": modelName,
    "OMS can import FMU": canSimulate,
    "Output": out,
    "Error": error,
    "Results Correct": resultsCorrect,
    "Failed Variables": failedVars,
    "Toal Variables": numRefVars,
    "Simulation Time": simTime,
    "Start Time": startTime,
    "Stop Time": stopTime,
    "Tolerance": relTol
  }, ignore_index=True)
  return df


def runTest(crossCheckDir, platform, omsVersion):
  """Iterate over all FMUs in "fmus" directory and run test.

  Run test for all FMUs inside "fmi-cross-check/fmus" directory for given platform and OMSimulator version.
  Saves result in file OMSimulator_Results.csv in fmi-cross-check directory.
  Returns data frame df with results.

  Keyword arguments:
  crossCheckDir -- string with absolute path to fmi-cross-check directory
  platform -- string with plaform defined by fmi-cross-check repository
  omsVersion -- string with version of OMSimulator to be tested
  """
  # Get start time
  startTime = time.strftime("%Y-%m-%d %H:%M:%S", time.localtime())
  t_sec = time.time()

  # Make sure we are in crossCheckDir
  os.chdir(fmiCrossCheckDir)

  # Create empty data frame for results.
  df = pd.DataFrame(columns=["FMI Version", "FMI Type", "Exporting Tool", "Exporting Tool Version", "Model Name", "OMS can import FMU", "Output", "Error", "Results Correct", "Failed Variables", "Total Variables", "Simulation Time", "Start Time", "Stop Time", "Tolerance"])

  # Iterate over tests
  for fmiVersion in ["2.0"]:
    for fmiType in ["me", "cs"]:
      toolsDir = os.path.join(crossCheckDir, "fmus", fmiVersion, fmiType, platform)
      for exportingToolID in os.listdir(toolsDir):
        versionsDir = os.path.join(toolsDir, exportingToolID) 
        for exportingToolVersion in os.listdir(versionsDir):
          fmusDir = os.path.join(versionsDir, exportingToolVersion)
          for fmu in os.listdir(fmusDir):
            # Skipt not compliant FMUs
            if ignoreNotCompliantWithLatestRules:
              if os.path.isfile(os.path.join(crossCheckDir, "fmus", fmiVersion, fmiType, platform, exportingToolID, exportingToolVersion, fmu, "notCompliantWithLatestRules")):
                continue

            df = addResult(crossCheckDir, fmiVersion, fmiType, platform, "OMSimulator", omsVersion, exportingToolID, exportingToolVersion, fmu, df)


  # Write result csv
  resultFile = os.path.join(crossCheckDir, "OMSimulator_Results.csv")
  df.to_csv(resultFile, index=False, header=True)

  # Get total time
  totalTime = (time.time() - t_sec)

  # Output result summary
  numFmus = len(df["OMS can import FMU"])
  fmusSimulated = list(df["OMS can import FMU"]).count(True)
  fmusVerified = list(df["Results Correct"]).count(True)
  print("\n\t# ##################################################### #")
  print("\t#             Final results summary                     # ")
  print("\t# \tTotal FMUs tested:\t\t" + str(numFmus) + "\t\t#")
  print("\t# \tTotal test time:\t\t" + time.strftime("%M:%S", time.gmtime(totalTime)) + " min\t#")
  print("\t# \tFMUs simulated succesfully:\t" + str(fmusSimulated) + " / " + str(numFmus) + "\t\t#")
  print("\t# \tFMUs verified:\t\t\t" + str(fmusVerified) + " / " + str(numFmus) + "\t\t#")
  print("\t# ##################################################### #")
  print("Detailed results saved in " + str(resultFile) + "\n")

  # Generate HTML
  if generateHTML:
    omsVersionShort = omsVersion
    omsVersion = subprocess.run([pathToOMSimulator, '--version'], stdout=subprocess.PIPE).stdout.decode()
    omsVersion = omsVersion.replace("\n", "")
    os.chdir(os.path.dirname(fmiCrossCheckDir))
    generateOverviewHTML(df, crossCheckDir, platform, startTime, totalTime, platform, omsVersion, omsVersionShort, ulimitOMSimulator, default_tolerance)

  return df


if not 2 <= len(sys.argv) <= 3:
  print("Usage: python3 addResults.py /path/to/fmi-cross-check/repo [path/to/OMSimulator]")
  quit()

# Change working dir to fmi-cross-check repo
fmiCrossCheckDir = os.path.abspath(str(sys.argv[1]))
os.chdir(fmiCrossCheckDir)
print("Set working directory to \"" + fmiCrossCheckDir + "\"")
if not (os.path.isdir("fmus") or os.path.isdir("results")):
  print("Can't find \"fmus\" or \"results\" directories in \"" + fmiCrossCheckDir + "\".\n Is this /fmi-cross-check from \"https://github.com/modelica/fmi-cross-check/\"?")
  raise Exception("Error: Not in fmi-cross-check repo!")

# Get OS short string
if platform.system() == "Linux":
  if platform.architecture()[0] == "64bit":
    platform = "linux64"
  elif platform.architecture()[0] == "32bit":
    platform = "linux32"
  else:
    raise Exception("Unsupported Linux distribution.")
elif platform.system() == "Darwin":
  if platform.architecture()[0] == "64bit":
    platform = "darwin64"
  else:
    raise Exception("Unsupported OSX distribution.")
elif platform.system() == "Windows":
  if platform.architecture()[0] == "64bit":
    platform = "win64"
  elif platform.architecture()[0] == "32bit":
    platform = "win32"
  else:
    raise Exception("Unsupported Linux distribution.")

# Get OMSimulator and version
if len(sys.argv) == 3:
  pathToOMSimulator = sys.argv[1]
  Exception("Not implemented yet. Add OMSimulator to PATH!")
else:
  pathToOMSimulator = "OMSimulator"

output = shutil.which(pathToOMSimulator)
if output is None:
  Exception("Can't execute pathToOMSimulator")

omsVersion = subprocess.run([pathToOMSimulator, '--version'], stdout=subprocess.PIPE).stdout.decode()
omsVersion = omsVersion.replace("OMSimulator ", "").replace("\n", "")

print("Using OMSimulator from \"" + output + "\" with version \"" + omsVersion + "\"\n")

if "dev" in omsVersion:
  omsVersion = omsVersion.split("dev")[0]
  omsVersion = omsVersion[0:-1]

# Run fmi-cross-check for OMSimulator
df = runTest(fmiCrossCheckDir, platform, omsVersion)