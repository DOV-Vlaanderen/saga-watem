ARG base_container=debian:bookworm
FROM ${base_container}
ARG DEBIAN_FRONTEND=noninteractive
ENV TZ=Etc/UTC
RUN apt-get update && apt-get upgrade -y && \
    apt-get -y install --no-install-recommends fakeroot git build-essential libsaga-dev saga cmake python3-pytest python3-rasterio python3-geopandas  ca-certificates debhelper devscripts zip && \
    apt-get -y install python3-distutils || echo "python3-distutils not available" && \
    rm -rf /var/lib/apt/lists/*
    
