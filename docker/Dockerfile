FROM cahik/ubuntu-qgis:latest

RUN touch /etc/ld.so.conf.d/usr-local.conf && \
    echo '/usr/local/lib' | tee -a /etc/ld.so.conf.d/usr-local.conf && \
    ldconfig

ENV LD_LIBRARY_PATH="$LD_LIBRARY_PATH:/usr/local/lib"

COPY install/ /
RUN dpkg -i viewshed_*.deb