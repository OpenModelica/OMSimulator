pipeline {
  agent none
  environment {
    CACHE_DIR = "testsuite/cache/${env.CHANGE_TARGET ?: env.GIT_BRANCH}"
  }
  options {
    newContainerPerStage()
    buildDiscarder(logRotator(numToKeepStr: "100", artifactNumToKeepStr: "5"))
  }
  parameters {
    booleanParam(name: 'MSVC64', defaultValue: true, description: 'Build with MSVC64 (often hangs)')
    booleanParam(name: 'LINUX', defaultValue: true, description: 'Build with LINUX')
    booleanParam(name: 'MINGW_UCRT64', defaultValue: true, description: 'Build with MINGW/UCRT64')
    booleanParam(name: 'MINGW_UCRT64_CLANG', defaultValue: true, description: 'Build with MINGW/UCRT64 with clang')
    booleanParam(name: 'MACOS_ARM64', defaultValue: false, description: 'Build with macOS-arm64 (M1 mac)')
    booleanParam(name: 'LINUX64_ASAN', defaultValue: false, description: 'Build with linux64 asan')
    booleanParam(name: 'SUBMODULE_UPDATE', defaultValue: false, description: 'Allow pull request to update submodules (disabled by default due to common user errors)')
    booleanParam(name: 'UPLOAD_BUILD_OPENMODELICA', defaultValue: false, description: 'Upload install artifacts to build.openmodelica.org/omsimulator. Activates MINGW_UCRT64 as well.')
    string(name: 'RUNTESTS_FLAG', defaultValue: '', description: 'runtests.pl flag')
  }
  stages {
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
    stage('build') {
      parallel {
        stage('linux64') {
          when {
            expression { return params.LINUX }
            beforeAgent true
          }
          agent {
            dockerfile {
              additionalBuildArgs '--pull'
              dir '.CI/cache'
              /* The cache Dockerfile makes /cache/runtest, etc world writable
               * This is necessary because we run the docker image as a user and need to
               * be able to have a global caching of the omlibrary parts and the runtest database.
               * Note that the database is stored in a volume on a per-node basis, so the first time
               * the tests run on a particular node, they might execute slightly slower
               */
              label 'linux'
              args "--mount type=volume,source=runtest-omsimulator-cache-linux64,target=/cache/runtest"
            }
          }
          environment {
            RUNTESTDB = "/cache/runtest/"
            NPROC = "${numPhysicalCPU}"
          }
          steps {
            buildOMS()

            partest()
            junit 'testsuite/partest/result.xml'

            sh '(cd install/ && tar czf "../OMSimulator-linux-amd64-`git describe --tags --abbrev=7 --match=v*.* --exclude=*-dev | sed \'s/-/.post/\'`.tar.gz" *)'

            sh 'cmake --build build/ --target install-docs'
            sh '(cd install/share/doc && zip -r "../../../OMSimulator-doc-`git describe --tags --abbrev=7 --match=v*.* --exclude=*-dev | sed \'s/-/.post/\'`.zip" *)'

            archiveArtifacts artifacts: 'OMSimulator-doc*.zip,OMSimulator-linux-amd64-*.tar.gz', fingerprint: true
            stash name: 'amd64-zip', includes: "OMSimulator-linux-amd64-*.tar.gz"
            stash name: 'docs', includes: "install/share/doc/**"
          }
        }
        stage('linux64-asan') {
          when {
            expression { return params.LINUX64_ASAN }
          }
          stages {
            stage('build') {
              agent {
                docker {
                  image 'docker.openmodelica.org/build-deps:v1.13'
                  label 'linux'
                  alwaysPull true
                }
              }
              environment {
                RUNTESTDB = "/cache/runtest/"
                ASAN = "ON"
              }
              steps {
                buildOMS()
                stash name: 'asan', includes: "install/**"
              }
            }
            stage('test') {
              agent {
                dockerfile {
                  additionalBuildArgs '--pull'
                  dir '.CI/cache'
                  /* The cache Dockerfile makes /cache/runtest, etc world writable
                   * This is necessary because we run the docker image as a user and need to
                   * be able to have a global caching of the omlibrary parts and the runtest database.
                   * Note that the database is stored in a volume on a per-node basis, so the first time
                   * the tests run on a particular node, they might execute slightly slower
                   */
                  label 'linux'
                  args "--mount type=volume,source=runtest-omsimulator-cache-linux64-asan,target=/cache/runtest " +
                       "--cap-add SYS_PTRACE --privileged " + // Needed for ASAN
                       "--oom-kill-disable -m 1024m --memory-swap 1024m" // Needed for ASAN
                }
              }
              environment {
                RUNTESTDB = "/cache/runtest/"
                ASAN = "ON"
              }
              steps {
                unstash name: 'asan'
                partest()
                junit 'testsuite/partest/result.xml'
              }
            }
          }
        }
        stage('alpine') {
          when {
            expression { return params.LINUX }
            beforeAgent true
          }
          agent {
            dockerfile {
              additionalBuildArgs '--pull'
              dir '.CI/alpine'
              label 'linux'
            }
          }
          steps {
            buildOMS()
          }
        }
        stage('linux-arm32') {
          when {
            expression { return false }
            beforeAgent true
          }
          stages {
            stage('cross-compile') {
              agent {
                docker {
                  image 'docker.openmodelica.org/armcross-omsimulator:v2.0'
                  label 'linux'
                  alwaysPull true
                }
              }
              environment {
                CROSS_TRIPLE = "arm-linux-gnueabihf"
                CC = "${env.CROSS_TRIPLE}-gcc"
                CXX = "${env.CROSS_TRIPLE}-g++"
                AR = "${env.CROSS_TRIPLE}-ar"
                RANLIB = "${env.CROSS_TRIPLE}-ranlib"
                FMIL_FLAGS = '-DFMILIB_FMI_PLATFORM=arm-linux-gnueabihf'
                detected_OS = 'Linux'
                VERBOSE = '1'
              }
              steps {
                sh 'printenv'
                buildOMS()
                sh '''
                # No so-files should ever exist in a bin/ folder
                ! ls install/bin/*.so 1> /dev/null 2>&1
                (cd install/ && tar czf "../OMSimulator-linux-arm32-`git describe --tags --abbrev=7 --match=v*.* --exclude=*-dev | sed \'s/-/.post/\'`.tar.gz" *)
                '''
                archiveArtifacts "OMSimulator-linux-arm32-*.tar.gz"
                stash name: 'arm32-zip', includes: "OMSimulator-linux-arm32-*.tar.gz"
                stash name: 'arm32-install', includes: "install/**"
              }
            }
            stage('test') {
              /* when {
                beforeAgent true
                expression { return false }
              } */
              agent {
                dockerfile {
                  additionalBuildArgs '--pull'
                  dir '.CI/cache-arm32'
                  /* The cache Dockerfile makes /cache/runtest, etc world writable
                   * This is necessary because we run the docker image as a user and need to
                   * be able to have a global caching of the omlibrary parts and the runtest database.
                   * Note that the database is stored in a volume on a per-node basis, so the first time
                   * the tests run on a particular node, they might execute slightly slower
                   */
                  label 'linux-arm32'
                  args '--memory=500m --memory-swap=500m ' +
                       "--mount type=volume,source=runtest-omsimulator-cache-arm32,target=/cache/runtest"
                }
              }
              environment {
                RUNTESTDB = "/cache/runtest/"
              }
              steps {
                unstash name: 'arm32-install'

                // Work-around for deleting logs
                sh 'find testsuite/ -name "*.lua" -exec sed -i /teardown_command/d {} ";"'

                partest()

                // Disable until working
                // junit 'testsuite/partest/result.xml'
              }
            }
          }
        }

        stage('arm64-macOS') {
          when {
            expression { return shouldWeBuildMacOSArm64() }
            beforeAgent true
          }
          stages {
            stage('build') {
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
                sh '''
                (cd install/ && zip -r "../OMSimulator-osx-`git describe --tags --abbrev=7 --match=v*.* --exclude=*-dev | sed \'s/-/.post/\'`.zip" *)
                '''

                archiveArtifacts "OMSimulator-osx-*.zip"
                stash name: 'osx-zip', includes: "OMSimulator-osx-*.zip"
                stash name: 'osx-install', includes: "install/**"
              }
            }
            stage('test') {
              /* when {
                beforeAgent true
                expression { return false }
              } */
              agent {
                label 'M1'
              }
              steps {
                unstash name: 'osx-install'
                withEnv(["RUNTESTDB=${env.HOME}/jenkins-cache/runtest/"]) {
                  partest()
                }
                junit 'testsuite/partest/result.xml'
              }
            }
          }
        }


        stage('mingw-ucrt64-gcc') {
          when {
            anyOf {
              expression { return shouldWeBuildMINGW() }
              expression { return shouldWeUploadArtifacts() }
              buildingTag()
            }
            beforeAgent true
          }
          stages {
            stage('build') {
              agent {
                label 'omsimulator-windows'
              }
              environment {
                PATH = "C:\\OMDevUCRT\\tools\\msys\\ucrt64\\bin;C:\\bin\\git\\bin;C:\\bin\\git\\usr\\bin;${env.PATH};"
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
                export PATH="/c/Program Files/TortoiseSVN/bin/:/c/bin/jdk/bin:/c/bin/nsis/:\$PATH:/c/bin/git/bin"
                cd "${env.WORKSPACE}/install/"
                zip -r "../OMSimulator-mingw-ucrt64-`git describe --tags --abbrev=7 --match=v*.* --exclude=*-dev | sed 's/-/.post/'`.zip" *
                """

                bat """
                C:\\OMDevUCRT\\tools\\msys\\usr\\bin\\sh --login -i '${env.WORKSPACE}/buildZip64.sh'
                """

                archiveArtifacts "OMSimulator-mingw-ucrt64*.zip"
                stash name: 'mingw-ucrt64-zip', includes: "OMSimulator-mingw-ucrt64-*.zip"
                stash name: 'mingw-ucrt64-install', includes: "install/**"
              }
            }
            stage('test') {
              agent {
                label 'omsimulator-windows'
              }
              environment {
                PATH = "C:\\OMDevUCRT\\tools\\msys\\ucrt64\\bin;C:\\bin\\git\\bin;C:\\bin\\git\\usr\\bin;${env.PATH};"
                OMDEV = "/c/OMDevUCRT"
                MSYSTEM = "UCRT64"
                RUNTESTDB="${env.HOME}/jenkins-cache/runtest/"
              }
              steps {
                unstash name: 'mingw-ucrt64-install'

                bat 'hostname'
                writeFile file: "testMinGWUCRT64-install.sh", text:"""#!/bin/sh
set -x -e
cd "${env.WORKSPACE}"
export PATH="/c/Program Files/TortoiseSVN/bin/:/c/bin/jdk/bin:/c/bin/nsis/:\$PATH:/c/bin/git/bin"
make -C testsuite difftool resources
cp -f "${env.RUNTESTDB}/"* testsuite/ || true
find testsuite/ -name "*.lua" -exec sed -i /teardown_command/d {} ";"
cd testsuite/partest
perl ./runtests.pl -j\$(nproc) -nocolour -with-xml ${params.RUNTESTS_FLAG}
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

                junit 'testsuite/partest/result.xml'
              }
            }
          }
        }

        stage('mingw-ucrt64-clang') {
          when {
            expression { return params.MINGW_UCRT64_CLANG }
            beforeAgent true
          }
          agent {
            label 'omsimulator-windows'
          }
          environment {
            PATH = "C:\\OMDevUCRT\\tools\\msys\\ucrt64\\bin;C:\\bin\\git\\bin;C:\\bin\\git\\usr\\bin;${env.PATH};"
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
            stage('build') {
              when {
                expression { return params.MSVC64 }
                beforeAgent true
              }
              agent {
                label 'omsimulator-windows'
              }
              environment {
                PATH = "C:\\OMDevUCRT\\tools\\msys\\ucrt64\\bin;C:\\bin\\git\\bin;C:\\bin\\git\\usr\\bin;${env.PATH};"
                OMDEV = "/c/OMDevUCRT"
                MSYSTEM = "UCRT64"
              }
              steps {
                bat 'hostname'
                writeFile file: "buildZip.sh", text: """#!/bin/sh
set -x -e
export PATH="/c/Program Files/TortoiseSVN/bin/:/c/Program Files/Git/bin/:/c/bin/jdk/bin:/c/bin/nsis/:\$PATH:/c/bin/git/bin"
cd "${env.WORKSPACE}/install/"
zip -r "../OMSimulator-win64-`git describe --tags --abbrev=7 --match=v*.* --exclude=*-dev | sed 's/-/.post/'`.zip" *
"""

                retry(2) { bat """
If Defined LOCALAPPDATA (echo LOCALAPPDATA: %LOCALAPPDATA%) Else (Set "LOCALAPPDATA=C:\\Users\\OpenModelica\\AppData\\Local")
set PATH=C:\\OMDevUCRT\\bin\\cmake\\bin;%PATH%

cmake -S . -B build/ -G "Visual Studio 15 2017 Win64" -DCMAKE_BUILD_TYPE=Release -DCMAKE_INSTALL_PREFIX=install/ -DOM_OMS_ENABLE_TESTSUITE:BOOL=ON
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
            stage('test') {
              agent {
                label 'omsimulator-windows'
              }
              environment {
                PATH = "C:\\OMDevUCRT\\tools\\msys\\ucrt64\\bin;C:\\bin\\git\\bin;C:\\bin\\git\\usr\\bin;${env.PATH};"
                OMDEV = "/c/OMDevUCRT"
                MSYSTEM = "UCRT64"
                RUNTESTDB="${env.HOME}/jenkins-cache/runtest/"
              }
              steps {
                unstash name: 'win64-install'

                bat 'hostname'
                writeFile file: "testMSVC64-install.sh", text:"""#!/bin/sh
set -x -e
cd "${env.WORKSPACE}"
export PATH="/c/Program Files/TortoiseSVN/bin/:/c/bin/jdk/bin:/c/bin/nsis/:\$PATH:/c/bin/git/bin"
make -C testsuite difftool resources
cp -f "${env.RUNTESTDB}/"* testsuite/ || true
find testsuite/ -name "*.lua" -exec sed -i /teardown_command/d {} ";"
cd testsuite/partest
perl ./runtests.pl -j\$(nproc) -platform=win -nocolour -with-xml ${params.RUNTESTS_FLAG}
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

                junit 'testsuite/partest/result.xml'
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
            // unstash name: 'arm32-zip'         // includes: "OMSimulator-linux-arm32-*.tar.gz"
            unstash name: 'mingw-ucrt64-zip'  // includes: "OMSimulator-mingw-ucrt64-*.zip"
            unstash name: 'win64-zip'         // includes: "OMSimulator-win64-*.zip"
            // unstash name: 'osx-zip'           // includes: "OMSimulator-osx-*.zip"

            sh "ls *.zip *.tar.gz"

            sshPublisher (
              publishers: [
                sshPublisherDesc(
                  configName: 'OMSimulator',
                  transfers: [
                    //sshTransfer(
                    //  remoteDirectory: "${DEPLOYMENT_PREFIX}linux-arm32/",
                    //  sourceFiles: 'OMSimulator-linux-arm32-*.tar.gz'),
                    sshTransfer(
                      remoteDirectory: "${DEPLOYMENT_PREFIX}linux-amd64/",
                      sourceFiles: 'OMSimulator-linux-amd64-*.tar.gz'),
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

void partest(cache=true, extraArgs='') {
  echo "cache: ${cache}, asan: ${env.ASAN}, running on node: ${env.NODE_NAME}"
  sh """
  make -C testsuite difftool resources
  cp -f "${env.RUNTESTDB}/"* testsuite/ || true
  """

  // Work-around for deleting logs
  sh 'find testsuite/ -name "*.lua" -exec sed -i /teardown_command/d {} ";"'

  sh ("""#!/bin/bash -x
  ulimit -t 1500

  cd testsuite/partest
  ./runtests.pl ${env.ASAN ? "-asan": ""} ${env.ASAN ? "-j1": "-j${numPhysicalCPU()}"} -nocolour -with-xml ${params.RUNTESTS_FLAG} ${extraArgs}
  CODE=\$?
  test \$CODE = 0 -o \$CODE = 7 || exit 1
  """
  + (cache ?
  """
  if test \$CODE = 0; then
    mkdir -p "${env.RUNTESTDB}/"
    cp ../runtest.db.* "${env.RUNTESTDB}/"
  fi
  """ : ''))
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

void buildOMS() {
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
     echo git fetch --tags
     echo cmake -S . -B build/ -G "MSYS Makefiles" -DCMAKE_BUILD_TYPE=Release -DCMAKE_INSTALL_PREFIX=install/ -DOM_OMS_ENABLE_TESTSUITE:BOOL=ON
     echo cmake --build build/ --parallel %NUMBER_OF_PROCESSORS% --target install -v
     ) > buildOMSimulatorWindows.sh

     set MSYSTEM=${env.MSYSTEM ? env.MSYSTEM : "UCRT64"}
     set MSYS2_PATH_TYPE=inherit
     cat buildOMSimulatorWindows.sh
     echo \$PATH
     %OMDEV%\\tools\\msys\\usr\\bin\\sh --login -c "cd `cygpath '${WORKSPACE}'` && chmod +x buildOMSimulatorWindows.sh && ./buildOMSimulatorWindows.sh && rm -f ./buildOMSimulatorWindows.sh"
    """)
  } else {
    echo "running on node: ${env.NODE_NAME}"
    def nproc = numPhysicalCPU()
    sh "git fetch --tags"
    if (isMac()) {
      sh('''#!/bin/zsh -l
       cmake -S . -B build/ -DCMAKE_BUILD_TYPE=Release -DCMAKE_INSTALL_PREFIX=install/ -DOM_OMS_ENABLE_TESTSUITE:BOOL=ON
       cmake --build build/ --parallel ${nproc} --target install -v
       ''')
    } else {
      sh "cmake -S . -B build/ -DCMAKE_BUILD_TYPE=Release -DCMAKE_INSTALL_PREFIX=install/ -DOM_OMS_ENABLE_TESTSUITE:BOOL=ON"
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

def shouldWeBuildMINGW() {
  if (isPR()) {
    if (pullRequest.labels.contains("CI/MINGW_UCRT64")) {
      return true
    }
    return params.MINGW_UCRT64
  }
  return true
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
