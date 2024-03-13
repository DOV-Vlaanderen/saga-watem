#include "Water_Erosion.h"



Water_Erosion::Water_Erosion()
{
    //-----------------------------------------------------
    // Give some information about your tool...

    Set_Name ( _TL ( "Watererosion based on LS (RUSLE)" ) );

    Set_Author ( _TL ( "2016-2017  - Johan Van de Wauw" ) );

    Set_Version ( VERSION );

    Set_Description ( _TW (
                          "Determine watererosion based on LS and crop (C) factor. " )
                    );


    //-----------------------------------------------------
    // Define your parameters list...


    Parameters.Add_Grid (
        NULL, "LS", "LS",
        "LS: topographic slope and length factor (dimensionless)",
        PARAMETER_INPUT
    );

    Parameters.Add_Grid (
        NULL, "K", "K: soil erosion sensitivity factor (ton ha MJ-1 mm-1)",
        "K: soil erosion sensitivity factor (ton ha MJ-1 mm-1)",
        PARAMETER_INPUT
    );

    Parameters.Add_Grid (
        NULL, "C", "C: crop and management factor (dimensionless)",
        "Grid containing the crop and management factor.",
        PARAMETER_INPUT
    );

    Parameters.Add_Value (
        NULL, "R", "rainfall erosivity factor (MJ mm ha-1 year-1)",
        "", PARAMETER_TYPE_Double, 880, 0, 1000
    );

    Parameters.Add_Value (
        NULL, "P", "erosion control factor (dimensionless) ",
        "erosion control factor (dimensionless) ", PARAMETER_TYPE_Double, 1, 0, 1
    );

    Parameters.Add_Value (
        NULL, "CORR", "correctiefactor grid",
        "Correction factor because LS calculations should use plots of 22.1 meters. 1.4 for grids of 5x5", PARAMETER_TYPE_Double, 1.4, 0, 20
    );

    Parameters.Add_Grid (
        NULL, "WATER_EROSION", _TL ( "Water erosion" ),
        "Average soil loss due to rill and interrill erosion (ton ha-1 year-1)",
        PARAMETER_OUTPUT
    );


    Parameters.Add_Value (
        NULL, "EROSION_CROP_MAX", "Cap maximum erosion per pixel",
        "", PARAMETER_TYPE_Bool, 1
    );

    Parameters.Add_Value (
        "EROSION_CROP_MAX", "EROSION_MAX", "Maximum erosion, higher values will be capped.",
        "", PARAMETER_TYPE_Double, 150, 0, 10000000
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
