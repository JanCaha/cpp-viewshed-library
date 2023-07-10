FROM ubuntu:jammy

RUN apt-get update && \
    apt-get -y -q install gpg wget lsb-core software-properties-common && \
    apt-get clean autoclean && \
    apt-get autoremove --yes && \
    rm -rf /var/lib/apt/lists/*

RUN gpg -k && \
    KEYRING=/usr/share/keyrings/ubuntugis-archive-keyring.gpg && \
    gpg --no-default-keyring --keyring $KEYRING --keyserver hkp://keyserver.ubuntu.com:80 --recv-keys 6B827C12C2D425E227EDCA75089EBE08314DF160 && \
    echo 'Types: deb deb-src' | tee -a /etc/apt/sources.list.d/ubuntugis-stable.sources && \
    echo 'URIs: https://ppa.launchpadcontent.net/ubuntugis/ppa/ubuntu' | tee -a /etc/apt/sources.list.d/ubuntugis-stable.sources && \
    echo 'Suites: '$(lsb_release -c -s) | tee -a /etc/apt/sources.list.d/ubuntugis-stable.sources && \
    echo 'Architectures: '$(dpkg --print-architecture) | tee -a /etc/apt/sources.list.d/ubuntugis-stable.sources && \
    echo 'Components: main' | tee -a /etc/apt/sources.list.d/ubuntugis-stable.sources && \
    echo 'Signed-By: '$KEYRING | tee -a /etc/apt/sources.list.d/ubuntugis-stable.sources

RUN export DEBIAN_FRONTEND=noninteractive && \
    echo 'debconf debconf/frontend select Noninteractive' | debconf-set-selections && \
    add-apt-repository ppa:jancaha/gis-tools && \
    apt-get update && \
    apt-get -y -q install --no-install-recommends gdal-bin libgdal-dev qtbase5-dev qtchooser qt5-qmake qtbase5-dev-tools && \
    apt-get -y install simplerasters viewshed viewshed-bin && \
    apt-get clean autoclean && \
    apt-get autoremove --yes && \
    rm -rf /var/lib/apt/lists/*
