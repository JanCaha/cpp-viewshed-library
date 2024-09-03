# first stage - build image with all dependencies and build the libraries
FROM ubuntu:noble AS build

RUN apt-get update && \
    apt-get -y -q install gpg wget lsb-release git cmake clang ninja-build python3-pip && \
    apt-get clean autoclean && \
    apt-get autoremove --yes && \
    rm -rf /var/lib/apt/lists/*

RUN gpg -k && \
    KEYRING=/usr/share/keyrings/ubuntugis-archive-keyring.gpg && \
    gpg --no-default-keyring --keyring $KEYRING --keyserver hkp://keyserver.ubuntu.com:80 --recv-keys 6B827C12C2D425E227EDCA75089EBE08314DF160 && \
    echo 'Types: deb deb-src' | tee -a /etc/apt/sources.list.d/ubuntugis.sources && \
    echo 'URIs: https://ppa.launchpadcontent.net/ubuntugis/ubuntugis-unstable/ubuntu' | tee -a /etc/apt/sources.list.d/ubuntugis.sources && \
    echo 'Suites: '$(lsb_release -c -s) | tee -a /etc/apt/sources.list.d/ubuntugis.sources && \
    echo 'Architectures: '$(dpkg --print-architecture) | tee -a /etc/apt/sources.list.d/ubuntugis.sources && \
    echo 'Components: main' | tee -a /etc/apt/sources.list.d/ubuntugis.sources && \
    echo 'Signed-By: '$KEYRING | tee -a /etc/apt/sources.list.d/ubuntugis.sources

RUN export DEBIAN_FRONTEND=noninteractive && \
    echo 'debconf debconf/frontend select Noninteractive' | debconf-set-selections && \
    apt-get update && \
    apt-get -y -q install --no-install-recommends gdal-bin libgdal-dev qtbase5-dev qtchooser qt5-qmake qtbase5-dev-tools && \
    apt-get clean autoclean && \
    apt-get autoremove --yes && \
    rm -rf /var/lib/apt/lists/*

RUN git clone https://github.com/JanCaha/cpp-simplerasters.git
RUN cd cpp-simplerasters && \
    mkdir build && \
    cmake \
    -S . \
    -B build \
    -G Ninja \
    -D CMAKE_BUILD_TYPE=Release \
    -D BUILD_TESTS:bool=off \
    -D PACK_DEB:bool=off \
    -D CMAKE_INSTALL_PREFIX=/usr && \
    cmake --build build --config Release --target all && \
    cmake --build build --config Release --target install

RUN git clone --recursive https://github.com/JanCaha/cpp-viewshed-library.git
RUN cd cpp-viewshed-library && \
    mkdir build && \
    cmake \
    -S . \
    -B build \
    -G Ninja \
    -D CMAKE_BUILD_TYPE=Release \
    -D PACK_DEB:bool=off \
    -D BUILD_DOCUMENTATION:bool=off \
    -D BUILD_TESTS:bool=off \
    -D CMAKE_INSTALL_PREFIX=/usr \
    -D NEEDS_QT:bool=on && \
    cmake --build build --config Release --target all && \
    cmake --build build --config Release --target install

RUN git clone --recursive https://github.com/JanCaha/viewshed-library-pybind11.git
RUN pip install pytest wheel build --break-system-packages
RUN cd viewshed-library-pybind11 && \
    python3 -m build .

## Second stage - minimal image with only copied build files from previous stage
FROM ubuntu:noble

RUN apt-get update && \
    apt-get -y -q install gpg lsb-release python3 python3-pip && \
    apt-get clean autoclean && \
    apt-get autoremove --yes && \
    rm -rf /var/lib/apt/lists/*

RUN gpg -k && \
    KEYRING=/usr/share/keyrings/ubuntugis-archive-keyring.gpg && \
    gpg --no-default-keyring --keyring $KEYRING --keyserver hkp://keyserver.ubuntu.com:80 --recv-keys 6B827C12C2D425E227EDCA75089EBE08314DF160 && \
    echo 'Types: deb deb-src' | tee -a /etc/apt/sources.list.d/ubuntugis.sources && \
    echo 'URIs: https://ppa.launchpadcontent.net/ubuntugis/ubuntugis-unstable/ubuntu' | tee -a /etc/apt/sources.list.d/ubuntugis.sources && \
    echo 'Suites: '$(lsb_release -c -s) | tee -a /etc/apt/sources.list.d/ubuntugis.sources && \
    echo 'Architectures: '$(dpkg --print-architecture) | tee -a /etc/apt/sources.list.d/ubuntugis.sources && \
    echo 'Components: main' | tee -a /etc/apt/sources.list.d/ubuntugis.sources && \
    echo 'Signed-By: '$KEYRING | tee -a /etc/apt/sources.list.d/ubuntugis.sources

RUN export DEBIAN_FRONTEND=noninteractive && \
    echo 'debconf debconf/frontend select Noninteractive' | debconf-set-selections && \
    apt-get update && \
    apt-get -y -q install --no-install-recommends gdal-bin qtbase5-dev qtchooser && \
    apt-get clean autoclean && \
    apt-get autoremove --yes && \
    rm -rf /var/lib/apt/lists/*

COPY --from=build /usr/include/simplerasters /usr/include/simplerasters
COPY --from=build /usr/include/viewshed /usr/include/viewshed
COPY --from=build /usr/lib/x86_64-linux-gnu/libsimplerasters.so* /usr/lib/x86_64-linux-gnu
COPY --from=build /usr/lib/x86_64-linux-gnu/libviewshed.so* /usr/lib/x86_64-linux-gnu
COPY --from=build /usr/lib/x86_64-linux-gnu/libviewshedwidgets.so* /usr/lib/x86_64-linux-gnu
COPY --from=build /usr/bin/viewshed /usr/bin
COPY --from=build /usr/bin/inverseviewshed /usr/bin
COPY --from=build /usr/bin/viewshedcalculator /usr/bin
COPY --from=build /usr/bin/losextractor /usr/bin
COPY --from=build /viewshed-library-pybind11/dist/*.whl /tmp

RUN pip install /tmp/*.whl --break-system-packages && \
    rm -rf /tmp/*.whl
