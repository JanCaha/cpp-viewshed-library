FROM ubuntu:jammy

RUN apt-get update && \
    apt-get -y -q install gpg wget lsb-core && \
    apt-get clean autoclean && \
    apt-get autoremove --yes && \
    rm -rf /var/lib/apt/lists/*

RUN gpg -k && \
    KEYRING=/usr/share/keyrings/qgis-archive-keyring.gpg && \
    wget -O $KEYRING https://download.qgis.org/downloads/qgis-archive-keyring.gpg && \
    touch /etc/apt/sources.list.d/qgis.sources && \
    echo 'Types: deb deb-src' | tee -a /etc/apt/sources.list.d/qgis.sources && \
    echo 'URIs: https://qgis.org/ubuntugis' | tee -a /etc/apt/sources.list.d/qgis.sources && \
    echo 'Suites: '$(lsb_release -c -s) | tee -a /etc/apt/sources.list.d/qgis.sources && \
    echo 'Architectures: '$(dpkg --print-architecture) | tee -a /etc/apt/sources.list.d/qgis.sources && \
    echo 'Components: main' | tee -a /etc/apt/sources.list.d/qgis.sources && \
    echo 'Signed-By: '$KEYRING | tee -a /etc/apt/sources.list.d/qgis.sources && \
    KEYRING=/usr/share/keyrings/ubuntugis-archive-keyring.gpg && \
    gpg --no-default-keyring --keyring $KEYRING --keyserver hkp://keyserver.ubuntu.com:80 --recv-keys 6B827C12C2D425E227EDCA75089EBE08314DF160 && \
    echo 'Types: deb deb-src' | tee -a /etc/apt/sources.list.d/ubuntugis-stable.sources && \
    echo 'URIs: https://ppa.launchpadcontent.net/ubuntugis/ppa/ubuntu' | tee -a /etc/apt/sources.list.d/ubuntugis-stable.sources && \
    echo 'Suites: '$(lsb_release -c -s) | tee -a /etc/apt/sources.list.d/ubuntugis-stable.sources && \
    echo 'Architectures: '$(dpkg --print-architecture) | tee -a /etc/apt/sources.list.d/ubuntugis-stable.sources && \
    echo 'Components: main' | tee -a /etc/apt/sources.list.d/ubuntugis-stable.sources && \
    echo 'Signed-By: '$KEYRING | tee -a /etc/apt/sources.list.d/ubuntugis-stable.sources && \
    echo 'Types: deb deb-src' | tee -a /etc/apt/sources.list.d/ubuntugis-unstable.sources && \
    echo 'URIs:https://ppa.launchpadcontent.net/ubuntugis/ubuntugis-unstable/ubuntu' | tee -a /etc/apt/sources.list.d/ubuntugis-unstable.sources && \
    echo 'Suites: '$(lsb_release -c -s)| tee -a /etc/apt/sources.list.d/ubuntugis-unstable.sources && \
    echo 'Architectures: '$(dpkg --print-architecture) | tee -a /etc/apt/sources.list.d/ubuntugis-unstable.sources && \
    echo 'Components: main' | tee -a /etc/apt/sources.list.d/ubuntugis-unstable.sources && \
    echo 'Signed-By: '$KEYRING | tee -a /etc/apt/sources.list.d/ubuntugis-unstable.sources


RUN export DEBIAN_FRONTEND=noninteractive && \
    echo 'debconf debconf/frontend select Noninteractive' | debconf-set-selections && \
    apt-get update && \
    apt-get -y -q install --no-install-recommends wget software-properties-common build-essential ca-certificates python3-pip dialog apt-utils && \
    apt update && \
    apt upgrade -y -q && \
    apt -y -q install qgis qgis-dev qgis-plugin-grass && \
    apt-get clean autoclean && \
    apt-get autoremove --yes && \
    rm -rf /var/lib/apt/lists/*

# RUN ldconfig /usr/local/lib
RUN touch /etc/ld.so.conf.d/usr-local.conf && \
    echo '/usr/local/lib' | tee -a /etc/ld.so.conf.d/usr-local.conf && \
    ldconfig

ENV LD_LIBRARY_PATH="$LD_LIBRARY_PATH:/usr/local/lib"

COPY install/ /
RUN dpkg -i viewshed_*.deb