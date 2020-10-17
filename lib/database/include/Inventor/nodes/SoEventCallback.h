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
 |	SoEventCallback node class - invokes callbacks when the handle
 |   event action is applied to this node.
 |
 |   Author(s): David Mott
 |
 ______________  S I L I C O N   G R A P H I C S   I N C .  ____________
 _______________________________________________________________________
 */

#pragma once

#include <Inventor/SoPath.h>
#include <Inventor/nodes/SoSubNode.h>
#include <Inventor/actions/SoHandleEventAction.h>

#include <vector>

class SoEvent;
class SoEventCallback;

/// callback function prototypes
typedef void SoEventCallbackCB(void *userData, SoEventCallback *node);

/// Node which invokes callbacks for events.
/// \ingroup Interaction
/// <tt>SoEventCallback</tt> will invoke application supplied
/// callback functions during <tt>SoHandleEventAction</tt> traversal.
/// Methods allow the application to specify which Inventor events should
/// trigger callbacks, and which path must be picked, if any, for the callback
/// invocation to occur. The application callback is able to get information
/// about the event and the pick detail, and may grab events, release events,
/// and set whether the event was handled.
///
/// If you register more than one callback
/// function in an <tt>SoEventCallback</tt> node, all the callback functions
/// will be invoked when an event occurs, even if one of the callbacks handles
/// the event. However, if the event is handled by any of the callback
/// functions, no other node in the scene graph will see the event.
///
/// \par File format/defaults:
/// \code
/// SoEventCallback {
/// }
/// \endcode
/// \sa SoInteraction, SoSelection, SoHandleEventAction, SoDragger
class INVENTOR_API SoEventCallback : public SoNode {

    SO_NODE_HEADER(SoEventCallback);

  public:
    /// Constructor creates an event callback node with no event interest
    /// and a NULL path.
    SoEventCallback();

    /// Set the path to monitor - the callbacks are only invoked
    /// if this path is picked. If path is NULL, then the callbacks
    /// are invoked automatically on every event of the type specified
    /// in setEventInterest, without a pick occurring.
    /// This makes its own copy of path.
    void setPath(SoPath *path);

    /// Get the path which must be picked in order for the callbacks
    /// to be invoked.
    const SoPath *getPath() { return pathOfInterest; }

    /// Specifies the callback functions to be invoked for different event
    /// types. When invoked, the callback function will be passed the \a
    /// userData, along with a pointer to this SoEventCallback node. For
    /// example, passing SoMouseButtonEvent::getClassTypeId()
    /// SoMouseEvGetClassTypeId()
    /// means callbacks will be invoked only when a mouse button is pressed
    /// or released.
    /// Passing SoEvent::getClassTypeId()
    /// for the \a eventType will cause the callback to be invoked for every
    /// event which passes through this event callback node.
    void addEventCallback(SoType eventType, SoEventCallbackCB *f,
                          void *userData = NULL);

    /// Remove the specified event callback.
    void removeEventCallback(SoType eventType, SoEventCallbackCB *f,
                             void *userData = NULL);

    //////////////////////
    //
    // These all provide information to callback functions. They
    // return NULL when called from anywhere but a callback function.
    //

    /// Returns the SoHandleEventAction currently traversing this node,
    /// or NULL if traversal is not taking place. This should be called
    /// only from callback functions.
    SoHandleEventAction *getAction() const { return eventAction; }

    /// Returns the event currently being handled,
    /// or NULL if traversal is not taking place. This should be called
    /// only from callback functions.
    const SoEvent *getEvent() const {
        return (eventAction != NULL ? eventAction->getEvent() : NULL);
    }

    /// Returns pick information during SoHandleEventAction traversal,
    /// or NULL if traversal is not taking place. This should be called
    /// only from callback functions.
    const SoPickedPoint *getPickedPoint() const {
        return (eventAction != NULL ? eventAction->getPickedPoint() : NULL);
    }

    /// Tells the node the event was handled. The callback function is
    /// responsible for setting whether the event was handled or not.
    /// If there is more than one callback function registered
    /// with an SoEventCallback node, all of them will be
    /// invoked, regardless of whether one has handled
    /// the event or not.
    /// This should be called only from callback functions.
    void setHandled() {
        if (eventAction != NULL)
            eventAction->setHandled();
    }

    /// Returns whether the event has been handled.
    /// This should be called only from callback functions.
    SbBool isHandled() const {
        return (eventAction != NULL) ? eventAction->isHandled() : FALSE;
    }

    /// Tells the event callback node to grab events.
    /// While grabbing, the node will consume all events;
    /// however, the callback functions are still only invoked for
    /// events of interest.
    void grabEvents() {
        if (eventAction != NULL)
            eventAction->setGrabber(this);
    }

    /// Tells the event callback node to release the grab.
    void releaseEvents() {
        if (eventAction != NULL)
            eventAction->releaseGrabber();
    }

    SoINTERNAL
  public:
    static void initClass(); // initialize the class

  protected:
    // Destructor - protected since ref/unref is what should destroy this
    virtual ~SoEventCallback();

    // This will be called during handleEventAction traversal.
    virtual void handleEvent(SoHandleEventAction *ha);

  private:
    // Only invoke callbacks if this path was picked.
    // If path is NULL, always invoke callbacks.
    SoPath *pathOfInterest;

    // List of callback functions, event types, and user data.
    // internal class for storing event types, callback funcs, user data
    typedef struct {
        SoType             eventType;
        SoEventCallbackCB *func;
        void *             userData;
    } SoEventCallbackData;

    std::vector<SoEventCallbackData> cblist;

    // This is set for each SoHandleEventAction traversal of this node
    // so that the apps callback routine can invoke methods on the action.
    SoHandleEventAction *eventAction;

    // This will look through cblist and invoke each callback function
    // that is interested in the passed event.
    void invokeCallbacks(const SoEvent *e);
};

