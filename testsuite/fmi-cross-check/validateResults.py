import os
import sys
import pandas as pd

from OMPython import OMCSessionZMQ
# Start omc
omc = OMCSessionZMQ()
omc.sendExpression('getVersion()')

# Global vars
ignoreNotCompliantWithLatestRules = True

default_tolerance = 1e-8
reltolDiffMinMax = 1e-4
rangeDelta = 0.002

def filterResultFile(resultFile, referenceFile, resultDir):
  """Filter result file from OMSimulator to contain same variables as reference file.
  Remove prefixes from varaible names.
  """
  # Only save columns from fmu and remove ".fmu" and "\"" from variable names
  csv_res = pd.read_csv(resultFile)
  fmu_vars = [s for s in list(csv_res.columns.values) if(s.startswith(" \"fmu.") or s == "time")]
  csv_res = csv_res[fmu_vars]

  fmu_vars = list(map(lambda x: x.replace('fmu.','').replace('"','').replace(' ',''),fmu_vars))
  csv_res.columns = fmu_vars

  # Save only variables from reference file
  csv_ref = pd.read_csv(referenceFile)
  refVariables = list(csv_ref.columns.values)

  filterLog = ""
  toRemove = []
  varsMissing = False
  for var in refVariables:
    if not var in csv_res:
      varsMissing = True
      filterLog = filterLog + "Result Diff: Variable \"" + var + "\" not found in result file but is in reference file.\\n"
      toRemove.append(var)
  refVariables = [elem for elem in refVariables if elem not in toRemove]

  csv_res = csv_res[refVariables]

  # Save filter log to file
  if varsMissing:
    open(os.path.join(resultDir, "FilterResults.log"), "w").write(filterLog)

  # Update result file
  csv_res.to_csv(resultFile, index = False, header=True)

  return varsMissing



def diffSimulationResults(resultFile, referenceFile, modelName, testFMUDir, resultDir, relTol, absTol):
  """Check results with OpenModelica's diffSimulationResults function.
  Will generate a csv file for each failed variable.
  Return if results are correct or not.
  """

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

  return resultsCorrect


def validateResult(testFMUDir, resultDir, modelName, fmiType):
  """Validate generated OMSimulator results
  Filter result files to only contain reference variables.
  Check simulation results with OpenModelica and diffSimulationResults
  """

  resultFile = os.path.join(resultDir, modelName + "_out.csv")
  referenceResult = os.path.join(testFMUDir, modelName + "_ref.csv")

  print("FMU: " + os.path.join(testFMUDir, modelName + ".fmu"))

  # Check if resultFile was generated
  if not os.path.isfile(resultFile):
    if os.path.isfile(os.path.join(testFMUDir,"notCompliantWithLatestRules")):
      open(os.path.join(resultDir,"rejected"), "w").close()
      print("\t REJECTED")
    else:
      open(os.path.join(resultDir,"failed"), "w").close()
      print("\t FAILED")
    return

  # Filter result file and check if all variables are in output file
  varsMissing = filterResultFile(resultFile, referenceResult, resultDir)
  if varsMissing:
    if os.path.isfile(os.path.join(testFMUDir,"notCompliantWithLatestRules")):
      open(os.path.join(resultDir,"rejected"), "w").close()
      print("\t REJECTED")
    else:
      open(os.path.join(resultDir,"failed"), "w").close()
      print("\t FAILED")
    return

  # Check results
  if os.path.isfile(resultFile):
    resultsCorrect = diffSimulationResults(resultFile, referenceResult, modelName, testFMUDir, resultDir, 0.1, 0.1)

  if resultsCorrect:
    open(os.path.join(resultDir,"passed"), "w").close()
    print("\t PASSED")
  else:
    if os.path.isfile(os.path.join(testFMUDir,"notCompliantWithLatestRules")):
      open(os.path.join(resultDir,"rejected"), "w").close()
      print("\t REJECTED")
    else:
      open(os.path.join(resultDir,"failed"), "w").close()
      print("\t FAILED")


def validateGeneratedResults(crossCheckDir, platform, omsVersion):
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
            validateResult(testFMUDir, resultDir, modelName, fmiType)


#
# Start point for script
#
if __name__ == "__main__":
  if not len(sys.argv) >= 4 or sys.argv[1].lower() == "--help":
    print("Usage: python3 validateResults.py /path/to/fmi-cross-check/repo FMIPlatform OMSimulatorVersion [OPTIONS]\n")
    print("Example: python3 validateResults.py fmi-cross-check \"linux64\" \"v2.1.0-dev-211-g1b24316-linux\" \n")
    quit()

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

  # Get OMSimulator version
  omsVersion = str(sys.argv[3])
  if "dev" in omsVersion:
    omsVersion = omsVersion.split("dev")[0]
    omsVersion = omsVersion[0:-1]

  # Get optional configurations
  for conf in sys.argv[4:]:
    if conf.lower() == "ignorenotnompliantwithtrules":
      ignoreNotCompliantWithLatestRules = True
    elif len(conf.split("=")) == 2:
      (name, arg) = conf.lower().split("=")
      if name == "ignorenotnompliantwithtrules":
        ignoreNotCompliantWithLatestRules = arg == True
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

  config = (ignoreNotCompliantWithLatestRules, default_tolerance, reltolDiffMinMax, rangeDelta)
  print("Using config (ignoreNotCompliantWithLatestRules, defTol, reltolDiffMinMax, rangeDelta) = " + str(config) + "\n")

  # Run import
  print("Validating FMUs")
  validateGeneratedResults(crossCheckDir, platform, omsVersion)
