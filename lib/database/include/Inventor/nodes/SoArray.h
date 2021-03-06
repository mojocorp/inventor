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
 |	This file defines the SoArray node class.
 |
 |   Author(s)		: Paul S. Strauss
 |
 ______________  S I L I C O N   G R A P H I C S   I N C .  ____________
 _______________________________________________________________________
 */

#pragma once

#include <Inventor/fields/SoSFEnum.h>
#include <Inventor/fields/SoSFShort.h>
#include <Inventor/fields/SoSFVec3f.h>
#include <Inventor/nodes/SoGroup.h>

/// Group node that creates a regular IxJxK array of copies of its children.
/// \ingroup Nodes
/// This group node traverses its children, in order, several times,
/// creating a regular 3D array of copies of them. The number of copies
/// in each of the three directions is specified by fields, as are the
/// vectors used to separate the copies in each of the three dimensions.
///
///
/// For example, an <tt>SoArray</tt> node can be used to create a 2x3x4 array
/// of copies of its children, where the separation vectors between
/// adjacent copies in the three array dimensions are (1,2,3), (-4,-5,-6),
/// and (7,8,9), respectively. The base point of the array can be set to
/// one of several values, as described in the <tt>origin</tt> field.
///
///
/// Copies are traversed so that the first dimension cycles most quickly,
/// followed by the second, and then the third. This order is important
/// because <tt>SoArray</tt> sets the current switch value to N before
/// traversing the children for the Nth time (for use with inherited
/// switch values - see <tt>SoSwitch</tt>).
///
/// \par Action behavior:
/// <b>SoGLRenderAction, SoCallbackAction, SoGetBoundingBoxAction,
/// SoRayPickAction</b> Traverses all children for each array element, saving
/// and restoring state before and after each traversal. <b>SoSearchAction</b>
/// Traverses all children once, setting the inherited switch value to
/// <tt>SO_SWITCH_ALL</tt> first.
///
/// \par File format/defaults:
/// \code
/// SoArray {
///    numElements1     1
///    numElements2     1
///    numElements3     1
///    separation1      1 0 0
///    separation2      0 1 0
///    separation3      0 0 1
///    origin           FIRST
/// }
/// \endcode
/// \sa SoMultipleCopy, SoSwitch
class INVENTOR_API SoArray : public SoGroup {

    SO_NODE_HEADER(SoArray);

  public:
    // Fields
    SoSFShort numElements1; ///< Number of elements in 1st direction
    SoSFShort numElements2; ///< Number of elements in 2nd direction
    SoSFShort numElements3; ///< Number of elements in 3rd direction

    SoSFVec3f separation1; ///< Separation vector in 1st direction
    SoSFVec3f separation2; ///< Separation vector in 2nd direction
    SoSFVec3f separation3; ///< Separation vector in 3rd direction

    /// Array origin:
    enum Origin {
        FIRST, ///< First copy is rendered at the current local origin;all other
               ///< copies are distributed relative to it"
        CENTER, ///< Copies are distributed relative to the center of the array
        LAST    ///< Last copy is rendered at the current local origin;all other
                ///< copies are distributed relative to it
    };

    SoSFEnum
        origin; ///< Defines the base point from which copies are distributed.

    /// Creates an array node with default settings.
    SoArray();

    /// Overrides default method on SoNode to return FALSE since arrays
    /// are effectively separators
    virtual SbBool affectsState() const;

    SoEXTENDER
  public:
    // Implement actions
    virtual void doAction(SoAction *action);
    virtual void callback(SoCallbackAction *action);
    virtual void GLRender(SoGLRenderAction *action);
    virtual void pick(SoPickAction *action);
    virtual void getBoundingBox(SoGetBoundingBoxAction *action);
    virtual void handleEvent(SoHandleEventAction *action);
    virtual void getMatrix(SoGetMatrixAction *action);
    virtual void search(SoSearchAction *action);

    SoINTERNAL
  public:
    static void initClass();

  protected:
    virtual ~SoArray();
};

