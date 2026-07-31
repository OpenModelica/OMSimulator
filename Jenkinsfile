pipeline {
  agent none
  options {
    newContainerPerStage()
    buildDiscarder(logRotator(numToKeepStr: "100", artifactNumToKeepStr: "5"))
  }
  parameters {
    booleanParam(name: 'MACOS_ARM64', defaultValue: false, description: 'Build with macOS-arm64 (M1 mac)')
    booleanParam(name: 'SUBMODULE_UPDATE', defaultValue: false, description: 'Allow pull request to update submodules (disabled by default due to common user errors)')
    booleanParam(name: 'UPLOAD_BUILD_OPENMODELICA', defaultValue: false, description: 'Upload install artifacts to build.openmodelica.org/omsimulator. Activates MINGW_UCRT64 as well.')
    string(name: 'CTEST_FLAGS', defaultValue: '', description: 'Extra flags passed to ctest, e.g. -R api')
  }
  stages {
    stage('pre-build') {
      parallel {
        stage('check') {
          when {
            changeRequest()
            beforeAgent true
          }
          agent {
            label 'linux'
          }
          steps {
            submoduleNoChange("3rdParty")
          }
        }
        stage('version') {
          agent {
            label 'linux'
          }
          steps {
            script {
              // The build containers have no access to the git reference repository
              // (/var/lib/jenkins/gitcache/OMSimulator.git), so every git command run
              // inside them fails. Resolve the version once here, outside of any
              // container, and hand it to the builds via version.txt / env.OMS_VERSION.
              sh 'git fetch --tags'
              def version = sh(returnStdout: true, script: "git describe --tags --abbrev=7 --match='v*.*' --exclude='*-dev' | sed 's/-/.post/'").trim()
              env.OMS_VERSION = version ?: 'unknown'
            }
            echo "OMSimulator version: ${env.OMS_VERSION}"
          }
        }
      }
    }
    stage('build-in-parallel') {
      parallel {
        stage('linux64-resolute-asan') {
          stages {
            stage('build-asan') {
              agent {
                docker {
                  image 'docker.openmodelica.org/build-deps:ubuntu-26.04-omsimulator'
                  label 'linux'
                  alwaysPull true
                }
              }
              environment {
                HOME = "/tmp/"
              }
              steps {
                buildOMS('-DASAN=ON')
                stash name: 'asan', includes: "install/**"
              }
            }
            stage('test-asan') {
              agent {
                docker {
                  image 'docker.openmodelica.org/build-deps:ubuntu-26.04-omsimulator'
                  label 'linux'
                  alwaysPull true
                  args "--cap-add SYS_PTRACE --privileged " + // Needed for ASAN
                       "--oom-kill-disable -m 1024m --memory-swap 1024m" // Needed for ASAN
                }
              }
              environment {
                // omc-diff is compiled here, and ccache needs a writable home.
                HOME = "/tmp/"
              }
              steps {
                unstash name: 'asan'
                runCTest(true)
                junit 'build-testsuite/ctest-result.xml'
              }
            }
          }
        }
        stage('linux64-noble') {
          agent {
            docker {
              image 'docker.openmodelica.org/build-deps:ubuntu-24.04-omsimulator'
              label 'linux'
              alwaysPull true
            }
          }
          environment {
            NPROC = "${numPhysicalCPU}"
            HOME = "/tmp/"
          }
          steps {
            buildOMS()

            runCTest()
            junit 'build-testsuite/ctest-result.xml'

            sh "(cd install/ && tar czf '../OMSimulator-linux-amd64-${env.OMS_VERSION}.tar.gz' *)"

            sh 'cmake --build build/ --target install-docs'
            sh "(cd install/share/doc && zip -r '../../../OMSimulator-doc-${env.OMS_VERSION}.zip' *)"

            archiveArtifacts artifacts: 'OMSimulator-doc*.zip,OMSimulator-linux-amd64-*.tar.gz', fingerprint: true
            stash name: 'amd64-zip', includes: "OMSimulator-linux-amd64-*.tar.gz"
            stash name: 'docs', includes: "install/share/doc/**"
          }
        }
        stage('linux64-jammy') {
          agent {
            docker {
              image 'docker.openmodelica.org/build-deps:ubuntu-22.04-omsimulator'
              label 'linux'
              alwaysPull true
            }
          }
          environment {
            HOME = "/tmp/"
          }
          steps {
            buildOMS()
            sh "(cd install/ && tar czf '../OMSimulator-linux-jammy-amd64-${env.OMS_VERSION}.tar.gz' *)"

            archiveArtifacts artifacts: 'OMSimulator-linux-jammy-amd64-*.tar.gz', fingerprint: true
            stash name: 'jammy-amd64-zip', includes: "OMSimulator-linux-jammy-amd64-*.tar.gz"
          }
        }
        stage('alpine') {
          agent {
            docker {
              image 'docker.openmodelica.org/build-deps:alpine-3.24-omsimulator'
              label 'linux'
              alwaysPull true
            }
          }
          environment {
            HOME = "/tmp/"
          }
          steps {
            buildOMS()
          }
        }

        stage('arm64-macOS') {
          when {
            expression { return shouldWeBuildMacOSArm64() }
            beforeAgent true
          }
          stages {
            stage('build-M1') {
              agent {
                label 'M1'
              }
              environment {
                PATH="/opt/homebrew/bin:/opt/homebrew/opt/openjdk/bin:/opt/homebrew/opt/icu4c/bin:/opt/homebrew/opt/icu4c/sbin:/usr/local/bin:${env.PATH}"
                PKG_CONFIG_PATH="/opt/homebrew/opt/icu4c/lib/pkgconfig"
                LDFLAGS="-L/opt/homebrew/opt/icu4c/lib"
                CPPFLAGS="-I/opt/homebrew/opt/icu4c/include"
              }
              steps {
                buildOMS()
                sh "(cd install/ && zip -r '../OMSimulator-osx-${env.OMS_VERSION}.zip' *)"

                archiveArtifacts "OMSimulator-osx-*.zip"
                stash name: 'osx-zip', includes: "OMSimulator-osx-*.zip"
                stash name: 'osx-install', includes: "install/**"
              }
            }
            stage('test-M1') {
              agent {
                label 'M1'
              }
              steps {
                unstash name: 'osx-install'
                runCTest()
                junit 'build-testsuite/ctest-result.xml'
              }
            }
          }
        }


        stage('mingw-ucrt64-gcc') {
          stages {
            stage('build-mingw-ucrt64-gcc') {
              agent {
                label 'omsimulator-windows'
              }
              environment {
                PATH = "C:\\OMDevUCRT\\tools\\msys\\ucrt64\\bin;C:\\bin\\git\\bin;C:\\bin\\git\\usr\\bin;C:\\Program Files\\Git\\bin;${env.PATH};"
                OMDEV = "/c/OMDevUCRT"
                CC = "gcc"
                CXX = "g++"
                MSYSTEM = "UCRT64"
                VERBOSE = '1'
              }
              steps {
                bat 'hostname'

                buildOMS()

                writeFile file: "buildZip64.sh", text: """#!/bin/sh
                set -x -e
                export PATH="/c/Program Files/TortoiseSVN/bin/:/c/bin/jdk/bin:/c/bin/nsis/:\$PATH:/c/bin/git/bin:/c/Program Files/Git/bin"
                cd "${env.WORKSPACE}/install/"
                zip -r "../OMSimulator-mingw-ucrt64-${env.OMS_VERSION}.zip" *
                """

                bat """
                C:\\OMDevUCRT\\tools\\msys\\usr\\bin\\sh --login -i '${env.WORKSPACE}/buildZip64.sh'
                """

                archiveArtifacts "OMSimulator-mingw-ucrt64-*.zip"
                stash name: 'mingw-ucrt64-zip', includes: "OMSimulator-mingw-ucrt64-*.zip"
                stash name: 'mingw-ucrt64-install', includes: "install/**"
              }
            }
            stage('test-mingw-ucrt64-gcc') {
              agent {
                label 'omsimulator-windows'
              }
              environment {
                PATH = "C:\\OMDevUCRT\\tools\\msys\\ucrt64\\bin;C:\\bin\\git\\bin;C:\\bin\\git\\usr\\bin;C:\\Program Files\\Git\\bin;${env.PATH};"
                OMDEV = "/c/OMDevUCRT"
                MSYSTEM = "UCRT64"
              }
              steps {
                unstash name: 'mingw-ucrt64-install'

                bat 'hostname'
                writeVersionFile()
                writeFile file: "testMinGWUCRT64-install.sh", text:"""#!/bin/sh
set -x -e
cd "${env.WORKSPACE}"
export PATH="/c/Program Files/TortoiseSVN/bin/:/c/bin/jdk/bin:/c/bin/nsis/:\$PATH:/c/bin/git/bin:/c/Program Files/Git/bin"
cmake -S . -B build-testsuite/ -G "MSYS Makefiles" -DCMAKE_BUILD_TYPE=Release -DCMAKE_INSTALL_PREFIX=install/ -DOMS_ENABLE_TESTSUITE:BOOL=ON
ctest --test-dir build-testsuite/ -j\$(nproc) --output-on-failure --output-junit ctest-result.xml ${params.CTEST_FLAGS} || true
test -f build-testsuite/ctest-result.xml
"""
                bat """
If Defined LOCALAPPDATA (echo LOCALAPPDATA: %LOCALAPPDATA%) Else (Set "LOCALAPPDATA=C:\\Users\\OpenModelica\\AppData\\Local")
set PATH=C:\\OMDevUCRT\\bin\\cmake\\bin\\;%PATH%
C:\\OMDevUCRT\\tools\\msys\\usr\\bin\\sh --login -i '${env.WORKSPACE}/testMinGWUCRT64-install.sh'
EXIT /b 0
:fail
ECHO Something went wrong!
EXIT /b 1
"""

                junit 'build-testsuite/ctest-result.xml'
              }
            }
          }
        }

        stage('mingw-ucrt64-clang') {
          agent {
            label 'omsimulator-windows'
          }
          environment {
            PATH = "C:\\OMDevUCRT\\tools\\msys\\ucrt64\\bin;C:\\bin\\git\\bin;C:\\bin\\git\\usr\\bin;C:\\Program Files\\Git\\bin;${env.PATH};"
            OMDEV = "/c/OMDevUCRT"
            VERBOSE = '1'
            CC="clang"
            CXX="clang++"
          }

          steps {
             buildOMS()
          }
        }


        stage('msvc64') {
          stages {
            stage('build-msvc64') {
              agent {
                label 'omsimulator-windows'
              }
              environment {
                PATH = "C:\\OMDevUCRT\\tools\\msys\\ucrt64\\bin;C:\\bin\\git\\bin;C:\\bin\\git\\usr\\bin;C:\\Program Files\\Git\\bin;${env.PATH};"
                OMDEV = "/c/OMDevUCRT"
                MSYSTEM = "UCRT64"
              }
              steps {
                bat 'hostname'
                writeVersionFile()
                writeFile file: "buildZip.sh", text: """#!/bin/sh
set -x -e
export PATH="/c/Program Files/TortoiseSVN/bin/:/c/Program Files/Git/bin/:/c/bin/jdk/bin:/c/bin/nsis/:\$PATH:/c/bin/git/bin:/c/Program Files/Git/bin"
cd "${env.WORKSPACE}/install/"
zip -r "../OMSimulator-win64-${env.OMS_VERSION}.zip" *
"""

                retry(2) { bat """
If Defined LOCALAPPDATA (echo LOCALAPPDATA: %LOCALAPPDATA%) Else (Set "LOCALAPPDATA=C:\\Users\\OpenModelica\\AppData\\Local")
set PATH=C:\\OMDevUCRT\\bin\\cmake\\bin;%PATH%

cmake -S . -B build/ -G "Visual Studio 15 2017 Win64" -DCMAKE_BUILD_TYPE=Release -DCMAKE_INSTALL_PREFIX=install/
IF NOT ["%ERRORLEVEL%"]==["0"] GOTO fail

cmake --build build/ --config Release --parallel %NUMBER_OF_PROCESSORS% --target install -v
IF NOT ["%ERRORLEVEL%"]==["0"] GOTO fail

call install\\bin\\OMSimulator.exe --version
IF NOT ["%ERRORLEVEL%"]==["0"] GOTO fail

C:\\OMDevUCRT\\tools\\msys\\usr\\bin\\sh --login -i '${env.WORKSPACE}/buildZip.sh'

EXIT /b 0

:fail
ECHO Something went wrong!
EXIT /b 1
""" }

                archiveArtifacts "OMSimulator-win64-*.zip"
                stash name: 'win64-zip', includes: "OMSimulator-win64-*.zip"
                stash name: 'win64-install', includes: "install/**"
              }
            }
            stage('test-msvc64') {
              agent {
                label 'omsimulator-windows'
              }
              environment {
                PATH = "C:\\OMDevUCRT\\tools\\msys\\ucrt64\\bin;C:\\bin\\git\\bin;C:\\bin\\git\\usr\\bin;C:\\Program Files\\Git\\bin;${env.PATH};"
                OMDEV = "/c/OMDevUCRT"
                MSYSTEM = "UCRT64"
              }
              steps {
                unstash name: 'win64-install'

                bat 'hostname'
                writeVersionFile()
                // The tests are driven from an MSYS2 shell but exercise the native
                // Windows build, so the platform has to be selected explicitly.
                writeFile file: "testMSVC64-install.sh", text:"""#!/bin/sh
set -x -e
cd "${env.WORKSPACE}"
export PATH="/c/Program Files/TortoiseSVN/bin/:/c/bin/jdk/bin:/c/bin/nsis/:\$PATH:/c/bin/git/bin:/c/Program Files/Git/bin"
cmake -S . -B build-testsuite/ -G "MSYS Makefiles" -DCMAKE_BUILD_TYPE=Release -DCMAKE_INSTALL_PREFIX=install/ -DOMS_ENABLE_TESTSUITE:BOOL=ON -DOMS_TESTSUITE_PLATFORM=win
ctest --test-dir build-testsuite/ -j\$(nproc) --output-on-failure --output-junit ctest-result.xml ${params.CTEST_FLAGS} || true
test -f build-testsuite/ctest-result.xml
"""
                bat """
If Defined LOCALAPPDATA (echo LOCALAPPDATA: %LOCALAPPDATA%) Else (Set "LOCALAPPDATA=C:\\Users\\OpenModelica\\AppData\\Local")
set PATH=C:\\OMDevUCRT\\bin\\cmake\\bin;%PATH%

C:\\OMDevUCRT\\tools\\msys\\usr\\bin\\sh --login -i '${env.WORKSPACE}/testMSVC64-install.sh'

EXIT /b 0

:fail
ECHO Something went wrong!
EXIT /b 1
"""

                junit 'build-testsuite/ctest-result.xml'
              }
            }
          }
        }

      }
    }

    stage('upload') {
      parallel {

        stage('upload-doc') {
          when {
            allOf {
              not {
                changeRequest()
              }
              anyOf {
                buildingTag()
                anyOf {
                  branch 'master'
                  branch 'jenkins' // For testing purposes
                  branch 'maintenance/**'
                }
              }
            }
            beforeAgent true
          }
          agent {
            label 'linux'
          }
          steps {
            unstash name: 'docs'
            sh "test ! -z '${env.GIT_BRANCH}'"
            sh "test ! '${env.GIT_BRANCH}' = 'null'"
            sshPublisher (
              publishers: [
                sshPublisherDesc(
                  configName: 'OMSimulator-doc',
                  transfers: [
                    sshTransfer(execCommand: "rm -rf .tmp/${env.GIT_BRANCH}"),
                    sshTransfer(
                      execCommand: "test ! -z '${env.GIT_BRANCH}' && rm -rf '/var/www/openmodelica.org-documentation/OMSimulator/${env.GIT_BRANCH}' && mkdir -p `dirname '/var/www/openmodelica.org-documentation/OMSimulator/.tmp/${env.GIT_BRANCH}'` && mv '/var/www/openmodelica.org-documentation/OMSimulator/.tmp/${env.GIT_BRANCH}' '/var/www/openmodelica.org-documentation/OMSimulator/${env.GIT_BRANCH}'",
                      remoteDirectory: ".tmp/${env.GIT_BRANCH}",
                      removePrefix: "install/share/doc",
                      sourceFiles: 'install/share/doc/**')
                  ]
                )
              ]
            )
          }
        }

        stage('upload-artifacts') {
          when {
            anyOf {
              buildingTag()
              expression { return shouldWeUploadArtifacts() }
            }
            beforeAgent true
          }
          environment {
            DEPLOYMENT_PREFIX = getDeploymentPrefix()
          }
          agent {
            label 'linux'
          }
          steps {
            unstash name: 'amd64-zip'         // includes: "OMSimulator-linux-amd64-*.tar.gz"
            unstash name: 'jammy-amd64-zip'   // includes: "OMSimulator-linux-jammy-amd64-*.tar.gz"
            unstash name: 'mingw-ucrt64-zip'  // includes: "OMSimulator-mingw-ucrt64-*.zip"
            unstash name: 'win64-zip'         // includes: "OMSimulator-win64-*.zip"
            // unstash name: 'osx-zip'           // includes: "OMSimulator-osx-*.zip"

            sh "ls *.zip *.tar.gz"

            sshPublisher (
              publishers: [
                sshPublisherDesc(
                  configName: 'OMSimulator',
                  transfers: [
                    sshTransfer(
                      remoteDirectory: "${DEPLOYMENT_PREFIX}linux-amd64/",
                      sourceFiles: 'OMSimulator-linux-amd64-*.tar.gz'),
                    sshTransfer(
                      remoteDirectory: "${DEPLOYMENT_PREFIX}linux-jammy-amd64/",
                      sourceFiles: 'OMSimulator-linux-jammy-amd64-*.tar.gz'),
                    sshTransfer(
                      remoteDirectory: "${DEPLOYMENT_PREFIX}win-mingw-ucrt64/",
                      sourceFiles: 'OMSimulator-mingw-ucrt64-*.zip'),
                    //sshTransfer(
                    //  remoteDirectory: "${DEPLOYMENT_PREFIX}osx/",
                    //  sourceFiles: 'OMSimulator-osx-*.zip'),
                    sshTransfer(
                      remoteDirectory: "${DEPLOYMENT_PREFIX}win-msvc64/",
                      sourceFiles: 'OMSimulator-win64-*.zip')
                  ]
                )
              ]
            )
          }
        }
      }
    }

  }
}

def numPhysicalCPU() {
  def uname = sh script: 'uname', returnStdout: true
  if (uname.startsWith("Darwin")) {
    return sh (
      script: 'sysctl hw.physicalcpu_max | cut -d" " -f2',
      returnStdout: true
    ).trim().toInteger() ?: 1
  } else {
    return sh (
      script: 'lscpu -p | egrep -v "^#" | sort -u -t, -k 2,4 | wc -l',
      returnStdout: true
    ).trim().toInteger() ?: 1
  }
}

/* Run the testsuite with CTest against the install tree in the workspace.
 *
 * The test stages run on a different agent than the build stages and only get
 * the install tree back via `unstash`, so the build directory is configured
 * from scratch here. Nothing of OMSimulator itself is compiled by that: the
 * testsuite-depends fixture only builds omc-diff and packs the FMUs and SSPs.
 */
void runCTest(boolean asan=false, String extraArgs='') {
  echo "asan: ${asan}, running on node: ${env.NODE_NAME}"
  writeVersionFile()
  sh "cmake -S . -B build-testsuite/ -DCMAKE_BUILD_TYPE=Release -DCMAKE_INSTALL_PREFIX=install/ -DOMS_ENABLE_TESTSUITE:BOOL=ON ${asan ? '-DOMS_TESTSUITE_ASAN:BOOL=ON' : ''}"

  // CTest exits non-zero when tests fail; the junit step reports those, so only
  // a missing report counts as a hard failure of this step.
  sh """#!/bin/bash -x
  ulimit -t 1500

  ctest --test-dir build-testsuite/ ${asan ? "-j1": "-j${numPhysicalCPU()}"} --output-on-failure --output-junit ctest-result.xml ${params.CTEST_FLAGS} ${extraArgs} || true
  test -f build-testsuite/ctest-result.xml
  """
}

def isWindows() {
  return !isUnix()
}

def isMac() {
  if (isUnix()) {
    def uname = sh script: 'uname', returnStdout: true
    if (uname.startsWith("Darwin")) {
      return true
    }
  }
  return false
}

/* Provide the version to CMake without running git.
 * CMake prefers version.txt over `git describe`, which can't work inside the
 * build containers because the git reference repository they were cloned from
 * isn't visible there.
 */
void writeVersionFile() {
  if (!env.OMS_VERSION) {
    error("env.OMS_VERSION is not set, the 'version' stage has to run before building.")
  }
  writeFile file: 'version.txt', text: "${env.OMS_VERSION}\n"
}

void buildOMS(extraCMakeArgs='') {
  writeVersionFile()
  if (isWindows()) {
    bat ("""
     If Defined LOCALAPPDATA (echo LOCALAPPDATA: %LOCALAPPDATA%) Else (Set "LOCALAPPDATA=C:\\Users\\OpenModelica\\AppData\\Local")
     set OMDEV=C:\\OMDevUCRT
     echo on
     (
     echo export MSYS_WORKSPACE="`cygpath '${WORKSPACE}'`"
     echo echo MSYS_WORKSPACE: \${MSYS_WORKSPACE}
     echo cd \${MSYS_WORKSPACE}
     echo export MAKETHREADS=-j%NUMBER_OF_PROCESSORS%
     echo set -ex
     echo cmake -S . -B build/ -G "MSYS Makefiles" -DCMAKE_BUILD_TYPE=Release -DCMAKE_INSTALL_PREFIX=install/ ${extraCMakeArgs}
     echo cmake --build build/ --parallel %NUMBER_OF_PROCESSORS% --target install -v
     ) > buildOMSimulatorWindows.sh

     set MSYSTEM=${env.MSYSTEM ? env.MSYSTEM : "UCRT64"}
     set MSYS2_PATH_TYPE=inherit
     type buildOMSimulatorWindows.sh
     echo %PATH%
     %OMDEV%\\tools\\msys\\usr\\bin\\sh --login -c "cd `cygpath '${WORKSPACE}'` && chmod +x buildOMSimulatorWindows.sh && ./buildOMSimulatorWindows.sh && rm -f ./buildOMSimulatorWindows.sh"
    """)
  } else {
    echo "running on node: ${env.NODE_NAME}"
    def nproc = numPhysicalCPU()
    if (isMac()) {
      sh("""#!/bin/zsh -l
       cmake -S . -B build/ -DCMAKE_BUILD_TYPE=Release -DCMAKE_INSTALL_PREFIX=install/ ${extraCMakeArgs}
       cmake --build build/ --parallel ${nproc} --target install -v
       """)
    } else {
      sh "cmake -S . -B build/ -DCMAKE_BUILD_TYPE=Release -DCMAKE_INSTALL_PREFIX=install/ ${extraCMakeArgs}"
      sh "cmake --build build/ --parallel ${nproc} --target install -v"
    }
  }
}

void submoduleNoChange(path) {
  if (shouldWeUpdateSubmodules()) {
    // Don't need to check
    return
  }
  a=sh(returnStdout: true, script: "git ls-tree origin/${env.CHANGE_TARGET} ${path}").trim()
  b=sh(returnStdout: true, script: "git ls-tree HEAD ${path}").trim()
  if (a != b) {
    throw new Exception("Did you intend to change a submodule? Set SUBMODULE_UPDATE in the run options.")
  }
}

def isPR() {
  return env.CHANGE_ID ? true : false
}

def getDeploymentPrefix() {
  if (isPR()) {
    return "experimental/pr-${env.CHANGE_ID}/"
  }
  return "nightly/"
}

def shouldWeUploadArtifacts() {
  if (isPR()) {
    if (pullRequest.labels.contains("CI/Upload Artifacts")) {
      return true
    }
    return params.UPLOAD_BUILD_OPENMODELICA
  }
  return true
}

def shouldWeUpdateSubmodules() {
  if (isPR()) {
    if (pullRequest.labels.contains("CI/Update Submodules")) {
      return true
    }
  }
  return params.SUBMODULE_UPDATE
}

def shouldWeBuildMacOSArm64() {
  /* M1 Mac takes 4h to do a 10 seconds cmake configure!!!!!
   * disable the M1 until we find out what the issue is
   */
  return false
  if (isPR()) {
    if (pullRequest.labels.contains("CI/macOS-arm64")) {
      return true
    }
    return params.MACOS_ARM64
  }
  return true
}
