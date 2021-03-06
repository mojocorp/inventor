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
#pragma once

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

/// 3D vector class.
/// \ingroup Basics
/// 3D vector class used to store 3D vectors and points. This class is used
/// throughout Inventor for arguments and return values.
/// \sa SbVec2f, SbVec4f, SbVec2s, SbRotation
class INVENTOR_API SbVec3f {
  public:
    /// Default constructor
    SbVec3f() {}

    /// Constructor given an array of 3 components
    SbVec3f(const float v[3]) {
        vec[0] = v[0];
        vec[1] = v[1];
        vec[2] = v[2];
    }

    /// Constructor given 3 individual components
    SbVec3f(float x, float y, float z) {
        vec[0] = x;
        vec[1] = y;
        vec[2] = z;
    }

    /// Constructor given 3 planes
    SbVec3f(SbPlane &p0, SbPlane &p1, SbPlane &p2);

    /// Returns right-handed cross product of vector and another vector
    SbVec3f cross(const SbVec3f &v) const;

    /// Returns dot (inner) product of vector and another vector
    float dot(const SbVec3f &v) const;

    /// Returns pointer to array of 3 components
    const float *getValue() const { return vec; }

    /// Returns 3 individual components
    void getValue(float &x, float &y, float &z) const;

    /// Returns geometric length of vector
    float length() const;

    /// Changes vector to be unit length, returning the length before
    /// normalization.
    float normalize();

    /// Negates each component of vector in place
    void negate();

    /// Sets value of vector from array of 3 components
    SbVec3f &setValue(const float v[3]) {
        vec[0] = v[0];
        vec[1] = v[1];
        vec[2] = v[2];
        return *this;
    }

    /// Sets value of vector from 3 individual components
    SbVec3f &setValue(float x, float y, float z) {
        vec[0] = x;
        vec[1] = y;
        vec[2] = z;
        return *this;
    }

    /// Sets value of vector to be convex combination of 3 other
    /// vectors, using barycentic coordinates
    SbVec3f &setValue(const SbVec3f &barycentic, const SbVec3f &v0,
                      const SbVec3f &v1, const SbVec3f &v2);

    /// Accesses indexed component of vector
    float &operator[](int i) { return (vec[i]); }

    /// Accesses indexed component of vector
    const float &operator[](int i) const { return (vec[i]); }

    /// Component-wise scalar multiplication operator
    SbVec3f &operator*=(float d);

    /// Component-wise scalar division operator
    SbVec3f &operator/=(float d) { return *this *= (1.0f / d); }

    /// Component-wise vector addition operator
    SbVec3f &operator+=(SbVec3f v);

    /// Component-wise vector subtraction operator
    SbVec3f &operator-=(SbVec3f v);

    /// Nondestructive unary negation - returns a new vector
    SbVec3f operator-() const;

    /// Component-wise binary scalar multiplication operator
    friend INVENTOR_API SbVec3f operator*(const SbVec3f &v, float d);

    /// Component-wise binary scalar multiplication operator
    friend INVENTOR_API SbVec3f operator*(float d, const SbVec3f &v) {
        return v * d;
    }

    /// Component-wise binary scalar division operator
    friend INVENTOR_API SbVec3f operator/(const SbVec3f &v, float d) {
        return v * (1.0f / d);
    }

    /// Component-wise binary vector addition operator
    friend INVENTOR_API SbVec3f operator+(const SbVec3f &v1, const SbVec3f &v2);

    /// Component-wise binary vector subtraction operator
    friend INVENTOR_API SbVec3f operator-(const SbVec3f &v1, const SbVec3f &v2);

    /// Equality comparison operator
    friend INVENTOR_API int operator==(const SbVec3f &v1, const SbVec3f &v2);
    /// Inequality comparison operator
    friend INVENTOR_API int operator!=(const SbVec3f &v1, const SbVec3f &v2) {
        return !(v1 == v2);
    }

    /// Equality comparison within given tolerance - the square of the
    /// length of the maximum distance between the two vectors
    SbBool equals(const SbVec3f &v, float tolerance) const;

    /// Returns principal axis that is closest (based on maximum dot
    /// product) to this vector
    SbVec3f getClosestAxis() const;

  protected:
    float vec[3]; // Storage for vector components
};

/// 3D vector class.
/// \ingroup Basics
/// 3D vector class used to store 3D vectors and points. This class is used
/// throughout Inventor for arguments and return values.
/// \sa SbVec2f, SbVec3f, SbVec4f, SbVec2s, SbRotation
class INVENTOR_API SbVec3s {
  public:
    /// Default constructor
    SbVec3s() {}

    /// Constructor given an array of 3 components
    SbVec3s(const short v[3]) {
        vec[0] = v[0];
        vec[1] = v[1];
        vec[2] = v[2];
    }

    /// Constructor given 3 individual components
    SbVec3s(short x, short y, short z) {
        vec[0] = x;
        vec[1] = y;
        vec[2] = z;
    }

    /// Returns right-handed cross product of vector and another vector
    SbVec3s cross(const SbVec3s &v) const;

    /// Returns dot (inner) product of vector and another vector
    short dot(const SbVec3s &v) const;

    /// Returns pointer to array of 3 components
    const short *getValue() const { return vec; }

    /// Returns 3 individual components
    void getValue(short &x, short &y, short &z) const;

    /// Negates each component of vector in place
    void negate();

    /// Sets value of vector from array of 3 components
    SbVec3s &setValue(const short v[3]) {
        vec[0] = v[0];
        vec[1] = v[1];
        vec[2] = v[2];
        return *this;
    }

    /// Sets value of vector from 3 individual components
    SbVec3s &setValue(short x, short y, short z) {
        vec[0] = x;
        vec[1] = y;
        vec[2] = z;
        return *this;
    }

    /// Accesses indexed component of vector
    short &operator[](int i) { return (vec[i]); }

    /// Accesses indexed component of vector
    const short &operator[](int i) const { return (vec[i]); }

    /// Component-wise scalar multiplication operator
    SbVec3s &operator*=(short d);

    /// Component-wise scalar division operator
    SbVec3s &operator/=(short d) {
        vec[0] /= d;
        vec[1] /= d;
        vec[2] /= d;
        return *this;
    }

    /// Component-wise vector addition operator
    SbVec3s &operator+=(SbVec3s v);

    /// Component-wise vector subtraction operator
    SbVec3s &operator-=(SbVec3s v);

    /// Nondestructive unary negation - returns a new vector
    SbVec3s operator-() const;

    /// Component-wise binary scalar multiplication operator
    friend INVENTOR_API SbVec3s operator*(const SbVec3s &v, short d);

    /// Component-wise binary scalar multiplication operator
    friend INVENTOR_API SbVec3s operator*(short d, const SbVec3s &v) {
        return v * d;
    }

    /// Component-wise binary scalar division operator
    friend INVENTOR_API SbVec3s operator/(const SbVec3s &v, short d) {
        return SbVec3s(v[0] / d, v[1] / d, v[2] / d);
    }

    /// Component-wise binary vector addition operator
    friend INVENTOR_API SbVec3s operator+(const SbVec3s &v1, const SbVec3s &v2);

    /// Component-wise binary vector subtraction operator
    friend INVENTOR_API SbVec3s operator-(const SbVec3s &v1, const SbVec3s &v2);

    /// Equality comparison operator
    friend INVENTOR_API int operator==(const SbVec3s &v1, const SbVec3s &v2);

    /// Inequality comparison operator
    friend INVENTOR_API int operator!=(const SbVec3s &v1, const SbVec3s &v2) {
        return !(v1 == v2);
    }

  protected:
    short vec[3]; // Storage for vector components
};

/// 2D vector class.
/// \ingroup Basics
/// 2D vector class used to store 2D vectors and points. This class is used
/// throughout Inventor for arguments and return values.
/// \sa SbVec3f, SbVec4f, SbVec2s, SbRotation
class INVENTOR_API SbVec2f {
  public:
    /// Default constructor
    SbVec2f() {}

    /// Constructor given an array of 2 components
    SbVec2f(const float v[2]) { setValue(v); }

    /// Constructor given 2 individual components
    SbVec2f(float x, float y) { setValue(x, y); }

    /// Returns dot (inner) product of vector and another vector
    float dot(const SbVec2f &v) const;

    /// Returns pointer to array of 2 components
    const float *getValue() const { return vec; }

    /// Returns 2 individual components
    void getValue(float &x, float &y) const;

    /// Returns geometric length of vector
    float length() const;

    /// Negates each component of vector in place
    void negate();

    /// Changes vector to be unit length, returning the length before
    /// normalization.
    float normalize();

    /// Sets value of vector from array of 2 components
    SbVec2f &setValue(const float v[2]);

    /// Sets value of vector from 2 individual components
    SbVec2f &setValue(float x, float y);

    /// Accesses indexed component of vector
    float &operator[](int i) { return (vec[i]); }

    /// Accesses indexed component of vector
    const float &operator[](int i) const { return (vec[i]); }

    /// Component-wise scalar multiplication operator
    SbVec2f &operator*=(float d);

    /// Component-wise scalar division operator
    SbVec2f &operator/=(float d) { return *this *= (1.0f / d); }

    /// Component-wise vector addition operator
    SbVec2f &operator+=(const SbVec2f &u);

    /// Component-wise vector subtraction operator
    SbVec2f &operator-=(const SbVec2f &u);

    /// Nondestructive unary negation - returns a new vector
    SbVec2f operator-() const;

    /// Component-wise binary scalar multiplication operator
    friend INVENTOR_API SbVec2f operator*(const SbVec2f &v, float d);

    /// Component-wise binary scalar multiplication operator
    friend INVENTOR_API SbVec2f operator*(float d, const SbVec2f &v) {
        return v * d;
    }

    /// Component-wise binary scalar division operator
    friend INVENTOR_API SbVec2f operator/(const SbVec2f &v, float d) {
        return v * (1.0f / d);
    }

    /// Component-wise binary vector addition operator
    friend INVENTOR_API SbVec2f operator+(const SbVec2f &v1, const SbVec2f &v2);

    /// Component-wise binary vector subtraction operator
    friend INVENTOR_API SbVec2f operator-(const SbVec2f &v1, const SbVec2f &v2);

    /// Equality comparison operator
    friend INVENTOR_API int operator==(const SbVec2f &v1, const SbVec2f &v2);

    /// Inequality comparison operator
    friend INVENTOR_API int operator!=(const SbVec2f &v1, const SbVec2f &v2) {
        return !(v1 == v2);
    }

    /// Equality comparison within given tolerance - the square of the
    /// length of the maximum distance between the two vectors
    SbBool equals(const SbVec2f &v, float tolerance) const;

  protected:
    float vec[2]; // Storage for vector components
};

/// 2D vector class.
/// \ingroup Basics
/// 2D vector class used to store 2D integer vectors and points. This class is
/// used throughout Inventor for arguments and return values. \sa SbVec3f,
/// SbVec4f, SbVec2f
class INVENTOR_API SbVec2s {
  public:
    /// Default constructor
    SbVec2s() {}

    /// Constructor given an array of 2 components
    SbVec2s(const short v[2]) { setValue(v); }

    /// Constructor given 2 individual components
    SbVec2s(short x, short y) { setValue(x, y); }

    /// Returns dot (inner) product of vector and another vector
    int32_t dot(const SbVec2s &v) const;

    /// Returns pointer to array of 2 components
    const short *getValue() const { return vec; }

    /// Returns 2 individual components
    void getValue(short &x, short &y) const;

    /// Negates each component of vector in place
    void negate();

    /// Sets value of vector from array of 2 components
    SbVec2s &setValue(const short v[2]);

    /// Sets value of vector from 2 individual components
    SbVec2s &setValue(short x, short y);

    /// Accesses indexed component of vector
    short &operator[](int i) { return (vec[i]); }

    /// Accesses indexed component of vector
    const short &operator[](int i) const { return (vec[i]); }

    /// Component-wise scalar multiplication operator
    SbVec2s &operator*=(int d);

    /// Component-wise scalar multiplication operator
    SbVec2s &operator*=(double d);

    /// Component-wise scalar division operator
    SbVec2s &operator/=(int d);

    /// Component-wise scalar division operator
    SbVec2s &operator/=(double d) { return *this *= (1.0 / d); }

    /// Component-wise vector addition operator
    SbVec2s &operator+=(const SbVec2s &u);

    /// Component-wise vector subtraction operator
    SbVec2s &operator-=(const SbVec2s &u);

    /// Nondestructive unary negation - returns a new vector
    SbVec2s operator-() const;

    /// Component-wise binary scalar multiplication operator
    friend INVENTOR_API SbVec2s operator*(const SbVec2s &v, int d);

    /// Component-wise binary scalar multiplication operator
    friend INVENTOR_API SbVec2s operator*(const SbVec2s &v, double d);

    /// Component-wise binary scalar multiplication operator
    friend INVENTOR_API SbVec2s operator*(int d, const SbVec2s &v) {
        return v * d;
    }

    /// Component-wise binary scalar multiplication operator
    friend INVENTOR_API SbVec2s operator*(double d, const SbVec2s &v) {
        return v * d;
    }

    /// Component-wise binary scalar division operator
    friend INVENTOR_API SbVec2s operator/(const SbVec2s &v, int d);

    /// Component-wise binary scalar division operator
    friend INVENTOR_API SbVec2s operator/(const SbVec2s &v, double d) {
        return v * (1.0 / d);
    }

    /// Component-wise binary vector addition operator
    friend INVENTOR_API SbVec2s operator+(const SbVec2s &v1, const SbVec2s &v2);

    /// Component-wise binary vector subtraction operator
    friend INVENTOR_API SbVec2s operator-(const SbVec2s &v1, const SbVec2s &v2);

    /// Equality comparison operator
    friend INVENTOR_API int operator==(const SbVec2s &v1, const SbVec2s &v2);

    /// Inequality comparison operator
    friend INVENTOR_API int operator!=(const SbVec2s &v1, const SbVec2s &v2) {
        return !(v1 == v2);
    }

  protected:
    short vec[2]; // Storage for vector components
};

/// 4D vector class.
/// \ingroup Basics
/// 4D vector class used to store homogeneous coordinates. This class is used
/// in Inventor for arguments and return values.
/// \sa SbVec2f, SbVec3f, SbVec2s, SbRotation
class INVENTOR_API SbVec4f {
  public:
    /// Default constructor
    SbVec4f() {}

    /// Constructor given an array of 4 components
    SbVec4f(const float v[4]) { setValue(v); }

    /// Constructor given 4 individual components
    SbVec4f(float x, float y, float z, float w) { setValue(x, y, z, w); }

    /// Returns dot (inner) product of vector and another vector
    float dot(const SbVec4f &v) const;

    /// Returns the real portion of the vector by dividing by the fourth value
    void getReal(SbVec3f &v) const;

    /// Returns pointer to array of 4 components
    const float *getValue() const { return vec; }

    /// Returns 4 individual components
    void getValue(float &x, float &y, float &z, float &w) const;

    /// Returns geometric length of vector
    float length() const;

    /// Negates each component of vector in place
    void negate();

    /// Changes vector to be unit length, returning the length before
    /// normalization.
    float normalize();

    /// Sets value of vector from array of 4 components
    SbVec4f &setValue(const float v[4]);

    /// Sets value of vector from 4 individual components
    SbVec4f &setValue(float x, float y, float z, float w);

    /// Accesses indexed component of vector
    float &operator[](int i) { return (vec[i]); }

    /// Accesses indexed component of vector
    const float &operator[](int i) const { return (vec[i]); }

    /// Component-wise scalar multiplication operator
    SbVec4f &operator*=(float d);

    /// Component-wise scalar division operator
    SbVec4f &operator/=(float d) { return *this *= (1.0f / d); }

    /// Component-wise vector addition operator
    SbVec4f &operator+=(const SbVec4f &u);

    /// Component-wise vector subtraction operator
    SbVec4f &operator-=(const SbVec4f &u);

    /// Nondestructive unary negation - returns a new vector
    SbVec4f operator-() const;

    /// Component-wise binary scalar multiplication operator
    friend INVENTOR_API SbVec4f operator*(const SbVec4f &v, float d);

    /// Component-wise binary scalar multiplication operator
    friend INVENTOR_API SbVec4f operator*(float d, const SbVec4f &v) {
        return v * d;
    }

    /// Component-wise binary scalar division operator
    friend INVENTOR_API SbVec4f operator/(const SbVec4f &v, float d) {
        return v * (1.0f / d);
    }

    /// Component-wise binary vector addition operator
    friend INVENTOR_API SbVec4f operator+(const SbVec4f &v1, const SbVec4f &v2);

    /// Component-wise binary vector subtraction operator
    friend INVENTOR_API SbVec4f operator-(const SbVec4f &v1, const SbVec4f &v2);

    /// Equality comparison operator
    friend INVENTOR_API int operator==(const SbVec4f &v1, const SbVec4f &v2);

    /// Inequality comparison operator
    friend INVENTOR_API int operator!=(const SbVec4f &v1, const SbVec4f &v2) {
        return !(v1 == v2);
    }

    /// Equality comparison within given tolerance - the square of the
    /// length of the maximum distance between the two vectors
    SbBool equals(const SbVec4f &v, float tolerance) const;

  protected:
    float vec[4]; // Storage for vector components
};
