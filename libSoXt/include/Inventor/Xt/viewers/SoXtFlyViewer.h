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
 * Copyright (C) 1990-93   Silicon Graphics, Inc.
 *
 _______________________________________________________________________
 ______________  S I L I C O N   G R A P H I C S   I N C .  ____________
 |
 |   $Revision: 1.1 $
 |
 |   Classes	: SoXtFlyViewer
 |
 |   Author(s)	: Alain Dumesny
 |
 ______________  S I L I C O N   G R A P H I C S   I N C .  ____________
 _______________________________________________________________________
 */

#ifndef  _SO_XT_FLY_VIEWER_
#define  _SO_XT_FLY_VIEWER_

#include <Inventor/Xt/viewers/SoXtConstrainedViewer.h>
#include <Inventor/SbLinear.h>
#include <Inventor/SbTime.h>

class	SoFieldSensor;

//////////////////////////////////////////////////////////////////////////////
//
//  Class: SoXtFlyViewer
//
//
//	Keys used by this viewer:
//	-------------------------
//
//  Left Mouse: Click to increase speed.
//  
//  <s> + Left Mouse: Alternative to the Seek button. Press (but do not
//	hold down) the <s> key, then click on a target object.
//  
//  <u> + Left Mouse: Press (but do not hold down) the <u> key, then
//	click on a target object to set the "up" direction to the surface normal.
//	By default +y is the "up" direction.
//  
//  Middle Mouse: Click to decrease speed.
//  
//  Left and Middle Mouse: Click boths simultaneously to stop.
//  
//  Ctrl: Hold the key down to temporary stop and rotate the viewpoint.
//  
//  Right Mouse: Open the popup menu.
//  
//////////////////////////////////////////////////////////////////////////////

/// Viewer component for flying through space, with a constant world up
/// \ingroup Viewers
/// Fly Viewer - this viewer is intended to simulate flight through space, with
/// a constant world up direction. The viewer only constrains the camera to
/// keep the user from flying upside down. No mouse buttons need to be pressed
/// in order to fly. The mouse position is used only for steering, while mouse
/// clicks are used to increase or decrease the viewer speed.
///
/// The viewer allows you to tilt your head up/down/right/left and move in the
/// direction you are looking (forward or backward). The viewer also supports
/// seek to quickly move the camera to a desired object or point.
///
/// <b>USAGE</b>
///
/// <b>Left Mouse:</b> Click to increase speed.
///
/// <b>\<s\> + Left Mouse:</b> Alternative to the Seek button. Press (but do not hold down) the \<s\> key, then click on a target object.
///
/// <b>\<u\> + Left Mouse:</b> Press (but do not hold down) the \<u\> key, then
/// click on a target object to set the "up" direction to the surface normal.
/// By default +y is the "up" direction.
///
/// <b>Middle Mouse:</b> Click to decrease speed.
///
/// <b>Left and Middle Mouse:</b> Click boths simultaneously to stop.
///
/// <b>Ctrl:</b> Hold the key down to temporary stop and rotate the viewpoint.
///
/// <b>Right Mouse:</b> Open the popup menu.
/// \sa SoXtFullViewer, SoXtViewer, SoXtComponent, SoXtRenderArea, SoXtWalkViewer, SoXtExaminerViewer, SoXtPlaneViewer
class SoXtFlyViewer : public SoXtConstrainedViewer {
 public:
    /// Constructor and destructor which specifies the viewer type.
    /// Please refer to the SoXtViewer reference page for a description of the viewer types.
    SoXtFlyViewer(
	Widget parent = NULL,
	const char *name = NULL, 
	SbBool buildInsideParent = TRUE, 
	SoXtFullViewer::BuildFlag flag = BUILD_ALL,
	SoXtViewer::Type type = BROWSER);
    ~SoXtFlyViewer();
    
    //
    // redefine these to add fly viewer functionality
    //
    virtual void    setViewing(SbBool onOrOff);
    virtual void    resetToHomePosition();
    virtual void    setCamera(SoCamera *cam);
    virtual void    setCursorEnabled(SbBool onOrOff);
    
    // This is redefined to prevent the camera type from being changed 
    virtual void    setCameraType(SoType type);
    
 protected:
  
    // This constructor takes a boolean whether to build the widget now.
    // Subclasses can pass FALSE, then call SoXtFlyViewer::buildWidget()
    // when they are ready for it to be built.
    SoEXTENDER
    SoXtFlyViewer(
	Widget parent,
	const char *name, 
	SbBool buildInsideParent, 
	SoXtFullViewer::BuildFlag flag, 
	SoXtViewer::Type type, 
	SbBool buildNow);
	    
    // redefine these
    virtual const char *    getDefaultWidgetName() const;
    virtual const char *    getDefaultTitle() const;
    virtual const char *    getDefaultIconTitle() const;
    
    // redefine those routines to do viewer specific stuff
    virtual void	processEvent(XAnyEvent *anyevent);
    virtual void	setSeekMode(SbBool onOrOff);
    virtual void	actualRedraw();
    virtual void    	rightWheelMotion(float);
    
    // add viewer preference stuff
    virtual void	createPrefSheet();
    
    // Define this to bring the viewer help card
    virtual void	openViewerHelpCard();
    
 private:
    // viewer state variables
    int		    mode;
    SbBool	    createdCursors;
    Cursor	    viewerCursor, seekCursor, upCursor;
    SbVec2s	    locator; // mouse position
    SbVec2s	    startPos; // mouse starting position
    SbRotation	    camStartOrientation;
    
    // variables used for doing moving animation
    SoFieldSensor   *animationSensor;
    SbTime	    prevAnimTime;
    float	    speed, maxSpeed, maxStraightSpeed;
    float	    speedLimit, speedLimitFactor;
    
    // preference sheet stuff
    Widget	    createFlyPrefSheetGuts(Widget parent);
    static void	    incPrefSheetButtonCB(Widget, SoXtFlyViewer *, void *);
    static void	    decPrefSheetButtonCB(Widget, SoXtFlyViewer *, void *);
    
    void	    defineCursors();
    void	    drawViewerFeedback();
    void	    switchMode(int newMode);
    void	    changeMaxStraightSpeed(SbBool increase);
    void	    calculateMaxSpeed();
    static void	    animationSensorCB(void *, SoSensor *);
    void	    doCameraAnimation();

    // this is called by both constructors
    void constructorCommon(SbBool buildNow);
};

#endif  /* _SO_XT_FLY_VIEWER_ */
