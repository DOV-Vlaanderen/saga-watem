#include "Calculate_C_Grid.h"


Calculate_C_Grid::Calculate_C_Grid()
{

    //-----------------------------------------------------
    // Give some information about your tool...

    Set_Name ( _TL ( "C calculation based on parcel grid" ) );

    Set_Author ( _TL ( "Copyright (c) 2017, Johan Van de Wauw" ) );

    Set_Version ( VERSION );

    Set_Description ( _TW (
                          "Calculation of the C-factor (crop and management factor) "
                          "map based on parcel grid. Uses fixed constants: \n"
                          "- 0.37 for cropland (code in [2,9999] \n"
                          "- 0 for rivers infrastructure/roads (code = -1 and -2)\n"
                          "- 0.001 for forests (code = 10000)"
                      )
                    );


    //-----------------------------------------------------
    // Define your parameters list...


    Parameters.Add_Grid (
        NULL,  "PRC", "Parcels",
        "Parcel grid with: \n"
        "- a unique identifier per parcel: [2,9999] \n"
        "- Forest = 10000  \n"
        "- Other = 1 \n"
        "- Infrastructure & roads = -2 \n"
        "- Rivers -1 \n",
        PARAMETER_INPUT
    );

    Parameters.Add_Grid (
        NULL, "C", "C Grid", "C-factor Grid", PARAMETER_OUTPUT, true, SG_DATATYPE_Short ); // we use a short to save memory
}

bool Calculate_C_Grid::On_Execute()
{
    CSG_Grid *PRC = Parameters ( "PRC" )->asGrid();
    CSG_Grid *C = Parameters ( "C" )->asGrid();
    C->Set_Scaling ( 0.001 );


    #pragma omp parallel for
    for ( int i = 0; i < C->Get_NCells(); i++ )
    {
        switch ( PRC->asInt ( i ) )
        {
            case 10000:
                C->Set_Value ( i, 0.001 );
                break;
            case -9999:
            case 0:
                C->Set_NoData ( i );
                break;
            case -1:
            case -2:
                C->Set_Value ( i, 0 );
                break;
            default:
                C->Set_Value ( i, 0.37 );
                break;
        }
    }

    return true;
}

Calculate_C_Grid::~Calculate_C_Grid()
{
}
