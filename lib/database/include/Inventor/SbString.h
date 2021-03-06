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
 |	This file contains definitions of the SbString
 |	class, which are useful variations on our friend, the
 |	character string.
 |
 |   Author(s)		: Paul S. Strauss, Nick Thompson
 |
 ______________  S I L I C O N   G R A P H I C S   I N C .  ____________
 _______________________________________________________________________
 */

#pragma once

#include <Inventor/SbBasic.h>

#include <string>

/// Class for smart character strings.
/// \ingroup Basics
/// Strings which have many convenience methods to make
/// string manipulation easier.
/// \sa SbName
class INVENTOR_API SbString {
  public:
    /// Default constructor
    SbString() {}

    /// Constructor that initializes to given character string.
    /// The given const char pointer is converted to Unicode using the
    /// fromUtf8() function.
    SbString(const char *str);

    /// Constructors take a character string, the subset of a character string
    /// from start to end (inclusive) The given const char pointer is converted
    /// to Unicode using the fromUtf8() function.
    SbString(const char *str, size_t start, size_t end);

    /// Constructor that initializes to given SbString
    SbString(const SbString &str) { string = str.string; }

    /// Constructor that initializes to string formed from given integer.
    /// For example, SbString(1234) gives the string "1234".
    SbString(int digitString);

    /// Destructor
    ~SbString();

    /// Returns a reasonable hash key for string
    uint32_t hash() { return SbString::hash(string.data()); }

    /// Returns the number of characters in this string.
    size_t getLength() const;

    /// Returns true if the string has no characters; otherwise returns false.
    bool isEmpty() const { return string.empty(); }

    /// Sets string to be the empty string ("").
    void makeEmpty();

    /// Returns pointer to the UTF-8 character string
    const char *getString() const { return string.c_str(); }

    /// Returns an std::wstring encoded in utf16.
    std::wstring toStdWString() const;

    /// Searches the string for the content specified in str, and returns the
    /// position of the first occurrence in the string. When pos is specified
    /// the search only includes characters on or after position pos, ignoring
    /// any possible occurrences in previous locations. If the content is not
    /// found, -1 is returned.
    int find(const SbString &str, int pos = 0) const;

    /// Searches the string for the content specified in str, and returns the
    /// position of the last occurrence in the string. When pos is specified,
    /// the search only includes characters between the beginning of the string
    /// and position pos, ignoring any possible occurrences after pos. If the
    /// content is not found, -1 is returned.
    int rfind(const SbString &str, int pos = -1) const;

    /// Returns new string representing sub-string from startChar to
    /// endChar, inclusive. If endChar is -1 (the default), the
    /// sub-string from startChar until the end is returned.
    SbString getSubString(int startChar, int endChar = -1) const;

    /// Deletes the characters from startChar to endChar, inclusive,
    /// from the string. If endChar is -1 (the default), all characters
    /// from startChar until the end are deleted.
    void deleteSubString(int startChar, int endChar = -1);

    /// Assigns str to this string and returns a reference to this string.
    SbString &operator=(const char *str);

    /// Assigns str to this string and returns a reference to this string.
    SbString &operator=(const SbString &str);

    /// Appends the string str onto the end of this string and returns a
    /// reference to this string.
    SbString &operator+=(const char *str);

    /// Appends the string str onto the end of this string and returns a
    /// reference to this string.
    SbString &operator+=(const SbString &str);

    /// Unary "not" operator; returns TRUE if string is empty ("")
    bool operator!() const { return string.empty(); }

    /// Equality operator for SbString/char* and SbString/SbString comparison
    friend INVENTOR_API bool operator==(const SbString &str, const char *s);

    /// Returns true if str1 is equal to str2; otherwise returns false.
    friend INVENTOR_API bool operator==(const char *s, const SbString &str) {
        return (str.string == s);
    }

    /// Returns true if str1 is equal to str2; otherwise returns false.
    friend INVENTOR_API bool operator==(const SbString &str1,
                                        const SbString &str2) {
        return (str1.string == str2.string);
    }

    /// Returns true if this string str1 is not equal to string str2; otherwise
    /// returns false.
    friend INVENTOR_API bool operator!=(const SbString &str, const char *s);

    /// Returns true if this string str1 is not equal to string str2; otherwise
    /// returns false.
    friend INVENTOR_API bool operator!=(const char *s, const SbString &str) {
        return (str.string != s);
    }

    /// Returns a string which is the result of concatenating s1 and s2.
    friend INVENTOR_API const SbString operator+(const SbString &s1,
                                                 const SbString &s2) {
        SbString t(s1);
        t += s2;
        return t;
    }

    friend INVENTOR_API int operator!=(const SbString &str1,
                                       const SbString &str2) {
        return (str1.string != str2.string);
    }

    /// Creates a string from ISO-8859-1.
    static SbString fromLatin1(const char *latin, int size = -1);

    /// Creates a string from UTF-8.
    static SbString fromUtf8(const char *utf8, int size = -1);

    /// Creates a string from UTF-16 (wide character).
    static SbString fromWideChar(const wchar_t *wcs, int size = -1);

    SoINTERNAL
  public:
    static uint32_t hash(const char *s); // Hash function

  private:
    std::string string; // String storage
};
