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
 * Copyright (C) 1990,91,92,93   Silicon Graphics, Inc.
 *
 _______________________________________________________________________
 ______________  S I L I C O N   G R A P H I C S   I N C .  ____________
 |
 |   $Revision: 1.2 $
 |
 |   Classes:
 |      SoIndexedNurbsCurve
 |
 |   Author(s)          : Dave Immel
 |
 ______________  S I L I C O N   G R A P H I C S   I N C .  ____________
 _______________________________________________________________________
 */

#include <glad/gl.h>
#include <GL/glu.h>
#include <Inventor/SbBox.h>
#include <Inventor/SoDB.h>
#include <Inventor/SoPrimitiveVertex.h>
#include <Inventor/actions/SoGLRenderAction.h>
#include <Inventor/actions/SoGetBoundingBoxAction.h>
#include <Inventor/actions/SoRayPickAction.h>
#include <Inventor/bundles/SoMaterialBundle.h>
#include <Inventor/caches/SoBoundingBoxCache.h>
#include <Inventor/details/SoDetail.h>
#include <Inventor/elements/SoComplexityElement.h>
#include <Inventor/elements/SoComplexityTypeElement.h>
#include <Inventor/elements/SoCoordinateElement.h>
#include <Inventor/elements/SoDrawStyleElement.h>
#include <Inventor/elements/SoGLTextureEnabledElement.h>
#include <Inventor/elements/SoGLLazyElement.h>
#include <Inventor/elements/SoModelMatrixElement.h>
#include <Inventor/elements/SoProjectionMatrixElement.h>
#include <Inventor/elements/SoViewingMatrixElement.h>
#include <Inventor/elements/SoViewportRegionElement.h>
#include <Inventor/nodes/SoIndexedNurbsCurve.h>

#include <vector>

SO_NODE_SOURCE(SoIndexedNurbsCurve);

////////////////////////////////////////////////////////////////////////
//
// Description:
//    This initializes the SoIndexedNurbsCurve class.
//
// Use: internal

void
SoIndexedNurbsCurve::initClass()
//
////////////////////////////////////////////////////////////////////////
{
    SO__NODE_INIT_CLASS(SoIndexedNurbsCurve, "IndexedNurbsCurve", SoShape);
}

////////////////////////////////////////////////////////////////////////
//
// Description:
//    Constructor
//
// Use: public

SoIndexedNurbsCurve::SoIndexedNurbsCurve()
//
////////////////////////////////////////////////////////////////////////
{
    SO_NODE_CONSTRUCTOR(SoIndexedNurbsCurve);

    SO_NODE_ADD_FIELD(numControlPoints, (0));
    SO_NODE_ADD_FIELD(coordIndex, (0));
    SO_NODE_ADD_FIELD(knotVector, (0.0));

    isBuiltIn = TRUE;
}

////////////////////////////////////////////////////////////////////////
//
// Description:
//    Destructor
//
// Use: private

SoIndexedNurbsCurve::~SoIndexedNurbsCurve()
//
////////////////////////////////////////////////////////////////////////
{}

////////////////////////////////////////////////////////////////////////
//
// Description:
//    Performs GL rendering of the NURBS surface.
//
// Use: private

void
SoIndexedNurbsCurve::GLRender(SoGLRenderAction *action)
//
////////////////////////////////////////////////////////////////////////
{
    // First see if the object is visible and should be rendered now
    if (!shouldGLRender(action))
        return;

    SoState *state = action->getState();
    state->push();

    // Draw unlit:
    if (SoLazyElement::getLightModel(state) != SoLazyElement::BASE_COLOR) {
        SoLazyElement::setLightModel(state, SoLazyElement::BASE_COLOR);
    }
    // Make sure textures are disabled, as texturing of NURBS curves
    // has not been implemented.
    if (SoGLTextureEnabledElement::get(state)) {
        SoGLTextureEnabledElement::set(state, FALSE);
    }

    // Make sure the first current material is sent to GL
    SoMaterialBundle mb(action);
    mb.sendFirst();

    //
    // Create a NURBS library renderer, and use it by making regular
    // GL calls to it.
    //
    GLUnurbs *nurbsObj = gluNewNurbsRenderer();

    // Draw the curve
    gluNurbsProperty(nurbsObj, GLU_NURBS_MODE, GLU_NURBS_TESSELLATOR);
    gluNurbsCallback(nurbsObj, GLU_NURBS_BEGIN, (void (*)())glBegin);
    gluNurbsCallback(nurbsObj, GLU_NURBS_END, (void (*)())glEnd);
    gluNurbsCallback(nurbsObj, GLU_NURBS_VERTEX, (void (*)())glVertex3fv);
    gluNurbsCallback(nurbsObj, GLU_NURBS_NORMAL, (void (*)())glNormal3fv);

    drawNURBS(nurbsObj, action);

    gluDeleteNurbsRenderer(nurbsObj);

    // Restore state
    state->pop();

    return;
}

////////////////////////////////////////////////////////////////////////
//
// Description:
//    Generates lines representing the NURBS curve.
//
// Use: protected

void
SoIndexedNurbsCurve::generatePrimitives(SoAction *action)
//
////////////////////////////////////////////////////////////////////////
{
    GLUnurbs *theNurb = gluNewNurbsRenderer();

    //
    // Draw the NURBS surface.  The SoPrimRender class will receive primitive
    // drawn by the NURBS library, triangulate them, and send them to the
    // callback.
    struct glu_cb_data {
        std::vector<SbVec3f> vertices;

        static void vertexCallback(float *vertex, glu_cb_data *userData) {
            userData->vertices.push_back(
                SbVec3f(vertex[0], vertex[1], vertex[2]));
        }
    } data;

    gluNurbsProperty(theNurb, GLU_NURBS_MODE, GLU_NURBS_TESSELLATOR);
    gluNurbsCallbackData(theNurb, &data);
    gluNurbsCallback(theNurb, GLU_NURBS_VERTEX_DATA,
                     (void(GLAPIENTRY *)())glu_cb_data::vertexCallback);

    drawNURBS(theNurb, action);

    gluDeleteNurbsRenderer(theNurb);

    for (size_t i = 0; i < data.vertices.size() - 1; i += 2) {
        SoPrimitiveVertex pv[2];
        pv[0].setPoint(data.vertices[i]);
        pv[1].setPoint(data.vertices[i + 1]);

        invokeLineSegmentCallbacks(action, &pv[0], &pv[1]);
    }
}

////////////////////////////////////////////////////////////////////////
//
// Description:
//    Redefines this to tell open caches that they contain lines.
//
// Use: extender

void
SoIndexedNurbsCurve::getBoundingBox(SoGetBoundingBoxAction *action)
//
////////////////////////////////////////////////////////////////////////
{
    // Let our parent class do the real work
    SoShape::getBoundingBox(action);

    // If there are any open bounding box caches, tell them that they
    // contain lines
    SoBoundingBoxCache::setHasLinesOrPoints(action->getState());
}

////////////////////////////////////////////////////////////////////////
//
// Description:
//    Computes bounding box for the NURBS curve.
//
// Use: protected

void
SoIndexedNurbsCurve::computeBBox(SoAction *action, SbBox3f &box,
                                 SbVec3f &center)
//
////////////////////////////////////////////////////////////////////////
{
    const int32_t numCoordIndices = coordIndex.getNum();

    // Check for a degenerate curve
    if (numCoordIndices == 0)
        return;

    //
    // Loop through coordinates, keeping max bounding box and sum of coords
    // If the coordinates are rational, divide the first three values by
    // the fourth value before extending the bounding box.
    //
    const int32_t *            coordIndices = coordIndex.getValues(0);
    const SoCoordinateElement *ce =
        SoCoordinateElement::getInstance(action->getState());
    center.setValue(0.0, 0.0, 0.0);
    if (ce->is3D()) {
        for (int i = 0; i < numCoordIndices; i++) {
            const SbVec3f &coord = ce->get3((int)coordIndices[i]);
            box.extendBy(coord);
            center += coord;
        }
    } else {
        for (int i = 0; i < numCoordIndices; i++) {
            const SbVec4f &coord = ce->get4((int)coordIndices[i]);
            SbVec3f        tmpCoord;
            coord.getReal(tmpCoord);
            box.extendBy(tmpCoord);
            center += tmpCoord;
        }
    }

    center /= (float)numCoordIndices;
}

////////////////////////////////////////////////////////////////////////
//
// Description:
//    Draws the NURBS curve.
//
// Use: protected

void
SoIndexedNurbsCurve::drawNURBS(GLUnurbs *render, SoAction *action)

//
////////////////////////////////////////////////////////////////////////
{
    SoState *state = action->getState();

    const SoCoordinateElement *ce = SoCoordinateElement::getInstance(state);

    const int32_t numCoordIndices = coordIndex.getNum();

    // Check for a degenerate curve
    if (numCoordIndices == 0)
        return;

    float val = SoComplexityElement::get(state);
    if (val < 0.0)
        val = 0.0f;
    if (val > 1.0)
        val = 1.0f;

    if (SoComplexityTypeElement::get(state) ==
        SoComplexityTypeElement::OBJECT_SPACE) {
        //
        // Set the nurbs properties to render the curve with uniform
        // tessellation over the whole curve.
        //
        gluNurbsProperty(render, GLU_SAMPLING_METHOD, GLU_DOMAIN_DISTANCE);
        gluNurbsProperty(render, GLU_U_STEP, 200 * val);
    } else {
        //
        // Set the nurbs properties to render the curve with a view
        // dependent tessellation using the given pixel tolerance.
        //
        float pixTolerance;

        if (val < 0.10f)
            pixTolerance = 10.f;
        else if (val < 0.20f)
            pixTolerance = 8.f;
        else if (val < 0.30f)
            pixTolerance = 6.f;
        else if (val < 0.40f)
            pixTolerance = 4.f;
        else if (val < 0.50f)
            pixTolerance = 2.f;
        else if (val < 0.70f)
            pixTolerance = 1.f;
        else if (val < 0.80f)
            pixTolerance = .5f;
        else if (val < 0.90f)
            pixTolerance = .25f;
        else
            pixTolerance = .125f;

        gluNurbsProperty(render, GLU_SAMPLING_METHOD, GLU_OBJECT_PATH_LENGTH);
        gluNurbsProperty(render, GLU_SAMPLING_TOLERANCE, pixTolerance);
    }

    if (!action->isOfType(SoGLRenderAction::getClassTypeId())) {
        const SbMatrix matModelView = SoModelMatrixElement::get(state) *
                                      SoViewingMatrixElement::get(state);
        const SbMatrix &matProjection = SoProjectionMatrixElement::get(state);

        const SbViewportRegion &vpRegion = SoViewportRegionElement::get(state);
        const SbVec2s &         vpOrig = vpRegion.getViewportOriginPixels();
        const SbVec2s &         vpSize = vpRegion.getViewportSizePixels();

        GLint viewport[4] = {vpOrig[0], vpOrig[1], vpSize[0], vpSize[1]};

        gluNurbsProperty(render, GLU_AUTO_LOAD_MATRIX, GL_FALSE);
        gluLoadSamplingMatrices(render, (float *)matModelView.getValue(),
                                (float *)matProjection.getValue(), viewport);
    }

    //
    // Get the array of control points.  Check to see if they are
    // non-rational or rational.
    //
    const int32_t *      coordIndices = coordIndex.getValues(0);
    std::vector<GLfloat> coords;
    if (ce->is3D()) {
        coords.resize(3 * numCoordIndices);
        for (int i = 0; i < numCoordIndices; i++) {
            const SbVec3f &c3 = ce->get3(coordIndices[i]);
            coords[3 * i + 0] = c3[0];
            coords[3 * i + 1] = c3[1];
            coords[3 * i + 2] = c3[2];
        }
    } else {
        coords.resize(4 * numCoordIndices);
        for (int i = 0; i < numCoordIndices; i++) {
            const SbVec4f &c4 = ce->get4(coordIndices[i]);
            coords[3 * i + 0] = c4[0];
            coords[3 * i + 1] = c4[1];
            coords[3 * i + 2] = c4[2];
            coords[3 * i + 3] = c4[3];
        }
    }

    //
    // Render the NURBS curve as if to the GL, but use the given
    // NurbsTessellator instead which will make calls to the
    // software NURBS library.
    //
    gluBeginCurve(render);
    gluNurbsCurve(render, knotVector.getNum(), (float *)knotVector.getValues(0),
                  ce->is3D() ? 3 : 4, coords.data(),
                  knotVector.getNum() - numControlPoints.getValue(),
                  ce->is3D() ? GL_MAP1_VERTEX_3 : GL_MAP1_VERTEX_4);
    gluEndCurve(render);
}
