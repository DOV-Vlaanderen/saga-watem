/**********************************************************
 * Version $Id$
 *********************************************************/

///////////////////////////////////////////////////////////
//                                                       //
//                         SAGA                          //
//                                                       //
//      System for Automated Geoscientific Analyses      //
//                                                       //
//                     Tool Library                      //
//                     Grid_Gridding                     //
//                                                       //
//-------------------------------------------------------//
//                                                       //
//                    LineDirection.h                    //
//                                                       //
//                 Copyright (C) 2019 by                 //
//                    Johan Van de Wauw                  //
//                      Olaf Conrad                      //
//                                                       //
//-------------------------------------------------------//
//                                                       //
// This file is part of 'SAGA - System for Automated     //
// Geoscientific Analyses'. SAGA is free software; you   //
// can redistribute it and/or modify it under the terms  //
// of the GNU General Public License as published by the //
// Free Software Foundation, either version 2 of the     //
// License, or (at your option) any later version.       //
//                                                       //
// SAGA is distributed in the hope that it will be       //
// useful, but WITHOUT ANY WARRANTY; without even the    //
// implied warranty of MERCHANTABILITY or FITNESS FOR A  //
// PARTICULAR PURPOSE. See the GNU General Public        //
// License for more details.                             //
//                                                       //
// You should have received a copy of the GNU General    //
// Public License along with this program; if not, see   //
// <http://www.gnu.org/licenses/>.                       //
//                                                       //
//-------------------------------------------------------//
//                                                       //
//    e-mail:     oconrad@saga-gis.org                   //
//                                                       //
//    contact:    Olaf Conrad                            //
//                Institute of Geography                 //
//                University of Goettingen               //
//                Goldschmidtstr. 5                      //
//                37077 Goettingen                       //
//                Germany                                //
//                                                       //
///////////////////////////////////////////////////////////

//---------------------------------------------------------


///////////////////////////////////////////////////////////
//														 //
//                                                       //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
#ifndef HEADER_INCLUDED__Shapes2Grid_H
#define HEADER_INCLUDED__Shapes2Grid_H


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
#include "MLB_Interface.h"


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
class CLineDirection : public CSG_Tool
{
public:
    CLineDirection(void);


protected:

    virtual bool				On_Execute				(void);
    virtual int					On_Parameter_Changed(CSG_Parameters *pParameters, CSG_Parameter *pParameter);
    virtual int					On_Parameters_Enable(CSG_Parameters *pParameters, CSG_Parameter *pParameter);


private:

    int							m_Multiple;
    TSG_Point_Int                   prev;

    CSG_Parameters_Grid_Target	m_Grid_Target;

    CSG_Grid					*m_pGrid;


    void						Set_Value				(int x, int y, double Value);


    void						Set_Line				(CSG_Shape *pShape);
    void						Set_Line_Fat			(TSG_Point a, TSG_Point b);
};


///////////////////////////////////////////////////////////
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
class CPolygons2Grid : public CSG_Tool
{
public:
    CPolygons2Grid(void);

    virtual CSG_String			Get_MenuPath			(void)	{
        return( _TL("Polygons") );
    }


protected:

    virtual bool				On_Execute				(void);

    virtual int					On_Parameter_Changed	(CSG_Parameters *pParameters, CSG_Parameter *pParameter);
    virtual int					On_Parameters_Enable	(CSG_Parameters *pParameters, CSG_Parameter *pParameter);


private:

    int							m_Multiple;

    CSG_Parameters_Grid_Target	m_Grid_Target;

    CSG_Grid					*m_pGrid, *m_pCoverage;


    TSG_Data_Type				Get_Data_Type			(int Field);

    void						Set_Value				(int x, int y, double Value, double Coverage);


};


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
#endif // #ifndef HEADER_INCLUDED__Shapes2Grid_H
