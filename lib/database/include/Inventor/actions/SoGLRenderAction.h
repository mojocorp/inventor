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
 |	Defines the SoGLRenderAction class
 |
 |   Author(s)		: Paul S. Strauss
 |
 ______________  S I L I C O N   G R A P H I C S   I N C .  ____________
 _______________________________________________________________________
 */

#pragma once

#include <vector>

#include <Inventor/SbViewportRegion.h>
#include <Inventor/actions/SoSubAction.h>
#include <Inventor/elements/SoShapeStyleElement.h>
#include <Inventor/elements/SoTransparencyTypeElement.h>
#include <Inventor/elements/SoStereoElement.h>
#include <Inventor/lists/SoPathList.h>

class SoGetBoundingBoxAction;

/// Callback functions used between rendering passes should be of this type.
typedef void SoGLRenderPassCB(void *userData);

/// Renders a scene graph using OpenGL.
/// \ingroup Actions
/// This class traverses a scene graph and renders it using the OpenGL
/// graphics library. It assumes that a valid window has been created
/// and initialized for proper OpenGL rendering. The <tt>SoXtRenderArea</tt>
/// class or any of its subclasses may be used to create such a window.
/// \sa SoSeparator, SoXtRenderArea
class INVENTOR_API SoGLRenderAction : public SoAction {

    SO_ACTION_HEADER(SoGLRenderAction);

  public:
    /// Various levels of transparency rendering quality
    enum TransparencyType {
        SCREEN_DOOR =
            SoTransparencyTypeElement::SCREEN_DOOR, ///< Use stipple patterns
                                                    ///< for screen-door
                                                    ///< transparency
        ADD = SoTransparencyTypeElement::ADD, ///< Use additive alpha blending
        DELAYED_ADD =
            SoTransparencyTypeElement::DELAYED_ADD, ///< Uses additive blending,
                                                    ///< rendering all
                                                    ///< transparent objects
                                                    ///< after opaque ones
        SORTED_OBJECT_ADD = SoTransparencyTypeElement::
            SORTED_OBJECT_ADD, ///< Same as DELAYED_ADD, but sorts transparent
                               ///< objects by distances of bounding boxes from
                               ///< camera
        BLEND = SoTransparencyTypeElement::BLEND, ///< Uses multiplicative alpha
                                                  ///< blending
        DELAYED_BLEND = SoTransparencyTypeElement::
            DELAYED_BLEND, ///< Uses multiplicative alpha blending, rendering
                           ///< all transparent objects after opaque ones
        SORTED_OBJECT_BLEND = SoTransparencyTypeElement::
            SORTED_OBJECT_BLEND ///< Same as DELAYED_BLEND, but sorts
                                ///< transparent objects by distances of
                                ///< bounding boxes from camera
    };

    /// Possible return codes from a render abort callback
    enum AbortCode {
        CONTINUE, ///< Continue traversal as usual
        ABORT,    ///< Stop traversing the rest of the graph
        PRUNE,    ///< Do not traverse this node or its children, but continue
        DELAY     ///< Delay rendering of this node until the second pass
    };

    /// Callback functions for render abort should be of this type.
    /// This typedef is defined within the class, since it needs to
    /// refer to the AbortCode enumerated type.
    typedef AbortCode SoGLRenderAbortCB(void *userData);

    /// Constructor. The  parameter defines the viewport region
    /// into which rendering will take place.
    SoGLRenderAction(const SbViewportRegion &viewportRegion);

    /// Destructor
    virtual ~SoGLRenderAction();

    /// Sets current viewport region to use for rendering
    void setViewportRegion(const SbViewportRegion &newRegion);

    /// Returns current viewport region
    const SbViewportRegion &getViewportRegion() const { return vpRegion; }

    /// Sets/returns current update area, which is the area of the
    /// viewport region that will actually be rendered into. This can
    /// be used for partial updates in applications that can manage
    /// them. The update area is specified in normalized viewport
    /// coordinates, where (0,0) is the lower left corner of the
    /// viewport and (1,1) is the upper right corner. The area is given
    /// as an origin and a size.
    void setUpdateArea(const SbVec2f &origin, const SbVec2f &size);
    void getUpdateArea(SbVec2f &origin, SbVec2f &size) const;

    /// Sets callback to call during rendering to test for an abort condition.
    /// It will be called for each node that is traversed.
    /// This allows applications to terminate rendering prematurely if some
    /// condition occurs. The callback function should return one of the
    /// AbortCode codes to indicate whether traversal should continue. Use of
    /// the various codes in a callback can allow applications to modify
    /// Inventor's default order of rendering objects in a scene graph.
    void setAbortCallback(SoGLRenderAbortCB *func, void *userData) {
        abortCB = func;
        abortData = userData;
    }

    /// Sets transparency quality level to use when rendering. The
    /// default is SCREEN_DOOR. (Note that SCREEN_DOOR transparency does not
    /// work in the case where transparency values are specified for each vertex
    /// of a shape. If this is the case, use one of the other transparency
    /// types.)
    void setTransparencyType(TransparencyType type);

    /// Returns transparency quality level to use when rendering. The
    /// default is SCREEN_DOOR.
    TransparencyType getTransparencyType() const { return transpType; }

    /// Sets smoothing flag. When on, smoothing uses OpenGL's line-
    /// and point-smoothing features to provide cheap antialiasing of lines
    /// and points. The default is FALSE.
    void setSmoothing(SbBool smooth);

    /// Returns smoothing flag.
    SbBool isSmoothing() const { return doSmooth; }

    /// Sets number of rendering passes for multipass rendering.
    /// Specifying more than one pass will result in antialiasing of the
    /// rendered scene, using OpenGL's accumulation buffer. (Camera nodes
    /// typically move their viewpoints a little bit for each pass to achieve
    /// the antialiasing.)  Each additional pass provides better
    /// antialiasing, but requires more rendering time The default is 1 pass.
    void setNumPasses(int num) { numPasses = num; }

    /// Returns number of rendering passes for multipass rendering.
    int getNumPasses() const { return numPasses; }

    /// Sets a flag indicating whether intermediate results are
    /// displayed after each antialiasing pass for progressive improvement
    /// (default is FALSE).
    void setPassUpdate(SbBool flag) { passUpdate = flag; }

    /// Returns a flag indicating whether intermediate results are
    /// displayed after each antialiasing pass for progressive improvement
    /// (default is FALSE).
    SbBool isPassUpdate() const { return passUpdate; }

    /// Sets a callback function to invoke between passes when antialiasing.
    /// Passing NULL (which is the default state) will cause a clear of the
    /// color and depth buffers to be performed.
    void setPassCallback(SoGLRenderPassCB *func, void *userData) {
        passCB = func;
        passData = userData;
    }

    /// Sets the OpenGL cache context. A cache context is just an
    /// integer identifying when OpenGL display lists (which are used for
    /// render caching) can be shared between render actions; for example, see
    /// the documentation on GLX contexts for information on when OpenGL
    /// display lists can be shared between GLX windows.
    void setCacheContext(uint32_t context);

    /// Returns the OpenGL cache context.
    uint32_t getCacheContext() const;

    /// Sets whether or not "remote" rendering is happening.
    /// Inventor's auto-render-caching algorithm will choose to cache
    /// more often when rendering is remote (the assumption being that
    /// performance will be better with display lists stored on the
    /// remote machine).  By default, it is assumed rendering is NOT
    /// remote.
    void setRenderingIsRemote(SbBool flag);

    /// Gets whether or not "remote" rendering is happening.
    SbBool getRenderingIsRemote() const;

    /// Invalidate the state, forcing it to be recreated at the next apply
    virtual void invalidateState();

    enum StereoMode {
        MONOSCOPIC = SoStereoElement::MONOSCOPIC,
        LEFT_VIEW = SoStereoElement::LEFT_VIEW,
        RIGHT_VIEW = SoStereoElement::RIGHT_VIEW
    };

    /// Sets the stereo mode.
    void setStereoMode(StereoMode mode);

    /// Sets the stereo offset.
    /// This is the eye separation when doing stereo rendering.
    /// Note that the stereo pair can be reversed by supplying a negative eye
    /// separation.
    void setStereoOffset(float offset);

    /// Sets the stereo balance adjustment.
    void setStereoBalance(float balance);
    SoEXTENDER
  public:
    // Returns current rendering pass number
    int getCurPass() const { return curPass; }

    // Returns TRUE if render action should abort - checks user callback
    SbBool abortNow() {
        return (hasTerminated() || (abortCB != NULL && checkAbort()));
    }

    // Returns TRUE if render action handles processing of a
    // transparent object (if it is to be sorted and rendered later).
    // If this returns FALSE, the object should just go ahead and
    // render itself.
    // The optional argument isTransparent ensures that the object being
    // rendered will be taken as transparent, regardless of transparency
    // in the state.  If it is false, the state is checked for transparency.
    SbBool handleTransparency(SbBool isTransparent = FALSE);

    SoINTERNAL
  public:
    static void initClass();

    // Returns TRUE if rendering is being delayed because of
    // transparency sorting or delaying
    SbBool isDelaying() const { return sortObjs || delayObjs; }

    // Adds to the list of paths to render after all other stuff
    // (including delayed/sorted transparent objects) have been
    // rendered. (Used for annotation nodes.)
    void addDelayedPath(SoPath *path);

    // Returns TRUE if currently rendering delayed paths
    SbBool isRenderingDelayedPaths() const { return renderingDelPaths; }

    int  getCullTestResults() { return cullBits; }
    void setCullTestResults(int b) { cullBits = b; }

  protected:
    // Initiates action on graph
    virtual void beginTraversal(SoNode *node);

  private:
    SbViewportRegion vpRegion;     // Current viewport region
    SbVec2f          updateOrigin; // Origin of update area
    SbVec2f          updateSize;   // Size of update area

    // Variables for render abort:
    SoGLRenderAbortCB *abortCB;   // Callback to test abort
    void *             abortData; // User data for abort callback

    // Variables for transparency, smoothing, and multi-pass rendering:
    TransparencyType  transpType; // Transparency quality type
    SbBool            doSmooth;   // Doing smoothing ?
    int               numPasses;  // Number of rendering passes
    int               curPass;    // Current pass
    SbBool            passUpdate; // Whether to update after each pass
    SoGLRenderPassCB *passCB;     // Callback between passes
    void *            passData;   // User data for pass callback

    // For SORTED_OBJECT_ADD or SORTED_OBJECT_BLEND transparency:
    SbBool renderingTranspObjs;          // TRUE when rendering transp objs
    SbBool delayObjs;                    // TRUE if transp objects are to be
                                         // delayed until after opaque ones
    SbBool                  sortObjs;    // TRUE if objects are to be sorted
    SoPathList              transpPaths; // Paths to transparent objects
    SoGetBoundingBoxAction *ba;          // For computing bounding boxes
    std::vector<float>      bboxes;      // Bounding boxes zmax of objects

    uint32_t cacheContext;    // GL cache context
    SbBool   remoteRendering; // Remote rendering?

    // Stuff needed to implement rendering of delayed paths
    SoPathList delayedPaths;      // List of paths to render
    SbBool     renderingDelPaths; // TRUE when rendering them

    // These flags determine which things have to be sent to GL when
    // the action is applied. They indicate what's changed since the
    // last time the action was applied.
    enum flags {
        TRANSPARENCY_TYPE = 0x01,
        SMOOTHING = 0x02,
        ALL = 0x03 // Initial value
    };
    uint32_t whatChanged;

    // Renders all passes starting at a node
    void renderAllPasses(SoNode *node);

    // Renders one pass starting at a node
    void renderPass(SoNode *node, int pass);

    // Renders objects that have been marked as transparent in same or
    // sorted order
    void renderTransparentObjs();

    // Enables or disables GL blending
    void enableBlending(SbBool enable);

    // Returns TRUE if render action should abort based on callback
    SbBool checkAbort();

    // Keep track of which planes we need to view-volume cull test
    // against:
    int cullBits;

    StereoMode stereoMode;
    float      stereoOffset;
    float      stereoBalance;
};

