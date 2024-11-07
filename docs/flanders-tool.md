# Processing with input data Flanders

On this page, the information for the application of the WaTEM module for data 
available for Flanders can be found. Note that extra tooling developed in the 
context of Flanders can be useful for your own case study. Following steps are 
defined:

- Generation of parcels map based on several input layers.
- Processing of digital elevation model (section 'digital elevation model').
- Definition of the C-grid from a parcel raster or from a shape attribute.
- Compute erosion map for Flanders.

## Parcels map generation

The parcel map that serves as input for the *upstream area* computation,
*LS computation* and *tillage erosion computation* is generated in the 
*Prepare parcel map* tool. The plot grid can be created in SAGA using the 
watem-flanders 'Prepare parcel Map'. The plot grid is created from the files
listed below. The order used is this (where later map layers are superimposed
on previous map layers):

- Landcover map with values:

  - 10000: forest
  - -1: open water
  - -2: infrastructure / roads

- GRB layers(geopoint - dataset GRBgis)

  - GBG (building to land), GBA (building attachment), WGA (road attachment), 
    KNW (structure): infrastructure (-2)

- Parcel map

  - Get values between 2 and 9999

- Waterways and roads

  - SBN (railway line), WBN (road line) (-2)
  - WLas (VHA lines), WTZ (VHA polygons) (-1)

Note these inputs are flanders specific. In the landcover raster, the values 
are used to define infrastructure, water and forest, whereas only the vector 
outlines in the GRB layer are used to define infrastructure. Similarly, the
vector outlines from WLAS and WTZ are used to define rivers, whereas SBN and 
WBN vector outlines are used to define infrastructure. We refer to the 
document of the 'erosiekaart' for more information on these input data layers.

Note that only the vector outlines are used to define binary rasters, i.e. no
attributes are used to define the binary rasters.

Tool: watem-flanders-1

## Filtering digital elevation model 

In some cases, filtering of the DEM can be usefull to smooth out outlies in 
the resulting RUSLE computation. This can be done with the
Grid Filter tool (any available in SAGA/QGIS to the users' liking). An 
alternative is to perform a 3x3 filter on the DTM within the boundaries of 
the parcels. This filters the DTM but only considers cells that lie within the
same plot. The input for this tool (watem-flanders-2) is the DEM and
[Parcels map](#Parcels-map-generation).

Tool: watem-flanders-2

## C-grid

A C-factor grid can be computed from:

- a parcel raster
- a parcel shape (and other data input from Flanders, i.e. GRB, WLAS)

The first tool only takes the [Parcels grid](#Parcels-map-generation) as input.
The Parcel map is translated as follows:

- 0.37 for cropland (code in [2,9999])
- 0 for water (open water/river) and infrastructure/roads (code = -1 and -2)
- 0.001 for forests (code = 10000)

In the second tool, the functionalities described in [Parcels map](#Parcels-map-generation) 
are used. In other words, the tool has as input (see definition 
[Parcels map](#Parcels-map-generation):

- Landcover map
- GRB layers(geopoint - dataset GRBgis)
- Parcel map
- Waterways and roads


As such the parcel-shape-tool is a joined tool of translating different data 
layers to a parcel map, and translating that map to the C-factor
(as done in the watem-flanders-3 tool). 

Tool: watem-flanders-3 and watem-flanders-4

## Complete tool

The complete tool requires the DEM (m), parcel grid 
(see [here](gettingstarted.md##compute-water-erosion)) and K-factor 
($\frac{\text{kg.h}}{\text{MJ.mm}}$) grid as input, 
into addition to the definition of the R- ($\frac{\text{MJ.mm}}{\text{m}^2.\text{h.year}}$) and 
P([0,1])-factor. The upslope area and
LS is computed from the DEM (and parcel grid for computation of LS within 
parcel boundaries). The C-factor is computed by 
[translating the parcel raster](#c-grid). We invite users to check out the 
[other options](overview_watem_flanders)

Tool: watem-flanders-5
