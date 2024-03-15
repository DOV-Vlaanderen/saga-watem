#include "Complete.h"

Complete::Complete()
{
    Set_Name ( _TL ( "Complete Calculation Erosion map (combines all steps)" ) );

    Set_Author ( "Based on Watem (KULeuven). Converted to SAGA by Johan Van de Wauw (2016-2017)" );

    Set_Version ( VERSION );

    Set_Description ( _TW ("Complete computation erosion raster in one step. "
                           "This model executes following steps: \n "
                           "- Calculate uparea (watem-1) \n"
                           "- LS calculation (watem-2) \n
                           "- C calculation based on parcel grid (watem-flanders-3) \n"
                           "- Watererosion based on LS (RUSLE) (optional tillage erosion) (watem-3 and watem-4) \n"
                      ) );

    Parameters.Add_Grid ( NULL, "DEM", _TL ( "DEM" ), _TL ( "Digital elevation model."), PARAMETER_INPUT );
    Parameters.Add_Grid ( NULL, "PRC", _TL ( "Percelen" ), _TL ( "Parcels as defined by return tool 'Prepare parcel Map (watem-flanders-1)'" ), PARAMETER_INPUT );
    Parameters.Add_Grid ( NULL, "K", _TL ("soil erodibility factor (K-factor, ton ha MJ-1 mm-1)" ), _TL ("soil erodibility factor (K-factor, ton ha MJ-1 mm-1)"), PARAMETER_INPUT );

    Parameters.Add_Grid ( NULL, "PIT", _TL ( "Pit" ), _TL ( "Pits as defined by return tool 'Calculate uparea (watem-1)'" ), PARAMETER_OUTPUT, true, SG_DATATYPE_DWord );
    Parameters.Add_Grid ( NULL, "UPSLOPE_AREA", _TL ( "UPAREA" ), _TL ("Upslope area as defined by return tool 'Calculate uparea (watem-1)'" ), PARAMETER_OUTPUT );
    Parameters.Add_Grid ( NULL, "LS", _TL ( "LS" ), _TL ( "LS as defined by return tool 'LS calculation (watem-2)'" ), PARAMETER_OUTPUT );
    Parameters.Add_Grid ( NULL, "TILL", _TL ( "Tillage Erosion" ), _TL ( "Average soil loss due to tillage erosion (ton ha-1 jaar-1)" ), PARAMETER_OUTPUT_OPTIONAL );

    Parameters.Add_Value (
        NULL, "R", "R",
        "rainfall erosivity factor (MJ mm ha-1 jaar-1)", PARAMETER_TYPE_Double, 880, 0, 1000
    );

    Parameters.Add_Value (
        NULL, "P", "P",
        "erosion control factor (dimensieloos) ", PARAMETER_TYPE_Double, 1, 0, 1
    );

    Parameters.Add_Value (
        NULL, "CORR", "Resolution correction factor",
        "Correction factor for resolution as RUSLE was determined on resolution of 22.1 meter. This value is set to 1.4 for grids of 5x5 (see Notebaert et al. 2006)",
        PARAMETER_TYPE_Double, 1.4, 0, 20
    );


    Parameters.Add_Grid (
        NULL, "WATER_EROSION", _TL ( "Water erosion" ),
        "A: Average soil loss due to rill and interrill erosion (ton ha-1 jaar-1)",
        PARAMETER_OUTPUT
    );

    Parameters.Add_Value (
        NULL, "PCTOCROP", "Parcel connectivity to cropland (%)",
        "", PARAMETER_TYPE_Double, 70, 0, 100
    );

    Parameters.Add_Value (
        NULL, "PCTOFOREST", "Parcel connectivity to forest (%)",
        "", PARAMETER_TYPE_Double, 100, 0, 100
    );

    Parameters.Add_Value (
        NULL, "SAVE_MEMORY", "Save memory",
        "Option to reduce memory for computation. Warning: can increase computationt time.",
        PARAMETER_TYPE_Bool, false
    );

    Parameters.Add_Value (
        NULL, "PIT_FLOW", "Flow from pits into closeby cells (within radius) (True/False)", "", PARAMETER_TYPE_Bool, true
    );

    Parameters.Add_Value (
        "PIT_FLOW", "PIT_RADIUS", "Search radius from pit.",
        "Maximum radius from a pit to which upstream water can flow", PARAMETER_TYPE_Int, 4, 0 );

    Parameters.Add_Value (
        NULL, "LS_USE_PRC", "Use parcel boundary for computations ",
        "Compute slope/LS within boundaries of parcels.", PARAMETER_TYPE_Bool, true
    );

    Parameters.Add_Choice ( "",
                            "METHOD", _TL ( "Method to compute LS" ),
                            _TL ( "" ),
                            CSG_String::Format ( "%s|%s|%s|%s|",
                                    _TL ( "Moore & Nieber 1989" ),
                                    _TL ( "Desmet & Govers 1996 (standard WaTEM)" ),
                                    _TL ( "Wischmeier & Smith 1978" ),
                                    _TL ( "Van Oost, 2003" )
                                               ), 1
                          );

    Parameters.Add_Value (
        NULL, "EROSION_CROP_MAX", "Use maximum allowed erosion per pixel (True/false)",
        "", PARAMETER_TYPE_Bool, 1
    );

    Parameters.Add_Value (
        "EROSION_CROP_MAX", "EROSION_MAX", "Maximum allowed erosion per pixel, higher values are set to this value.",
        "", PARAMETER_TYPE_Double, 150, 0, 10000000
    );


};

bool Complete::On_Execute ( void )
{
    bool savememory = Parameters ( "SAVE_MEMORY" )->asBool();

    CSG_Grid *K = Parameters ( "K" )->asGrid();
    CSG_Grid *PRC = Parameters ( "PRC" )->asGrid();
    CSG_Grid *DEM = Parameters ( "DEM" )->asGrid();
    CSG_Grid *UPSLOPE_AREA = Parameters ( "UPSLOPE_AREA" )->asGrid();
    CSG_Grid *PIT = Parameters ( "PIT" )->asGrid();
    CSG_Grid *water_erosion = Parameters ( "WATER_EROSION" )->asGrid();

    CSG_Grid *ls = Parameters ( "LS" )->asGrid();

    if ( savememory )
    {
        K->Set_Cache ( true );
        water_erosion->Set_Cache ( true );
        ls->Set_Cache ( true );
    }

    SG_RUN_TOOL_ExitOnError ( "watem", 1, //uparea,
                              SG_TOOL_PARAMETER_SET ( "DEM", Parameters ( "DEM" ) )
                              && SG_TOOL_PARAMETER_SET ( "PRC", Parameters ( "PRC" ) )
                              && SG_TOOL_PARAMETER_SET ( "PIT", Parameters ( "PIT" ) )
                              && SG_TOOL_PARAMETER_SET ( "UPSLOPE_AREA", Parameters ( "UPSLOPE_AREA" ) )
                              && SG_TOOL_PARAMETER_SET ( "PCTOCROP", Parameters ( "PCTOCROP" ) )
                              && SG_TOOL_PARAMETER_SET ( "PCTOFOREST", Parameters ( "PCTOFOREST" ) )
                              && SG_TOOL_PARAMETER_SET ( "PCTOROAD", Parameters ( "PCTOCROP" ) )
                              && SG_TOOL_PARAMETER_SET ( "PIT_FLOW", Parameters ( "PIT_FLOW" ) )
                              && SG_TOOL_PARAMETER_SET ( "PIT_RADIUS", Parameters ( "PIT_RADIUS" ) )
                            );// als waarde voor PCTOROAD wordt hier (bewust) ook de waarde van crop gebruikt

    if ( savememory )
    {
        DEM->Set_Index ( false );
        //DEM->Set_Cache(true);
        //PRC->Set_Cache(true);
        PIT->Set_Cache ( true );
        ls->Set_Cache ( false );
    }

    SG_RUN_TOOL_ExitOnError ( "watem", 2, //LS calculation,
                              SG_TOOL_PARAMETER_SET ( "DEM", Parameters ( "DEM" ) )
                              && SG_TOOL_PARAMETER_SET ( "UPSLOPE_AREA", Parameters ( "UPSLOPE_AREA" ) )
                              && SG_TOOL_PARAMETER_SET ( "USEPRC", Parameters ( "LS_USE_PRC" ) )
                              && SG_TOOL_PARAMETER_SET ( "PRC", Parameters ( "PRC" ) )
                              && SG_TOOL_PARAMETER_SET ( "LS", Parameters ( "LS" ) )
                              && SG_TOOL_PARAMETER_SET ( "METHOD", Parameters ( "METHOD" ) )
                            );

    if ( savememory )
    {
        DEM->Set_Cache ( true );
        UPSLOPE_AREA->Set_Cache ( true );
    }

    // include connectivity in ls area name
    int conn = 100 - Parameters ( "PCTOCROP" )->asDouble();
    CSG_String connectivity_string = CSG_String::Format ( "%d", conn );

    ls->Set_Name ( "LS_" + connectivity_string );

    // C grid genereren op basis van percelengrid
    CSG_Grid *C = new CSG_Grid ( Get_System(), SG_DATATYPE_Short ); // we use a short to save memory

    SG_RUN_TOOL_ExitOnError ( "watem_flanders", 3, //
                              SG_TOOL_PARAMETER_SET ( "PRC", Parameters ( "PRC" ) )
                              && SG_TOOL_PARAMETER_SET ( "C", C )
                            );

    if ( savememory )
    {
        PRC->Set_Cache ( true );
        water_erosion->Set_Cache ( false );
        K->Set_Cache ( false );
    }

    SG_RUN_TOOL_ExitOnError ( "watem", 3, //watererosie op basis LS,
                              SG_TOOL_PARAMETER_SET ( "LS", Parameters ( "LS" ) )
                              && SG_TOOL_PARAMETER_SET ( "K", Parameters ( "K" ) )
                              && SG_TOOL_PARAMETER_SET ( "C", C )
                              && SG_TOOL_PARAMETER_SET ( "WATER_EROSION", Parameters ( "WATER_EROSION" ) )
                              && SG_TOOL_PARAMETER_SET ( "R", Parameters ( "R" ) )
                              && SG_TOOL_PARAMETER_SET ( "P", Parameters ( "P" ) )
                              && SG_TOOL_PARAMETER_SET ( "CORR", Parameters ( "CORR" ) )
                              && SG_TOOL_PARAMETER_SET ( "EROSION_CROP_MAX", Parameters ( "EROSION_CROP_MAX" ) )
                              && SG_TOOL_PARAMETER_SET ( "EROSION_MAX", Parameters ( "EROSION_MAX" ) )
                            );

    delete C;

    water_erosion->Set_Name ( "watero_" + connectivity_string );

    if ( savememory )
    {
        K->Set_Cache ( true );
        PIT->Set_Cache ( false );
        UPSLOPE_AREA->Set_Cache ( false );
        ls->Set_Cache ( false );
    }

    if ( Parameters ( "TILL" )->asGrid() != NULL )
    {
        SG_RUN_TOOL_ExitOnError ( "watem", 4, //tillage erosion op basis LS,
                                  SG_TOOL_PARAMETER_SET ( "DEM", Parameters ( "DEM" ) )
                                  && SG_TOOL_PARAMETER_SET ( "PRC", Parameters ( "PRC" ) )
                                  && SG_TOOL_PARAMETER_SET ( "TILL", Parameters ( "TILL" ) )
                                );
    }
    return true;
}

Complete::~Complete()
{
}
