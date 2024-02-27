# Model description

The aim of this page is to explain how the water erosion is computed. The 
water erosion is derived from the RUSLE-equation 
($E = \frac{\text{kg}}{\text{m}^{2}.\text{year}}$) (Revised
Universal Soil Loss Equation, Renard et al., 1997):

$$ E = R \cdot K \cdot LS \cdot C \cdot P $$

with

- $R$: rainfall erosivity factor ($\frac{\text{MJ.mm}}{\text{m}^2.\text{h.year}}$)
- $K$: soil erodibility factor ($\frac{\text{kg.h}}{\text{MJ.mm}}$)
- $LS$: topographical slope and length factor (-)
- $C$: crop management factor (-, $$\in [0,1]$$)
- $P$: erosion control factor (-, $$\in [0,1]$$)

In SAGA-WaTEM, an adjusted formula is used to compensate for:

- Very high RUSLE-values: values above 150 are set equal to 150.
- Difference in resolution: the S-values are compensated by 
  dividing them by 1.4 (see Notebaert et al. 2006).

In the following text, it is explained how the subfactors are calculated.

## LS-factor

The L-factor is computed according the formula of Desmet & Govers (1996):

$$ L = \frac{(A+D^2)^{m+1}-A^{m+1}}{D^{m+2}.x^m.22,13^m} $$

with: 

- $A$: upstream area (m$^2$) (see next section).
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

$$ S = 16.8 * \text{slope} - 0.5 $$

Note another formula for S is used in the case that the upstream area is 
smaller than 25 m$^2$: 

$$ S = 3.0 * \text{slope}^{0.8} + 0.56 $$

## Upslope area

The upslope or upstream area is computed as:

$$ A = {\sum_1^n{\text{influx}_i}} $$

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
  the lowest adjacent pixel. Not all water flows to the adjacent parcel in
  this case. Between two parcels, only 30% of the water flows through. Towards 
  a forest, this is only 0%. These values are adjustable (connectivity).

  
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

The outgoing sediment volume of a cell is distributed to one or two target 
pixels.

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

Notebaert, B., Govers, G., Verstraeten, G., Van Oost, K., Poesen, J., Van
Rompaey, A., 2006, Verfijnde erosiekaart Vlaanderen: eindrapport. K.U.
Leuven, Leuven.
https://omgeving.vlaanderen.be/sites/default/files/atoms/files/Verfijnde_erosiekaart.pdf

Renard, K.G., Foster, G.R., Weesies, G.A., McCool, D.K., Yoder, D.C.,
1997, Predicting soil erosion by water: a guide to conservation planning with
the revised universal soil loss equation (RUSLE), Agriculture Handbook. U.S.
Department of Agriculture, Washington.
https://www.ars.usda.gov/ARSUserFiles/64080530/RUSLE/AH_703.pdf

Van Oost, K., Govers, G., Desmet, P., 2000, Evaluating the effects of changes
in landscape structure on soil erosion by water and tillage. Landscape 
Ecology 15, 577–589. https://doi.org/10.1023/A:1008198215674