# Cannot be parametrized in Jenkins...
FROM ghcr.io/openmodelica/build-deps:v1.26.0-dev

# We don't know the uid of the user who will build, so make the /cache directories world writable

RUN apt-get update                                                             \
  && apt-get install -y                                                        \
    libasan6                                                                   \
    libxcursor-dev                                                             \
    libxi-dev                                                                  \
    libxinerama-dev                                                            \
    libxrandr-dev                                                              \
  && mkdir -p /cache/runtest/ /cache/omlibrary/                                \
  && chmod ugo+rwx /cache/runtest/ /cache/omlibrary/
