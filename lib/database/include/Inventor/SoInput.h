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
 |	This file contains the definition of the SoInput class.
 |
 |   Classes:
 |	SoInput, SoInputFile (internal)
 |
 |   Author(s)		: Paul S. Strauss
 |
 ______________  S I L I C O N   G R A P H I C S   I N C .  ____________
 _______________________________________________________________________
 */

#pragma once

#include <Inventor/misc/SoBasic.h>
#include <Inventor/SbString.h>
#include <Inventor/SoDB.h>

#include <vector>

class SbDict;
class SoNode;
class SoPath;
class SoBase;
class SoDB;
class SbStringList;
struct SoInputFile;

/// Used to read Inventor data files.
/// \ingroup General
/// This class is used by the <tt>SoDB</tt> reading routines when reading
/// Inventor data files.  It supports both ASCII (default) and binary
/// Inventor formats.  Users can also register additional
/// valid file headers.
/// When reading, <tt>SoInput</tt> skips over Inventor comments
/// (from '#' to end of
/// line) and can stack input files. When EOF is reached, the stack is
/// popped.  This class can also be used to read from a buffer in memory.
/// \sa SoDB, SoOutput, SoTranReceiver
class INVENTOR_API SoInput {
  public:
    /// Constructor (default SoInput reads from stdin)
    SoInput();

    /// Destructor closes file if SoInput opened it.
    ~SoInput();

    /// Adds a directory to list of directories to search to find named
    /// files to open. Directories searched in order. By default, the list
    /// contains just the current directory.
    static void addDirectoryFirst(const SbString &dirName);
    static void addDirectoryLast(const SbString &dirName);

    /// The SoInput class maintains a global list of directories that is
    /// searched to find files when opening them. Directories are searched in
    /// order. Each of these routines adds directories to the list, either at
    /// the beginning ("First") or the end ("Last"). The last two routines add
    /// directories named in the value of the given environment variable.
    /// Directories may be separated by colons or whitespace in the variable's
    /// value.
    static void addEnvDirectoriesFirst(const SbString &envVarName);
    static void addEnvDirectoriesLast(const SbString &envVarName);

    /// Removes named directory from the list.
    static void removeDirectory(const SbString &dirName);

    /// Clears the list of directories (including the current directory).
    static void clearDirectories();

    /// Returns the list of directories as an SbStringList.
    static const SbStringList &getDirectories();

    /// Sets file pointer to read from. Clears the stack of input files if
    /// necessary.
    void setFilePointer(FILE *newFP);

    /// Opens named file, sets file pointer to result. Clears the stack of
    /// input files if necessary. This returns FALSE on error; if
    /// \a okIfNotFound is FALSE (the default), this prints an error message
    /// if the file could not be found.
    SbBool openFile(const SbString &fileName, SbBool okIfNotFound = FALSE);

    /// Opens named file, pushing the resulting file pointer onto the stack.
    /// Returns FALSE on error.
    SbBool pushFile(const SbString &fileName);

    /// Closes all files on stack opened with #openFile() or #pushFile()
    void closeFile();

    /// Returns TRUE if the currently open file is a valid Inventor file; that
    /// is, it begins with a valid Inventor header, or one that has been
    /// registered with SoDB::registerHeader.
    SbBool isValidFile();

    /// Returns a pointer to the current file, or NULL if reading from a buffer.
    FILE *getCurFile() const;

    /// Returns full name (including directory path) of current file, or empty
    /// string if reading from a buffer.
    const SbString &getCurFileName() const;

    /// Sets an in-memory buffer to read from, along with its size.
    void setBuffer(const void *bufPointer, size_t bufSize);

    /// Returns number of bytes read from buffer. Returns 0 if not
    /// reading from a buffer.
    size_t getNumBytesRead() const;

    /// Returns the header of the file being read.
    SbString getHeader();

    /// Returns the Inventor file version of the file being read (e.g. 2.1).
    /// If the file has a header registered through SoDB::registerHeader(),
    /// the returned version is the Inventor version registered with the header.
    float getIVVersion() const;

    SoEXTENDER
  public:
    // Returns whether current file/buffer being read is binary
    SbBool isBinary();

    // Reads next character from current file/buffer. Returns FALSE on
    // EOF or error.
    SbBool get(char &c);

    // Reads next ASCII character from current buffer. Returns FALSE on
    // EOF or error.
    SbBool getASCIIBuffer(char &c);

    // Reads next ASCII character from current file. Returns FALSE on
    // EOF or error.
    SbBool getASCIIFile(char &c);

    // Reads next ASCII format hex value from current file/buffer.
    // Returns FALSE on EOF or error.
    SbBool readHex(uint32_t &l);

    // Reads item of particular type from current file pointer/buffer. All
    // skip white space before reading and return FALSE on EOF or if
    // item could not be read.
    SbBool read(char &c);
    SbBool read(SbString &s);
    SbBool read(SbName &n, SbBool validIdent = FALSE);
    SbBool read(int &i);
    SbBool read(unsigned int &i);
    SbBool read(short &s);
    SbBool read(unsigned short &s);
    SbBool read(float &f);
    SbBool read(double &d);
    SbBool readBinaryArray(unsigned char *c, size_t length);
    SbBool readBinaryArray(int32_t *l, size_t length);
    SbBool readBinaryArray(float *f, size_t length);
    SbBool readBinaryArray(double *d, size_t length);

    // Returns TRUE if current file/buffer is at EOF
    SbBool eof() const;

    SoINTERNAL
  public:
    // 	Init function sets up global directory list
    static void init();

    // Clean-up.
    static void finish();

    // Constructor that gets reference dictionary from another SoInput
    SoInput(SoInput *dictIn);

    // Fills in passed string to contain description of current
    // location in all open input files
    void getLocationString(SbString &string) const;

    // Puts a just-read character or string back in input stream/buffer
    void putBack(char c);
    void putBack(const char *string);

    // Adds a reference to dictionary in current file.  This may also
    // add a reference to the global dictionary if addToGlobalDict is
    // TRUE (the default).
    void addReference(const SbName &name, SoBase *base,
                      SbBool addToGlobalDict = TRUE);

    // Removes a reference to dictionary in current file.  This may
    // also remove a reference from the global dictionary.
    void removeReference(const SbName &name);

    // Looks up a reference, returning the base pointer or NULL
    SoBase *findReference(const SbName &name) const;

    // Looks for named file. Returns false if not found.
    static bool findFile(const SbString &fileName, SbString &fullName);

  private:
    static SbStringList *      directories; // Directory search path.
    std::vector<SoInputFile *> files;       // Stack of SoInputFiles (depth >=1)
    struct SoInputFile *       curFile;     // Top of stack
    SbString                   backBuf;     // For strings that are put back
    int backBufIndex;                       // Index into backBuf (-1 if no buf)

    std::vector<char> tmpBuffer; // Buffer for binary read from file

    char backupBuf[8];    // Buffer for storing data that
                          // has been read but can't be put back.
    SbBool backupBufUsed; // True if backupBuf contains data

    // Set the Inventor version number of the current file
    void setIVVersion(float version);

    // Initializes reading from file
    bool initFile(const SbString &fileName, SbString *fullName,
                  SbDict *refDict = NULL);

    // Checks current file for ASCII/binary header comment. Returns
    // FALSE if no header was found.
    SbBool checkHeader();

    // Returns TRUE if reading from memory buffer rather than file
    SbBool fromBuffer() const;

    // Skips over white space in input. Pops file if EOF is hit.
    // Returns FALSE on error.
    SbBool skipWhiteSpace();

    // Pops current file from stack, if possible.
    SbBool popFile();

    // Returns number of bytes left in current buffer
    size_t freeBytesInBuf() const;

    // Reads integer, unsigned integer, or floating-point number.
    // Returns FALSE on EOF or error
    SbBool readInteger(int32_t &l);
    SbBool readUnsignedInteger(uint32_t &l);
    SbBool readReal(double &d);

    // Reads unsigned integer string into str. Accepts decimal, octal,
    // and hex integers. Returns FALSE on EOF or error
    SbBool readUnsignedIntegerString(char *str);

    // Reads string of decimal or hexadecimal digits into string.
    // Returns number of bytes read.
    int readDigits(char *string);
    int readHexDigits(char *string);

    // Reads given character from buffer into string. Returns 0 or 1
    int readChar(char *string, char charToRead);

    // Convert datatypes to network format during writing
    void convertShort(const char *from, short *s);
    void convertInt32(const char *from, int32_t *l);
    void convertFloat(const char *from, float *f);
    void convertDouble(const char *from, double *d);
    void convertShortArray(const char *from, short *to, size_t len);
    void convertInt32Array(const char *from, int32_t *to, size_t len);
    void convertFloatArray(const char *from, float *to, size_t len);
    void convertDoubleArray(const char *from, double *to, size_t len);
};
