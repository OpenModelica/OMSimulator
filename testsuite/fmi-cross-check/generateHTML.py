import os
import time
import shutil
from OMPython import OMCSessionZMQ

# TODO Less global variables...
reltolDiffMinMax = 1e-4
rangeDelta = 0.002

htmlResultDir = "html"
filesDir = "files"

def generateOverviewHTML(result_df, crossCheckDir, platform, timeStart, totalTime, sysInfo, omsVersion, omsVersionShort, ulimitOMSimulator, default_tolerance):

  # Remove html directory
  shutil.rmtree(htmlResultDir, ignore_errors=True)

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


  htmltpl=open("fmi-cross-check.html.tpl").read()

  # Replace keywords from HTML template
  htmltpl = htmltpl.replace("#Total#", str(numFmus))
  htmltpl = htmltpl.replace("#Simulation#", str(fmusSimulated))
  htmltpl = htmltpl.replace("#Verification#", str(fmusVerified))

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

  open(os.path.join(htmlResultDir,"fmi-cross-check.html"), "w").write(htmltpl)


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
