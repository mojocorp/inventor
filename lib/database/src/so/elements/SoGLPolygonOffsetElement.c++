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
 |	SoGLPolygonOffsetElement
 |
 |   Author(s)		: Morgan Leborgne
 |
 ______________  S I L I C O N   G R A P H I C S   I N C .  ____________
 _______________________________________________________________________
 */
#include <Inventor/elements/SoGLPolygonOffsetElement.h>
#include <glad/gl.h>

SO_ELEMENT_SOURCE(SoGLPolygonOffsetElement);

////////////////////////////////////////////////////////////////////////
//
// Description:
//    Destructor.
//
// Use: private

SoGLPolygonOffsetElement::~SoGLPolygonOffsetElement()
//
////////////////////////////////////////////////////////////////////////
{}

////////////////////////////////////////////////////////////////////////
//
// Description:
//    Initializes SoGLLinePatternElement class.
//
// Use: internal

void
SoGLPolygonOffsetElement::initClass()
//
////////////////////////////////////////////////////////////////////////
{
    SO_ELEMENT_INIT_CLASS(SoGLPolygonOffsetElement, SoPolygonOffsetElement);
}

////////////////////////////////////////////////////////////////////////
//
// Description:
//    Initializes element.
//
// Use: public

void
SoGLPolygonOffsetElement::init(SoState *state)
//
////////////////////////////////////////////////////////////////////////
{
    // Initialize base class stuff
    SoPolygonOffsetElement::init(state);
    copiedFromParent = NULL;
    whatChanged = 0;
}

////////////////////////////////////////////////////////////////////////
//
// Description:
//    Overrides push() method to copy polygon offset
//
// Use: public

void
SoGLPolygonOffsetElement::push(SoState *state)
//
////////////////////////////////////////////////////////////////////////
{
    const SoGLPolygonOffsetElement *prevElt =
        (const SoGLPolygonOffsetElement *)getNextInStack();

    offsetfactor = prevElt->offsetfactor;
    offsetunits = prevElt->offsetunits;
    style = prevElt->style;
    active = prevElt->active;

    copiedFromParent = state;
}

////////////////////////////////////////////////////////////////////////
//
// Description:
//    Pops element, causing side effects in GL.
//
// Use: public

void
SoGLPolygonOffsetElement::pop(SoState *state, const SoElement *childElt)
//
////////////////////////////////////////////////////////////////////////
{
    // Since popping this element has GL side effects, make sure any
    // open caches capture it.  We may not send any GL commands, but
    // the cache dependency must exist even if we don't send any GL
    // commands, because if the element changes, the _lack_ of GL
    // commands here is a bug (remember, GL commands issued here are
    // put inside the cache).
    capture(state);
    copiedFromParent = NULL;

    // If the previous element didn't have the same value...
    const SoGLPolygonOffsetElement *child =
        (const SoGLPolygonOffsetElement *)childElt;

    if (offsetfactor != child->offsetfactor ||
        offsetunits != child->offsetunits)
        whatChanged |= VALUE_MASK;
    if (style != child->style)
        whatChanged |= STYLE_MASK;
    if (active != child->active)
        whatChanged = VALUE_MASK | STYLE_MASK;

    if (whatChanged)
        send();
}

////////////////////////////////////////////////////////////////////////
//
// Description:
//    Sets polygon offset in element.
//
// Use: protected, virtual

void
SoGLPolygonOffsetElement::setElt(float _factor, float _units, Style _styles,
                                 SbBool _active)
//
////////////////////////////////////////////////////////////////////////
{
    // Optimization:  on push, we copy the value from the previous
    // element.  If the element is set to the same value, we don't
    // bother sending it, but do add a cache dependency on the
    // previous element.
    whatChanged = 0;

    if (offsetfactor != _factor || offsetunits != _units)
        whatChanged |= VALUE_MASK;
    if (style != _styles)
        whatChanged |= STYLE_MASK;
    if (active != _active)
        whatChanged = VALUE_MASK | STYLE_MASK;

    if (whatChanged) {
        SoPolygonOffsetElement::setElt(_factor, _units, _styles, _active);
        send();
        copiedFromParent = NULL;
    } else if (copiedFromParent) {
        SoGLPolygonOffsetElement *parent =
            (SoGLPolygonOffsetElement *)getNextInStack();
        parent->capture(copiedFromParent);
    }
}

////////////////////////////////////////////////////////////////////////
//
// Description:
//    Sends polygon offset in element to GL.
//
// Use: private

void
SoGLPolygonOffsetElement::send()
//
////////////////////////////////////////////////////////////////////////
{
    if (active) {
        if (whatChanged & STYLE_MASK) {
            if (style & FILLED)
                glEnable(GL_POLYGON_OFFSET_FILL);
            else
                glDisable(GL_POLYGON_OFFSET_FILL);
            if (style & LINES)
                glEnable(GL_POLYGON_OFFSET_LINE);
            else
                glDisable(GL_POLYGON_OFFSET_LINE);
            if (style & POINTS)
                glEnable(GL_POLYGON_OFFSET_POINT);
            else
                glDisable(GL_POLYGON_OFFSET_POINT);
        }

        if ((whatChanged & VALUE_MASK)) {
            glPolygonOffset(offsetfactor, offsetunits);
        }
    } else {
        glDisable(GL_POLYGON_OFFSET_FILL);
        glDisable(GL_POLYGON_OFFSET_LINE);
        glDisable(GL_POLYGON_OFFSET_POINT);
    }
}
