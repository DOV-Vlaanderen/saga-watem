# Model description

There are two main output variables computed by WaTEM:

- Water erosion: soil erosion occurring by natural rainfall water.
- Tillage erosion: soil erosion occurring in cultivated fields due to the
  movement of soil by tillage. 

The aim of this page is to explain how water and tillage erosion are 
computed in WaTEM and to describe the implemented formulas. 

## Water erosion
Water erosion is computed by using the RUSLE-equation 
($A = \frac{\text{kg}}{\text{ha.year}}$) (Revised
Universal Soil Loss Equation, Renard et al., 1997):

$$ A = R \cdot K \cdot LS \cdot C \cdot P $$

with

- $R$: rainfall erosivity factor ($\frac{\text{MJ.mm}}{\text{ha.h.year}}$)
- $K$: soil erodibility factor ($\frac{\text{kg.h}}{\text{MJ.mm}}$)
- $LS$: topographical slope and length factor (-)
- $C$: crop management factor (-, $$\in [0,1]$$)
- $P$: erosion control factor (-, $$\in [0,1]$$)

In SAGA-WaTEM, additional parameters can be set to compensate for very high 
RUSLE-values: values above 150 are set equal to 150, this value can be adapted
in the code. The explanation on how the subfactors are calculated are 
described in below.

__NOTE__:

It is important to note that the units of $R$ and $K$ can be adapted, for
instance compatible with $J$ instead of $MJ$, as long as the units for $R$
and $K$ are synchronized. For reasons of clarity, the units are defined as a 
guidelines (see also module reference).

## Tillage erosion

Tillage erosion, or soil translocation by tillage, is calculated according to
the method of Van Oost et al. (2000). For every pixel the outgoing flux
$Q_{s,t}$ (kg.m$^{-1}$) due to tillage translocation is calculated as (
slope as gradient = dx/dh)

$$ Q_{s,t} = k_{til} \cdot \text{slope} $$

with

- $ktil$: tillage transport coefficient (kg.m$^{-1}$)

The local erosion or deposition rate by tillage ($E_t$) can be calculated 
as:

$$ E_t = - \frac{Q_{s,t}}{dx} $$

Soil redistribution by tillage only takes place within agricultural fields.
In these areas, tillage may be modelled as a diffusion process.
The functional equation implies that tillage erosion is controlled by the 
change in slope gradient, not by the slope gradient itself, so that erosion 
takes place on convexities while soil accumulation occurs in concavities. 
The intensity of the process is controlled by the value of a single constant,
and is set to a uniform value (in current code ktil is equal to 600 kg/m).  

## Subfactors

### R-factor

The erosive power of rainfall is quantified via the rainfall erosivity
factor (R-factor). The R-factor quantifies the mean annual average 
rainfall erosivity, calculated by combining rainfall events over 
multiple years (22 years according to the USLE definition) and is 
expressed in $\frac{MJ.mm}{ha.h.year}$. In the current module, one can 
define one singular value for the R-factor. 


### K-factor

The soil erodibility factor quantifies the change in the soil loss per unit
of rainfall energy. The unit of soil erodibility factor is expressed in soil 
loss per rainfall erosion index unit following Renard et al. (1997). 
Soil erodibility can be calculated from the USLE nomograph or using empirical
equations predicting the K-factor from the geometric mean particle diameter 
and organic matter content. The K-factor has large temporal variations, so 
the values always represent a long term average. In the module, the K-factor
is defined as an input raster (grid) (expressed in $\frac{kg.ha}{MJ.mm}$).


### LS-factor

The LS-factor can be computed using several formula's, these are 
explained below. The formula for the computation of the upslope area is also
given in this section. It is important to note that LS-factor is computed in 
one tool. Although the output can be given for S and L seperatly, the formula's
are always applicable to the LS-factor (see below and also 
[tool 6 in the module reference](overview_watem)). Computations are 
done based on a digital elevation model (raster) and parcels if one only wants 
pixels inside the same parcels to be used for slope calculation, to avoid high
incorrect slopes due to border effects such as sunken roads and embankments
(see also tool 6).

**Desmet & Govers (1996)**

$$ L = \frac{(A_U+D^2)^{m+1}-A_U^{m+1}}{D^{m+2}.x^m.22.13^m} $$

with: 

- $A_U$: upstream area (m$^2$) (see next section).
- $m$: exponent (-).
- $D$: grid resolution (m).
- $x$: factor incorporating the flow direction (-).

$x$ is calculated as a function of the aspect of the pixel:

$$ x = |sin(\text{aspect})| + |cos(\text{aspect})|$$

The exponent is computed with McCool et al. (1989, 1987)

$$ m = \frac{\beta}{\beta + 1} $$

with $\beta$:

$$ \beta = \frac{\frac{sin(\theta)}{0.0896}}{3.sin^{0.8}(\theta) + 0.56}$$

where $\theta$ stands for the slope of the pixel in percentages.

The S-factor is, when the slope is lower than 9 %, computed as:

$$ S = 10.8 * sin(\text{slope}) + 0.03 $$

Otherwise (larger than or equal to 9 %):

$$ S = 16.8 * sin(\text{slope}) - 0.5 $$

Note another formula for S is used in the case that the upstream area is 
smaller than 25 m$^2$: 

$$ S'= \text{minimum}(3.0 * \text{sin(slope)}^{0.8} + 0.56, S)$$  
 
**Van Oost et al. (2003)**

The formula's for Van Oost et al. (2023) are equal to those of 
Desmet & Govers (1996), except Van Oost et al. (2003) defines $m$ as a 
function of the upstream area 
$A_U$ (see Desmet & Govers (1996)). If the upstream area ($A_{U,\text{ref}}$) is 
smaller than 10 000 ha, than $m$ is calculated as:

$$ m = 0.3+\frac{A_U}{A_{U,\text{ref}}}^c $$

otherwise $m$ is set to 0.72. In the model $c$ is ‘hard coded’ as 0.8, meaning 
that this value is fixed for this model and cannot be changed by the user.

**Moore & Nieber (1989)**

$$LS = (0.4 + 1) * (A_U / 22.13)^{0.4}*\frac{sin(\text{slope})}{0.0896}^{1.3} $$

**Wischmeier and Smith (1978)**

If the slope is higher than 3 % (0.050 rad):

$$ LS = \frac{A_U}{22.13} * (65.41 * sin(\text{slope})^2 + 4.56*sin(\text{slope}) + 0.065) $$

Otherwise:

$$ LS = (\frac{A_U}{22.13})^{3*\text{slope}^{0.6}} * (65.41 * sin(\text{slope})^2 + 4.56*sin(\text{slope}) + 0.065 )$$


#### Upslope area

The upslope or upstream area is computed as:

$$ A_U = {\sum_1^n{\text{influx}_i}} $$

with:
- $n$: number of neighbors
- influx$_i$: influx area of neighboring pixel $i$

Pits are determined before computing the upslope area. These pits are defined 
as points or areas lower than the surrounding landscape, where there is a net
inflow. These are used to fill in the value of the upslope area for pits/flat
areas. The upslope area here equals the sum of all water flowing into the pits.
The calculation of the upslope area proceeds from the highest to the lowest 
pixel. Following rules are defined:

- If the pixel is adjacent to a river: all water flows to the river, even if 
  this pixel is not a lower pixel.
- Within a parcel: first, the aspect (direction of steepest slope) is 
  determined. Based on this, the water is distributed over the two neighboring
  pixels in the direction of the aspect (if they are lower than the current 
  cell).
- At the edge of a parcel: the water will primarily stay within the parcel, 
  i.e. the water flows to the lowest pixel in the parcel next to the current 
  pixel. Only if there is no lower pixel within the field, the water flows to 
  the lowest adjacent pixel. 

Three additional parameters are introduced to account for the concept of
connectivity between land uses 'road', 'forest' and 'cropland': 

- Parcel connectivity to cropland (%)
- Parcel connectivity to forest (%)
- Parcel connectivity to road (%).

The values defined in this parameter are used to multiply the computed 
upstream area:

$$ A_U = A_U * (100-\text{connectivity})/100 $$

The default values are 70, 100 and 70, for respectively the connectivity 
defined for roads, forest and cropland. It is important to note that this 
connectivity is defined based on the landuse of the **target** pixel.

#### LS correction
Notebaert et al. (2005) describes that changes in spatial resolution have 
major scaling effects on topographic variables like the L and S-factor. The
LS-factor will decrease on a higher resolution (smaller pixels, more height
information) and extreme LS values will occur more. To be able to compare the
calculated RUSLE values on different spatial resolutions, a correction factor
can be calculated. This correction factor is calculated as:

$$ LS_{corr} = \frac{LS_{avg,x}}{LS_{avg,y}} $$

with

- $LS_{avg,x}$: the average LS factor in a catchment on resolution x m.
- $LS_{avg,y}$: the average LS factor in a catchment on resolution y m.

The input variable is a float (default value 1, i.e. no correction). The 
LS-factor in the model is divided by this variable. The value is set to 1.4 
by Notebaert et al. (2005) for an application for Flanders.

### C-factor

The crop management factor (C-factor) is a dimensionless factor (0 – 1) 
that represents the erosional susceptibility of a given land use type 
compared to a non-vegetated or bare landcover. Within WaTEM, the
user can provide a C-factor grid (raster) representing the spatial variability 
in land use, e.g. on a field parcel basis.

### P-factor

The support practice factor is a dimensionless factor that represents the
ratio of soil loss for a field with structural soil and water conservation 
(SWC) measures compared to a situation without. In the current module, one can 
define one singular value for the P-factor, between 0 and 1. Alternatively, 
if one wishes to vary the P-factor over space, one can set the P-factor by
including it in the C-factor raster / grid (i.e. multiply input C-factor 
with desired P-factor in preprocesing).

## Implementation

WaTEM is implemented on a grid (raster). It computes the RUSLE based on the 
grid values for the LS-, K- and C- together with the input singular value for
the P- and R-factor.

The core module takes the K- and C-factor as raster inputs and the R- and
P-factor as singular value inputs. It calculates the LS-Factor from a 
given DEM and then calculates the Erosion Amount based on the 
RUSLE equation. In the preprocessing module for Flanders an extra tool is
provided to define the C-factor value based on Land Use information. If similar data are available, this tooling can be used for 
other regions as well. When a singular cell contains a NODATA value for
K, C or LS (computed based on DTM, so NODATA in DTM), then a no value is 
computed for the RUSLE equation (equal to NODATA).

## References
Desmet, P.J.J., Govers, G., 1996, A gis procedure for automatically
calculating the USLE LS factor on topographically complex landscapes.
Journal of Soil and Water Conservation 51, 427–433.
https://www.jswconline.org/content/51/5/427

McCool, D.K., Brown, C.L., Foster, R.G., Mutchler, K.C., Meyer, D.L., 1987,
Revised Slope Steepness Factor for the Universal Soil Loss Equation. 
Transactions of the ASAE 30, 1387–1396. https://doi.org/10.13031/2013.30576

McCool, D.K., Foster, G.R., Mutchler, K.C., Meyer, D.L., 1989. Revised Slope
Length Factor for the Universal Soil Loss Equation. Transactions of the 
ASAE 32, 1571–1576. https://doi.org/10.13031/2013.31192

Notebaert, B,. Govers, G.n Verstraeten, G., Van Oost, K., Ruysschaert, G., 
Poesen, J., Van Rompay, A. (2005): Verfijnde ersoiekaart Vlaanderen: 
eindrapport, Departement Omgeving, Brussel, 53 pp.

Renard, K.G., Foster, G.R., Weesies, G.A., McCool, D.K., Yoder, D.C.,
1997, Predicting soil erosion by water: a guide to conservation planning with
the revised universal soil loss equation (RUSLE), Agriculture Handbook. U.S.
Department of Agriculture, Washington.
https://www.ars.usda.gov/ARSUserFiles/64080530/RUSLE/AH_703.pdf

Van Oost, K., Govers, G., Desmet, P., 2000, Evaluating the effects of changes
in landscape structure on soil erosion by water and tillage. Landscape 
Ecology 15, 577–589. https://doi.org/10.1023/A:1008198215674

Van Oost, K., 2003. Spatial modeling of soil redistribution processes in 
agricultural landscapes. (PhD thesis). KU Leuven, Leuven.

Wischmeier, W.H. and Smith, D.D. (1978) Predicting Rainfall Erosion Losses. 
A Guide to Conservation Planning. The USDA Agricultural Handbook No. 537, 
Maryland.
