# Run fmi-cross-check with OMSimulator
The [fmi-cross-check repository]( https://github.com/modelica/fmi-cross-check)
contains exported FMUs and results from various tools that take part in the FMI
Cross-Check.

To validate the results [FMPy](https://github.com/CATIA-Systems/FMPy) is used.


## Dependencies
  - OMsimulator in your PATH
  - Python3 with modules
    - OMSimulator
    - pandas
    - OMPython
  - Docker ***or*** FMPy

Either you need FMPy installed and in your PATH or you can use a Docker image with it.

## Run the test
### On Linux 64bit
If oyu have FMPy installed set
```makefile
FMPY_DOCKER=
```
in Makefile.

Now call the Makefile and that's it

```bash
$ make all
```



### On Windows 64
Change `OS_SHORT=win64` in `Makefile.`
You need to have python3 in your PATH.
Use anaconda python with pandas installed or
(not recomended) add pandas with pacman to python3 used by mingw of OMDev.

From mingw bash:
```bash
$ make import-FMUs
```

From PowerShell:
```powershell
> docker run `
  -it `
  --rm `
  -v path\to\fmi-cross-check:/fmi-cross-check `
  -w /fmi-cross-check anheuermann/fmpy:bionic-amd64 `
  python3 -m fmpy.cross_check.validate_vendor_repo --clean-up
```
Watch out for the `\` used on the Windows path to the fmi-cross-check repository.


## Results
We upload results for every commit to https://libraries.openmodelica.org/fmi-crosschecking/OMSimulator/.


## Creating PRs to fmi-cross-check

To create a pull request to the [modelica/fmi-cross-check](https://github.com/modelica/fmi-cross-check) repository see their readme with the rules.
You will need to run the validation and correct all errors reported. To reduce the size of a result file from OMSimulator you can use:
```bash
python3 reduceCsvSize.py inputfile.csv
```
It will iteratively halve the size until the result file is smaller than 1 MB.



## Trouble shooting
  - FMPy is missing.
    - If you don't have FMPy installed you can use a [Docker image](https://hub.docker.com/repository/docker/anheuermann/fmpy) with FMPy.
    Change `FMPY_DOCKER` in Makefile accordingly. This also works on Windows.
  - Python can't find module `OMSimulator`.
      - Download the latest pre-compiled binary for your system from
        https://test.openmodelica.org/jenkins/job/OMSimulator/job/master/lastSuccessfulBuild/artifact/
        and add the path to `OMSimulator.py` in `addResults.py`, e.g.:
        ```python
        sys.path.append("/opt/OMSimulator/lib")
        from OMSimulator import OMSimulator
        ```
  - FMI 1.0 is not tested.
     - We don't support FMI 1.0 so we don't test it.
  - Not all FMUs are tested.
    - We ignore FMUs which are not compliant to latest FMI Cross-Check rules. When the file 
      `notCompliantWithLatestRules` is present we won't simulate the FMU. To change this set
      ```python
      ignoreNotCompliantWithLatestRules = False
      ```
      in addResults.py.
  - The Makefile is not working any more.
    - We don't test the Makefile with CI and run the Python scripts directly to generate the results.