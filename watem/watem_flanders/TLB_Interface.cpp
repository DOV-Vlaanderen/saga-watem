///////////////////////////////////////////////////////////
//                                                       //
//                         SAGA                          //
//                                                       //
//      System for Automated Geoscientific Analyses      //
//                                                       //
//                     Tool Library                      //
//                    watem_flanders                     //
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
            return ( _TL ( "Watem - specific modules for Flanders" ) );

        case TLB_INFO_Category:
            return ( _TL ( "Watem" ) );

        case TLB_INFO_Author:
            return ( SG_T ( "Johan Van de Wauw 2016 - 2017 - gebaseerd op originele pascal code van KULeuven."
                            "Ontwikkeld in opdracht van Vlaamse overheid, departement Omgeving." ) );

        case TLB_INFO_Description:
            return ( _TL ( "SAGA implementation of WaTEM model (KULeuven), with modules for processing inputfiles. \n\n"
                           "WaTEM/SEDEM is a spatially distributed soil erosion and sediment delivery model that is developed at the Physical"
                           "and Regional Geography Research Group (K.U.Leuven, Belgium) as a user-friendly and optimised version of the WaTEM and SEDEM models, "
                           "which were also developed at K.U.Leuven. These models were specifically developed to simulate the impact of soil conservation and sediment "
                           "control measures as well as land use changes in the framework of an integrated catchment management, on the local soil loss and sediment "
                           "delivery to rivers. This model combines the WaTEM and SEDEM models and aims at providing useful information for land managers to select "
                           "the most effective catchment management scenario.)."

                         ) );

        case TLB_INFO_Version:
            return ( SG_T ( "1.7.1" ) );

        case TLB_INFO_Menu_Path:
            return ( _TL ( "Watem|Watem - Flanders" ) );
    }
}


//---------------------------------------------------------
// 3. Include the headers of your tools here...

#include "Create_Parcel_Grid.h"
#include "Parcel_Filter.h"
#include "Calculate_C_Grid.h"
#include "Parcel_C_Grid.h"
#include "Complete.h"

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
            return ( new Create_Parcel_Grid );
        case 2:
            return ( new Parcel_Filter );
        case 3:
            return ( new Calculate_C_Grid );
        case 4:
            return ( new Parcel_C_Grid );
        case 5:
            return ( new Complete );
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
