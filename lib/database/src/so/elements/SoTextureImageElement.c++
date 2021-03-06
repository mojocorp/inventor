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
 |   $Revision $
 |
 |   Classes:
 |	SoTextureImageElement class.
 |
 |   Author(s)		: Gavin Bell
 |
 ______________  S I L I C O N   G R A P H I C S   I N C .  ____________
 _______________________________________________________________________
 */

#include <Inventor/elements/SoTextureImageElement.h>

SO_ELEMENT_SOURCE(SoTextureImageElement);

////////////////////////////////////////////////////////////////////////
//
// Description:
//    Initializes SoTextureImageElement class.
//
// Use: internal

void
SoTextureImageElement::initClass() {
    SO_ELEMENT_INIT_CLASS(SoTextureImageElement, SoReplacedElement);
}

////////////////////////////////////////////////////////////////////////
//
// Description:
//    Destructor.
//
// Use: private

SoTextureImageElement::~SoTextureImageElement()
//
////////////////////////////////////////////////////////////////////////
{}

////////////////////////////////////////////////////////////////////////
//
// Description:
//    Initializes element
//
// Use: public

void
SoTextureImageElement::init(SoState *state)
//
////////////////////////////////////////////////////////////////////////
{
    SoReplacedElement::init(state);
}

////////////////////////////////////////////////////////////////////////
//
// Description:
//    Sets texture image in element accessed from state.
//
// Use: public, static

void
SoTextureImageElement::set(SoState *state, SoNode *node, const SbImage &img,
                           int wrapS, int wrapT, int model, int minFilter,
                           int magFilter, const SbColor &blendColor)
//
////////////////////////////////////////////////////////////////////////
{
    SoTextureImageElement *elt;

    // Get an instance we can change (pushing if necessary)
    elt = (SoTextureImageElement *)getElement(state, classStackIndex, node);

    elt->setElt(img, wrapS, wrapT, model, minFilter, magFilter, blendColor);
}

////////////////////////////////////////////////////////////////////////
//
// Description:
//    Really do the set.  This is virtual so the GLTextureImageElement
//    can easily override the generic behavior to send textures to GL.
//
// Use: protected, virtual

void
SoTextureImageElement::setElt(const SbImage &_image, int _wrapS, int _wrapT,
                              int _model, int _minFilter, int _magFilter,
                              const SbColor &_blendColor)
//
////////////////////////////////////////////////////////////////////////
{
    image = _image;
    wrapS = _wrapS;
    wrapT = _wrapT;
    model = _model;
    minFilter = _minFilter;
    magFilter = _magFilter;
    blendColor = _blendColor;
}

////////////////////////////////////////////////////////////////////////
//
// Description:
//    Returns texture image from state.
//
// Use: public, static

const SbImage &
SoTextureImageElement::get(SoState *state, int &_wrapS, int &_wrapT,
                           int &_model, int &_minFilter, int &_magFilter,
                           SbColor &_blendColor)
//
////////////////////////////////////////////////////////////////////////
{
    const SoTextureImageElement *elt;

    elt =
        (const SoTextureImageElement *)getConstElement(state, classStackIndex);

    _wrapS = elt->wrapS;
    _wrapT = elt->wrapT;
    _model = elt->model;
    _minFilter = elt->minFilter;
    _magFilter = elt->magFilter;
    _blendColor = elt->blendColor;

    return elt->image;
}

// Deprecated
const unsigned char *
SoTextureImageElement::get(SoState *state, SbVec2s &_size, int &_numComponents,
                           int &_wrapS, int &_wrapT, int &_model,
                           SbColor &_blendColor) {
    const SoTextureImageElement *elt;

    elt =
        (const SoTextureImageElement *)getConstElement(state, classStackIndex);

    _size = SbVec2s(elt->image.getSize().getValue());
    _numComponents = elt->image.getNumComponents();
    _wrapS = elt->wrapS;
    _wrapT = elt->wrapT;
    _model = elt->model;
    _blendColor = elt->blendColor;

    return elt->image.getConstBytes();
}
////////////////////////////////////////////////////////////////////////
//
// Description:
//    Returns TRUE if the texture contains transparency info.
//
// Use: public, static

SbBool
SoTextureImageElement::containsTransparency(SoState *state)
//
////////////////////////////////////////////////////////////////////////
{
    const SoTextureImageElement *elt;

    elt =
        (const SoTextureImageElement *)getConstElement(state, classStackIndex);

    return elt->image.hasAlphaChannel();
}

////////////////////////////////////////////////////////////////////////
//
// Description:
//    Get default image (a NULL 0 by 0 by 0 image)
//
// Use: public

const unsigned char *
SoTextureImageElement::getDefault(SbVec2s &s, int &nc)
//
////////////////////////////////////////////////////////////////////////
{
    s[0] = 0;
    s[1] = 0;
    nc = 0;
    return NULL;
}

////////////////////////////////////////////////////////////////////////
//
// Description:
//    Prints element for debugging.
//
// Use: public
//
////////////////////////////////////////////////////////////////////////

#ifdef DEBUG
void
SoTextureImageElement::print(FILE *fp) const {
    SoReplacedElement::print(fp);
}
#else  /* DEBUG */
void
SoTextureImageElement::print(FILE *) const {}
#endif /* DEBUG */
