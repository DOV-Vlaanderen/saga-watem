# SAGA GIS WATEM module
This repository contains a [SAGA-GIS](https://saga-gis.org) translation of the [WATEM erosion model](https://watem-sedem.github.io/). It also contains modules that can be used in preprocessing and postprocessing the result of that program.

More details on the modules can be found in:
- [Getting started](docs/gettingstarted.md)
- [Application to Flanders](docs/flanders-tool.md)
- [Topology](docs/topology.md)

# Installing SAGA

## On windows
On windows, download version SAGA 8.5.1 from the website, a direct link to the file is here [saga-8.5.1_x64.zip](https://sourceforge.net/projects/saga-gis/files/SAGA%20-%208/SAGA%20-%208.5.1/saga-8.5.1_x64.zip/download). Alternatively, you can use SAGA 9.2.0 from the same website. Other versions have not been tested.

Get the windows built dll files from the releases page and copy these dll files into the tools subfolder of saga and restart. You can also drag and drop the dll on saga if they are stored in a different location.

If you want to test unreleased version of SAGA, go to the github action page for [windows builds](https://github.com/DOV-Vlaanderen/saga-watem/actions/workflows/cmake-windows.yml). You can find snapshots of the dll here.

## On linux
Packages are built for Debian Linux (Bullseye) and Ubuntu LTS (Focal). The pacakges can be grabbed from the release page.

# Building saga
## On windows

Download SAGA (compiled version, 8.5.1 using the link above) and create an environmental variable `SAGA` containing the path where saga was installed/unzipped. Install Visual Studio or Visual studio build tools and cmake and create the solution file using cmake:

```
mkdir build
cmake ..
```

This will create a solution that can be opened with visual studio, or can be built using msbuild. 


## On debian/ubuntu linux:

Make sure you have build-essential, cmake and libsaga-dev installed

```
sudo apt install build-essential cmake libsaga-dev
```

Next, build and install from source:

```
rm -rf build
mkdir build
cd build
cmake ..
make
make DESTDIR=libs install
cd ..
```

To build the documentation do (make sure you have dependencies in ci/requirements.txt) installed:

```
cd docs
mkdir tool_html
SAGA_TLB=../build/libs/saga saga_cmd --create-docs=tool_html
make html
```


## Running automated tests

A few automated tests are written in python. To run these under linux, run the compilation as mentioned above.

Next install the dependencies either in an environment or as apt packages and run

```
sudo apt install python-pytest python-rasterio python-geopandas
pytest-3
```

