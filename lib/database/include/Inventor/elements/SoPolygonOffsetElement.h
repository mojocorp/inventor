/*
 *
 *  Copyright (C) 2000 Silicon Graphics, Inc.  All Rights Reserved.
 *
 *  This library is free software; you can redistribute it and/or
 *  modify it under the terms of the GNU Lesser General Public
 *  License as published by the Free Software Foundation; either
 *  version 2.1 of the License, or (at your option) any later version.
 *
 *  This library is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 *  Lesser General Public License for more details.
 *
 *  Further, this software is distributed without any warranty that it is
 *  free of the rightful claim of any third person regarding infringement
 *  or the like.  Any license provided herein, whether implied or
 *  otherwise, applies only to this software file.  Patent licenses, if
 *  any, provided herein do not apply to combinations of this program with
 *  other software, or any other product whatsoever.
 *
 *  You should have received a copy of the GNU Lesser General Public
 *  License along with this library; if not, write to the Free Software
 *  Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 *
 *  Contact information: Silicon Graphics, Inc., 1600 Amphitheatre Pkwy,
 *  Mountain View, CA  94043, or:
 *
 *  http://www.sgi.com
 *
 *  For further information regarding this notice, see:
 *
 *  http://oss.sgi.com/projects/GenInfo/NoticeExplan/
 *
 */

//  -*- C++ -*-

/*
 * Copyright (C) 1990,91   Silicon Graphics, Inc.
 *
 _______________________________________________________________________
 ______________  S I L I C O N   G R A P H I C S   I N C .  ____________
 |
 |   $Revision: 1.1 $
 |
 |   Description:
 |	This file defines the SoPolygonOffsetElement class.
 |
 |   Author(s)		: Morgan Leborgne
 |
 ______________  S I L I C O N   G R A P H I C S   I N C .  ____________
 _______________________________________________________________________
 */
#ifndef  _SO_POLYGON_OFFSET_ELEMENT_
#define  _SO_POLYGON_OFFSET_ELEMENT_

#include <Inventor/elements/SoReplacedElement.h>

//////////////////////////////////////////////////////////////////////////////
//
//  Class: SoPolygonOffsetElement
//
//  Element that stores the current polygon offset.
//
//  This class allows read-only access to the top element in the state
//  to make accessing several values in it more efficient. Individual
//  values must be accessed through this instance.
//
//////////////////////////////////////////////////////////////////////////////

SoEXTENDER class SoPolygonOffsetElement : public SoReplacedElement
{
   SO_ELEMENT_HEADER(SoPolygonOffsetElement);

   public:

      /// Polygon mode
      enum Style {
         FILLED = 0x01,
         LINES  = 0x02,
         POINTS = 0x04
      };

      /// Initializes element
      virtual void	init(SoState * state);

      /// Sets the current polygon offset
      static void	set(SoState *state, SoNode *node, float factor, float units, Style styles, SbBool on);
     
      /// Returns the polygon offset from an element
      static void	get(SoState *state, float &factor, float &units, Style &styles, SbBool &on);
      
      /// Returns the default polygon offset.
      static void	getDefault(float &factor, float &units, Style &styles, SbBool &on);

   protected:

      Style  style;
      SbBool active;
      float  offsetfactor;
      float  offsetunits;

      // Destructor.
      virtual		~SoPolygonOffsetElement();

      virtual void	setElt(float factor, float units, Style styles, SbBool on);

   SoINTERNAL public:

      // Initializes the SoPolygonOffsetElement class.
      static void       initClass();
};

#endif  // _SO_POLYGON_OFFSET_ELEMENT_