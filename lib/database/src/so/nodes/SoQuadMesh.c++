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
 |      SoQuadMesh
 |
 |   Author(s)          : Paul S. Strauss
 |
 ______________  S I L I C O N   G R A P H I C S   I N C .  ____________
 _______________________________________________________________________
 */

#include <Inventor/misc/SoGL.h>
#include <Inventor/SoPickedPoint.h>
#include <Inventor/SoPrimitiveVertex.h>
#include <Inventor/actions/SoGLRenderAction.h>
#include <Inventor/actions/SoRayPickAction.h>
#include <Inventor/bundles/SoNormalBundle.h>
#include <Inventor/bundles/SoTextureCoordinateBundle.h>
#include <Inventor/caches/SoNormalCache.h>
#include <Inventor/details/SoFaceDetail.h>
#include <Inventor/elements/SoGLCoordinateElement.h>
#include <Inventor/elements/SoGLTextureCoordinateElement.h>
#include <Inventor/elements/SoNormalBindingElement.h>
#include <Inventor/elements/SoShapeHintsElement.h>
#include <Inventor/elements/SoGLLazyElement.h>
#include <Inventor/elements/SoGLCacheContextElement.h>
#include <Inventor/errors/SoDebugError.h>
#include <Inventor/misc/SoState.h>
#include <Inventor/nodes/SoMaterialBinding.h>
#include <Inventor/nodes/SoNormalBinding.h>
#include <Inventor/nodes/SoQuadMesh.h>
//////////////////////////////////////////////////////////////////////////////

// Constants for influencing auto-caching algorithm:

// If fewer than this many vertices, AND not using the vertexProperty
// node, auto-cache.

const int AUTO_CACHE_QM_MIN_WITHOUT_VP = 20;

// And the number above which we'll say caches definitely SHOULDN'T be
// built (because they'll use too much memory):
const int AUTO_CACHE_QM_MAX = 1000;

SO_NODE_SOURCE(SoQuadMesh);

////////////////////////////////////////////////////////////////////////
//
// Description:
//    This initializes the SoQuadMesh class.
//
// Use: internal

void
SoQuadMesh::initClass()
//
////////////////////////////////////////////////////////////////////////
{
    SO__NODE_INIT_CLASS(SoQuadMesh, "QuadMesh", SoNonIndexedShape);
}

////////////////////////////////////////////////////////////////////////
//
// Description:
//    Constructor
//
// Use: public

SoQuadMesh::SoQuadMesh()
//
////////////////////////////////////////////////////////////////////////
{
    SO_NODE_CONSTRUCTOR(SoQuadMesh);
    SO_NODE_ADD_FIELD(verticesPerColumn, (1));
    SO_NODE_ADD_FIELD(verticesPerRow, (1));
    isBuiltIn = TRUE;
}

////////////////////////////////////////////////////////////////////////
//
// Description:
//    Destructor
//
// Use: private

SoQuadMesh::~SoQuadMesh()
//
////////////////////////////////////////////////////////////////////////
{}

////////////////////////////////////////////////////////////////////////
//
// Description:
//    Computes bounding box of vertices of mesh.
//
// Use: protected

void
SoQuadMesh::computeBBox(SoAction *action, SbBox3f &box, SbVec3f &center)
//
////////////////////////////////////////////////////////////////////////
{
    //  compute the number of vertices the mesh uses
    int numVerts =
        (int)(verticesPerColumn.getValue() * verticesPerRow.getValue());
    // Next, call the method on SoNonIndexedShape that computes the
    // bounding box and center of the given number of coordinates
    computeCoordBBox(action, numVerts, box, center);
}

////////////////////////////////////////////////////////////////////////
//
// Description:
//    Overrides standard method to create an SoFaceDetail instance
//    representing a picked intersection with a quad in the mesh.
//
// Use: protected, virtual

SoDetail *
SoQuadMesh::createTriangleDetail(SoRayPickAction *        action,
                                 const SoPrimitiveVertex *v1,
                                 const SoPrimitiveVertex *,
                                 const SoPrimitiveVertex *, SoPickedPoint *)
//
////////////////////////////////////////////////////////////////////////
{
    // When we get here, the detail in each vertex already points to an
    // SoFaceDetail (on the stack in generatePrimitives()). This detail
    // contains the correct part index for the row and face index for the
    // correct quad. However, this face detail does not contain any point
    // details. We have to create a new SoFaceDetail instance and set up
    // the SoPointDetail instances inside it to contain the 4 vertices of
    // the picked quad.

    SoFaceDetail *      newFD = new SoFaceDetail;
    const SoFaceDetail *oldFD = (const SoFaceDetail *)v1->getDetail();

    // Make room in the detail for 4 vertices
    newFD->setNumPoints(4);

    // Find index of quad that was hit and the row it is in
    int quadIndex = (int)oldFD->getFaceIndex();
    int row = (int)oldFD->getPartIndex();

    // We need the bindings to set up the material/normals stuff
    Binding materialBinding = getMaterialBinding(action);
    Binding normalBinding = getNormalBinding(action);
    if (normalBinding == PER_VERTEX)
        normalBinding = PER_VERTEX;

    SoPointDetail             pd;
    int                       vert;
    SoTextureCoordinateBundle tcb(action, FALSE);

    // "Upper left" corner
    vert = row + quadIndex; // Number of quads + 1 extra per row
    pd.setCoordinateIndex(vert);
    pd.setMaterialIndex(getBindIndex(materialBinding, row, quadIndex, vert));
    pd.setNormalIndex(getBindIndex(normalBinding, row, quadIndex, vert));
    pd.setTextureCoordIndex(tcb.isFunction() ? 0 : vert);
    newFD->setPoint(0, &pd);

    // "Lower left" corner
    vert += (int)verticesPerRow.getValue();
    pd.setCoordinateIndex(vert);
    pd.setMaterialIndex(getBindIndex(materialBinding, row, quadIndex, vert));
    pd.setNormalIndex(getBindIndex(normalBinding, row, quadIndex, vert));
    pd.setTextureCoordIndex(tcb.isFunction() ? 0 : vert);
    newFD->setPoint(1, &pd);

    // "Lower right" corner
    vert++;
    pd.setCoordinateIndex(vert);
    pd.setMaterialIndex(getBindIndex(materialBinding, row, quadIndex, vert));
    pd.setNormalIndex(getBindIndex(normalBinding, row, quadIndex, vert));
    pd.setTextureCoordIndex(tcb.isFunction() ? 0 : vert);
    newFD->setPoint(2, &pd);

    // "Upper right" corner
    vert -= (int)verticesPerRow.getValue();
    pd.setCoordinateIndex(vert);
    pd.setMaterialIndex(getBindIndex(materialBinding, row, quadIndex, vert));
    pd.setNormalIndex(getBindIndex(normalBinding, row, quadIndex, vert));
    pd.setTextureCoordIndex(tcb.isFunction() ? 0 : vert);
    newFD->setPoint(3, &pd);

    // The face/part indices are in the incoming details
    newFD->setFaceIndex(quadIndex);
    newFD->setPartIndex(row);

    return newFD;
}

////////////////////////////////////////////////////////////////////////
//
// Description:
//    Returns current material binding from action's state.
//
// Use: private, static

SoQuadMesh::Binding
SoQuadMesh::getMaterialBinding(SoAction *action)
//
////////////////////////////////////////////////////////////////////////
{
    switch (SoMaterialBindingElement::get(action->getState())) {
    case SoMaterialBindingElement::OVERALL:
        return OVERALL;

    case SoMaterialBindingElement::PER_PART:
    case SoMaterialBindingElement::PER_PART_INDEXED:
        return PER_ROW;

    case SoMaterialBindingElement::PER_FACE:
    case SoMaterialBindingElement::PER_FACE_INDEXED:
        return PER_QUAD;

    case SoMaterialBindingElement::PER_VERTEX:
    case SoMaterialBindingElement::PER_VERTEX_INDEXED:
        return PER_VERTEX;
    }
    return OVERALL; // Shut up C++ compiler
}

////////////////////////////////////////////////////////////////////////
//
// Description:
//    Returns current normal binding from action's state.
//
// Use: private, static

SoQuadMesh::Binding
SoQuadMesh::getNormalBinding(SoAction *action)
//
////////////////////////////////////////////////////////////////////////
{
    switch (SoNormalBindingElement::get(action->getState())) {
    case SoNormalBindingElement::OVERALL:
        return OVERALL;

    case SoNormalBindingElement::PER_PART:
    case SoNormalBindingElement::PER_PART_INDEXED:
        return PER_ROW;

    case SoNormalBindingElement::PER_FACE:
    case SoNormalBindingElement::PER_FACE_INDEXED:
        return PER_QUAD;

    case SoNormalBindingElement::PER_VERTEX:
    case SoNormalBindingElement::PER_VERTEX_INDEXED:
        return PER_VERTEX;
    }
    return OVERALL; // Shut up C++ compiler
}

////////////////////////////////////////////////////////////////////////
//
// Description:
//    Returns index (of material or normal) based on given binding.
//
// Use: private, static

int
SoQuadMesh::getBindIndex(Binding binding, int row, int quad, int vert)
//
////////////////////////////////////////////////////////////////////////
{
    int ret;

    switch (binding) {
    case OVERALL:
        ret = 0;
        break;
    case PER_ROW:
        ret = row;
        break;
    case PER_QUAD:
        ret = quad;
        break;
    case PER_VERTEX:
        ret = vert;
        break;
    }

    return ret;
}

////////////////////////////////////////////////////////////////////////
//
// Description:
//    Figures out normals, if necessary.  Returns TRUE if normals were
//    figured out (and the normal binding should be PER_VERTEX).
//
// Use: private

void
SoQuadMesh::figureNormals(SoState *state, SoNormalBundle *nb)
//
////////////////////////////////////////////////////////////////////////
{
    // See if there is a valid normal cache. If so, tell the normal
    // bundle to use it.
    SoNormalCache *normCache = getNormalCache();
    if (normCache != NULL && normCache->isValid(state)) {
        nb->set(normCache->getNum(), normCache->getNormals());
        return;
    }

    const SoNormalElement *normElt = SoNormalElement::getInstance(state);

    int numNeeded =
        (int)(verticesPerColumn.getValue() * verticesPerRow.getValue());

    // See if there are enough normals in the state...
    if (normElt->getNum() < numNeeded)
        generateDefaultNormals(state, nb);
}

////////////////////////////////////////////////////////////////////////
//
// Description:
//    Generate default normals, if necessary.  Returns non-NULL if normals
//    were generated (and the normal binding should be PER_VERTEX).
//    If they are generated, a state->push() is done-- the calling
//    code MUST do a state->pop() after using the normals!  Also, the
//    calling code is responsible for freeing the normals returned.
//
// Use: private

SbBool
SoQuadMesh::generateDefaultNormals(SoState *state, SoNormalBundle *nb)
//
////////////////////////////////////////////////////////////////////////
{
    //    Because we know the connectivity of a quad mesh, we can do this
    //    much more efficiently than the other shapes.  Need to check for
    //    wrap-around between the first and last rows and columns.

    int32_t                    c, r, t, r0, r1, c0, c1;
    SbVec3f                    dr, dc, norm, *normals;
    const SoCoordinateElement *ce = NULL;
    const SbVec3f *            vpCoords = NULL;

    SoVertexProperty *vp = (SoVertexProperty *)vertexProperty.getValue();
    if (vp && vp->vertex.getNum() > 0) {
        vpCoords = vp->vertex.getValues(0);
    } else {
        ce = SoCoordinateElement::getInstance(state);
    }

    // Figure out whether or not polys are clockwise or
    // counter-clockwise
    SoShapeHintsElement::VertexOrdering vertexOrdering;
    SoShapeHintsElement::ShapeType      shapeType;
    SoShapeHintsElement::FaceType       faceType;
    SoShapeHintsElement::get(state, vertexOrdering, shapeType, faceType);
    SbBool ccw = TRUE;
    if (vertexOrdering == SoShapeHintsElement::CLOCKWISE)
        ccw = FALSE;

    int32_t curCoord = startIndex.getValue();
    int     startInd = (int)curCoord;
    int32_t numPerCol = verticesPerColumn.getValue();
    int32_t numPerRow = verticesPerRow.getValue();

    t = startIndex.getValue();

    int32_t numNeeded = startInd + (numPerCol * numPerRow);
    normals = new SbVec3f[numNeeded];

#define CX(r, c) ce->get3((int)(t + (r)*numPerRow + (c)))
#define VX(r, c) vpCoords[((int)(t + (r)*numPerRow + (c)))]

    // r is row number, it goes from 0 to numPerCol
    // c is col number, it goes from 0 to numPerRow

    for (r = 0; r < numPerCol; r++) {
        for (c = 0; c < numPerRow; c++) {
            // for now, forget about wraparound
            r0 = r - 1;
            r1 = r + 1;
            c0 = c - 1;
            c1 = c + 1;
            if (ce == NULL) {
                if (r0 < 0) {
                    if (VX(0, c) == VX(numPerCol - 1, c))
                        r0 = numPerCol - 2;
                    else
                        r0 = 0;
                }
                if (r1 == numPerCol) {
                    if (VX(0, c) == VX(numPerCol - 1, c))
                        r1 = 1;
                    else
                        r1 = numPerCol - 1;
                }

                if (c0 < 0) {
                    if (VX(r, 0) == VX(r, numPerRow - 1))
                        c0 = numPerRow - 2;
                    else
                        c0 = 0;
                }
                if (c1 == numPerRow) {
                    if (VX(r, 0) == VX(r, numPerRow - 1))
                        c1 = 1;
                    else
                        c1 = numPerRow - 1;
                }

                dr = VX(r1, c) - VX(r0, c);
                dc = VX(r, c1) - VX(r, c0);

            } else if (ce->is3D()) {
                if (r0 < 0) {
                    if (CX(0, c) == CX(numPerCol - 1, c))
                        r0 = numPerCol - 2;
                    else
                        r0 = 0;
                }
                if (r1 == numPerCol) {
                    if (CX(0, c) == CX(numPerCol - 1, c))
                        r1 = 1;
                    else
                        r1 = numPerCol - 1;
                }

                if (c0 < 0) {
                    if (CX(r, 0) == CX(r, numPerRow - 1))
                        c0 = numPerRow - 2;
                    else
                        c0 = 0;
                }
                if (c1 == numPerRow) {
                    if (CX(r, 0) == CX(r, numPerRow - 1))
                        c1 = 1;
                    else
                        c1 = numPerRow - 1;
                }

                dr = CX(r1, c) - CX(r0, c);
                dc = CX(r, c1) - CX(r, c0);
            }
            // For 4D coordinates, ce->get3() returns a reference to a
            // value that goes away on the next call, so we have to
            // save them
            else {
                SbVec3f t3;
                if (r0 < 0) {
                    t3 = CX(0, c);
                    if (t3 == CX(numPerCol - 1, c))
                        r0 = numPerCol - 2;
                    else
                        r0 = 0;
                }
                if (r1 == numPerCol) {
                    t3 = CX(0, c);
                    if (t3 == CX(numPerCol - 1, c))
                        r1 = 1;
                    else
                        r1 = numPerCol - 1;
                }

                if (c0 < 0) {
                    t3 = CX(r, 0);
                    if (t3 == CX(r, numPerRow - 1))
                        c0 = numPerRow - 2;
                    else
                        c0 = 0;
                }
                if (c1 == numPerRow) {
                    t3 = CX(r, 0);
                    if (t3 == CX(r, numPerRow - 1))
                        c1 = 1;
                    else
                        c1 = numPerRow - 1;
                }

                t3 = CX(r1, c);
                dr = t3 - CX(r0, c);
                t3 = CX(r, c1);
                dc = t3 - CX(r, c0);
            }
            norm = dr.cross(dc);
            norm.normalize();
            if (!ccw)
                norm.negate();
            normals[startInd + r * numPerRow + c] = norm;
        }
    }

    nb->set(numNeeded, normals);

    // Cache the resulting normals
    setNormalCache(state, numNeeded, normals);

    return TRUE;
}

////////////////////////////////////////////////////////////////////////
//
// Description:
//    Implements GL rendering.
//
// Use: extender

void
SoQuadMesh::GLRender(SoGLRenderAction *action)
//
////////////////////////////////////////////////////////////////////////
{
    SoState *state = action->getState();

    // Get ShapeStyleElement
    const SoShapeStyleElement *shapeStyle = SoShapeStyleElement::get(state);

    // First see if the object is visible and should be rendered now:
    if (shapeStyle->mightNotRender()) {
        if (!shouldGLRender(action))
            return;
    }

    if (vpCache.mightNeedSomethingFromState(shapeStyle)) {
        SoVertexProperty *vp = (SoVertexProperty *)vertexProperty.getValue();
        vpCache.fillInCache(vp, state);

        totalNumVertices =
            verticesPerColumn.getValue() * verticesPerRow.getValue();

        if (vpCache.shouldGenerateNormals(shapeStyle)) {

            // See if there is a normal cache we can use. If not,
            // generate normals and cache them.
            SoNormalCache *normCache = getNormalCache();
            if (normCache == NULL || !normCache->isValid(state)) {
                SoNormalBundle nb(action, FALSE);
                nb.initGenerator(totalNumVertices);
                generateDefaultNormals(state, &nb);
                normCache = getNormalCache();
            }
            vpCache.numNorms = normCache->getNum();
            vpCache.normalPtr = (const char *)normCache->getNormals();
        }

        SoTextureCoordinateBundle *tcb = NULL;
        uint32_t                   useTexCoordsAnyway = 0;
        if (vpCache.shouldGenerateTexCoords(shapeStyle)) {
            state->push();
            tcb = new SoTextureCoordinateBundle(action, TRUE, TRUE);
        } else if (shapeStyle->isTextureFunction() &&
                   vpCache.haveTexCoordsInVP()) {
            state->push();
            useTexCoordsAnyway = SoVertexPropertyCache::TEXCOORD_BIT;
            SoGLTextureCoordinateElement::setTexGen(state, this, NULL);
        }

        // If lighting or texturing is off, this vpCache and other things
        // need to be reconstructed when lighting or texturing is turned
        // on, so we set the bits in the VP cache:
        if (!shapeStyle->needNormals())
            vpCache.needFromState |= SoVertexPropertyCache::NORMAL_BITS;
        if (!shapeStyle->needTexCoords())
            vpCache.needFromState |= SoVertexPropertyCache::TEXCOORD_BIT;

        // If doing multiple colors, turn on ColorMaterial:
        if (vpCache.getNumColors() > 1) {
            SoGLLazyElement::setColorMaterial(state, TRUE);
        }
        //
        // Ask LazyElement to setup:
        //
        SoGLLazyElement *lazyElt =
            (SoGLLazyElement *)SoLazyElement::getInstance(state);

        if (vpCache.colorIsInVtxProp()) {
            lazyElt->send(state, SoLazyElement::ALL_MASK);
            lazyElt->sendVPPacked(state, (unsigned char *)vpCache.getColors(0));
        } else
            lazyElt->send(state, SoLazyElement::ALL_MASK);

#ifdef DEBUG
        // check if enough vertices:
        if (vpCache.numVerts < totalNumVertices + startIndex.getValue()) {
            SoDebugError::post("SoQuadMesh::GLRender",
                               "Too few vertices specified;"
                               " need %d, have %d",
                               totalNumVertices + startIndex.getValue(),
                               vpCache.numVerts);
        }
        // Check for enough colors, normals, texcoords:
        int numNormalsNeeded = 0;
        if (shapeStyle->needNormals())
            switch (vpCache.getNormalBinding()) {
            case SoNormalBindingElement::OVERALL:
                numNormalsNeeded = 1;
                break;
            case SoNormalBindingElement::PER_VERTEX:
            case SoNormalBindingElement::PER_VERTEX_INDEXED:
                numNormalsNeeded = totalNumVertices + startIndex.getValue();
                break;
            // Note that PER_FACE is really PER_QUAD
            case SoNormalBindingElement::PER_FACE:
            case SoNormalBindingElement::PER_FACE_INDEXED:
                numNormalsNeeded = (verticesPerColumn.getValue() - 1) *
                                   (verticesPerRow.getValue() - 1);
                break;
            // Note that PER_PART is really PER_ROW
            case SoNormalBindingElement::PER_PART:
            case SoNormalBindingElement::PER_PART_INDEXED:
                numNormalsNeeded = (verticesPerColumn.getValue() - 1);
                break;
            }
        if (vpCache.getNumNormals() < numNormalsNeeded)
            SoDebugError::post("SoQuadMesh::GLRender",
                               "Too few normals specified;"
                               " need %d, have %d",
                               numNormalsNeeded, vpCache.getNumNormals());

        if ((shapeStyle->needTexCoords() || useTexCoordsAnyway) &&
            !vpCache.shouldGenerateTexCoords(shapeStyle)) {

            if (vpCache.getNumTexCoords() <
                totalNumVertices + startIndex.getValue())
                SoDebugError::post("SoQuadMesh::GLRender",
                                   "Too few texture coordinates specified;"
                                   " need %d, have %d",
                                   totalNumVertices + startIndex.getValue(),
                                   vpCache.getNumTexCoords());
        }
        int numColorsNeeded = 0;
        switch (vpCache.getMaterialBinding()) {
        case SoMaterialBindingElement::OVERALL:
            break;
        case SoMaterialBindingElement::PER_VERTEX:
        case SoMaterialBindingElement::PER_VERTEX_INDEXED:
            numColorsNeeded =
                verticesPerColumn.getValue() * verticesPerRow.getValue() +
                startIndex.getValue();
            break;
        case SoMaterialBindingElement::PER_FACE:
        case SoMaterialBindingElement::PER_FACE_INDEXED:
            numColorsNeeded = (verticesPerColumn.getValue() - 1) *
                              (verticesPerRow.getValue() - 1);
            break;
        case SoMaterialBindingElement::PER_PART:
        case SoMaterialBindingElement::PER_PART_INDEXED:
            numColorsNeeded = (verticesPerColumn.getValue() - 1);
            break;
        }
        if (vpCache.getNumColors() < numColorsNeeded)
            SoDebugError::post("SoQuadMesh::GLRender",
                               "Too few diffuse colors specified;"
                               " need %d, have %d",
                               numColorsNeeded, vpCache.getNumColors());
#endif

        // Call the appropriate render loop:
        GLRenderGeneric(action,
                        useTexCoordsAnyway | vpCache.getRenderCase(shapeStyle));

        // If doing multiple colors, turn off ColorMaterial:
        if (vpCache.getNumColors() > 1) {
            SoGLLazyElement::setColorMaterial(state, FALSE);
            ((SoGLLazyElement *)SoLazyElement::getInstance(state))
                ->reset(state, SoLazyElement::DIFFUSE_MASK);
        }

        // Influence auto-caching algorithm:
        if (verticesPerColumn.getValue() < AUTO_CACHE_QM_MIN_WITHOUT_VP &&
            vpCache.mightNeedSomethingFromState(shapeStyle)) {
            SoGLCacheContextElement::shouldAutoCache(
                state, SoGLCacheContextElement::DO_AUTO_CACHE);
        } else if (totalNumVertices > AUTO_CACHE_QM_MAX &&
                   !SoGLCacheContextElement::getIsRemoteRendering(state)) {
            SoGLCacheContextElement::shouldAutoCache(
                state, SoGLCacheContextElement::DONT_AUTO_CACHE);
        }

        if (tcb) {
            delete tcb;
            state->pop();
        } else if (useTexCoordsAnyway)
            state->pop();

    } else {
        // If doing multiple colors, turn on ColorMaterial:
        if (vpCache.getNumColors() > 1) {
            SoGLLazyElement::setColorMaterial(state, TRUE);
        }
        //
        // Ask LazyElement to setup:
        //
        SoGLLazyElement *lazyElt =
            (SoGLLazyElement *)SoLazyElement::getInstance(state);

        if (vpCache.colorIsInVtxProp()) {
            lazyElt->send(state, SoLazyElement::ALL_MASK);
            lazyElt->sendVPPacked(state, (unsigned char *)vpCache.getColors(0));
        } else
            lazyElt->send(state, SoLazyElement::ALL_MASK);

        // Call the appropriate render loop:
        GLRenderGeneric(action, vpCache.getRenderCase(shapeStyle));

        // If doing multiple colors, turn off ColorMaterial:
        if (vpCache.getNumColors() > 1) {
            SoGLLazyElement::setColorMaterial(state, FALSE);
            ((SoGLLazyElement *)SoLazyElement::getInstance(state))
                ->reset(state, SoLazyElement::DIFFUSE_MASK);
        }

        // Influence auto-caching algorithm:
        if (totalNumVertices > AUTO_CACHE_QM_MAX &&
            !SoGLCacheContextElement::getIsRemoteRendering(state)) {

            SoGLCacheContextElement::shouldAutoCache(
                state, SoGLCacheContextElement::DONT_AUTO_CACHE);
        }
    }

    return;
}

////////////////////////////////////////////////////////////////////////
//
// Description:
//    Redefines this to invalidate caches.
//
// Use: private

void
SoQuadMesh::notify(SoNotList *list)
//
////////////////////////////////////////////////////////////////////////
{
    if ((list->getLastRec()->getType() == SoNotRec::CONTAINER) &&
        ((list->getLastField() == &vertexProperty) ||
         (list->getLastField() == &verticesPerRow) ||
         (list->getLastField() == &verticesPerColumn))) {
        vpCache.invalidate();
    }

    SoNonIndexedShape::notify(list);
}

////////////////////////////////////////////////////////////////////////
//
// Description:
// Sets pv to contain the vertex defined by curVert
//
////////////////////////////////////////////////////////////////////////
#define GenVertex                                                              \
    matlIndex = getBindIndex(materialBinding, row, quadIndex, curVert);        \
    normIndex = getBindIndex(normalBinding, row, quadIndex, curVert);          \
    tcIndex = tcb.isFunction() ? 0 : curVert;                                  \
                                                                               \
    pv.setPoint(ce->get3(curVert));                                            \
    pv.setNormal(nb.get(normIndex));                                           \
    pv.setMaterialIndex(matlIndex);                                            \
                                                                               \
    if (!tcb.isFunction())                                                     \
        pv.setTextureCoords(tcb.get(tcIndex));                                 \
                                                                               \
    if (!forPicking) {                                                         \
        if (tcb.isFunction())                                                  \
            pv.setTextureCoords(tcb.get(pv.getPoint(), pv.getNormal()));       \
        pd.setCoordinateIndex(curVert);                                        \
        pd.setMaterialIndex(matlIndex);                                        \
        pd.setNormalIndex(normIndex);                                          \
        pd.setTextureCoordIndex(tcIndex);                                      \
    }                                                                          \
    shapeVertex(&pv)

////////////////////////////////////////////////////////////////////////
//
// Description:
//    Generates triangles representing a quad mesh.
//
// Use: protected

void
SoQuadMesh::generatePrimitives(SoAction *action)
//
////////////////////////////////////////////////////////////////////////
{
    // When generating primitives for picking, there is no need to
    // create details now, since they will be created in
    // createTriangleDetail() when an intersection is found (but we
    // need to use the face detail to figure out the rest of it).
    // Otherwise, we create a face detail containing the 3 points of
    // the generated triangle, using the stuff in SoShape.
    // We also delay computing default texture coordinates.
    SbBool forPicking = action->isOfType(SoRayPickAction::getClassTypeId());

    // put the vertexProperty into the state:
    SoState *state = action->getState();
    state->push();
    SoVertexProperty *vp = (SoVertexProperty *)vertexProperty.getValue();
    if (vp) {
        vp->doAction(action);
    }

    SoPrimitiveVertex          pv;
    SoFaceDetail               fd;
    SoPointDetail              pd;
    const SoCoordinateElement *ce;
    int                        topVert, botVert, curVert;
    int                        quadIndex, matlIndex, normIndex, tcIndex;
    int                        row, col, numRows, numCols;
    Binding                    materialBinding, normalBinding;
    SoNormalBundle             nb(action, FALSE);
    SoTextureCoordinateBundle  tcb(action, FALSE, !forPicking);

    ce = SoCoordinateElement::getInstance(action->getState());

    materialBinding = getMaterialBinding(action);
    normalBinding = getNormalBinding(action);

    // Get number of rows and columns (of quads, not vertices)
    numRows = (int)verticesPerColumn.getValue() - 1;
    numCols = (int)verticesPerRow.getValue() - 1;

    topVert = (int)startIndex.getValue();
    botVert = topVert + numCols + 1;

    if (forPicking) {
        pv.setTextureCoords(SbVec4f(0.0, 0.0, 0.0, 0.0));
        pv.setDetail(&fd);
    } else
        pv.setDetail(&pd);

    // Generate default normals, if necessary:
    if (SoNormalElement::getInstance(state)->getNum() == 0) {
        figureNormals(action->getState(), &nb);
        normalBinding = PER_VERTEX;
    }

    for (row = 0; row < numRows; row++) {

        fd.setPartIndex(row);

        // If either material or normal binding is per quad, we have
        // to generate each quad separately, to get the materials and
        // normals in the details to be correct
        if (materialBinding == PER_QUAD || normalBinding == PER_QUAD) {

            for (col = -1; col < numCols; col++) {
                if (col >= 0) {
                    // Generate a triangle strip from the 4 vertices of the
                    // independent quad, in this order:
                    //		topVert-1, botVert-1, botVert, topVert

                    quadIndex = row * numCols + col;

                    fd.setFaceIndex(quadIndex);

                    beginShape(action, TRIANGLE_STRIP, forPicking ? NULL : &fd);

                    curVert = topVert - 1;
                    GenVertex;

                    curVert = botVert - 1;
                    GenVertex;

                    curVert = topVert;
                    GenVertex;

                    curVert = botVert;
                    GenVertex;

                    endShape();
                }

                topVert++;
                botVert++;
            }
        }

        // We can generate triangle strips, which is a little easier
        else {
            beginShape(action, TRIANGLE_STRIP, forPicking ? NULL : &fd);

            for (col = -1; col < numCols; col++) {

                quadIndex = row * numCols + col;

                fd.setFaceIndex(quadIndex);

                // Generate two vertices for each column:
                //	topVert, then botVert

                curVert = topVert;
                GenVertex;

                curVert = botVert;
                GenVertex;

                topVert++;
                botVert++;
            }

            endShape();
        }
    }

    state->pop();
}

////////////////////////////////////////////////////////////////////////
//
// Description:
//
//
// Use: private
void
SoQuadMesh::GLRenderGeneric(SoGLRenderAction *action, int rendercase)
//
////////////////////////////////////////////////////////////////////////
{

    // 5 bits used to determine the rendering case are:
    // 43210  BITS            Routine suffix
    // -----  ----            --------------
    // 00...  Overall mtl     (Om)
    // 01...  Part mtl        (Pm)
    // 10...  Face mtl        (Fm)
    // 11...  Vtx mtl         (Vm)
    // ..00.  Overall/No norm (On)
    // ..01.  Part norm       (Pn)
    // ..10.  Face norm       (Fn)
    // ..11.  Vtx norm        (Vn)
    // ....0  No texcoord     -none-
    // ....1  Vtx texcoord    (T)
    bool Om = ((rendercase >> 3) == 0);
    bool Pm = ((rendercase >> 3) == 1);
    bool Fm = ((rendercase >> 3) == 2);
    bool Vm = ((rendercase >> 3) == 3);
    bool On = (((rendercase >> 1) & 3) == 0);
    bool Pn = (((rendercase >> 1) & 3) == 1);
    bool Fn = (((rendercase >> 1) & 3) == 2);
    bool Vn = (((rendercase >> 1) & 3) == 3);
    bool T = ((rendercase & 1) == 1);

    if (On) {
        // Send one normal, if there are any normals in vpCache:
        if (vpCache.getNumNormals() > 0)
            vpCache.sendNormal(vpCache.getNormals(0));
    }
    const char *       vertexPtr = vpCache.getVertices(startIndex.getValue());
    const unsigned int vertexStride = vpCache.getVertexStride();
    const unsigned int vertexRowStride =
        vertexStride * verticesPerRow.getValue();

    const char *colorPtr = vpCache.getColors(Vm ? startIndex.getValue() : 0);
    const unsigned int colorStride = vpCache.getColorStride();
    const unsigned int colorRowStride = colorStride * verticesPerRow.getValue();

    const char *normalPtr = vpCache.getNormals(Vn ? startIndex.getValue() : 0);
    const unsigned int normalStride = vpCache.getNormalStride();
    const unsigned int normalRowStride =
        normalStride * verticesPerRow.getValue();

    const char *       texCoordPtr = vpCache.getTexCoords(0);
    const unsigned int texCoordStride = vpCache.getTexCoordStride();
    const unsigned int texCoordRowStride =
        texCoordStride * verticesPerRow.getValue();

    const int numRows = verticesPerColumn.getValue() - 1;
    const int nv =
        (Fm | Fn) ? verticesPerRow.getValue() - 1 : verticesPerRow.getValue();

    for (int row = 0; row < numRows; row++) {
        if (Pm) {
            (*vpCache.colorFunc)(colorPtr);
            colorPtr += colorStride;
        }
        if (Pn) {
            (*vpCache.normalFunc)(normalPtr);
            normalPtr += normalStride;
        }
        if (Fn | Fm) {
            glBegin(GL_QUADS);
            for (int v = 0; v < nv; v++) {
                if (Fm) {
                    (*vpCache.colorFunc)(colorPtr);
                    colorPtr += colorStride;
                }
                if (Fn) {
                    (*vpCache.normalFunc)(normalPtr);
                    normalPtr += normalStride;
                }
                if (Vm) {
                    (*vpCache.colorFunc)(colorPtr);
                }
                if (Vn) {
                    (*vpCache.normalFunc)(normalPtr);
                }
                if (T) {
                    (*vpCache.texCoordFunc)(texCoordPtr);
                }
                (*vpCache.vertexFunc)(vertexPtr);
                if (Vm) {
                    (*vpCache.colorFunc)(colorPtr + colorRowStride);
                }
                if (Vn) {
                    (*vpCache.normalFunc)(normalPtr + normalRowStride);
                }
                if (T) {
                    (*vpCache.texCoordFunc)(texCoordPtr + texCoordRowStride);
                }
                (*vpCache.vertexFunc)(vertexPtr + vertexRowStride);
                if (Vm) {
                    (*vpCache.colorFunc)(colorPtr + colorStride +
                                         colorRowStride);
                }
                if (Vn) {
                    (*vpCache.normalFunc)(normalPtr + normalStride +
                                          normalRowStride);
                }
                if (T) {
                    (*vpCache.texCoordFunc)(texCoordPtr + texCoordStride +
                                            texCoordRowStride);
                }
                (*vpCache.vertexFunc)(vertexPtr + vertexStride +
                                      vertexRowStride);
                if (Vm) {
                    (*vpCache.colorFunc)(colorPtr + colorStride);
                    colorPtr += colorStride;
                }
                if (Vn) {
                    (*vpCache.normalFunc)(normalPtr + normalStride);
                    normalPtr += normalStride;
                }
                if (T) {
                    (*vpCache.texCoordFunc)(texCoordPtr + texCoordStride);
                    texCoordPtr += texCoordStride;
                }
                (*vpCache.vertexFunc)(vertexPtr + vertexStride);
                vertexPtr += vertexStride;
            }
            glEnd(); // GL_QUADS
            // At the end of a row skip over last vertex:
            vertexPtr += vertexStride;
            if (T) {
                texCoordPtr += texCoordStride;
            }
            if (Vm) {
                colorPtr += colorStride;
            }
            if (Vn) {
                normalPtr += normalStride;
            }

        } // end Fn | Fm (Quads)

        // Do Strip rendering if both ~Fn and ~Fm
        if (Vm | Om | Pm) {
            if (Vn | On | Pn) {
                glBegin(GL_TRIANGLE_STRIP);
                for (int v = 0; v < nv; v++) {

                    if (Vm) {
                        (*vpCache.colorFunc)(colorPtr);
                    }
                    if (Vn) {
                        (*vpCache.normalFunc)(normalPtr);
                    }
                    if (T) {
                        (*vpCache.texCoordFunc)(texCoordPtr);
                    }
                    (*vpCache.vertexFunc)(vertexPtr);

                    if (Vm) {
                        (*vpCache.colorFunc)(colorPtr + colorRowStride);
                        colorPtr += colorStride;
                    }
                    if (Vn) {
                        (*vpCache.normalFunc)(normalPtr + normalRowStride);
                        normalPtr += normalStride;
                    }
                    if (T) {
                        (*vpCache.texCoordFunc)(texCoordPtr +
                                                texCoordRowStride);
                        texCoordPtr += texCoordStride;
                    }
                    (*vpCache.vertexFunc)(vertexPtr + vertexRowStride);
                    vertexPtr += vertexStride;
                }

                glEnd();
            }
        } // end Strip rendering
    }
}
