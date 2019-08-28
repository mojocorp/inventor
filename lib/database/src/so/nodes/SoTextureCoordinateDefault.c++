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

/*
 * Copyright (C) 1990,91   Silicon Graphics, Inc.
 *
 _______________________________________________________________________
 ______________  S I L I C O N   G R A P H I C S   I N C .  ____________
 |
 |   $Revision: 1.1 $
 |
 |   Classes:
 |      SoTextureCoordinateDefault.  Forces shapes to generate their
 |   own, default texture coordinates.
 |
 |   Author(s)          : Thad Beier, Gavin Bell
 |
 ______________  S I L I C O N   G R A P H I C S   I N C .  ____________
 _______________________________________________________________________
 */

#include <Inventor/actions/SoCallbackAction.h>
#include <Inventor/actions/SoGLRenderAction.h>
#include <Inventor/actions/SoPickAction.h>
#include <Inventor/elements/SoGLTextureCoordinateElement.h>
#include <Inventor/nodes/SoTextureCoordinateDefault.h>

SO_NODE_SOURCE(SoTextureCoordinateDefault);

////////////////////////////////////////////////////////////////////////
//
// Description:
//    This initializes the SoTextureCoordinateDefault class.
//
// Use: internal

void
SoTextureCoordinateDefault::initClass()
//
////////////////////////////////////////////////////////////////////////
{
    SO__NODE_INIT_CLASS(SoTextureCoordinateDefault,
              "TextureCoordinateDefault", SoTextureCoordinateFunction);

    // Elements are enabled by SoTextureCoordinate nodes.
}

////////////////////////////////////////////////////////////////////////
//
// Description:
//    Constructor
//
// Use: public

SoTextureCoordinateDefault::SoTextureCoordinateDefault()
//
////////////////////////////////////////////////////////////////////////
{
    SO_NODE_CONSTRUCTOR(SoTextureCoordinateDefault);
    isBuiltIn = TRUE;
}

////////////////////////////////////////////////////////////////////////
//
// Description:
//    Destructor
//
// Use: public

SoTextureCoordinateDefault::~SoTextureCoordinateDefault()
//
////////////////////////////////////////////////////////////////////////
{
}

////////////////////////////////////////////////////////////////////////
//
// Description:
//    Does GL render action.
//
// Use: protected

void
SoTextureCoordinateDefault::GLRender(SoGLRenderAction *action)
//
////////////////////////////////////////////////////////////////////////
{
    SoState *state = action->getState();

    SoGLTextureCoordinateElement::setTexGen(state, this, NULL);

    SoTextureCoordinateDefault::doAction(action);
}

////////////////////////////////////////////////////////////////////////
//
// Description:
//    Does callback action thing.
//
// Use: protected

void
SoTextureCoordinateDefault::callback(SoCallbackAction *action)
//
////////////////////////////////////////////////////////////////////////
{
    SoTextureCoordinateDefault::doAction(action);
}

////////////////////////////////////////////////////////////////////////
//
// Description:
//    Does pick action...
//
// Use: protected

void
SoTextureCoordinateDefault::pick(SoPickAction *action)
//
////////////////////////////////////////////////////////////////////////
{
    SoTextureCoordinateDefault::doAction(action);
}

////////////////////////////////////////////////////////////////////////
//
// Description:
//    doAction.  Add this node to the state
//
// Use: Extender public

void
SoTextureCoordinateDefault::doAction(SoAction *action)
//
////////////////////////////////////////////////////////////////////////
{
    SoState *state = action->getState();
    // Store a pointer to the function used to compute the texture
    // coordinates in the state.
    SoTextureCoordinateElement::setDefault(state, this);
}
