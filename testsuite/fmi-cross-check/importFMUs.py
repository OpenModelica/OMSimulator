import os
import sys
import shutil
import subprocess
import time
import pandas as pd

# Global vars
debugPrint = True
ignoreNotCompliantWithLatestRules = True

ulimitOMSimulator = 60
default_tolerance = 1e-8
reltolDiffMinMax = 1e-4
rangeDelta = 0.002

def generateLua(modelName, testFMUDir, resultDir, fmiType):
  """Generate Lua Script that will be called by OMSimulator.
  Reads experiment settings from <FMUName>_ref.opt and optional start values.
  Returns path to generated Lua file. 
  """
  # Get some paths
  testFMU = os.path.relpath(os.path.join(testFMUDir, modelName + ".fmu"), resultDir)
  if sys.platform == "win32":
    testFMU = testFMU.replace("\\", "\\\\")
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

  expFile = open(os.path.join(resultDir, "OMSimulator_exp.log"), "w")
  expFile.write("Start Time: " + str(startTime) + "\n")
  expFile.write("Stop Time: " + str(stopTime) + "\n")
  expFile.write("Relative Tolerance: " + str(relTol) + "\n")
  expFile.close()

  return luaFilePath


def simulateFMU(omsimulator, testFMUDir, resultDir, modelName, fmiType, luaFile):
  """Import and simulate a FMU with OMSimulator
  Run given luaFile with given OMSimulator executable and measure time.
  Will save outputs to "OMSimulator_out.log", "OMSimulator_err.log" and "OMSimulator_exp.log".
  Return call comand cmd.
  """
  tempDir = os.path.join(resultDir,"temp")
  intervals = "500"

  # Run lua file with OMSimulator via shell
  cmd = ["--stripRoot=true",
          "--skipCSVHeader=true",
          "--addParametersToCSV=true",
          "--intervals=" + intervals,
          "--suppressPath=true",
          "--timeout=" + str(ulimitOMSimulator),
          os.path.relpath(luaFile, resultDir)]

  # Call OMSimulator and measure time
  simTimeStart = time.time()
  proc = subprocess.Popen(omsimulator+cmd, cwd=resultDir, stdout=subprocess.PIPE, stderr=subprocess.PIPE)
  cmd = ' '.join(omsimulator+cmd)
  simTime = time.time() - simTimeStart
  (out, err) = proc.communicate()
  exitCode = proc.returncode

  # Save outputs
  open(os.path.join(resultDir, "OMSimulator_out.log"), "w").write(out.decode())
  open(os.path.join(resultDir, "OMSimulator_err.log"), "w").write(err.decode())
  expFile = open(os.path.join(resultDir, "OMSimulator_exp.log"), "a")
  expFile.write("Exit Code: " + str(exitCode) + "\n")
  expFile.write("Simulation Time: " + str(simTime) + "\n")
  expFile.close()

  if debugPrint:
    print("cmd:\n"+cmd)
    print("stdout:")
    print(out.decode())
    print("stderr:")
    print(err.decode())
    print("Wrote log files to " + resultDir)
    print("\n\n")

  # Check if result file was generated
  if (not os.path.isfile(os.path.join(resultDir,  modelName+"_out.csv"))) and (exitCode == 0):
    exitCode = 1

  # Delete temp files
  shutil.rmtree(tempDir, ignore_errors=True)

  return cmd


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


def importFMU(crossCheckDir, testFMUDir, resultDir, modelName, fmiType, omsimulator):
  """Import and simulate FMU with OMSimulator
  Generate a Lua script and run it with 
  """
  testFMU = os.path.join(testFMUDir, modelName + ".fmu")
  print("Testing " + os.path.relpath(testFMU, crossCheckDir))

  if not os.path.isfile(testFMU):
    raise Exception("FMU \"" + testFMU + "\" not found!")

  os.makedirs(resultDir, exist_ok = True)

  # Generate lua file
  luaFile = generateLua(modelName, testFMUDir, resultDir, fmiType)

  # Simulate FMU with OMSimulator
  cmd = simulateFMU(omsimulator,testFMUDir, resultDir, modelName, fmiType, luaFile)

  # Generate README
  createREADME(resultDir, modelName, cmd, luaFile)


def simulateWithOMSimulator(crossCheckDir, platform, omsimulator, omsVersion):
  """Iterate over all FMUs in "fmus" directory and simulate them with OMSimulator.

  Run test for all FMUs inside "fmi-cross-check/fmus" directory for given platform and OMSimulator version.
  Saves result in file OMSimulator_Results.csv in fmi-cross-check directory.
  Returns data frame df with results.

  Keyword arguments:
  crossCheckDir -- string with absolute path to fmi-cross-check directory
  platform -- string with plaform defined by fmi-cross-check repository
  omsVersion -- string with version of OMSimulator to be tested
  """

  # Make sure we are in crossCheckDir
  os.chdir(crossCheckDir)

  # Get start time
  t_sec = time.time()

  # Iterate over tests
  for fmiVersion in ["2.0"]:
    for fmiType in ["me", "cs"]:
      toolsDir = os.path.join(crossCheckDir, "fmus", fmiVersion, fmiType, platform)
      for exportingToolID in os.listdir(toolsDir):
        versionsDir = os.path.join(toolsDir, exportingToolID) 
        for exportingToolVersion in os.listdir(versionsDir):
          fmusDir = os.path.join(versionsDir, exportingToolVersion)
          for modelName in os.listdir(fmusDir):
            # Skipt not compliant FMUs
            if ignoreNotCompliantWithLatestRules:
              if os.path.isfile(os.path.join(crossCheckDir, "fmus", fmiVersion, fmiType, platform, exportingToolID, exportingToolVersion, modelName, "notCompliantWithLatestRules")):
                continue

            # Import FMUs
            testFMUDir = os.path.join(crossCheckDir, "fmus", fmiVersion, fmiType, platform,         \
                                      exportingToolID, exportingToolVersion, modelName)
            resultDir = os.path.join(crossCheckDir, "results", fmiVersion, fmiType,                 \
                                     platform, "OMSimulator", omsVersion,                           \
                                     exportingToolID, exportingToolVersion, modelName)
            importFMU(crossCheckDir, testFMUDir, resultDir, modelName, fmiType, omsimulator)

  # Get total time
  totalTime = (time.time() - t_sec)
  print("Time needed for test: " + time.strftime("%M:%S", time.gmtime(totalTime)))


#
# Start point for script
#
if __name__ == "__main__":
  if not len(sys.argv) >= 4 or sys.argv[1].lower() == "--help":
    print("Usage: python3 importFMUs.py /path/to/fmi-cross-check/repo FMIPlatform path/to/OMSimulator [OPTIONS]\n")
    print("Example: python3 importFMUs.py fmi-cross-check \"linux64\" \"OpenModelica/build/bin/OMSimulator\" \n")
    quit()

  # Get OMSimulator and version
  omsimulator = str(sys.argv[3])
  if "wine" in omsimulator:
    (tmp1, tmp2) = omsimulator.split(" ",1)
    if shutil.which(tmp1) is None:
      raise Exception("Can't find \""+ tmp1 + "\"")
    tmp2 = tmp2.replace('\\ ', " ")
    if shutil.which(tmp2) is None:
      raise Exception("Can't find \""+ tmp1 + "\"")
    print(tmp2)
    tmp2 = os.path.abspath(tmp2)
    omsimulator = [tmp1, tmp2]
  else:
    if shutil.which(omsimulator) is None:
      raise Exception("Can't find \""+ omsimulator + "\"")
    omsimulator = [os.path.abspath(shutil.which(omsimulator))]

  tmpCall = omsimulator.copy()
  tmpCall.append('--version')
  omsVersion = subprocess.run(tmpCall, stdout=subprocess.PIPE).stdout.decode()
  omsVersion = omsVersion.replace("OMSimulator ", "").replace("\n", "")

  print("Using OMSimulator from \"" + ' '.join(omsimulator) + "\" with version \"" + omsVersion + "\"")

  # Change working dir to fmi-cross-check repo
  crossCheckDir = os.path.abspath(str(sys.argv[1]))
  os.chdir(crossCheckDir)
  print("Set working directory to \"" + crossCheckDir + "\"")
  if not (os.path.isdir("fmus") or os.path.isdir("results")):
    print("Can't find \"fmus\" or \"results\" directories in \"" + crossCheckDir + "\".\n Is this /fmi-cross-check from \"https://github.com/modelica/fmi-cross-check/\"?")
    raise Exception("Error: Not in fmi-cross-check repo!")

  # Get platform string
  platform = str(sys.argv[2])
  if not platform in ["linux64", "linux32", "win64", "win32", "darwin64", "c-code"]:
    raise Exception("Unknown 2nd arguemnt platform: \"" + platform + "\"")

  if "post" in omsVersion:
    omsVersion = omsVersion.split("post")[0]
    omsVersion = omsVersion[0:-1]

  # Get optional configurations
  for conf in sys.argv[4:]:
    if conf.lower() == "debugprint":
      debugPrint = True
    elif conf.lower() == "ignorenotnompliantwithtrules":
      ignoreNotCompliantWithLatestRules = True
    elif len(conf.split("=")) == 2:
      (name, arg) = conf.lower().split("=")
      if name == "debugprint":
        debugPrint = arg == True
      elif name == "ignorenotnompliantwithtrules":
        ignoreNotCompliantWithLatestRules = arg == True
      elif name == "omsTimeOut":
        ulimitOMSimulator = float(arg)
        if ulimitOMSimulator <= 0:
          Exception("Invalid value \""+ arg + "\" for flag \"omsTimeOut\"")
      elif name == "deftol":
        default_tolerance = float(arg)
        if default_tolerance <= 0:
          Exception("Invalid value \""+ arg + "\" for flag \"defTol\"")
      elif name == "reltoldiffminmax":
        reltolDiffMinMax = float(arg)
        if reltolDiffMinMax <= 0:
          Exception("Invalid value \""+ arg + "\" for flag \"reltolDiffMinMax\"")
      elif name == "rangedelta":
        rangeDelta = float(arg)
        if rangeDelta <= 0:
          Exception("Invalid value \""+ arg + "\" for flag \"rangeDelta\"")
      else:
        Exception("Unknown flag \"" + name + "\"")
    else:
      Exception("Unknown flag \"" + conf + "\"")

  config = (debugPrint, ignoreNotCompliantWithLatestRules, ulimitOMSimulator, default_tolerance, reltolDiffMinMax, rangeDelta)
  print("Using config (debugPrint, ignoreNotCompliantWithLatestRules, omsTimeOut, defTol, reltolDiffMinMax, rangeDelta) = " + str(config) + "\n")

  # Run import
  simulateWithOMSimulator(crossCheckDir, platform, omsimulator, omsVersion)

  print("Tested OMSimulator version:")
  print(omsVersion)
