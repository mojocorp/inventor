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
 * Copyright (C) 1990,91,92,93   Silicon Graphics, Inc.
 *
 _______________________________________________________________________
 ______________  S I L I C O N   G R A P H I C S   I N C .  ____________
 |
 |   $Revision: 1.1 $
 |
 |   Description:
 |	This file contains the definition of the SoOffscreenRenderer class.
 |
 |   Classes:
 |	SoOffscreenRenderer
 |
 |   Author(s)		: Dave Immel
 |
 ______________  S I L I C O N   G R A P H I C S   I N C .  ____________
 _______________________________________________________________________
 */

#ifndef  _SO_OFFSCREEN_RENDERER_
#define  _SO_OFFSCREEN_RENDERER_

#include <stdio.h>
#include <Inventor/SbColor.h>
#include <Inventor/SbViewportRegion.h>
#include <Inventor/SbVec.h>
#include <Inventor/SbImage.h>

class SoNode;
class SoPath;
class SoGLRenderAction;

//////////////////////////////////////////////////////////////////////////////
//
//  Class: SoOffscreenRenderer
//
//  This file contains the definition of the SoOffscreenRenderer class.
//  This class is used for rendering a scene graph to an offscreen memory
//  buffer which can be used for printing or generating textures.
//
//  The implementation of this class uses the X Pixmap for rendering.
//
//////////////////////////////////////////////////////////////////////////////

class SoOffscreenRenderer {
 public:

    // Constructor
    SoOffscreenRenderer( const SbViewportRegion &viewportRegion );
    SoOffscreenRenderer( SoGLRenderAction *ra );

    // Destructor
    ~SoOffscreenRenderer();

    enum Components {
        LUMINANCE              = SbImage::Format_Luminance,
        LUMINANCE_TRANSPARENCY = SbImage::Format_Luminance_Alpha,
        RGB                    = SbImage::Format_RGB24, // The default
        RGB_TRANSPARENCY       = SbImage::Format_RGBA32
    };

    static float	getScreenPixelsPerInch();

    // Set/get the components to be rendered
    void		setComponents( Components components )
				{comps = components;}
    Components		getComponents() const
				{return comps;}

    // Set/get the viewport region
    void		setViewportRegion( const SbViewportRegion &region );
    const SbViewportRegion  &getViewportRegion() const;

    // Get the maximum supported resolution of the viewport.
    static SbVec2s	getMaximumResolution();

    // Set/get the background color
    void		setBackgroundColor( const SbColor &c )
				{backgroundColor = c;}
    const SbColor & getBackgroundColor() const
				{return backgroundColor;}

    // Set and get the render action to use
    void                      setGLRenderAction(SoGLRenderAction *ra);
    SoGLRenderAction *        getGLRenderAction() const;

    // Render the given scene into a buffer
    SbBool		render( SoNode *scene );
    SbBool		render( SoPath *scene );

	// Returns the SbImage containing the rendered image.
    const SbImage & getImage() const { return pixelBuffer; }
	
    // Return the buffer containing the rendering
    const unsigned char *     getBuffer() const;

    // Write the buffer as a .rgb file into the given FILE
    SbBool		writeToRGB( FILE *fp ) const;

    // Write the buffer into encapsulated PostScript.  If a print size is
    // not given, adjust the size of the print so it is WYSIWYG with respect
    // to the viewport region on the current device.
    SbBool		writeToPostScript( FILE *fp ) const;
    SbBool		writeToPostScript( FILE *fp,
                                const SbVec2f &printSize ) const;

  private:
    SbImage             pixelBuffer;
    Components          comps;
    SbColor		backgroundColor;
    SoGLRenderAction	*userAction, *offAction;
    int                 cacheContext;
    SbViewportRegion    renderedViewport;

    // These are used for rendering to the offscreen pixmap
    struct SbGLXContext* ctx;

    bool renderGeneric( class SoBase *base );
};

#endif /* _SO_OFFSCREEN_RENDERER_ */

