# Dev Container

The Visual Studio Dev Containers extension lets you use a Docker container as a
full-featured development environment.

With this setup it is easy to reproduce a similar state to what Jenkins is doing
in our continuous integration when building and testing OMSimulator.

## Available Containers

Each image uses the `omsimulator` add-on of the
[build-deps](https://github.com/OpenModelica/build-deps) images, i.e. the
OpenModelica base image plus the extra X11 development libraries OMSimulator
needs.

- [build-deps-ubuntu-26](./build-deps-ubuntu-26/devcontainer.json): Ubuntu 26.04
  based build container.
- [build-deps-ubuntu-24](./build-deps-ubuntu-24/devcontainer.json): Ubuntu 24.04
  based build container.
- [build-deps-ubuntu-22](./build-deps-ubuntu-22/devcontainer.json): Ubuntu 22.04
  (Jammy) based build container, matching the `linux-jammy-amd64` artifact.
- [alpine](./alpine/devcontainer.json): Alpine (musl) based build container,
  mirroring the Jenkins `alpine` stage ([../.CI/alpine](./../.CI/alpine/)).

## Usage in Visual Studio Code

Make sure you have the Dev Containers extension
[ms-vscode-remote.remote-containers](https://marketplace.visualstudio.com/items?itemName=ms-vscode-remote.remote-containers)
and Docker installed and running.

Open the command palette (`Ctrl+Shift+P`) and run
`>Dev Containers: Open Folder in Container...`, select the OMSimulator
directory. Then select a devcontainer.json file to start.

Inside the container OMSimulator is built the same way as in CI:

```sh
cmake -S . -B build/ -DCMAKE_BUILD_TYPE=Release -DCMAKE_INSTALL_PREFIX=install/ -DOMS_ENABLE_TESTSUITE:BOOL=ON
cmake --build build/ --parallel $(nproc) --target install
```

## New Dev Container

Check directory [../.CI/](./../.CI/) for the Dockerfiles used by Jenkins and
construct your own dev container in a similar way.

For more details check
[https://containers.dev/implementors/json_reference/](https://containers.dev/implementors/json_reference/).

## Adding VSCode Extensions

Use `customizations` in `devcontainer.json` to add more extensions to your dev
container.

## Caveats

- Ubuntu 24.04 and 26.04 ship a default `ubuntu` user (UID 1000) and are used
  directly via `remoteUser`. Ubuntu 22.04 and Alpine have no default non-root
  user, so they use an additional Dockerfile to create one matching your host
  user.
- Because on Windows and Unix the environment variable containing the user name
  are different and only one should be set, both are added to devcontainer.json.
  If your user name isn't correct update it:

  ```diff
  -"${localEnv:USER}${localEnv:USERNAME}"
  +"username"
  ```
