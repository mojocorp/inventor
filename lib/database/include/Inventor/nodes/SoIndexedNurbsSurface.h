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
 |	This file defines the SoIndexedNurbsSurface node class.
 |
 |   Author(s)		: Dave Immel
 |
 ______________  S I L I C O N   G R A P H I C S   I N C .  ____________
 _______________________________________________________________________
 */

#pragma once

#include <Inventor/SbBox.h>
#include <Inventor/fields/SoSFInt32.h>
#include <Inventor/fields/SoMFFloat.h>
#include <Inventor/fields/SoMFInt32.h>
#include <Inventor/nodes/SoShape.h>

class SoState;
class SoProfileBundle;
class SoShapeBundle;
class GLUnurbs;

/// Indexed NURBS surface shape node.
/// \ingroup Nodes
/// This shape node represents a NURBS surface based on the knot vectors
/// and the control points that you specify. The #uKnotVector and
/// #vKnotVector fields specify floating-point arrays of values; the
/// values are the coordinates of the knot points in the surface, and you
/// must enter them in non-decreasing order.  The #numUControlPoints
/// and #numVControlPoints fields specify the number of control points
/// the surface will have in the U and V parametric directions, and will
/// use the current coordinates that are indexed from the #coordIndex
/// field.
///
/// You can get a surface of minimum order (2) in the U or V directions by
/// specifying two more knots than control points in that direction and
/// having at least two control points in that direction.  This surface
/// would appear creased in one direction.
///
/// You can get a surface of maximum order (8) in the U or V directions by
/// specifying eight more knots than control points in that direction and
/// having at least eight control points in that direction.
///
/// The control points of the NURBS surface are transformed by the current
/// cumulative transformation. The surface is drawn with the current light
/// model and drawing style. The coordinates, normals, and texture
/// coordinates of a surface are generated, so you cannot bind explicit
/// normals or texture coordinates to a NURBS surface. The first material
/// in the state is applied to the entire surface.
///
/// The surface is trimmed according to the currently defined profiles curves.
///
/// When default texture coordinates are applied to a NURBS surface, the
/// edges of the texture square are stretched to fit the surface. The axes
/// of the texture are called S and T; S is horizontal and T is vertical.
/// The axes of the NURBS surface are called U and V; U is horizontal and
/// V is vertical. You can also define texture coordinates explicitly with
/// the S,T location point, the knot vectors, and the current texture
/// coordinates.
///
/// \par Action behavior:
/// <b>SoGLRenderAction</b>
/// Draws the surface based on the current coordinates, material, and so on.
/// <b>SoRayPickAction</b>
/// Picks the surface based on the current coordinates and transformation.
/// <b>SoGetBoundingBoxAction</b>
/// Computes the bounding box that encloses all control points of the
/// surface with the current transformation applied to them. Sets the
/// center to the average of the control points.
/// <b>SoCallbackAction</b>
/// If any triangle callbacks are registered with the action, they will be
/// invoked for each successive triangle approximating the surface.
///
/// \par File format/defaults:
/// \code
/// SoIndexedNurbsSurface {
///    numUControlPoints	0
///    numVControlPoints	0
///    numSControlPoints	0
///    numTControlPoints	0
///    coordIndex	0
///    uKnotVector	0
///    vKnotVector	0
///    sKnotVector	0
///    tKnotVector	0
///    textureCoordIndex	-1
/// }
/// \endcode
/// \sa SoIndexedNurbsCurve,SoNurbsSurface,SoProfile
class INVENTOR_API SoIndexedNurbsSurface : public SoShape {

    SO_NODE_HEADER(SoIndexedNurbsSurface);

  public:
    // Fields
    SoSFInt32
        numUControlPoints; ///< Number of control points in the U direction.
    SoSFInt32
              numVControlPoints; ///< Number of control points in the V direction.
    SoMFInt32 coordIndex;        ///< Coordinate indices.
    SoMFFloat uKnotVector;       ///< The knot vectors in the U direction.
    SoMFFloat vKnotVector;       ///< The knot vectors in the V direction.
    SoSFInt32
        numSControlPoints; ///< Number of control points in the S direction.
    SoSFInt32
              numTControlPoints; ///< Number of control points in the T direction.
    SoMFInt32 textureCoordIndex; ///< Texture coordinate indices.
    SoMFFloat sKnotVector;       ///< The knot vectors in the S direction.
    SoMFFloat tKnotVector;       ///< The knot vectors in the T direction.

    /// Creates an indexed NURBS surface node with default settings.
    SoIndexedNurbsSurface();

    SoEXTENDER
  public:
    virtual void GLRender(SoGLRenderAction *action);

    SoINTERNAL
  public:
    static void initClass();

  protected:
    // Generates triangles representing surface
    virtual void generatePrimitives(SoAction *action);

    // Computes bounding box of surface
    virtual void computeBBox(SoAction *action, SbBox3f &box, SbVec3f &center);

    ~SoIndexedNurbsSurface();

  private:
    // Renders the surface
    void drawNURBS(GLUnurbs *render, SoAction *action, bool doTextures);
};

