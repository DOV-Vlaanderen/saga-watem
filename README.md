# SAGA GIS WaTEM module
This repository contains a [SAGA-GIS](https://saga-gis.org) translation of the [WaTEM erosion model](https://watem-sedem.github.io/). It also contains modules that can be used in preprocessing and postprocessing the result of that program.

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

# Legacy 
The saga-watem module is a derivative product of the code that was devoloped 
for the 'erosiekaart' for Flanders (see below). The tool was developed 
in 2016 (in Dutch), and was fully documented and refactored in 2024 by Fluves.

Consequently, the current tooling is structured in a different way compared 
to the initial tool. As of now, it consists of two tools (watem and 
watem-flanders) to make a clear distinction between tools relevant for 
computing the gross erosion (core) and tools relevant for preprocessing data from 
Flanders as input for the core tool.

The tool is now structured as follows:

in 2016 (in Dutch), and was fully documented and refactored in 2024 with a  

- watem: 

    1. Calculate uparea (used to be module 3.1)
    2. LS-calculations (used to be module 3.2)
    3. Watererosion based on LS (RUSLE) (used to be module 3.4)
    4. Tillage erosion calculations (used to be module 3.5)	 

- watem-flanders: 

    1. Prepare parcel map (used to be module 1)
    2. 3x3 filter within parcel boundaries (used to be module 2)
    3. C calculation based on parcel grid (used to be module 3.3)
    4. C calculations shape attribute (module 3.6)
    5. Compute Calculations Erosion map (combines all steps) (used to be 2.5) 