#include "Tillage_Erosion.h"


Tillage_Erosion::Tillage_Erosion()
{
    Set_Name ( _TL ( "Tillage erosion calculation" ) );

    Set_Author ( _TL ( "Copyright (c) 2006 - 2016 by KU Leuven. Converted to SAGA/C++ by Johan Van de Wauw" ) );

    Set_Version ( VERSION );

    Set_Description ( _TW (
                          "Calculation of Tillage Erosion based on DTM (grid) "
                          "and parcels (grid). Based on the code from WaTEM "
                          "(KU Leuven). Tillage erosion, or soil translocation "
                          "by tillage, is calculated according to the method "
                          "of Van Oost et al. (2000). "
                          "See model description for more information on "
                          "the formula's. Note that the tillage transport "
                          "coefficient is hardcoded being 600 kg/m."
                      )
                    );


    //-----------------------------------------------------
    // Define your parameters list...


    Parameters.Add_Grid (
        NULL, "DEM", "Digital elevation model",
        "Digital Elevation Model (m).",
        PARAMETER_INPUT
    );

    Parameters.Add_Grid (
        NULL, "PRC", "Parcel grid",
        "Parcel grid with: \n"
        "- a unique identifier per parcel: [1,9999] \n"
        "- Forest = 10000  \n"
        "- Infrastructure & roads = -2 \n"
        "- Water (rivers and open water) = -1 \n",
        PARAMETER_INPUT
    );


    Parameters.Add_Grid (
        NULL, "TILL", "Tillage erosion",
        "Average soil loss due to tillage erosion (kg ha-1 year-1)",
        PARAMETER_OUTPUT
    );

}

bool Tillage_Erosion::On_Execute()
{

    int teller, i, j;
    double ADJUST, outflow, PART1, PART2, SN, CSN, asp;
    CSG_Grid *INFLOW = new CSG_Grid ( Get_System(), SG_DATATYPE_Double );
    CSG_Grid *DTM = Parameters ( "DEM" )->asGrid();
    CSG_Grid *PRC = Parameters ( "PRC" )->asGrid();
    CSG_Grid *TILL = Parameters ( "TILL" )->asGrid();


    CSG_Grid *FINISH = new CSG_Grid ( Get_System(), SG_DATATYPE_Bit );

    int k1, k2, l1, l2;

    const int ktil = 600;
    double res = Get_Cellsize();

    int ncol = Get_NX();
    int nrow = Get_NY();

    for ( teller = 0; teller < Get_NCells() ; teller++ )
    {
        DTM->Get_Sorted ( teller, j, i, true );


        if ( ( PRC->asDouble ( j, i ) <= 0 ) | ( PRC->asInt ( j, i ) >= 10000 ) )
        {
            continue;
        }

        double Slope, Aspect;
        DTM->Get_Gradient ( j, i, Slope, Aspect );

        ADJUST = fabs ( cos ( Aspect ) ) + fabs ( sin ( Aspect ) );

        outflow = ktil * sin ( ( double ) Slope ) * Get_Cellsize() * ADJUST;

        PART1 = 0.0;
        PART2 = 0.0;
        k1 = 0;
        l1 = 0;
        k2 = 0;
        l2 = 0;
        CSN = fabs ( cos ( ( double ) Aspect ) ) /
              ( fabs ( sin ( ( double ) Aspect ) ) + fabs ( cos ( ( double ) Aspect ) ) );

        SN = fabs ( sin ( ( double ) Aspect ) ) /
             ( fabs ( sin ( ( double ) Aspect ) ) + fabs ( cos ( ( double ) Aspect ) ) );

        asp = Aspect * ( 180 / M_PI );

        //  K1 en K2 zijn omgekeerde van in watem omdat grid anders georienteerd is
        if ( asp <= 90.0 )
        {
            PART1 = outflow * CSN;
            PART2 = outflow * SN;
            k1 = 1;
            l1 = 0;
            k2 = 0;
            l2 = 1;
        }
        else
        {
            if ( asp > 90.0 && asp < 180.0 )
            {
                PART1 = outflow * SN;
                PART2 = outflow * CSN;
                k1 = 0;
                l1 = 1;
                k2 = -1;
                l2 = 0;
            }
            else
            {
                if ( asp >= 180.0 && asp <= 270.0 )
                {
                    PART1 = outflow * CSN;
                    PART2 = outflow * SN;
                    k1 = -1;
                    l1 = 0;
                    k2 = 0;
                    l2 = -1;
                }
                else
                {
                    if ( asp > 270.0 )
                    {
                        PART1 = outflow * SN;
                        PART2 = outflow * CSN;
                        k1 = 0;
                        l1 = -1;
                        k2 = 1;
                        l2 = 0;
                    }
                }
            }
        }

        if ( !is_InGrid ( j + l1, i + k1 ) || ( ( ( DTM->asDouble ( j + l1, i + k1 ) > DTM->asDouble ( j, i ) ) || ( PRC->asInt ( j + l1, i + k1 ) != PRC->asInt ( j, i ) ) ) || FINISH->asInt ( j + l1, i + k1 ) == 1 ) )
        {

            outflow -= PART1;
        }
        else
        {
            INFLOW->Add_Value ( j + l1, i + k1, PART1 );

        }

        if ( !is_InGrid ( j + l2, i + k2 ) || ( ( DTM->asDouble ( j + l2, i + k2 ) > DTM->asDouble ( j, i ) ) || ( PRC->asInt ( j + l2, i + k2 ) != PRC->asInt ( j, i ) ) ) || FINISH->asInt ( j + l2, i + k2 ) == 1 )
        {

            outflow -= PART2;
        }
        else
        {
            INFLOW->Add_Value ( j + l2, i + k2, PART2 );

        }

        FINISH->Set_Value ( j, i, 1 );

        TILL->Set_Value ( j, i, ( INFLOW->asDouble ( j, i ) - outflow ) / ( res * res ) );
    }


    #pragma omp parallel for
    for ( int i = 0; i < nrow; i++ )
    {
        for ( int j = 0; j < ncol; j++ )
        {

            if ( TILL->asDouble ( j, i ) > 0 )
            {
                TILL->Set_Value ( j, i, 0 );
            }
            else
            {
                TILL->Set_Value ( j, i, TILL->asDouble ( j, i ) * -10 );
            }
        }
    }

    FINISH->Delete();
    INFLOW->Delete();

    return true;
}

Tillage_Erosion::~Tillage_Erosion()
{
}
