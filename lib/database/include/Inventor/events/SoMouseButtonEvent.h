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
 * Copyright (C) 1990,91,92   Silicon Graphics, Inc.
 *
 _______________________________________________________________________
 ______________  S I L I C O N   G R A P H I C S   I N C .  ____________
 |
 |   $Revision: 1.2 $
 |
 |   Classes:
 |	SoMouseButtonEvent
 |
 |   Author(s): David Mott, Gavin Bell
 |
 ______________  S I L I C O N   G R A P H I C S   I N C .  ____________
 _______________________________________________________________________
 */

#pragma once

#include <Inventor/SbBasic.h>
#include <Inventor/events/SoButtonEvent.h>

// some convenience macros for determining if an event matches

#define SO_MOUSE_PRESS_EVENT(EVENT, BUTTON)                                    \
    (SoMouseButtonEvent::isButtonPressEvent(EVENT, SoMouseButtonEvent::BUTTON))

#define SO_MOUSE_RELEASE_EVENT(EVENT, BUTTON)                                  \
    (SoMouseButtonEvent::isButtonReleaseEvent(EVENT,                           \
                                              SoMouseButtonEvent::BUTTON))

/// Mouse button press and release events.
/// \ingroup Events
/// <tt>SoMouseButtonEvent</tt> represents mouse button press and release events
/// in the Inventor event model.
/// \sa SoEvent, SoButtonEvent, SoKeyboardEvent, SoLocation2Event,
/// SoMotion3Event, \sa SoSpaceballButtonEvent,SoHandleEventAction,
/// SoEventCallback, SoSelection, SoInteraction,SoXtDevice
class INVENTOR_API SoMouseButtonEvent : public SoButtonEvent {

    SO_EVENT_HEADER();

  public:
    enum Button {
        ANY = 0,     ///< Any button
        BUTTON1 = 1, ///< First mouse button
        BUTTON2 = 2, ///< Second mouse button
        BUTTON3 = 3  ///< Third mouse button
    };

    /// Default constructor
    SoMouseButtonEvent();

    /// Destructor
    virtual ~SoMouseButtonEvent();

    /// Set which mouse button generated the event.
    void setButton(SoMouseButtonEvent::Button b) { button = b; }

    /// Get which mouse button generated the event.
    SoMouseButtonEvent::Button getButton() const { return button; }

    /// Returns whether the passed event is a mouse button press event
    /// of the passed button. When SoMouseButtonEvent::ANY is passed,
    /// this returns TRUE if the event represents a button press
    /// or release of any mouse button.
    static SbBool isButtonPressEvent(const SoEvent *            e,
                                     SoMouseButtonEvent::Button whichButton);

    /// Returns whether the passed event is a mouse button release event
    /// of the passed button. When SoMouseButtonEvent::ANY is passed,
    /// this returns TRUE if the event represents a button press
    /// or release of any mouse button.
    static SbBool isButtonReleaseEvent(const SoEvent *            e,
                                       SoMouseButtonEvent::Button whichButton);

    SoINTERNAL
  public:
    static void initClass();

  private:
    Button button; // which button
};

