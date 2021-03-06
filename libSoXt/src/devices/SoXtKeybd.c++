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
 * Copyright (C) 1990,91,92   Silicon Graphics, Inc.
 *
 _______________________________________________________________________
 ______________  S I L I C O N   G R A P H I C S   I N C .  ____________
 |
 |   $Revision: 1.1 $
 |
 |   Classes:
 |	SoXtKeyboard
 |
 |   Author(s): David Mott
 |
 ______________  S I L I C O N   G R A P H I C S   I N C .  ____________
 _______________________________________________________________________
 */

#include <X11/X.h>
#include <Inventor/SbTime.h>
#include <Inventor/Xt/devices/SoXtKeyboard.h>

// forward references
static SoEvent *translateKeyEvent(XKeyEvent *, SoButtonEvent::State);

////////////////////////////////////////////////////////////////////////
//
// Description:
//   Constructor.
//
// public
//
SoXtKeyboard::SoXtKeyboard(EventMask whichEvents)
//
////////////////////////////////////////////////////////////////////////
{
//??? for DEBUG, need to make sure the passed event mask is valid 
//??? for this device
    eventMask = whichEvents;
    
    // allocate a keyboard event just once
    keyEvent = new SoKeyboardEvent;
}

////////////////////////////////////////////////////////////////////////
//
// Description:
//   Destructor.
//
// public
//
SoXtKeyboard::~SoXtKeyboard()
//
////////////////////////////////////////////////////////////////////////
{
    delete keyEvent;
}

////////////////////////////////////////////////////////////////////////
//
// Description:
//   This selects input for all keyboard events which occur in w.
// The callback routine is proc, and the callback data is clientData.
//
// virtual public
//
void
SoXtKeyboard::enable(
    Widget w,
    XtEventHandler proc, 
    XtPointer clientData,
    Window)
//
////////////////////////////////////////////////////////////////////////
{
    XtAddEventHandler(w, eventMask, FALSE, proc, clientData);
}

////////////////////////////////////////////////////////////////////////
//
// Description:
//   This unselects input for keyboard events which occur in w,
// i.e. keyboard events will no longer be recognized.
//
// virtual public
//
void
SoXtKeyboard::disable(
    Widget w,
    XtEventHandler proc, 
    XtPointer clientData)
//
////////////////////////////////////////////////////////////////////////
{
    XtRemoveEventHandler(w, eventMask, FALSE, proc, clientData);
}

////////////////////////////////////////////////////////////////////////
//
// Description:
//   This returns an SoEvent for the passed X event, if the event
// was generated by the keyboard.
//
// static public
//
const SoEvent *
SoXtKeyboard::translateEvent(XAnyEvent *xe)
//
////////////////////////////////////////////////////////////////////////
{
    SoEvent *event = NULL;
    
    // switch on event type
    switch (xe->type) {
	case KeyPress:
	    event = translateKeyEvent((XKeyEvent *) xe, SoButtonEvent::DOWN);
	    break;
	    
	case KeyRelease:
	    event = translateKeyEvent((XKeyEvent *) xe, SoButtonEvent::UP);
	    break;
	    
	default:
	    break;
    }
    
    return event;
}

SoKeyboardEvent *
SoXtKeyboard::translateKeyEvent(XKeyEvent *ke,
				SoButtonEvent::State whichState)
{
    // NOTE: SoKeyboardConstants.h has provided an exact mapping
    // from X11/keysymdef.h for keyboard symbols. This is why we simply
    // call XLookupKeysym to get the key constant.

    SoKeyboardEvent::Key whichKey;
    whichKey = (SoKeyboardEvent::Key) XLookupKeysym(ke, 0);
    
    setEventPosition(keyEvent, ke->x, ke->y);
    int32_t secs = ke->time / 1000;
    keyEvent->setTime(SbTime(secs, 1000 * (ke->time - 1000 * secs)));
    keyEvent->setShiftDown(ke->state & ShiftMask);
    keyEvent->setCtrlDown(ke->state & ControlMask);
    keyEvent->setAltDown(ke->state & Mod1Mask);
    
    keyEvent->setState(whichState);
    keyEvent->setKey(whichKey);
    
    // NOTE: in X, the key masks are backwards during press and release.
    //       That is, during a 'shift key press', the shift key mask
    //       says that the key is up. So, when we get a key press/release
    //       event for shift/alt/ctrl, we must set the opposite value
    //       for the shiftDown/altDown/ctrlDown flags.
    if ( whichKey == SoKeyboardEvent::LEFT_SHIFT ||
	 whichKey == SoKeyboardEvent::RIGHT_SHIFT )
	 keyEvent->setShiftDown( !keyEvent->wasShiftDown() );

    if ( whichKey == SoKeyboardEvent::LEFT_CONTROL ||
	 whichKey == SoKeyboardEvent::RIGHT_CONTROL )
	 keyEvent->setCtrlDown( !keyEvent->wasCtrlDown() );

    if ( whichKey == SoKeyboardEvent::LEFT_ALT ||
	 whichKey == SoKeyboardEvent::RIGHT_ALT )
	 keyEvent->setAltDown( !keyEvent->wasAltDown() );

    return keyEvent;
}
