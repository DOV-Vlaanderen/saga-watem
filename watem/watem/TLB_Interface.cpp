/**********************************************************
 * Version $Id: TLB_Interface.cpp 1921 2014-01-09 10:24:11Z oconrad $
 *********************************************************/

///////////////////////////////////////////////////////////
//                                                       //
//                         SAGA                          //
//                                                       //
//      System for Automated Geoscientific Analyses      //
//                                                       //
//                     Tool Library                      //
//                       watem                           //
//                                                       //
//-------------------------------------------------------//
//                                                       //
//                   TLB_Interface.cpp                   //
//                                                       //
//                 Copyright (C) 2017 by                 //
//                   Johan Van de Wauw                   //
//                                                       //
//-------------------------------------------------------//
//                                                       //
// This file is part of 'SAGA - System for Automated     //
// Geoscientific Analyses'. SAGA is free software; you   //
// can redistribute it and/or modify it under the terms  //
// of the GNU General Public License as published by the //
// Free Software Foundation; version 2 of the License.   //
//                                                       //
// SAGA is distributed in the hope that it will be       //
// useful, but WITHOUT ANY WARRANTY; without even the    //
// implied warranty of MERCHANTABILITY or FITNESS FOR A  //
// PARTICULAR PURPOSE. See the GNU General Public        //
// License for more details.                             //
//                                                       //
// You should have received a copy of the GNU General    //
// Public License along with this program; if not,       //
// write to the Free Software Foundation, Inc.,          //
// 51 Franklin Street, 5th Floor, Boston, MA 02110-1301, //
// USA.                                                  //
//                                                       //
///////////////////////////////////////////////////////////

//---------------------------------------------------------


///////////////////////////////////////////////////////////
//														 //
//           The Tool Link Library Interface             //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
// 1. Include the appropriate SAGA-API header...

#include "TLB_Interface.h"


//---------------------------------------------------------
// 2. Place general tool library informations here...

CSG_String Get_Info ( int i )
{
    switch ( i )
    {
        case TLB_INFO_Name:
        default:
            return ( _TL ( "Watem" ) );

        case TLB_INFO_Category:
            return ( _TL ( "Watem" ) );

        case TLB_INFO_Author:
            return ( SG_T ( "Johan Van de Wauw 2016 - 2017 - gebaseerd op originele pascal code van KULeuven."
                            "Ontwikkeld in opdracht van Vlaamse overheid, departement Omgeving." ) );

        case TLB_INFO_Description:
            return ( _TL ( "SAGA Implementatie van het WaTEM model (KULeuven). \n\n"
                           "WaTEM is a spatially distributed soil erosion model that was developed at the Physical"
                           "and Regional Geography Research Group (K.U.Leuven, Belgium)."
                           "The development of the SAGA-WaTEM module was financed by Vlaamse overheid "
                           "(Vlaams Planbureau voor de Omgeving, Departement Omgeving)."

                         ) );

        case TLB_INFO_Version:
            return ( SG_T ( "1.7.1" ) );

        case TLB_INFO_Menu_Path:
            return ( _TL ( "Watem|Watem" ) );
    }
}


//---------------------------------------------------------
// 3. Include the headers of your tools here...

#include "Calculate_LS_Watem.h"
#include "Calculate_Uparea.h"
#include "Water_Erosion.h"
#include "Tillage_Erosion.h"

//---------------------------------------------------------
// 4. Allow your tools to be created here...

CSG_Tool 		*Create_Tool ( int i )
{
    // Don't forget to continuously enumerate the case switches
    // when adding new tools! Also bear in mind that the
    // enumeration always has to start with [case 0:] and
    // that [default:] must return NULL!...

    switch ( i )
    {
        case 0:
            return ( TLB_INTERFACE_SKIP_TOOL );
        case 1:
            return ( new CCalculate_Uparea );
        case 2:
            return ( new CCalculate_LS_Watem );
        case 3:
            return ( new Water_Erosion );
        case 4:
            return ( new Tillage_Erosion );
        default:
            return ( NULL );
    }
}


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
//{{AFX_SAGA

TLB_INTERFACE

//}}AFX_SAGA
