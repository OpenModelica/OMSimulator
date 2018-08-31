pipeline {
  agent none
  environment {
    CACHE_DIR = "testsuite/cache/${env.CHANGE_TARGET ?: env.GIT_BRANCH}"
  }
  options {
    newContainerPerStage()
  }
  stages {
    stage('build') {
      parallel {
        stage('linux64') {
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
            sh '''
            # No so-files should ever exist in a bin/ folder
            ! ls install/linux/bin/*.so 1> /dev/null 2>&1
            (cd install/linux && tar czf "../../OMSimulator-linux-amd64-`git describe`.tar.gz" *)
            '''

            archiveArtifacts "OMSimulator-linux-amd64-*.tar.gz"

            partest()

            junit 'testsuite/partest/result.xml'

            // Temporary debugging
            // archiveArtifacts artifacts: 'testsuite/**/*.log', fingerprint: true

            sh 'make doc doc-html doc-doxygen'
            sh '(cd install/linux/doc && zip -r "../../../OMSimulator-doc-`git describe`.zip" *)'
            archiveArtifacts artifacts: 'OMSimulator-doc*.zip', fingerprint: true
            stash name: 'docs', includes: "install/linux/doc/**"
          }
        }
        stage('linux64-asan') {
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
                stash name: 'asan', includes: "install/linux/**"
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
                       "--cap-add SYS_PTRACE " + // Needed for ASAN
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
        stage('centos6') {
          agent {
            dockerfile {
              additionalBuildArgs '--pull'
              dir '.CI/centos6'
              label 'linux'
            }
          }
          environment {
            SHELLSTART = """
            export CC="/opt/rh/devtoolset-6/root/usr/bin/gcc"
            export CXX="/opt/rh/devtoolset-6/root/usr/bin/g++"
            """
            OMSFLAGS = "CERES=OFF"
          }
          steps {
            buildOMS()
          }
        }
        stage('linux32') {
          agent {
            dockerfile {
              additionalBuildArgs '--pull'
              dir '.CI/cache-32'
              /* The cache Dockerfile makes /cache/runtest, etc world writable
               * This is necessary because we run the docker image as a user and need to
               * be able to have a global caching of the omlibrary parts and the runtest database.
               * Note that the database is stored in a volume on a per-node basis, so the first time
               * the tests run on a particular node, they might execute slightly slower
               */
              label 'linux'
              args "--mount type=volume,source=runtest-omsimulator-cache-linux32,target=/cache/runtest"
            }
          }
          environment {
            RUNTESTDB = "/cache/runtest/"
          }
          steps {
            buildOMS()
            sh '''
            # No so-files should ever exist in a bin/ folder
            ! ls install/linux/bin/*.so 1> /dev/null 2>&1
            (cd install/linux && tar czf "../../OMSimulator-linux-i386-`git describe`.tar.gz" *)
            '''

            archiveArtifacts "OMSimulator-linux-i386-*.tar.gz"

            partest()

            // Disable until working
            // junit 'testsuite/partest/result.xml'
          }
        }
        stage('linux-arm32') {
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
                ! ls install/linux/bin/*.so 1> /dev/null 2>&1
                (cd install/linux && tar czf "../../OMSimulator-linux-arm32-`git describe`.tar.gz" *)
                '''

                archiveArtifacts "OMSimulator-linux-arm32-*.tar.gz"
                stash name: 'arm32-install', includes: "install/linux/**"
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

        stage('osxcross') {
          stages {
            stage('cross-compile') {
              agent {
                docker {
                  image 'docker.openmodelica.org/osxcross-omsimulator:v2.0'
                  label 'linux'
                  alwaysPull true
                }
              }
              environment {
                CROSS_TRIPLE = "x86_64-apple-darwin15"
                CC = "${env.CROSS_TRIPLE}-cc"
                CXX = "${env.CROSS_TRIPLE}-c++"
                AR = "${env.CROSS_TRIPLE}-ar"
                RANLIB = "${env.CROSS_TRIPLE}-ranlib"
                FMIL_FLAGS = '-DFMILIB_FMI_PLATFORM=darwin64'
                detected_OS = 'Darwin'
                VERBOSE = '1'
                BOOST_ROOT = '/opt/osxcross/macports/pkgs/opt/local/'
              }
              steps {
                buildOMS()
                sh '''
                (cd install/mac && zip -r "../../OMSimulator-osx-`git describe`.zip" *)
                '''

                archiveArtifacts "OMSimulator-osx*.zip"
                stash name: 'osx-install', includes: "install/mac/**"
              }
            }

            stage('test') {
              /* when {
                beforeAgent true
                expression { return false }
              } */
              agent {
                label 'osx'
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
        stage('mingw64-cross') {
          agent {
            docker {
              image 'docker.openmodelica.org/msyscross-omsimulator:v2.0'
              label 'linux'
              alwaysPull true
            }
          }
          environment {
            CROSS_TRIPLE = "x86_64-w64-mingw32"
            CC = "${env.CROSS_TRIPLE}-gcc-posix"
            CXX = "${env.CROSS_TRIPLE}-g++-posix"
            CPPFLAGS = '-I/opt/pacman/mingw64/include'
            LDFLAGS = '-L/opt/pacman/mingw64/lib'
            AR = "${env.CROSS_TRIPLE}-ar-posix"
            RANLIB = "${env.CROSS_TRIPLE}-ranlib-posix"
            detected_OS = 'MINGW64'
            VERBOSE = '1'
            BOOST_ROOT = '/opt/pacman/mingw64/'
          }

          steps {
            buildOMS()
            sh '''
            (cd install/mingw && zip -r "../../OMSimulator-mingw64-`git describe`.zip" *)
            '''

            archiveArtifacts "OMSimulator-mingw64*.zip"
            stash name: 'mingw64-install', includes: "install/mingw/**"
          }
        }

        stage('msvc64') {
          agent {
            label 'omsimulator-windows'
          }
          environment {
            PATH = "${env.PATH};C:\\bin\\git\\bin;C:\\bin\\git\\usr\\bin;C:\\OMDev\\tools\\msys\\mingw64\\bin\\"
            OMDEV = "/c/OMDev"
            MSYSTEM = "MINGW64"
          }

          steps {
            writeFile file: "buildZip.sh", text: """#!/bin/sh
set -x -e
export PATH="/c/Program Files/TortoiseSVN/bin/:/c/bin/jdk/bin:/c/bin/nsis/:\$PATH:/c/bin/git/bin"
cd "${env.WORKSPACE}/install/win"
zip -r "../../OMSimulator-win64-`git describe`.zip" *
"""

            bat """
set BOOST_ROOT=C:\\local\\boost_1_64_0
set PATH=C:\\bin\\cmake\\bin;%PATH%

call configWinVS.bat VS15-Win64
IF NOT ["%ERRORLEVEL%"]==["0"] GOTO fail

call buildWinVS.bat VS15-Win64
IF NOT ["%ERRORLEVEL%"]==["0"] GOTO fail

call install\\win\\bin\\OMSimulator.exe --version
IF NOT ["%ERRORLEVEL%"]==["0"] GOTO fail

C:\\OMDev\\tools\\msys\\usr\\bin\\sh --login -i '${env.WORKSPACE}/buildZip.sh'

EXIT /b 0

:fail
ECHO Something went wrong!
EXIT /b 1
"""

            archiveArtifacts "OMSimulator-win64*.zip"
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
          /*
          // Does  not pass GIT_BRANCH env.var
          options {
            skipDefaultCheckout()
          }
          */
          steps {
            unstash name: 'docs'
            sh "test ! -z '${env.GIT_BRANCH}'"
            sh "test ! '${env.GIT_BRANCH}' = 'null'"
            sshPublisher(publishers: [sshPublisherDesc(configName: 'OMSimulator-doc', transfers: [sshTransfer(execCommand: "rm -rf .tmp/${env.GIT_BRANCH}"), sshTransfer(execCommand: "test ! -z '${env.GIT_BRANCH}' && rm -rf '/var/www/doc/OMSimulator/${env.GIT_BRANCH}' && mkdir -p `dirname '/var/www/doc/OMSimulator/.tmp/${env.GIT_BRANCH}'` && mv '/var/www/doc/OMSimulator/.tmp/${env.GIT_BRANCH}' '/var/www/doc/OMSimulator/${env.GIT_BRANCH}'", remoteDirectory: ".tmp/${env.GIT_BRANCH}", removePrefix: "install/linux/doc", sourceFiles: 'install/linux/doc/**')])])
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
  echo "cache: ${cache}, asan: ${env.ASAN}"
  sh """
  make -C testsuite difftool
  cp -f "${env.RUNTESTDB}/"* testsuite/ || true
  """

  // Work-around for deleting logs
  sh 'find testsuite/ -name "*.lua" -exec sed -i /teardown_command/d {} ";"'

  sh ("""#!/bin/bash -x
  ulimit -t 1500
  ${env.ASAN ? "" : "ulimit -v 6291456" /* Max 6GB per process */}

  cd testsuite/partest
  ./runtests.pl ${env.ASAN ? "-asan -fast": ""} -j${numPhysicalCPU()} -nocolour -with-xml ${extraArgs}
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

void buildOMS() {
  echo "${env.NODE_NAME}"
  def nproc = numPhysicalCPU()
  sh """
  ${env.SHELLSTART ?: ""}
  export HOME="${'$'}PWD"
  git fetch --tags
  make config-3rdParty ${env.CC ? "CC=" + env.CC : ""} ${env.CXX ? "CXX=" + env.CXX : ""} ${env.OMSFLAGS ?: ""} -j${nproc}
  make config-OMSimulator -j${nproc} ${env.ASAN ? "ASAN=ON" : ""} ${env.OMSFLAGS ?: ""}
  make OMSimulator -j${nproc} ${env.ASAN ? "DISABLE_RUN_OMSIMULATOR_VERSION=1" : ""}
  """
}
