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
 |   $Revision: 1.3 $
 |
 |   Description:
 |	This file contains definitions of various linear algebra classes,
 |	such as vectors, coordinates, etc..
 |
 |   Classes:
 |	SbVec3f
 |	SbVec2f
 |	SbVec2s
 |	SbVec4f
 |
 |   Author(s)		: Paul S. Strauss, Nick Thompson,
 |			  David Mott, Alain Dumesny
 |
 ______________  S I L I C O N   G R A P H I C S   I N C .  ____________
 _______________________________________________________________________
 */
#ifndef _SB_VEC_
#define _SB_VEC_

#include <Inventor/SbBasic.h>

//////////////////////////////////////////////////////////////////////////////
//
//  Class: SbVec3f
//
//  3D vector used to represent points or directions. Each component of
//  the vector is a floating-point number.
//
//  WARNING!!!!!  Transcription of arrays of this class assume that the
//                only data stored in this class are three consecutive values.
//                Do not add any extra data members!!!
//
//////////////////////////////////////////////////////////////////////////////

class SbPlane;

class SbVec3f {
  public:
    // Default constructor
    SbVec3f()						{ }

    // Constructor given an array of 3 components
    SbVec3f(const float v[3])
     { vec[0] = v[0]; vec[1] = v[1]; vec[2] = v[2]; }

    // Constructor given 3 individual components
    SbVec3f(float x, float y, float z)
     { vec[0] = x; vec[1] = y; vec[2] = z; }

    // Constructor given 3 planes
    SbVec3f(SbPlane &p0, SbPlane &p1, SbPlane &p2);

    // Returns right-handed cross product of vector and another vector
    SbVec3f	cross(const SbVec3f &v) const;

    // Returns dot (inner) product of vector and another vector
    float	dot(const SbVec3f &v) const;

    // Returns pointer to array of 3 components
    const float	*getValue() const			{ return vec; }

    // Returns 3 individual components
    void	getValue(float &x, float &y, float &z) const;

    // Returns geometric length of vector
    float	length() const;

    // Changes vector to be unit length
    float	normalize();

    // Negates each component of vector in place
    void	negate();

    // Sets value of vector from array of 3 components
    SbVec3f &	setValue(const float v[3])
     { vec[0] = v[0]; vec[1] = v[1]; vec[2] = v[2]; return *this; }

    // Sets value of vector from 3 individual components
    SbVec3f &	setValue(float x, float y, float z)
     { vec[0] = x; vec[1] = y; vec[2] = z; return *this; }

    // Sets value of vector to be convex combination of 3 other
    // vectors, using barycentic coordinates
    SbVec3f &	setValue(const SbVec3f &barycentic,
        const SbVec3f &v0, const SbVec3f &v1, const SbVec3f &v2);

    // Accesses indexed component of vector
    float &	  operator [](int i) 		{ return (vec[i]); }
    const float & operator [](int i) const	{ return (vec[i]); }

    // Component-wise scalar multiplication and division operators
    SbVec3f &	operator *=(float d);

    SbVec3f &	operator /=(float d)
    { return *this *= (1.0 / d); }

    // Component-wise vector addition and subtraction operators
    SbVec3f &	operator +=(SbVec3f v);
    SbVec3f &	operator -=(SbVec3f v);

    // Nondestructive unary negation - returns a new vector
    SbVec3f	operator -() const;

    // Component-wise binary scalar multiplication and division operators
    friend SbVec3f	operator *(const SbVec3f &v, float d);
    friend SbVec3f	operator *(float d, const SbVec3f &v)
    { return v * d; }
    friend SbVec3f	operator /(const SbVec3f &v, float d)
    { return v * (1.0 / d); }

    // Component-wise binary vector addition and subtraction operators
    friend SbVec3f	operator +(const SbVec3f &v1, const SbVec3f &v2);

    friend SbVec3f	operator -(const SbVec3f &v1, const SbVec3f &v2);

    // Equality comparison operator
    friend int		operator ==(const SbVec3f &v1, const SbVec3f &v2);
    friend int		operator !=(const SbVec3f &v1, const SbVec3f &v2)
    { return !(v1 == v2); }

    // Equality comparison within given tolerance - the square of the
    // length of the maximum distance between the two vectors
    SbBool		equals(const SbVec3f v, float tolerance) const;

    // Returns principal axis that is closest (based on maximum dot
    // product) to this vector
    SbVec3f		getClosestAxis() const;

  protected:
    float	vec[3];		// Storage for vector components
};

//////////////////////////////////////////////////////////////////////////////
//
//  Class: SbVec2f
//
//  2D vector used to represet points or directions. Each component of
//  the vector is a float.
//
//////////////////////////////////////////////////////////////////////////////



class SbVec2f {
  public:

    // Default constructor
    SbVec2f()						{ }

    // Constructor given an array of 2 components
    SbVec2f(const float v[2])				{ setValue(v); }

    // Constructor given 2 individual components
    SbVec2f(float x, float y)				{ setValue(x, y); }

    // Returns dot (inner) product of vector and another vector
    float	dot(const SbVec2f &v) const;

    // Returns pointer to array of 2 components
    const float	*getValue() const			{ return vec; }

    // Returns 2 individual components
    void	getValue(float &x, float &y) const;

    // Returns geometric length of vector
    float	length() const;

    // Negates each component of vector in place
    void	negate();

    // Changes vector to be unit length
    float	normalize();

    // Sets value of vector from array of 2 components
    SbVec2f &	setValue(const float v[2]);

    // Sets value of vector from 2 individual components
    SbVec2f &	setValue(float x, float y);

    // Accesses indexed component of vector
    float &	  operator [](int i) 		{ return (vec[i]); }
    const float & operator [](int i) const 	{ return (vec[i]); }

    // Component-wise scalar multiplication and division operators
    SbVec2f &	operator *=(float d);

    SbVec2f &	operator /=(float d)
    { return *this *= (1.0 / d); }

    // Component-wise vector addition and subtraction operators
    SbVec2f &	operator +=(const SbVec2f &u);
    SbVec2f &	operator -=(const SbVec2f &u);

    // Nondestructive unary negation - returns a new vector
    SbVec2f	operator -() const;

    // Component-wise binary scalar multiplication and division operators
    friend SbVec2f	operator *(const SbVec2f &v, float d);
    friend SbVec2f	operator *(float d, const SbVec2f &v)
    { return v * d; }
    friend SbVec2f	operator /(const SbVec2f &v, float d)
    { return v * (1.0 / d); }

    // Component-wise binary vector addition and subtraction operators
    friend SbVec2f	operator +(const SbVec2f &v1, const SbVec2f &v2);

    friend SbVec2f	operator -(const SbVec2f &v1, const SbVec2f &v2);

    // Equality comparison operator
    friend int		operator ==(const SbVec2f &v1, const SbVec2f &v2);
    friend int		operator !=(const SbVec2f &v1, const SbVec2f &v2)
    { return !(v1 == v2); }

    // Equality comparison within given tolerance - the square of the
    // length of the maximum distance between the two vectors
    SbBool		equals(const SbVec2f v, float tolerance) const;

  protected:
    float	vec[2];		// Storage for vector components
};

//////////////////////////////////////////////////////////////////////////////
//
//  Class: SbVec2s
//
//  2D vector used to represet points or directions. Each component of
//  the vector is a (short) integer.
//
//////////////////////////////////////////////////////////////////////////////



class SbVec2s {
  public:

    // Default constructor
    SbVec2s()						{ }

    // Constructor given an array of 2 components
    SbVec2s(const short v[2])				{ setValue(v); }

    // Constructor given 2 individual components
    SbVec2s(short x, short y)				{ setValue(x, y); }

    // Returns dot (inner) product of vector and another vector
    int32_t	dot(const SbVec2s &v) const;

    // Returns pointer to array of 2 components
    const short	*getValue() const			{ return vec; }

    // Returns 2 individual components
    void	getValue(short &x, short &y) const;

    // Negates each component of vector in place
    void	negate();

    // Sets value of vector from array of 2 components
    SbVec2s &	setValue(const short v[2]);

    // Sets value of vector from 2 individual components
    SbVec2s &	setValue(short x, short y);

    // Accesses indexed component of vector
    short &	  operator [](int i) 		{ return (vec[i]); }
    const short & operator [](int i) const 	{ return (vec[i]); }

    // Component-wise scalar multiplication and division operators
    SbVec2s &	operator *=(int d);
    SbVec2s &	operator *=(double d);

    SbVec2s &	operator /=(int d);
    SbVec2s &	operator /=(double d)
    { return *this *= (1.0 / d); }

    // Component-wise vector addition and subtraction operators
    SbVec2s &	operator +=(const SbVec2s &u);
    SbVec2s &	operator -=(const SbVec2s &u);

    // Nondestructive unary negation - returns a new vector
    SbVec2s	operator -() const;

    // Component-wise binary scalar multiplication and division operators
    friend SbVec2s	operator *(const SbVec2s &v, int d);
    friend SbVec2s	operator *(const SbVec2s &v, double d);
    friend SbVec2s	operator *(int d, const SbVec2s &v)
    { return v * d; }
    friend SbVec2s	operator *(double d, const SbVec2s &v)
    { return v * d; }
    friend SbVec2s	operator /(const SbVec2s &v, int d);
    friend SbVec2s	operator /(const SbVec2s &v, double d)
    { return v * (1.0 / d); }

    // Component-wise binary vector addition and subtraction operators
    friend SbVec2s	operator +(const SbVec2s &v1, const SbVec2s &v2);

    friend SbVec2s	operator -(const SbVec2s &v1, const SbVec2s &v2);

    // Equality comparison operator
    friend int		operator ==(const SbVec2s &v1, const SbVec2s &v2);
    friend int		operator !=(const SbVec2s &v1, const SbVec2s &v2)
    { return !(v1 == v2); }

  protected:
    short	vec[2];		// Storage for vector components
};

//////////////////////////////////////////////////////////////////////////////
//
//  Class: SbVec4f
//
//  4D vector used to represet rational points or directions. Each component of
//  the vector is a float.
//
//////////////////////////////////////////////////////////////////////////////



class SbVec4f {
  public:

    // Default constructor
    SbVec4f()						{ }

    // Constructor given an array of 4 components
    SbVec4f(const float v[4])				{ setValue(v); }

    // Constructor given 4 individual components
    SbVec4f(float x, float y, float z, float w)	      { setValue(x, y, z, w); }

    // Returns dot (inner) product of vector and another vector
    float	dot(const SbVec4f &v) const;

    // Returns the real portion of the vector by dividing by the fourth value
    void	getReal(SbVec3f &v) const;

    // Returns pointer to array of 4 components
    const float	*getValue() const			{ return vec; }

    // Returns 4 individual components
    void	getValue(float &x, float &y, float &z, float &w) const;

    // Returns geometric length of vector
    float	length() const;

    // Negates each component of vector in place
    void	negate();

    // Changes vector to be unit length
    float	normalize();

    // Sets value of vector from array of 4 components
    SbVec4f &	setValue(const float v[4]);

    // Sets value of vector from 4 individual components
    SbVec4f &	setValue(float x, float y, float z, float w);

    // Accesses indexed component of vector
    float &	  operator [](int i) 		{ return (vec[i]); }
    const float & operator [](int i) const	{ return (vec[i]); }

    // Component-wise scalar multiplication and division operators
    SbVec4f &	operator *=(float d);

    SbVec4f &	operator /=(float d)
    { return *this *= (1.0 / d); }

    // Component-wise vector addition and subtraction operators
    SbVec4f &	operator +=(const SbVec4f &u);
    SbVec4f &	operator -=(const SbVec4f &u);

    // Nondestructive unary negation - returns a new vector
    SbVec4f	operator -() const;

    // Component-wise binary scalar multiplication and division operators
    friend SbVec4f	operator *(const SbVec4f &v, float d);
    friend SbVec4f	operator *(float d, const SbVec4f &v)
    { return v * d; }
    friend SbVec4f	operator /(const SbVec4f &v, float d)
    { return v * (1.0 / d); }

    // Component-wise binary vector addition and subtraction operators
    friend SbVec4f	operator +(const SbVec4f &v1, const SbVec4f &v2);

    friend SbVec4f	operator -(const SbVec4f &v1, const SbVec4f &v2);

    // Equality comparison operator
    friend int		operator ==(const SbVec4f &v1, const SbVec4f &v2);
    friend int		operator !=(const SbVec4f &v1, const SbVec4f &v2)
    { return !(v1 == v2); }

    // Equality comparison within given tolerance - the square of the
    // length of the maximum distance between the two vectors
    SbBool		equals(const SbVec4f v, float tolerance) const;

  protected:
    float	vec[4];		// Storage for vector components
};

#endif /* _SB_VEC_ */