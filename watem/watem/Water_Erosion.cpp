#include "Water_Erosion.h"



Water_Erosion::Water_Erosion()
{
    //-----------------------------------------------------
    // Give some information about your tool...

    Set_Name ( _TL ( "Watererosion based on LS (RUSLE)" ) );

    Set_Author ( _TL ( "2016-2017  - Johan Van de Wauw" ) );

    Set_Version ( VERSION );

    Set_Description ( _TW (
                          "Determine watererosion based on LS(grid)-, "
                          "K(grid)-, C(grid)-, R(value)- and P(value)-factor. "
                          "A correction for accounting differences in "
                          "resolution can be used (see 'Resolution correction "
                          "factor'). In addition the RUSLE values can be "
                          "limited to a maximum value (see 'Flag maximum "
                          "gross erosion').")
                    );


    //-----------------------------------------------------
    // Define your parameters list...


    Parameters.Add_Grid (
        NULL, "LS", "LS",
        "LS: topographic slope and length factor (dimensionless), can be created using "
        "the 'LS calculations' tool (watem-2).",
        PARAMETER_INPUT
    );

    Parameters.Add_Grid (
        NULL, "K", "K-factor grid",
        "soil erodibility factor (K-factor, ton ha MJ-1 mm-1)",
        PARAMETER_INPUT
    );

    Parameters.Add_Grid (
        NULL, "C", "C-factor Grid",
        "Grid containing the crop and management factor. Should be defined as "
        "a grid with values between 0 and 1.",
        PARAMETER_INPUT
    );

    Parameters.Add_Value (
        NULL, "R", "R-factor value", "rainfall erosivity factor (MJ mm ha-1 year-1)"
        , PARAMETER_TYPE_Double, 880, 0, 1000
    );

    Parameters.Add_Value (
        NULL, "P", "P-factor value", "erosion control factor (dimensionless, between 0 and 1) ",
         PARAMETER_TYPE_Double, 1, 0, 1
    );

    Parameters.Add_Value (
        NULL, "CORR", "Resolution correction factor",
        "Correction factor for resolution as RUSLE was determined on resolution of 22.1 meter. This value is set to 1.4 for grids of 5x5 (see Notebaert et al. 2006)",
        PARAMETER_TYPE_Double, 1.4, 0, 20
    );

    Parameters.Add_Grid (
        NULL, "WATER_EROSION", _TL ( "Water erosion" ),
        "Average soil loss due to rill and interrill erosion (ton ha-1 year-1)",
        PARAMETER_OUTPUT
    );


    Parameters.Add_Value (
        NULL, "EROSION_CROP_MAX", "Flag maximum gross erosion",
        "Use maximum allowed erosion per pixel (True/false)", PARAMETER_TYPE_Bool, 1
    );

    Parameters.Add_Value (
        "EROSION_CROP_MAX", "EROSION_MAX", "Maximum allowed gross erosion.",
        "Maximum allowed erosion per pixel, higher values are set to the specific defined value (default 150).", PARAMETER_TYPE_Double, 150, 0, 10000000
    );


}


bool Water_Erosion::On_Execute()
{
    //-----------------------------------------------------
    // Get parameter settings...
    K = Parameters ( "K" )->asGrid();
    C = Parameters ( "C" )->asGrid();
    LS = Parameters ( "LS" )->asGrid();


    water_erosion = Parameters ( "WATER_EROSION" )->asGrid();


    R = Parameters ( "R" )->asDouble();
    P = Parameters ( "P" )->asDouble();
    double corr = Parameters ( "CORR" )->asDouble();
    double erosion_crop_max = Parameters ( "EROSION_MAX" )->asDouble();
    bool erosion_crop = Parameters ( "EROSION_CROP_MAX" )->asBool();
    water_erosion->Set_NoData_Value ( -99999 );
    #pragma omp parallel for
    for ( int i = 0; i < Get_NCells(); i++ )
    {
        if ( C->is_NoData ( i ) || LS->is_NoData ( i ) || K->is_NoData ( i ) )
        {
            water_erosion->Set_NoData ( i );
        }
        else
        {
            double v = R * K->asDouble ( i ) * LS->asDouble ( i ) * C->asDouble ( i ) * P / corr;
            if ( erosion_crop && ( v > erosion_crop_max ) )
            {
                v = erosion_crop_max;
            }
            if ( v < 0 )
            {
                v = -99999;
            }
            water_erosion->Set_Value ( i, v );
        }

    }



    //todo: stijl zoals pixelkaart

    return true;
}

Water_Erosion::~Water_Erosion()
{
}
