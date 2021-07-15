#!/usr/bin/env python3

import argparse
import os
import shutil
import subprocess
import tempfile

import pandas as pd

__author__ = 'Lennart Ochel <lennart.ochel@ri.se>'

VERBOSE = False

def verify_result(modelName, resultsDir, fmuDir, omc):
  if os.path.isfile(os.path.join(resultsDir, 'passed')):
    return
  if os.path.isfile(os.path.join(resultsDir, 'rejected')):
    return
  if os.path.isfile(os.path.join(resultsDir, 'failed')):
    return

  res_out = os.path.join(resultsDir, modelName + '_out.csv')
  print('  * verifying results')

  if os.path.getsize(res_out) > 100*1024*1024:
    print('  * result file too big')
    print('  -> failed')
    with open(os.path.join(resultsDir, 'failed'), 'w') as f:
      f.write('result file too big\n')
    return

  # fix variable names
  with open(res_out) as f:
    lines = f.readlines()
  lines[0] = lines[0].replace('fmu.', '')
  with open(res_out, 'w') as f:
    f.writelines(lines)

  ref_csv = os.path.join(fmuDir, modelName + '_ref.csv')
  if not os.path.isfile(ref_csv):
    print('  * Reference results not found')
    print('  -> rejected')
    with open(os.path.join(resultsDir, 'rejected'), 'w') as f:
      f.write('Reference results not found\n')
    return

  with open(os.path.join(resultsDir, 'compare_results.mos'), 'w') as f:
    f.write(f"cd(\"{resultsDir}\");")
    f.write(f"(success, vars) := diffSimulationResults(\"{modelName}_out.csv\", \"../../../../../../../../../{fmuDir}/{modelName}_ref.csv\", \"{modelName}.diff\", relTol=1e-4, relTolDiffMinMax=1e-4, rangeDelta=0.002);\n")
    f.write("\n")
    f.write("err_str := getErrorString();\n")
    f.write("if err_str <> \"\" then\n")
    f.write("  writeFile(\"failed\", err_str);\n")
    f.write("  exit(1);\n")
    f.write("end if;\n")
    f.write("\n")
    f.write("if not success then\n")
    f.write("  writeFile(\"failed\", sum(\"Signal '\" + v + \"' doesn't match reference results\\n\" for v in vars));\n")
    f.write("  exit(1);\n")
    f.write("else\n")
    f.write("  writeFile(\"passed\", \"\");\n")
    f.write("end if;\n")

  proc = subprocess.Popen([omc, os.path.join(resultsDir, 'compare_results.mos')], stdout=subprocess.PIPE, stderr=subprocess.PIPE)
  (out, err) = proc.communicate()
  exitCode = proc.returncode

  if os.path.isfile(os.path.join(resultsDir, 'passed')):
    print('  -> passed')
  if os.path.isfile(os.path.join(resultsDir, 'rejected')):
    print('  -> rejected')
  if os.path.isfile(os.path.join(resultsDir, 'failed')):
    print('  -> failed')

def executeLuaScript(luaScriptPath, resultDir, modelName, oms):
  cmd = [oms,
         f'--workingDir={resultDir}',
         '--stripRoot=true',
         '--skipCSVHeader=true',
         '--addParametersToCSV=true',
         '--suppressPath=true',
         '--timeout=60',
         modelName + '.lua']

  # call OMSimulator and measure time
  print(f'  * {" ".join(cmd)}')
  proc = subprocess.Popen(cmd, stdout=subprocess.PIPE, stderr=subprocess.PIPE)
  (out, err) = proc.communicate()
  exitCode = proc.returncode

  with open(luaScriptPath, 'r') as luaFile, open(os.path.join(resultDir, 'README.md'),'a') as readme:
    readme.write(f'The following command and script was used to simulate `{modelName}.fmu`:\n')
    readme.write('```bash\n')
    readme.write('> ' + ' '.join(cmd) + '\n')
    readme.write('```\n\n')
    readme.write(f'{modelName}.lua:\n')
    readme.write('```lua\n')
    # read content from first file
    for line in luaFile:
      # append content to second file
      readme.write(line)
    readme.write('```\n')
    readme.write('See the [OMSimulator documentation](https://openmodelica.org/doc/OMSimulator/master/html/index.html) for more information.\n\n')

    if VERBOSE:
      readme.write('Stdout:\n')
      readme.write('```\n')
      readme.write(out.decode())
      readme.write('```\n\n')
      readme.write('Stderr:\n')
      readme.write('```\n')
      readme.write(err.decode())
      readme.write('```\n\n')

  if exitCode != 0:
    print(f'  * exit code {exitCode}')
    print('  -> failed')
    with open(os.path.join(resultDir, 'failed'), 'w') as f:
      f.write(err.decode())
    return

  # check if result file was generated
  if not os.path.isfile(os.path.join(resultDir, f'{modelName}_out.csv')):
    print('  * no result file generated')
    print('  -> failed')
    with open(os.path.join(resultDir, 'failed'), 'w') as f:
      f.write(err.decode())
    return

def generateLuaScript(modelName, fmuDir, resultDir, fmiType):
  # Get some paths
  fmuPath = os.path.join(fmuDir, modelName + '.fmu')

  startTime = 0.0
  stopTime = 1.0
  relTol = 1e-6
  absTol = 1e-6
  inputCSV = os.path.join(fmuDir, modelName + '_in.csv')
  refOptFile = os.path.join(fmuDir, modelName + '_ref.opt')

  df = pd.read_csv(refOptFile, delimiter=',', index_col=0, header=None)

  if 'StartTime' in df.index:
    startTime = df.loc['StartTime', 1]

  if 'StopTime' in df.index:
    stopTime = df.loc['StopTime', 1]

  if 'RelTol' in df.index:
    if not df.loc['RelTol', 1] == 0:
      relTol = df.loc['RelTol', 1]

  if 'AbsTol' in df.index:
    if not df.loc['AbsTol', 1] == 0:
      absTol = df.loc['AbsTol', 1]

  stepSize = (stopTime - startTime) / 500
  if 'StepSize' in df.index:
    if not df.loc['StepSize', 1] == 0:
      stepSize = df.loc['StepSize', 1]

  outputIntervalLength = (stopTime - startTime) / 500
  if 'OutputIntervalLength' in df.index:
    if not df.loc['OutputIntervalLength', 1] == 0:
      outputIntervalLength = df.loc['OutputIntervalLength', 1]

  # input file
  inputs = []
  if os.path.isfile(inputCSV):
    df = pd.read_csv(inputCSV, delimiter=',')
    inputs = list(df.columns.values[1:])
    print('  * test has inputs:{}'.format(inputs))

  # system type
  if fmiType == 'me':
    systemType = 'oms_system_sc'
  elif fmiType == 'cs':
    systemType = 'oms_system_wc'
  else:
    print(f'  * Unsupportet FMU type {fmiType}')
    return ''

  tempDir = os.path.join(tempfile.gettempdir(), 'cross-check')
  tempDir = tempDir.replace('\\', '\\\\')
  luaScriptPath = os.path.join(resultDir, modelName + '.lua')
  f = open(luaScriptPath, 'w')

  f.write('-- lua file for ' + modelName + '.fmu\n')
  f.write('oms_setTempDirectory(\''+tempDir+'\')\n')
  f.write('oms_newModel(\'model\')\n')
  f.write('oms_addSystem(\'model.root\', ' + systemType + ')\n')

  f.write('\n-- instantiate FMU\n')
  f.write('oms_addSubModel(\'model.root.fmu\', \'../../../../../../../../../' + fmuPath + '\')\n')
  if len(inputs) > 0:
    f.write('oms_addSubModel(\'model.root.input\', \'../../../../../../../../../' + inputCSV + '\')\n')

  if len(inputs) > 0:
    f.write('\n-- connect inputs to FMU\n')
    for input in inputs:
      f.write('oms_addConnection(\'model.root.input.' + input + '\', \'model.root.fmu.' + input + '\')\n')

  f.write('\n-- simulation settings\n')
  f.write('oms_setResultFile(\'model\', \'' + modelName + '_out.csv\')\n')
  f.write(f'oms_setLoggingInterval(\'model\', {outputIntervalLength})\n')
  f.write(f'oms_setStartTime(\'model\', {startTime})\n')
  f.write(f'oms_setStopTime(\'model\', {stopTime})\n')
  f.write(f'oms_setTolerance(\'model\', {absTol}, {relTol})\n')
  if fmiType == 'me':
    f.write(f'oms_setVariableStepSize(\'model\', 1e-12, 1e-12, {stepSize})\n')
  elif fmiType == 'cs':
    f.write(f'oms_setFixedStepSize(\'model\', {stepSize})\n')

  f.write('\n-- instantiate, initialize and simulate\n')
  f.write('oms_instantiate(\'model\')\n')
  f.write('oms_initialize(\'model\')\n')
  f.write('oms_simulate(\'model\')\n')
  f.write('oms_terminate(\'model\')\n')
  f.write('oms_delete(\'model\')\n')
  f.close()

  return luaScriptPath

def runTest(modelName, fmuDir, resultDir, fmiType, oms, omc):
  fmuPath = os.path.join(fmuDir, modelName + '.fmu')
  print('Testing {}'.format(fmuPath))

  os.makedirs(resultDir)

  if not os.path.isfile(fmuPath):
    print('  * FMU not found')
    print('  -> rejected')
    with open(os.path.join(resultDir, 'rejected'), 'w') as f:
      f.write('FMU not found\n')
    return

  luaScriptPath = generateLuaScript(modelName, fmuDir, resultDir, fmiType)
  if not luaScriptPath:
    print('  -> skipped')
    return

  executeLuaScript(luaScriptPath, resultDir, modelName, oms)

  # verify results
  verify_result(modelName, resultDir, fmuDir, omc)

def _main():
  # parse command-line arguments
  parser = argparse.ArgumentParser(description='fmi-cross-check with OMSimulator', allow_abbrev=False)
  parser.add_argument('--root', default='..', type=str, help='Root directory of the fmi-cross-check repository')
  parser.add_argument('--version', default='v2.1.1', type=str, help='OMSimulator version')
  parser.add_argument('--platform', default='win64', type=str, help='Platform to test')
  parser.add_argument('--oms', default='.omsimulator/OMSimulator-mingw64-v2.1.1/bin/OMSimulator.exe', type=str, help='Path to OMSimulator')
  parser.add_argument('--omc', default='omc', type=str, help='Path to omc')
  args = parser.parse_args()

  # make sure we are in the fmi-cross-check directory
  rootDir = args.root
  os.chdir(rootDir)

  # clean up temp directory
  tempDir = os.path.join(tempfile.gettempdir(), 'cross-check')
  shutil.rmtree(tempDir, ignore_errors=True)

  # clean up previous results
  for fmiVersion in ['2.0']:
    for fmiType in ['me', 'cs']:
      path = os.path.join('results', fmiVersion, fmiType, args.platform, 'OMSimulator', args.version)
      shutil.rmtree(path, ignore_errors=True)

  # generate new results
  for fmiVersion in ['2.0']:
    for fmiType in ['me', 'cs']:
      toolsDir = os.path.join('fmus', fmiVersion, fmiType, args.platform)
      for exportingToolID in os.listdir(toolsDir):
        versionsDir = os.path.join(toolsDir, exportingToolID)
        for exportingToolVersion in os.listdir(versionsDir):
          fmusDir = os.path.join(versionsDir, exportingToolVersion)
          for modelName in os.listdir(fmusDir):
            fmuDir = os.path.join('fmus', fmiVersion, fmiType, args.platform, exportingToolID, exportingToolVersion, modelName)
            resultDir = os.path.join('results', fmiVersion, fmiType, args.platform, 'OMSimulator', args.version, exportingToolID, exportingToolVersion, modelName)
            if os.path.isfile(os.path.join(fmuDir, 'notCompliantWithLatestRules')):
              print('Skipping {} because of notCompliantWithLatestRules file'.format(fmuDir))
              continue
            runTest(modelName, fmuDir, resultDir, fmiType, args.oms, args.omc)
            if os.path.exists(os.path.join(resultDir, modelName + '.lua')):
              os.remove(os.path.join(resultDir, modelName + '.lua'))
            if os.path.exists(os.path.join(resultDir, 'compare_results.mos')):
              os.remove(os.path.join(resultDir, 'compare_results.mos'))
            files_in_directory = os.listdir(resultDir)
            filtered_files = [file for file in files_in_directory if file.endswith(".csv")]
            for file in filtered_files:
              path_to_file = os.path.join(resultDir, file)
              os.remove(path_to_file)

if __name__ == '__main__':
  _main()
