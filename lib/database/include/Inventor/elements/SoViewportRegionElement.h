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
 |	This file defines the SoViewportRegionElement class.
 |
 |   Author(s)		: Paul S. Strauss
 |
 ______________  S I L I C O N   G R A P H I C S   I N C .  ____________
 _______________________________________________________________________
 */

#ifndef  _SO_VIEWPORT_REGION_ELEMENT
#define  _SO_VIEWPORT_REGION_ELEMENT

#include <Inventor/SbViewportRegion.h>
#include <Inventor/elements/SoSubElement.h>

//////////////////////////////////////////////////////////////////////////////
//
//  Class: SoViewportRegionElement
//
//  Element that stores the current viewport region, which indicates
//  which part of the window is being rendered into. This information
//  is also needed during picking and bounding box computation to
//  determine sizes/locations of screen-space objects.
//
//////////////////////////////////////////////////////////////////////////////

SoEXTENDER class SoViewportRegionElement : public SoElement {

    SO_ELEMENT_HEADER(SoViewportRegionElement);

  public:
    // Initializes element
    virtual void	init(SoState *state);

    // Sets the viewport region
    static void		set(SoState *state,
			    const SbViewportRegion &vpReg);

    // Returns current viewport region from the state
    static const SbViewportRegion & get(SoState *state);

    // Returns TRUE if the viewports match
    virtual SbBool	matches(const SoElement *elt) const;

    // Create and return a copy of this element
    virtual SoElement	*copyMatchInfo() const;

    // Prints element (for debugging)
    virtual void	print(FILE *fp) const;

  SoINTERNAL public:
    // Initializes the SoViewportRegionElement class
    static void		initClass();

  protected:
    SbViewportRegion	viewportRegion;

    // Sets the region in an instance
    virtual void	setElt(const SbViewportRegion &vpReg);

    virtual ~SoViewportRegionElement();

  private:
    static SbViewportRegion *emptyViewportRegion;
};

#endif /* _SO_VIEWPORT_REGION_ELEMENT */
