///////////////////////////////////////////////////////////
//                                                       //
//                         SAGA                          //
//                                                       //
//      System for Automated Geoscientific Analyses      //
//                                                       //
//                     Tool Library                      //
//                         Watem                         //
//                                                       //
//-------------------------------------------------------//
//                                                       //
//               Calculate_LS_Watem.cpp                  //
//                                                       //
///////////////////////////////////////////////////////////

//---------------------------------------------------------


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
#include "Calculate_LS_Watem.h"

#include <algorithm>

///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
CCalculate_LS_Watem::CCalculate_LS_Watem ( void )
{
    //-----------------------------------------------------
    // Give some information about your tool...

    Set_Name	( _TL ( "LS calculation" ) );

    Set_Author	( _TL ( "Copyright (c) 2003 Olaf Conrad. Modified by Johan Van de Wauw (2016) to make model identical to WaTEM" ) );

    Set_Version ( VERSION );

    Set_Description	( _TW (
                          "This module computes the LS-factor given a digital "
                          "elevation model, parcel grid and upslope area (see "
                          "calculate uparea. The slope at boundaries is "
                          "calculated perpendicular to parcel boundaries. "
                          "Different methods can be used to compute the LS "
                          "(see 'Method to compute LS').")
                    );


    //-----------------------------------------------------
    // Define your parameters list...


    Parameters.Add_Grid (
        NULL, "DEM", "Digital elevation model",
        "Digital elevation model (m)",
        PARAMETER_INPUT
    );


    Parameters.Add_Grid (
        NULL, "UPSLOPE_AREA", _TL ( "Upslope Area" ),
        "Upslope Area: area that flow to a certain pixel. Can be created using "
        "the 'Calculate uparea' tool (watem-1).",
        PARAMETER_INPUT
    );

    CSG_Parameter *useprc = Parameters.Add_Bool (
                                NULL, "USEPRC", "Use parcel borders in slope calculation.",
                                "If enabled, only pixels inside the same parcels will be used for slope calculation, to avoid high"
                                "incorrect slopes due to border effects such as sunken roads and embankments. ", true
                            );

    Parameters.Add_Grid (
        useprc, "PRC", "Parcel grid",
        "Parcel grid with: \n"
        "- a unique identifier per parcel: [1,9999] \n"
        "- Forest = 10000  \n"
        "- Infrastructure & roads = -2 \n"
        "- Water (river/open water) -1 \n",
        PARAMETER_INPUT_OPTIONAL
    );

    Parameters.Add_Grid (
        NULL, "LS"	, _TL ( "Calculated LS" ),
        _TL ( "Calculated LS factor" ),
        PARAMETER_OUTPUT
    );

    Parameters.Add_Grid (
        NULL, "SLOPE", _TL ( "Slope" ),
        _TL ( "Optional Slope output grid " ),
        PARAMETER_OUTPUT_OPTIONAL
    );
    Parameters.Add_Grid (
        NULL, "L"	, _TL ( "Calculated L" ),
        _TL ( "This will contain L. " ),
        PARAMETER_OUTPUT_OPTIONAL
    );
    Parameters.Add_Grid (
        NULL, "S"	, _TL ( "Calculated S" ),
        _TL ( "This will contain S. " ),
        PARAMETER_OUTPUT_OPTIONAL
    );

    Parameters.Add_Choice ( "",
                            "METHOD", _TL ( "Method to compute LS" ),
                            _TL ( "There are four options available: \n "
                                  " - Moore & Nieber (1989):  see model description-page."
                                  " - Desmet & Govers (1996): see model description-page. Using McCool et al. (1989, 1987) for the parameter m. \n"
                                  " - Van Oost (2003): same as Desmet & Govers, but with a different computation model for the parameter m. \n"
                                  " - Wishmeier a Smith (1978): see model description-page.\n"
                                  " - \n."),
                            CSG_String::Format ( "%s|%s|%s|%s|",
                                    _TL ( "Moore & Nieber 1989" ),
                                    _TL ( "Desmet & Govers 1996 (default of WaTEM)" ),
                                    _TL ( "Wischmeier & Smith 1978" ),
                                    _TL ( "Van Oost, 2003" )
                                               ), 1
                          );

}

//---------------------------------------------------------
CCalculate_LS_Watem::~CCalculate_LS_Watem ( void )
{}


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
bool CCalculate_LS_Watem::On_Execute ( void )
{
    int		x, y;

    //-----------------------------------------------------
    // Get parameter settings...

    m_pDEM		= Parameters ( "DEM" )->asGrid();
    m_pUp_Area = Parameters ( "UPSLOPE_AREA" )->asGrid();
    PRC = Parameters ( "PRC" )->asGrid();
    use_prc = Parameters ( "USEPRC" )->asBool();

    m_pLS		= Parameters ( "LS" )->asGrid();
    m_pL		= Parameters ( "L" )->asGrid();
    m_pS		= Parameters ( "S" )->asGrid();
    m_pSlope = Parameters ( "SLOPE" )->asGrid();
    m_Stability = 0;
    m_Erosivity = 1;
    m_Method = Parameters ( "METHOD" )->asInt();

    //-----------------------------------------------------
    // Check for valid parameter settings...


    //-----------------------------------------------------
    // Execute calculation...

    for ( y = 0; y < Get_NY() && Set_Progress ( y ); y++ )
    {
        #pragma omp parallel for
        for ( x = 0; x < Get_NX(); x++ )
        {
            if ( m_pDEM->is_NoData ( x, y ) )
            {
                m_pLS->Set_NoData ( x, y );
                if ( m_pSlope )
                {
                    m_pSlope->Set_NoData ( x, y );
                }
            }
            else
            {
                m_pLS->Set_Value ( x, y, Get_LS ( x, y ) );
            }
        }
    }


    //-----------------------------------------------------
    // Return 'true' if everything went okay...

    return ( true );
}


double CCalculate_LS_Watem::Get_LS ( int x, int y )
{
    double	L, S, LS, Slope, Aspect, Area, sin_Slope;

    //-----------------------------------------------------


    if ( !m_pDEM->Get_Gradient ( x, y, Slope, Aspect ) )
    {
        return ( -1.0 );
    }

    if ( use_prc )
    {
        CSG_Grid_System system = Get_System();

        int current_parcel = PRC->asInt ( x, y );
        double	z = m_pDEM->asDouble ( x, y ), dz[4];
        bool in_parcel[4] = { false, false, false, false };
        for ( int i = 0, iDir = 0, ix, iy; i < 4; i++, iDir += 2 )
        {
            if ( is_InGrid (
                        ix = system.Get_xTo ( iDir, x ),
                        iy = system.Get_yTo ( iDir, y ) ) && ( current_parcel == PRC->asInt ( ix, iy ) ) && !m_pDEM->is_NoData ( ix, iy ) )
            {
                dz[i] = m_pDEM->asDouble ( ix, iy ) - z;
                in_parcel[i] = true;
            }
            else
                if ( is_InGrid (
                            ix = system.Get_xFrom ( iDir, x ),
                            iy = system.Get_yFrom ( iDir, y ) ) && ( current_parcel == PRC->asInt ( ix, iy ) ) && !m_pDEM->is_NoData ( ix, iy ) )
                {
                    dz[i] = z - m_pDEM->asDouble ( ix, iy );
                    in_parcel[i] = true;
                }
                else
                {
                    dz[i] = 0.0;
                }
        }
        double G = 0;
        double H = 0;

        if ( in_parcel[0] || in_parcel[2] )
        {
            G = ( dz[0] - dz[2] ) / ( ( in_parcel[0] + in_parcel[2] ) * Get_Cellsize() );
        }
        if ( in_parcel[1] || in_parcel[3] )
        {
            H = ( dz[1] - dz[3] ) / ( ( in_parcel[1] + in_parcel[3] ) * Get_Cellsize() );
        }
        Slope = atan ( sqrt ( G * G + H * H ) );

    }





    //if (m_Method_Slope == 1)	// distance weighted average up-slope slope
    //{
    //	Slope = m_pUp_Slope->asDouble(x, y);
    //}

    if ( Slope <= 0.0 )
    {
        Slope = 0.000001;
    }
    if ( Aspect < 0.0 )
    {
        Aspect = 0.0;
    }

    if ( m_pSlope )
    {
        m_pSlope->Set_Value ( x, y, Slope );
    }

    sin_Slope = sin ( Slope );

    Area = m_pUp_Area->asDouble ( x, y );

    //-----------------------------------------------------
    switch ( m_Method )
    {
        //-----------------------------------------------------
        default:	// Moore and Nieber
            {
                LS = ( 0.4 + 1 ) * pow ( Area / 22.13, 0.4 ) * pow ( sin_Slope / 0.0896, 1.3 );
            }
            break;

        //-----------------------------------------------------
        case 1:		// Desmet and Govers
            {
                double	m, x;

                double beta = m_Erosivity * ( sin_Slope / 0.0896 ) / ( 3.0 * pow ( sin_Slope, 0.8 ) + 0.56 );
                m = beta / ( 1.0 + beta );

                x = fabs ( sin ( Aspect ) ) + fabs ( cos ( Aspect ) );

                // x: coefficient that adjusts for width of flow at the center of the cell.
                // It has a value of 1.0 when the flow is toward a side and sqrt(2.0) when
                // the flow is toward a corner (Kinnel 2005).

                L = ( pow ( Area + Get_Cellarea(), m + 1.0 ) - pow ( Area, m + 1.0 ) )
                    / ( pow ( Get_Cellsize(), m + 2.0 ) * pow ( 22.13, m ) * pow ( x, m ) );

                //-----------------------------------------------------

                if ( Slope < 0.08975817419 )		// <  9% (= atan(0.09)), ca. 5 Degree
                {
                    S = 10.8 * sin_Slope + 0.03;
                }
                else
                    if ( m_Stability == 0 )		// >= 9%, stable
                    {
                        S = 16.8 * sin_Slope - 0.5;
                    }
                    else							// >= 9%, thawing, unstable
                    {
                        S = pow ( sin_Slope / 0.896, 0.6 );
                    }

                if ( Area / ( x * Get_Cellsize() ) < 5.0 )
                {
                    //opm: dit staat zo in oorspronkelijke pascal code - niet volgens documentatie
                    //Sfactor: = 3.0*(power(SIN(slope[i, j]), 0.8)) + 0.56

                    // op overleg met KUL werd vermeld dat we hier eventueel minimum van de twee zouden kunnen nemen. Nog niet aangepast
                    S = std::min ( 3.0 * pow ( sin_Slope, 0.8 ) + 0.56, S );
                }

                LS = L * S;
            }
            break;

        //-----------------------------------------------------
        case 2:		// Wischmeier and Smith
            {
                if ( Slope > 0.0505 )	// >  ca. 3�
                {
                    LS = sqrt ( Area / 22.13 )
                         * ( 65.41 * sin_Slope * sin_Slope + 4.56 * sin_Slope + 0.065 );
                }
                else					// <= ca. 3�
                {
                    LS = pow ( Area / 22.13, 3.0 * pow ( Slope, 0.6 ) )
                         * ( 65.41 * sin_Slope * sin_Slope + 4.56 * sin_Slope + 0.065 );
                }
            }
            break;

        case 3: // Van Oost
            {
                double adjust = fabs ( sin ( Aspect ) ) + fabs ( cos ( Aspect ) );
                double exp = 0;
                if ( Area < 10000 )
                {
                    exp = 0.3 + pow ( Area / 10000, 0.8 );
                }
                else
                {
                    exp = 0.72;
                }

                if ( exp > 0.72 )
                {
                    exp = 0.72;
                }

                S = -1.5 + 17 / ( 1 + pow ( 2.718281828, 2.3 - 6.1 * sin ( Slope ) ) );
                L = ( pow ( Area + Get_Cellarea(), exp + 1 ) - pow ( Area, exp + 1 ) )
                    / ( pow ( adjust, exp ) * pow ( Get_Cellsize(), exp + 2 ) * pow ( 22.13, exp ) );

                LS = S * L;

            }

    }
    if ( m_pL )
    {
        m_pL->Set_Value ( x, y, L );
    }
    if ( m_pS )
    {
        m_pS->Set_Value ( x, y, S );
    }

    return ( LS );
}
