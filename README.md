# SAGA GIS WaTEM module
This repository contains a [SAGA-GIS](https://saga-gis.org) translation of 
the [WaTEM erosion model](https://watem-sedem.github.io/). It also contains 
modules that can be used in preprocessing and postprocessing the result of
that program.

WaTEM (Van Oost et al. 2000) is a spatially distributed model that computes 
erosion by rainfall and tillage: 

- Water erosion: soil erosion occurring by natural rainfall water.
- Tillage erosion: soil erosion occurring in cultivated fields due to the
  movement of soil by tillage. 

The main usage of this package is to compute WaTEM. Currently, the module is
split in two submodules, one to compute WaTEM, and one to process data for
applying and running WaTEM for flanders.

The installation guidelines can be found [here](docs/installation.md), whereas 
the description of the model is presented [here](docs/description.md). An
[example](docs/gettingstarted.md) is presented to help users get started. In 
addition, it is exemplified how his module is used for an application in 
[Flanders](docs/flanders-tool.md). Note that specific submodules have been 
programmed for processing the data for an application in Flanders.

Currently, the module contains two extra module, the see [Topology and line 
direction](docs/topology.md).

Consult the module reference for more in-depth information module usage.

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