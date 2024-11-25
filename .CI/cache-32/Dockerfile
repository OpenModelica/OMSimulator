# Cannot be parametrized in Jenkins...
FROM docker.openmodelica.org/build-deps:v1.16-i386

# We don't know the uid of the user who will build, so make the /cache directories world writable

RUN apt install libasan6 libxrandr-dev libxinerama-dev libxcursor-dev libxi-dev && \
    mkdir -p /cache/runtest/ /cache/omlibrary/ && chmod ugo+rwx /cache/runtest/ /cache/omlibrary/
