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
 |	This file defines the SoShape node class.
 |
 |   Author(s)		: Paul S. Strauss
 |
 ______________  S I L I C O N   G R A P H I C S   I N C .  ____________
 _______________________________________________________________________
 */

#ifndef  _SO_SHAPE_
#define  _SO_SHAPE_

#include <Inventor/SbLinear.h>
#include <Inventor/nodes/SoSubNode.h>

#include <GL/glu.h>

class SoCube;
class SoDetail;
class SoFaceDetail;
class SoMaterialBundle;
class SoPickedPoint;
class SoPointDetail;
class SoPrimitiveVertex;
class SoState;

// C-api: end

//  GLU Library version dependency.  Introduced in Irix 6.2.
#ifndef gluTESSELATOR
#if (defined(__glu_h__) || defined(GLU_H)) && defined(GLU_VERSION_1_2)
#define gluTESSELATOR  GLUtesselator
#else
#define gluTESSELATOR  GLUtriangulatorObj
extern "C" struct GLUtriangulatorObj;
#endif
#endif

// C-api: begin

//////////////////////////////////////////////////////////////////////////////
//
//  Class: SoShape
//
//  Abstract base shape node class, which provides some convenience
//  functions for its subclasses.
//
//////////////////////////////////////////////////////////////////////////////

// C-api: abstract
class SoShape : public SoNode {

    SO_NODE_ABSTRACT_HEADER(SoShape);

  public:
    // Overrides default method on SoNode
    virtual SbBool	affectsState() const;

  SoINTERNAL public:
    static void		initClass();

    // This type is used by the triangle shape generation methods
    // (beginShape, etc.)
    enum TriangleShape {
	TRIANGLE_STRIP,
	TRIANGLE_FAN,
	TRIANGLES,
	POLYGON
    };

  SoEXTENDER public:
    // Implements bounding box method using virtual computeBBox()
    // method. Subclasses should not need to override this.
    virtual void	getBoundingBox(SoGetBoundingBoxAction *action);

    // Implements rendering by rendering each primitive generated by
    // subclass. May be overridden in subclass.
    virtual void	GLRender(SoGLRenderAction *action);

    // Implements picking along a ray by intersecting the ray with
    // each primitive generated by subclass. May be overridden in
    // subclass.
    virtual void	rayPick(SoRayPickAction *action);

    // Implements primitive generation for all shapes.  Calls the 
    // generatePrimitives for the shape.
    virtual void	callback(SoCallbackAction *action);

    // This can be used by subclasses when the complexity type is
    // SCREEN_SPACE to determine how many window pixels are covered by
    // the shape. It returns in rectSize the number of pixels in the
    // window rectangle that covers the given 3D bounding box.
    static void		getScreenSize(SoState *state,
				      const SbBox3f &boundingBox,
				      SbVec2s &rectSize);

    // Computes bounding box for subclass using information in the
    // given action (which may not necessarily be an
    // SoGetBoundingBoxAction). This is used by getBoundingBox() and
    // when rendering or picking a shape with bounding-box complexity.
    // Subclasses must define this method.
    virtual void	computeBBox(SoAction *action, SbBox3f &box,
				    SbVec3f &center) = 0;
  protected:

    // Constructor
    SoShape();

    // This method MUST be defined by each subclass to generate
    // primitives (triangles, line segments, points) that represent
    // the shape.
    virtual void	generatePrimitives(SoAction *action) = 0;

    // Returns TRUE if the shape should be rendered. Subclasses can
    // call this in their rendering methods to determine whether to
    // continue.  
    virtual SbBool	shouldGLRender(SoGLRenderAction *action);

    // Returns TRUE if the shape may be picked. Subclasses can call
    // this in their picking methods to determine whether to continue.
    SbBool		shouldRayPick(SoRayPickAction *action);

    // These can be used by solid shapes to indicate so during
    // rendering. They take care of setting up shape hints in the
    // state. Calls to these two methods should bracket the rendering
    // code.
    void		beginSolidShape(SoGLRenderAction *action);
    void		endSolidShape(SoGLRenderAction *action);

    // Computes a picking ray in the object space of the shape
    // instance. The picking ray is stored in the SoRayPickAction for
    // later access by the subclass. The second form takes a matrix to
    // concatenate with the current transformation matrix. It can be
    // used, for example, if a shape has sizing or positioning info
    // built into it.
    void		computeObjectSpaceRay(SoRayPickAction *action);
    void		computeObjectSpaceRay(SoRayPickAction *action,
					      const SbMatrix &matrix);

    // These methods are called during picking to create details
    // representing extra info about a pick intersection. The default
    // methods implemented in SoShape return NULL. Subclasses can
    // override these methods to set up specific detail instances that
    // contain the extra info. Subclasses can get whatever information
    // they need from the SoPickedPoint, and can set information in it
    // as well.
    virtual SoDetail *	createTriangleDetail(SoRayPickAction *action,
					     const SoPrimitiveVertex *v1,
					     const SoPrimitiveVertex *v2,
					     const SoPrimitiveVertex *v3,
					     SoPickedPoint *pp);
    virtual SoDetail *	createLineSegmentDetail(SoRayPickAction *action,
						const SoPrimitiveVertex *v1,
						const SoPrimitiveVertex *v2,
						SoPickedPoint *pp);
    virtual SoDetail *	createPointDetail(SoRayPickAction *action,
					  const SoPrimitiveVertex *v,
					  SoPickedPoint *pp);

    // These methods invoke whatever callbacks are registered with the
    // primitive generation procedures. If primitives are being
    // generated for picking or rendering purposes, the appropriate
    // SoShape methods are called. If primitives are being generated
    // for the SoCallbackAction, the callbacks registered with the
    // action are called.
    void		invokeTriangleCallbacks(SoAction *action,
						const SoPrimitiveVertex *v1,
						const SoPrimitiveVertex *v2,
						const SoPrimitiveVertex *v3);
    void		invokeLineSegmentCallbacks(SoAction *action,
						  const SoPrimitiveVertex *v1,
						  const SoPrimitiveVertex *v2);
    void		invokePointCallbacks(SoAction *action,
					     const SoPrimitiveVertex *v);

    // These methods can be used by subclasses to generate triangles
    // more easily when those triangles are part of a larger
    // structure, such as a triangle strip, triangle fan, or
    // triangulated polygon, according to the TriangleShape enumerated
    // type. The sequence of calls is similar to GL's: begin a shape,
    // send vertices of that shape, then end the shape.
    //
    // If the face detail passed to beginShape() is NULL (the
    // default), the details in the vertices will be used as is.
    // Note that some vertices may be copied into local storage; the
    // detail pointers are copied as well, so the details themselves
    // should be consistent for the duration of the shape generation.
    //
    // If the face detail passed to beginShape() is non-NULL, the
    // details in the vertices are assumed to be SoPointDetails. Each
    // vertex of each triangle generated will contain a pointer to the
    // face detail, which will be filled with three copies of the
    // point details from the relevant vertices. Since copies of the
    // point details are made, the storage for each point detail
    // passed to shapeVertex() can be re-used by the caller.
    void		beginShape(SoAction *action, TriangleShape shapeType,
				   SoFaceDetail *faceDetail = NULL);
    void		shapeVertex(const SoPrimitiveVertex *v);
    void		endShape();

    virtual ~SoShape();
    
    // Applies GLRender action to the bounding box surrounding the shape.
    // This is used to render shapes when BOUNDING_BOX complexity is on.
    void		GLRenderBoundingBox(SoGLRenderAction *action);

  private:

    // These are used when rendering or picking a shape whose
    // complexity is set to BOUNDING_BOX. The SoGetBoundingBoxAction
    // is used to compute the bounding box of the shape, and the cube
    // is used as a surrogate object when rendering it.
    static SoGetBoundingBoxAction	*bboxAct;
    static SoCube			*bboxCube;

    // These are used when using generatePrimitives() to do rendering.
    // They store global info that is needed to render each primitive
    // correctly.
    static SbBool	sendTexCoords;	// TRUE if coords should be sent
    static SoMaterialBundle *matlBundle;// Bundle used to send materials

    // These are used for triangle primitive generation (beginShape, etc.)
    static TriangleShape	primShapeType;	// Type of shape generated
    static SoFaceDetail	*faceDetail;	// Detail used for each primitive
    static int		nestLevel;	// Level of beginShape() nesting
    static SoAction	*primAction;	// Action primitives generated for
    static int		primVertNum;	// Number of vertices so far
    static int		polyVertNum;	// Number of poly vertices so far
    static SoShape	*primShape;	// Shape primitives generated for

    // These are static for speed, so we don't have to allocate them
    // once for each polygon or set of polygons.
    static SoPrimitiveVertex *primVerts;	// Array of saved vertices
    static SoPointDetail     *vertDetails;	// Array of vertex details
    static SoPrimitiveVertex *polyVerts;	// Array of saved poly vertices
    static SoPointDetail     *polyDetails;	// Array of poly details
    static int numPolyVertsAllocated;	// Size of polyVerts array
    static GLUtriangulatorObj  *tobj;	// Tesselator (for concave polygons)
    
    // These methods are used to render primitives generated by
    // subclasses. The userData pointer points to the SoShape instance.
    void		GLRenderTriangle(SoGLRenderAction *action,
					 const SoPrimitiveVertex *v1,
					 const SoPrimitiveVertex *v2,
					 const SoPrimitiveVertex *v3);
    void		GLRenderLineSegment(SoGLRenderAction *action,
					    const SoPrimitiveVertex *v1,
					    const SoPrimitiveVertex *v2);
    void		GLRenderPoint(SoGLRenderAction *action,
				      const SoPrimitiveVertex *v);

    // These methods are used to pick primitives generated by
    // subclasses. The userData pointer points to the SoShape instance.
    void		rayPickTriangle(SoRayPickAction *action,
					const SoPrimitiveVertex *v1,
					const SoPrimitiveVertex *v2,
					const SoPrimitiveVertex *v3);
    void		rayPickLineSegment(SoRayPickAction *action,
					   const SoPrimitiveVertex *v1,
					   const SoPrimitiveVertex *v2);
    void		rayPickPoint(SoRayPickAction *action,
				     const SoPrimitiveVertex *v);

    // Applies rayPick action to the bounding box surrounding the shape.
    // This is used to pick shapes when BOUNDING_BOX complexity is on.
    void		rayPickBoundingBox(SoRayPickAction *action);

    // Implements shapeVertex() for one of the three triangle-based types
    void		triangleVertex(const SoPrimitiveVertex *v,
				       int vertToReplace);

    // Helper routine that allocate space for concave polygon vertices
    void		allocateVerts();

    // Static methods called by the GLU tesselation routines
    static void beginCB(GLenum primType);
    static void vtxCB(void *vertex);
    static void endCB();
    static void errorCB(GLenum err);
};

#endif /* _SO_SHAPE_ */
