import os
import sys
import subprocess
import time
import shutil
import pandas as pd
from OMPython import OMCSessionZMQ

# Global variables
ignoreNotCompliantWithLatestRules = True

ulimitOMSimulator = 60
default_tolerance = 1e-8
reltolDiffMinMax = 1e-4
rangeDelta = 0.002

htmlResultDir = "html"
filesDir = "files"

def generateOverviewHTML(crossCheckDir, platform, omsVersion, omsVersionShort, timeStart, totalTime, sysInfo):

  # Remove html directory
  shutil.rmtree(htmlResultDir, ignore_errors=True)

  # Generate data frame
  result_df = constructDF(crossCheckDir, platform, omsVersionShort)

  # Create directories for HTML files
  for dirs in [htmlResultDir, os.path.join(htmlResultDir,filesDir)]:
    if not os.path.exists(dirs):
      os.mkdir(dirs)

  # Read result data frame
  numFmus = len(result_df["OMS can import FMU"])
  fmusSimulated = list(result_df["OMS can import FMU"]).count(True)
  fmusVerified = list(result_df["Results Correct"]).count(True)

  # Generate result table
  resultsTable = ""
  for index, row in result_df.iterrows():
    fmiVersion = row["FMI Version"]
    fmiType = row["FMI Type"]
    exportingToolID = row["Exporting Tool"]
    exportingToolVersion = row["Exporting Tool Version"]
    modelName = row["Model Name"]
    testFMUDir = os.path.join(crossCheckDir, "fmus", fmiVersion, fmiType, platform,         \
                              exportingToolID, exportingToolVersion, modelName)
    resultDir = os.path.join(crossCheckDir, "results", fmiVersion, fmiType,                 \
                              platform, "OMSimulator", omsVersionShort,                     \
                              exportingToolID, exportingToolVersion, modelName)
    fileName = row["FMI Version"].replace(".", "") + "."              \
             + row["FMI Type"] + "."                                  \
             + row["Exporting Tool"].replace(".", "_") + "."          \
             + row["Exporting Tool Version"].replace(".", "_") + "."  \
             + row["Model Name"]

    # Create directory for model
    modelDir = os.path.join(htmlResultDir, filesDir, fileName)
    if not os.path.exists(modelDir):
      os.mkdir(modelDir)

    modelLog = generateSimLogHTML(row, testFMUDir ,resultDir, modelDir, fileName)

    if row["OMS can import FMU"]:
      simulated = "<td bgcolor=\"#SimulationColor#\">Yes</td>"
    else:
      simulated = "<td bgcolor=\"#FFCC66\">No</td>"

    if not row["OMS can import FMU"]:
      verified = "<td bgcolor=\"#FFCC66\"></td>"
    elif row["Results Correct"]:
      verified =  "<td bgcolor=\"#VerificationColor#\">Yes</td>"
    else:
      linkTarget = generateFailedVerificationHTML(row, testFMUDir, resultDir, modelDir, fileName)
      vailedVars = str(len(row["Failed Variables"]))
      totalVars = str(int(row["Toal Variables"]))
      verified = "<td bgcolor=\"#FFCC66\">"                 \
               + "<a href=\"" + linkTarget + "\">"          \
               + vailedVars+"/"+totalVars+" failed</td>"    \
               + "</a>"                                     \
               + "</td>\n"
      # TODO Add verivifaction time

    simTime = str(round(row["Simulation Time"],2))

    row = "      <tr>\n"                                                \
        + "        <td>" + fmiVersion + "</td>\n"                       \
        + "        <td>" + fmiType + "</td>\n"                          \
        + "        <td>" + exportingToolID + "</td>\n"                  \
        + "        <td>" + exportingToolVersion + "</td>\n"             \
        + "        <td>" + modelLog + "</td>\n"                         \
        + "        " + verified + "\n"                                  \
        + "        " + simulated + "\n"                                 \
        + "        <td>" + simTime + "</td>\n"                          \
        + "      </tr>\n"
    resultsTable = resultsTable + row

  commitshort = subprocess.check_output(['git', 'rev-parse', '--short', 'HEAD'], cwd=crossCheckDir).decode('utf-8')
  commitfull = subprocess.check_output(['git', 'rev-parse', 'HEAD'], cwd=crossCheckDir).decode('utf-8')

  htmltpl=open("fmi-cross-check.html.tpl").read()

  # Replace keywords from HTML template
  htmltpl = htmltpl.replace("#Total#", str(numFmus))
  htmltpl = htmltpl.replace("#Simulation#", str(fmusSimulated))
  htmltpl = htmltpl.replace("#Verification#", str(fmusVerified))

  htmltpl = htmltpl.replace("#commitshort#", str(commitshort))
  htmltpl = htmltpl.replace("#commitfull#", str(commitfull))

  htmltpl = htmltpl.replace("#totalTime#", time.strftime("%M:%S", time.gmtime(totalTime)))
  htmltpl = htmltpl.replace("#sysInfo#", sysInfo)
  htmltpl = htmltpl.replace("#omsVersion#", omsVersion)
  htmltpl = htmltpl.replace("#timeStart#", str(timeStart))
  htmltpl = htmltpl.replace("#ulimitOMSimulator#", str(ulimitOMSimulator))
  htmltpl = htmltpl.replace("#default_tolerance#", str(default_tolerance))
  omc = OMCSessionZMQ()
  omcVersion = omc.sendExpression("getVersion()")
  htmltpl = htmltpl.replace("#referenceTool#", omcVersion + " with function diffSimulationResults")

  htmltpl = htmltpl.replace("      #testsHTML#", resultsTable)

  resultHTML = os.path.join(htmlResultDir,"fmi-cross-check.html")
  open(resultHTML, "w").write(htmltpl)

  # Copy dygraph.js into html/
  shutil.copyfile("dygraph.js", os.path.join(htmlResultDir,"dygraph.js"))

  return resultHTML


def constructDF(crossCheckDir, platform, omsVersion):

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
          for modelName in os.listdir(fmusDir):
            # Skipt not compliant FMUs
            if ignoreNotCompliantWithLatestRules:
              if os.path.isfile(os.path.join(crossCheckDir, "fmus", fmiVersion, fmiType, platform, exportingToolID, exportingToolVersion, modelName, "notCompliantWithLatestRules")):
                continue
            
            testFMUDir = os.path.join(crossCheckDir, "fmus", fmiVersion, fmiType, platform,         \
                                      exportingToolID, exportingToolVersion, modelName)
            resultDir = os.path.join(crossCheckDir, "results", fmiVersion, fmiType,                 \
                                     platform, "OMSimulator", omsVersion,                           \
                                     exportingToolID, exportingToolVersion, modelName)

            # Check for result file
            canSimulate = os.path.isfile(os.path.join(resultDir, modelName + "_out.csv"))

            output = open(os.path.join(resultDir, "OMSimulator_out.log"), "r").read()
            error = open(os.path.join(resultDir, "OMSimulator_err.log"), "r").read()

            resultsCorrect = os.path.isfile(os.path.join(resultDir, "passed"))

            failedVars = []
            for f in os.listdir(resultDir):
               if ".diff." in f:
                  failedVars.append(f.split(".diff.")[1].split(".csv")[0])

            csv_ref = pd.read_csv(os.path.join(testFMUDir, modelName + "_ref.csv"))
            numRefVars = len(list(csv_ref.columns.values))-1

            experimentSettings = open(os.path.join(resultDir, "OMSimulator_exp.log"), "r").read()
            simTime = experimentSettings.split("Simulation Time: ")[1].split("\n")[0]
            startTime = experimentSettings.split("Start Time: ")[1].split("\n")[0]
            stopTime = experimentSettings.split("Stop Time: ")[1].split("\n")[0]
            relTol = experimentSettings.split("Relative Tolerance: ")[1].split("\n")[0]

            df = df.append({
              "FMI Version": fmiVersion,
              "FMI Type": fmiType,
              "Exporting Tool": exportingToolID,
              "Exporting Tool Version": exportingToolVersion,
              "Model Name": modelName,
              "OMS can import FMU": canSimulate,
              "Output": output,
              "Error": error,
              "Results Correct": resultsCorrect,
              "Failed Variables": failedVars,
              "Toal Variables": numRefVars,
              "Simulation Time": float(simTime),
              "Start Time": startTime,
              "Stop Time": stopTime,
              "Tolerance": relTol
            }, ignore_index=True)

  df.sort_values(by=["Exporting Tool", "Exporting Tool Version", "FMI Version", "FMI Type", "Model Name"])
  return df


def generateFailedVerificationHTML(row, testFMUDir, resultDir, modelDir, fileName):

  htmlVarList = ""
  failedVars = row["Failed Variables"]
  if len(failedVars) > 0:
    htmlVarList = "    <ul>"
  for var in failedVars:
    htmlVarList = htmlVarList                                                                           \
                + "      <li>" + var + "\n"                                                         \
                + "        <a href=\"" + fileName + ".diff." + var + ".html\">(javascript)</a>\n"   \
                + "        <a href=\"" + fileName + ".diff." + var + ".csv\">(csv)</a>\n"           \
                + "      </li>\n"
    generateFailedVarHTML(row, testFMUDir, resultDir, fileName, var)

  if len(failedVars) > 0:
    htmlVarList = htmlVarList + "    </ul>\n"

  htmlStr = "<html>\n"                                                            \
          + "  <body>\n"                                                          \
          + "    <h1>" + fileName + "</h1>"                                       \
          + "    <p>Start time: " + str(row["Start Time"]) + "</p>"               \
          + "    <p>Stop time: " + str(row["Stop Time"]) + "</p>"                 \
          + "    <p>Simulated using tolerance: " + str(row["Tolerance"]) + "</p>" \
          + htmlVarList                                                           \
          + "   </body>\n"                                                        \
          + "</html>"

  # Write html file
  open(os.path.join(modelDir, fileName + ".html"), "w").write(htmlStr)

  return os.path.join(filesDir, fileName, fileName + ".html")

def generateFailedVarHTML(row, testFMUDir, resultDir, fileName, var):

  # Copy csv file for var to html/files/fileName
  csvFilePath = os.path.join(resultDir, row["Model Name"] + ".diff." + var + ".csv")
  csvFileName = fileName + ".diff." + var + ".csv"
  shutil.copyfile(csvFilePath, os.path.join(htmlResultDir, filesDir, fileName, csvFileName))

  # Generate html
  htmltpl=open("failedVar.html.tpl").read()

  htmltpl = htmltpl.replace("#RelTol#", str(row["Tolerance"]))
  htmltpl = htmltpl.replace("#MaxMin#", str(reltolDiffMinMax))
  htmltpl = htmltpl.replace("#RangeDelta#", str(rangeDelta))
  htmltpl = htmltpl.replace("#CSVFile#", csvFileName)
  htmltpl = htmltpl.replace("#FailedVar#", var)

  open(os.path.join(htmlResultDir, filesDir, fileName, fileName + ".diff." + var + ".html"), "w").write(htmltpl)

def generateSimLogHTML(row, testFMUDir, resultDir, modelDir, fileName):

  modelName = row["Model Name"]
  logHtml = fileName + ".log.html"
  linkHTML = "<a href=\"" + os.path.join(filesDir, fileName, logHtml) + "\">" + modelName + "</a>"

  # Parse README to html/files/model
  readmeFile = os.path.join(resultDir, "README.md")

  readmeContent=open(readmeFile).read()
  infoLog = row["Output"]
  errorLog = row["Error"]

  content = """
<!DOCTYPE html>
<html>
  <body>
    <h1>Simulation setup</h1>
    <textarea cols="180" rows="%i">%s</textarea>
    <h1>Stdout:</h1>
    <textarea cols="180" rows="%i">%s</textarea>
    <h1>Stderr:</h1>
    <textarea cols="180" rows="%i">%s</textarea>
  </body>
</html>
""" % (readmeContent.count("\n")+1, readmeContent, infoLog.count("\n")+1, infoLog, errorLog.count("\n")+1, errorLog)

  open(os.path.join(modelDir, logHtml), "w").write(content)

  return linkHTML


#
# Start point for script
#
if __name__ == "__main__":
  if not len(sys.argv) >= 4 or sys.argv[1].lower() == "--help":
    print("Usage: python3 generateHTML.py /path/to/fmi-cross-check/repo FMIPlatform OMSimulatorVersion OMSimulatorVersionShort sysInfo startTime testDurationInSec [OPTIONS]\n")
    print("Example: python3 generateHTML.py fmi-cross-check \"linux64\" \"v2.1.0-dev-211-g1b24316-linux\" \"v2.1.0\" \"Ubunut 18.04 TLS\" \"14:00:00\" 15462\n")
    quit()

  # Check path to fmi-cross-check repo is correct
  crossCheckDir = os.path.abspath(str(sys.argv[1]))
  if not (os.path.isdir(os.path.join(crossCheckDir,"fmus")) or os.path.isdir(os.path.join(crossCheckDir,"results"))):
    print("Can't find \"fmus\" or \"results\" directories in \"" + crossCheckDir + "\".\n Is this /fmi-cross-check from \"https://github.com/modelica/fmi-cross-check/\"?")
    raise Exception("Error: Not in fmi-cross-check repo!")

  # Get platform string
  platform = str(sys.argv[2])
  if not platform in ["linux64", "linux32", "win64", "win32", "darwin64", "c-code"]:
    raise Exception("Unknown 2nd arguemnt platform: \"" + platform + "\"")

  # Get OMS version
  omsVersion = str(sys.argv[3])
  omsVersionShort = str(sys.argv[4])

  # Get System info
  sysInfo = str(sys.argv[5])

  # Get start time
  timeStart = str(sys.argv[6])

  # Get test duration in seconds
  totalTime = float(sys.argv[7])

  # Get optional configurations
  for conf in sys.argv[8:]:
    if conf.lower() == "ignorenotnompliantwithtrules":
      ignoreNotCompliantWithLatestRules = True
    elif len(conf.split("=")) == 2:
      (name, arg) = conf.lower().split("=")
      if name == "ignorenotnompliantwithtrules":
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

  config = (ulimitOMSimulator, default_tolerance, reltolDiffMinMax, rangeDelta)
  print("Using config (omsTimeOut, defTol, reltolDiffMinMax, rangeDelta) = " + str(config) + "\n")

  generateOverviewHTML(crossCheckDir, platform, omsVersion, omsVersionShort, timeStart, totalTime, sysInfo)
