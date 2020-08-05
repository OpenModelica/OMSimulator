import os
import platform
import sys
import pandas as pd
import shutil
from tempfile import NamedTemporaryFile
import subprocess
import sys
#sys.path.append("/opt/OMSimulator-dev-181/lib")
from OMSimulator import OMSimulator

debugPrint = False
ignoreNotCompliantWithLatestRules = True

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
      + "    --timeout=60 \\\n"                                                \
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

  # Delete temp files
  shutil.rmtree(tempDir, ignore_errors=True)

  return (exitCode, cmd, outStr, errStr)


def filterReultFile(resultFile, referenceFile, resultDir):
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
  resultsCorrect = True
  resultDiff = ""
  wrongVars = []

  logDiffFile = os.path.join(testFMUDir, modelName + "_diff.log")

  # Compare results for each variable
  csv_ref = pd.read_csv(referenceFile)
  refVariables = list(csv_ref.columns.values)
  refVariables.pop(0) # Remove time

  with OMSimulator() as oms:
    if not debugPrint:
      with HideOutput():
        oms.setWorkingDirectory(resultDir)
        oms.setLogFile(logDiffFile)
    else:
      oms.setWorkingDirectory(resultDir)
      oms.setLogFile(logDiffFile)
    for var in refVariables:
      status = oms.compareSimulationResults(resultFile, referenceFile, var, relTol, absTol)
      if not status == 1:
        resultsCorrect = False
        if debugPrint:
          print("Variable \"" + var + "\" not correct.")
        wrongVars.append(var)

  if not debugPrint:
    sys.stdout = sys.__stdout__

  for wrongVar in wrongVars:
    with open(logDiffFile, 'a') as file:
      file.write("Variable \"" + wrongVar + "\" not correct.\n")

  with open(logDiffFile, "r") as file:
    resultDiff = file.read()
    if debugPrint:
      print(resultDiff)

  # Remove temp files
  try:
    os.remove(logDiffFile)
  except:
    # This is not working on Windows: PermissionError because OMSimulator does not close the log file
    print("Error: Could not remove \"" + logDiffFile + "\"")

  return (resultsCorrect, resultDiff)


def generateLua(modelName, testFMUDir, resultDir, fmiType):

  # Get some paths
  testFMU = os.path.relpath(os.path.join(testFMUDir, modelName + ".fmu"), resultDir)
  inputCSV = os.path.relpath(os.path.join(testFMUDir, modelName + "_in.csv"), resultDir)
  luaFilePath = os.path.join(resultDir, modelName + ".lua")

  # Set OMSimulator settings
  tempDir = "temp"
  startTime = "0.0"
  stopTime = "1.0"
  relTol = "1e-8"
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

  return (luaFilePath, float(relTol))


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
              importingToolVersion, exporingToolID, exportingToolVersion,
              modelName):

  testFMUDir = os.path.join(crossCheckDir, "fmus", fmiVersion, fmiType, platform,         \
                            exporingToolID, exportingToolVersion, modelName)
  resultDir = os.path.join(crossCheckDir, "results", fmiVersion, fmiType,                 \
                           platform, importingToolID, importingToolVersion,       \
                           exporingToolID, exportingToolVersion, modelName)

  testFMU = os.path.join(testFMUDir, modelName + ".fmu")
  print("Testing " + os.path.relpath(testFMU, crossCheckDir))

  if not os.path.isfile(testFMU):
    raise Exception("FMU \"" + testFMU + "\" not found!")

  os.makedirs(resultDir, exist_ok = True)

  # Generate lua file
  (luaFile, relTol) = generateLua(modelName, testFMUDir, resultDir, fmiType)

  # Simulate FMU with OMSimulator
  (exitCode, cmd, out, error) = simulate(testFMUDir, resultDir, modelName, fmiType, luaFile)

  # Generate README
  createREADME(resultDir, modelName, cmd, luaFile)

  # Filter result file and check if all variables are in output file
  resultFile = os.path.join(resultDir, modelName + "_out.csv")
  referenceResult = os.path.join(testFMUDir, modelName + "_ref.csv")
  resultDiff = ""
  if os.path.isfile(resultFile):
    resultDiff = filterReultFile(resultFile, referenceResult, resultDir)
  else:
    open(os.path.join(resultDir,"failed"), "w").close()

  # Check results
  resultsCorrect = False
  if os.path.isfile(resultFile):
    (resultsCorrect, resultDiff2) = diffSimulationResults(resultFile, referenceResult, modelName, testFMUDir, resultDir, 0.1, 0.1)
    resultDiff = resultDiff + "\n" + resultDiff2

  if not (os.path.isfile(os.path.join(resultDir,"failed")) or os.path.isfile(os.path.join(resultDir,"rejected"))):
    open(os.path.join(resultDir,"passed"), "w").close()

  if exitCode==0:
    return (True, out, error, resultDiff, resultsCorrect)
  else:
    if os.path.isfile(os.path.join(testFMUDir,"notCompliantWithLatestRules")):
      open(os.path.join(resultDir,"rejected"), "w").close()
    else:
      open(os.path.join(resultDir,"failed"), "w").close()
    return (False, out, error, resultDiff, resultsCorrect)


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
  # Make sure we are in crossCheckDir
  os.chdir(fmiCrossCheckDir)

  # Create empty data frame for results.
  df = pd.DataFrame(columns=["FMI Version", "FMI Type", "Exporting Tool", "Exporting Tool Version", "Model Name", "OMS can import FMU", "Output", "Error", "Results Correct", "Result comments"])

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

            (result, output, error, resultComments, resultsCorrect) = addResult(crossCheckDir, fmiVersion, fmiType, platform, "OMSimulator", omsVersion, exportingToolID, exportingToolVersion, fmu)
            df = df.append({
              "FMI Version": fmiVersion,
              "FMI Type": fmiType,
              "Exporting Tool": exportingToolID,
              "Exporting Tool Version": exportingToolVersion,
              "Model Name": fmu,
              "OMS can import FMU": result,
              "Output": output,
              "Error": error,
              "Results Correct": resultsCorrect,
              "Result comments": resultComments
            }, ignore_index=True)

  # Write result csv
  resultFile = os.path.join(crossCheckDir, "OMSimulator_Results.csv")
  df.to_csv(resultFile, index=False, header=True)

  # Output result summary
  numFmus = len(df["OMS can import FMU"])
  fmusSimulated = list(df["OMS can import FMU"]).count(True)
  fmusVerified = list(df["Results Correct"]).count(True)
  print("\n\t# ############################################# #")
  print("\t#             Final results summary             # ")
  print("\t# \tTotal FMUs tested:\t\t" + str(numFmus) + "\t#")
  print("\t# \tFMUs simulated succesfully:\t" + str(fmusSimulated) + " / " + str(numFmus) + "\t#")
  print("\t# \tFMUs verified:\t\t\t" + str(fmusVerified) + " / " + str(numFmus) + "\t#")
  print("\t# ############################################# #")
  print("Detailed results saved in " + str(resultFile) + "\n")

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