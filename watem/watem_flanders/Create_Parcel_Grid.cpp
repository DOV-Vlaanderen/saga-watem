#include "Create_Parcel_Grid.h"



Create_Parcel_Grid::Create_Parcel_Grid()
{
    Set_Name ( _TL ( "Prepare parcel Map" ) );

    Set_Author ( "Johan Van de Wauw (2017)" );

    Set_Version ( VERSION );

    Set_Description ( _TW ( "Flanders specific: Creates a parcel grid for "
                            "creating the erosion map based on parcel map, "
                            "'Grootschalig Referentie Bestand' (GRB), the "
                            "'Vlaams Hydrologische Atlas' (VHA) and "
                            "Landcover map. Returns parcels with codes: \n"
                            "\n"
                            " - infrastructure and roads: -2 \n"
                            " - water (open water / river): -1 \n"
                            " - parcels: [1,9999] \n"
                            " - forest: 10000 \n"
                            "\n"
                            "Only the geometry of the input shapes is used for "
                            "mapping. Note that the input instruction refers to "
                            "data specific attributes (i.e. GRB, VHA, in "
                            "dutch). We refer to the GRB and VHA metadata at "
                            "'datavindplaats' of www.vlaanderen.be for more "
                            "information. GRB overwrites parcel, river "
                            "overwrites GRB. "
                          ) );

    Parameters.Add_Shapes ( NULL, "PARCEL_SHAPES", "Parcels", "Polygon shape of parcels.", PARAMETER_INPUT, SHAPE_TYPE_Polygon );
    Parameters.Add_Grid ( NULL, "LANDCOVER", "Landcover", "Landcover with values 10000 (forest), 1 (parcels), -1 (water) and -2 (infrastructure & roads).", PARAMETER_INPUT );

    Parameters.Add_Grid ( NULL, "PRC", "Parcel grid", "Output grid  with: \n"
        "- a unique identifier per parcel: [1,9999] \n"
        "- Forest = 10000  \n"
        "- Infrastructure & roads = -2 \n"
        "- Water (rivers and open water) = -1 \n", PARAMETER_OUTPUT, true, SG_DATATYPE_Short );
    Parameters.Add_Shapes ( NULL, "WTZ", "GRB WTZ (VHA-polygon)", "VHA (polygons).", PARAMETER_INPUT, SHAPE_TYPE_Polygon );
    Parameters.Add_Shapes ( NULL, "WLAS", "VHA lines", "VHA (lines).", PARAMETER_INPUT, SHAPE_TYPE_Line );

    Parameters.Add_Shapes ( NULL, "SBN", "GRB Sbn (spoorbaan)", "GRB Sbn (spoorbaan).", PARAMETER_INPUT, SHAPE_TYPE_Polygon );
    Parameters.Add_Shapes ( NULL, "WBN", "GRB Wbn (wegbaan)", "GRB Wbn (wegbaan).", PARAMETER_INPUT, SHAPE_TYPE_Polygon );

    Parameters.Add_Shapes ( NULL, "WGA", "GRB Wga (wegaanhorigheid)", "GRB Wga (wegaanhorigheid).", PARAMETER_INPUT, SHAPE_TYPE_Polygon );

    Parameters.Add_Shapes ( NULL, "GBG", "GRB Gbg (gebouw aan de grond)", "GRB Gbg (gebouw aan de grond).", PARAMETER_INPUT, SHAPE_TYPE_Polygon );
    Parameters.Add_Shapes ( NULL, "GBA", "GRB Gba (gebouwaanhorigheid)", "GRB Gba (gebouwaanhorigheid).", PARAMETER_INPUT, SHAPE_TYPE_Polygon );
    Parameters.Add_Shapes ( NULL, "TRN", "(NOT IMPLEMENTED) GRB Trn (terrein)", "GRB Trn (Terrein) - only certain classes are used", PARAMETER_INPUT, SHAPE_TYPE_Polygon );
    Parameters.Add_Shapes ( NULL, "KNW", "GRB Knw (Kunstwerk)", "GRB Knw (Kunstwerk)", PARAMETER_INPUT, SHAPE_TYPE_Polygon );

}


Create_Parcel_Grid::~Create_Parcel_Grid()
{
}


CSG_Grid *BinaryShapetoGrid ( CSG_Parameter *shape, const CSG_Grid_System &system, const int poly_type )
{
    CSG_Grid   *grid = new CSG_Grid ( system, SG_DATATYPE_Bit );
    bool result;
    SG_RUN_TOOL ( result, "grid_gridding", 0,
                  SG_TOOL_PARAMETER_SET ( "INPUT", shape )
                  && SG_TOOL_PARAMETER_SET ( "TARGET_DEFINITION", 1 )	// grid or grid system
                  && SG_TOOL_PARAMETER_SET ( "GRID", grid )
                  && SG_TOOL_PARAMETER_SET ( "OUTPUT", 0 ) // data / no data
                  && SG_TOOL_PARAMETER_SET ( "MULTIPLE", 1 )
                  && SG_TOOL_PARAMETER_SET ( "POLY_TYPE", poly_type ) //cell: als er een pixels overlapt met infrastructuur moet hele pixels infrastructuur  worden
                );
    return grid;
}

bool Create_Parcel_Grid::On_Execute()
{
    CSG_Grid *prc = Parameters ( "PRC" )->asGrid();
    CSG_Grid *landuse = Parameters ( "LANDCOVER" )->asGrid();

    // grid maken van GRB achtergrondlagen (gebouwen, kunstwerken)
    CSG_Grid *gbg, *gba, *wga, *knw;
    gbg = BinaryShapetoGrid ( Parameters ( "GBG" ), prc->Get_System(), 1 );
    gba = BinaryShapetoGrid ( Parameters ( "GBA" ), prc->Get_System(), 1 );
    wga = BinaryShapetoGrid ( Parameters ( "WGA" ), prc->Get_System(), 1 );
    knw = BinaryShapetoGrid ( Parameters ( "KNW" ), prc->Get_System(), 1 );

    // landgebruik combineren met GRB achtergrondlagen
    #pragma omp parallel for
    for ( int i = 0; i < prc->Get_NCells(); i++ )
    {
        if ( gbg->asInt ( i ) == 1 || gba->asInt ( i ) == 1 || wga->asInt ( i ) == 1 || knw->asInt ( i ) == 1 )
        {
            prc->Set_Value ( i, -2 );
        }
        else
        {
            prc->Set_Value ( i, landuse->asInt ( i ) );
        }
    }

    delete gbg;
    delete gba;
    delete wga;
    delete knw;

    CSG_Grid *parcel_grid = new CSG_Grid ( prc->Get_System(), SG_DATATYPE_Long );

    SG_RUN_TOOL_ExitOnError ( "grid_gridding", 0,
                              SG_TOOL_PARAMETER_SET ( "INPUT", Parameters ( "PARCEL_SHAPES" ) )
                              && SG_TOOL_PARAMETER_SET ( "TARGET_DEFINITION", 1 )	// grid or grid system
                              && SG_TOOL_PARAMETER_SET ( "GRID", parcel_grid )
                              && SG_TOOL_PARAMETER_SET ( "OUTPUT", 1 )
                              && SG_TOOL_PARAMETER_SET ( "MULTIPLE", 1 )
                              && SG_TOOL_PARAMETER_SET ( "POLY_TYPE", 0 )
                              && SG_TOOL_PARAMETER_SET ( "GRID_TYPE", 2 ) //integer 4 byte
                            );

    // vervolgens worden deze percelen hernummerd en over het landgebruik gelegd

    #pragma omp parallel for
    for ( int i = 0; i < prc->Get_NCells(); i++ )
    {
        if ( !parcel_grid->is_NoData ( i ) && parcel_grid->asInt ( i ) >= 0 )
        {
            prc->Set_Value ( i, ( parcel_grid->asInt ( i ) % 9997 ) + 2 );
        }
    }
    delete parcel_grid;


    // grids maken van wegen en waterwegen

    CSG_Grid *wbn, *sbn;
    wbn = BinaryShapetoGrid ( Parameters ( "WBN" ), prc->Get_System(), 0 );
    sbn = BinaryShapetoGrid ( Parameters ( "SBN" ), prc->Get_System(), 0 );


    CSG_Grid *vha_pol;
    vha_pol = BinaryShapetoGrid ( Parameters ( "WTZ" ), prc->Get_System(), 0 );


    CSG_Grid *vha_line;
    vha_line = new CSG_Grid ( prc->Get_System(), SG_DATATYPE_Bit );

    SG_RUN_TOOL_ExitOnError ( "grid_gridding", 0,
                              SG_TOOL_PARAMETER_SET ( "INPUT", Parameters ( "WLAS" ) )
                              && SG_TOOL_PARAMETER_SET ( "TARGET_DEFINITION", 1 )	// grid or grid system
                              && SG_TOOL_PARAMETER_SET ( "GRID", vha_line )
                              && SG_TOOL_PARAMETER_SET ( "OUTPUT", 0 ) // data / no data
                              && SG_TOOL_PARAMETER_SET ( "MULTIPLE", 1 )
                              && SG_TOOL_PARAMETER_SET ( "LINE_TYPE", 0 ) //thin lines
                            );

    // verschillende grids combineren
    #pragma omp parallel for
    for ( int i = 0; i < prc->Get_NCells(); i++ )
    {
        if ( vha_line->asInt ( i ) == 1 || vha_pol->asInt ( i ) == 1 )
        {
            prc->Set_Value ( i, -1 );
        }
        else
            if ( wbn->asInt ( i ) == 1 || sbn->asInt ( i ) == 1 )
            {
                prc->Set_Value ( i, -2 );
            }
    }


    delete vha_pol;
    delete vha_line;
    delete wbn;
    delete sbn;
    //todo: no data op 0 zetten!
    // nu komt nodata van landuse denk ik?


    return true;
}
