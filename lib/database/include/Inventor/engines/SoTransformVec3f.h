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
 |   $Revision: 1.1 $
 |
 |   Description:
 |	This file contains the declaration of the TransformVec3f engine
 |
 |   Classes:
 |	SoTransformVec3f
 |
 |   Author(s)		: Ronen Barzel
 |
 ______________  S I L I C O N   G R A P H I C S   I N C .  ____________
 _______________________________________________________________________
 */

#pragma once

#include <Inventor/engines/SoSubEngine.h>
#include <Inventor/fields/SoMFMatrix.h>
#include <Inventor/fields/SoMFVec3f.h>

/// Transforms a 3D vector by a 4x4 matrix.
/// \ingroup Engines
/// This engine takes as input a three dimensional floating-point vector and
/// a transformation matrix.  The vector is assumed to be a row vector.
///
///
/// The engine multiplies the vector by the matrix and returns the result
/// in the output #point.
/// The output #direction contains the result when the matrix multiplication
/// assumes the vector is a direction, and therefore ignores the translation
/// part of the matrix.
/// The output #normalDirection contains the normalized #direction
///
/// \par File format/defaults:
/// \code
/// SoTransformVec3f {
///    vector   0 0 0
///    matrix   1 0 0 0
///             0 1 0 0
///             0 0 1 0
///             0 0 0 1
/// }
/// \endcode
/// \sa SoEngineOutput
class INVENTOR_API SoTransformVec3f : public SoEngine {

    SO_ENGINE_HEADER(SoTransformVec3f);

  public:
    // Inputs
    SoMFVec3f  vector;
    SoMFMatrix matrix;

    // Outputs
    SoEngineOutput point;           ///< (SoMFVec3f) vector-matrix multiply
    SoEngineOutput direction;       ///< (SoMFVec3f) direction-matrix multiply
    SoEngineOutput normalDirection; ///< (SoMFVec3f) direction, normalized

    /// Constructor
    SoTransformVec3f();

    SoINTERNAL
  public:
    static void initClass();

  private:
    // Destructor
    ~SoTransformVec3f();

    // Evaluation method
    virtual void evaluate();
};

