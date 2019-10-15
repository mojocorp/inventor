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
 * Copyright (C) 1990,91   Silicon Graphics, Inc.
 *
 _______________________________________________________________________
 ______________  S I L I C O N   G R A P H I C S   I N C .  ____________
 |
 |   $Revision: 1.10 $
 |
 |   Classes:
 |      SoFontOutline
 |      SoOutlineFontCache
 |
 |
 |   Author(s)          : Gavin Bell
 |
 ______________  S I L I C O N   G R A P H I C S   I N C .  ____________
 _______________________________________________________________________
 */

#include <cstring>
#include <cmath>
#include <machine.h>
#include <Inventor/caches/SoOutlineFontCache.h>
#include <Inventor/elements/SoCacheElement.h>
#include <Inventor/elements/SoComplexityElement.h>
#include <Inventor/elements/SoComplexityTypeElement.h>
#include <Inventor/elements/SoCreaseAngleElement.h>
#include <Inventor/elements/SoFontNameElement.h>
#include <Inventor/elements/SoFontSizeElement.h>
#include <Inventor/elements/SoGLCacheContextElement.h>
#include <Inventor/elements/SoGLTextureEnabledElement.h>
#include <Inventor/elements/SoModelMatrixElement.h>
#include <Inventor/elements/SoProjectionMatrixElement.h>
#include <Inventor/elements/SoViewingMatrixElement.h>
#include <Inventor/elements/SoViewportRegionElement.h>
#include <Inventor/elements/SoMaterialBindingElement.h>
#include <Inventor/elements/SoProfileElement.h>
#include <Inventor/elements/SoProfileCoordinateElement.h>
#include <Inventor/nodes/SoProfile.h>
#include <Inventor/nodes/SoShape.h>

SbBool SoOutlineFontCache::tesselationError = FALSE;
SbPList *SoOutlineFontCache::fonts = NULL;
FLcontext SoOutlineFontCache::context = NULL;
iconv_t SoOutlineFontCache::conversionCode = NULL;

// First, a more convenient structure for outlines:
class SoFontOutline {

  public:
    // Constructor, takes a pointer to the font-library outline
    // structure and the font's size:
    SoFontOutline(FLoutline *outline, float fontSize);
    // Destructor
    ~SoFontOutline();

    // Query routines:
    int		getNumOutlines() { return numOutlines; }
    int		getNumVerts(int i) { return numVerts[i]; }
    SbVec2f	&getVertex(int i, int j) { return verts[i][j]; }
    SbVec2f	getCharAdvance() { return charAdvance; }

    static SoFontOutline *getNullOutline();

  private:
    // Internal constructor used by getNullOutline:
    SoFontOutline();

    // This basically mimics the FLoutline structure, with the
    // exception that the font size is part of the outline:
    int numOutlines;
    int *numVerts;
    SbVec2f **verts;
    SbVec2f charAdvance;
};

////////////////////////////////////////////////////////////////////////
//
// Description:
//    Find an appropriate font, given a state.  A bunch of elements
//    (fontName, fontSize, creaseAngle, complexity and profile) must
//    be enabled in the state...
//
// Use: static, internal

SoOutlineFontCache *
SoOutlineFontCache::getFont(SoState *state, SbBool forRender)
//
////////////////////////////////////////////////////////////////////////
{
    if (fonts == NULL) {
    // One-time font library initialization
    fonts = new SbPList;
    context = flCreateContext(NULL, FL_FONTNAME, NULL,
                  1.0, 1.0);
    if (context == NULL) {
#ifdef DEBUG
        SoDebugError::post("SoText3::getFont",
                   "flCreateContext returned NULL");
#endif
        return NULL;
    }
    flMakeCurrentContext(context);
    flSetHint(FL_HINT_FONTTYPE, FL_FONTTYPE_OUTLINE);
    }
    else if (context == NULL) return NULL;
    else {
    if (flGetCurrentContext() != context)
        flMakeCurrentContext(context);
    }

    SoOutlineFontCache *result = NULL;
    for (int i = 0; i < fonts->getLength() && result == NULL; i++) {
    SoOutlineFontCache *c = (SoOutlineFontCache *) (*fonts)[i];
    if (forRender ? c->isRenderValid(state) : c->isValid(state)) {
        result = c; // Loop will terminate...
        result->ref(); // Increment ref count
    }
    }
    // If none match:
    if (result == NULL) {
    result = new SoOutlineFontCache(state);

    }
    return result;
}
////////////////////////////////////////////////////////////////////////
//
// Description:
//    Create a list of font numbers from a list of font names
//
// Use: private

GLubyte *
SoOutlineFontCache::createUniFontList(const char* fontNameList)
//
////////////////////////////////////////////////////////////////////////
{
    char *s, *s1, *ends;
    FLfontNumber fn;
    float mat[2][2];

    mat[0][0] = mat[1][1] = 1.0;
    mat[0][1] = mat[1][0] = 0.0;

    //Make a copy of fontNameList so we don't disturb the one we are passed.
    //Find \n at end of namelist:
    char * nameCopy = new char[strlen(fontNameList)+1];
    strcpy(nameCopy, fontNameList);

    //find the last null in nameCopy.
    s = ends = (char *)strrchr(nameCopy, '\0');
    *s = ';';  /* put a guard in the end of string */


    s = (char*)nameCopy;
    fontNums = new SbPList;

    while ((s1 = (char *)strchr(s, ';'))) {
       *s1 = (char)NULL;  /* font name is pointed to s */

       if ((fn = flCreateFont((const GLubyte*)s, mat, 0, NULL)) == (FLfontNumber)0) {
#ifdef DEBUG
        SoDebugError::post("SoOutlineFontCache::createUniFontList",
        "Cannot create font %s", s);
#endif
       }
       else fontNums->append((void*)(unsigned long)fn);
       if(s1 == ends) break;
       s = (s1 + 1);  /* move to next font name */
    }

    if (fontNums->getLength() == 0 ) return NULL;

    // create a comma-separated list of font numbers:
    char *fontList = new char[10*fontNums->getLength()];
    fontList[0] = '\0';
    for (int i = 0; i< fontNums->getLength(); i++ ){
    fn = (FLfontNumber)(long)(*fontNums)[i];
    sprintf(&fontList[strlen(fontList)], "%d,", fn);
    }
    fontList[strlen(fontList) - 1] = '\0'; // the last ',' is replaced with NULL

    delete [] nameCopy;

    return (GLubyte *)fontList;

}
////////////////////////////////////////////////////////////////////////
//
// Description:
//    Sees if this font is valid.  If it is valid, it also makes it
//    current.
//
// Use: public

SbBool
SoOutlineFontCache::isValid(const SoState *state) const
//
////////////////////////////////////////////////////////////////////////
{
    SbBool result = SoCache::isValid(state);

    if (result) {
    if (flGetCurrentContext() != context) {
        flMakeCurrentContext(context);
    }
    }
    return result;
}

////////////////////////////////////////////////////////////////////////
//
// Description:
//    Figures out if this cache is valid for rendering.
//
// Use: internal

SbBool
SoOutlineFontCache::isRenderValid(SoState *state) const
//
////////////////////////////////////////////////////////////////////////
{
    // Special cache case here:  if we generated side display lists
    // without texture coordinates AND we need texture coordinates,
    // we'll have to regenerate and this cache is invalid:
    if (sideList) {
    if (!sidesHaveTexCoords &&
        SoGLTextureEnabledElement::get(state)) {
        return FALSE;
    }
    }

    if (!isValid(state)) return FALSE;

    if (frontList &&
    frontList->getContext() != SoGLCacheContextElement::get(state))
    return FALSE;
    if (sideList &&
    sideList->getContext() != SoGLCacheContextElement::get(state))
    return FALSE;

    return TRUE;
}

////////////////////////////////////////////////////////////////////////
//
// Description:
//    Constructor.  Called by getFont().
//
// Use: private

SoOutlineFontCache::SoOutlineFontCache(SoState *state) :
    SoCache(state)
//
////////////////////////////////////////////////////////////////////////
{
    ref();

    frontList = sideList = NULL;

    // Add element dependencies explicitly here; making 'this' the
    // CacheElement doesn't work if we are being constructed in an
    // action that doesn't have caches.
    SbName font = SoFontNameElement::get(state);
    addElement(state->getConstElement(
    SoFontNameElement::getClassStackIndex()));
    if (font == SoFontNameElement::getDefault()) {
    font = SbName("Utopia-Regular");
    }

    float uems;

    // Remember size
    fontSize = SoFontSizeElement::get(state);
    addElement(state->getConstElement(
    SoFontSizeElement::getClassStackIndex()));

    // Figure out complexity...
    float complexity = SoComplexityElement::get(state);
    addElement(state->getConstElement(
    SoComplexityElement::getClassStackIndex()));
    addElement(state->getConstElement(
    SoComplexityTypeElement::getClassStackIndex()));

    switch (SoComplexityTypeElement::get(state)) {
      case SoComplexityTypeElement::OBJECT_SPACE:
    {
        // Two ramps-- complexity of zero  == 250/1000 of an em
        //             complexity of .5    == 20/1000 of an em
        //             complexity of 1     == 1/1000 of an em
        const float ZERO = 250;
        const float HALF = 20;
        const float ONE = 1;
        if (complexity > 0.5) uems = (2.0-complexity*2.0)*(HALF-ONE)+ONE;
        else uems = (1.0-complexity*2.0)*(ZERO-HALF)+HALF;
    }
    break;

      case SoComplexityTypeElement::SCREEN_SPACE:
    {
        SbVec3f p(fontSize, fontSize, fontSize);
        SbVec2s rectSize;

        SoShape::getScreenSize(state, SbBox3f(-p, p), rectSize);
        float maxSize =
        (rectSize[0] > rectSize[1] ? rectSize[0] : rectSize[1]);
        uems = 250.0 / (1.0 + 0.25 * maxSize * complexity *
                complexity);

        // We have to manually add the dependency on the
        // projection, view and model matrix elements (these are
        // gotten in the SoShape::getScreenSize routine), and the
        // ViewportRegionElement:
        addElement(state->getConstElement(
        SoProjectionMatrixElement::getClassStackIndex()));
        addElement(state->getConstElement(
        SoViewingMatrixElement::getClassStackIndex()));
        addElement(state->getConstElement(
        SoModelMatrixElement::getClassStackIndex()));
        addElement(state->getConstElement(
        SoViewportRegionElement::getClassStackIndex()));
    }
    break;

      case SoComplexityTypeElement::BOUNDING_BOX:
    {
        uems = 20;
    }
    break;
    }
    flSetHint(FL_HINT_TOLERANCE, uems);

    fontNumList = createUniFontList(font.getString());

    // If error creating font:
    if (fontNumList == NULL) {
    // Try Utopia-Regular, unless we just did!
    if (font != SbName("Utopia-Regular")) {
#ifdef DEBUG
        SoDebugError::post("SoText3::getFont",
              "Couldn't find font %s, replacing with Utopia-Regular",
               font.getString());
#endif
        fontNumList = createUniFontList("Utopia-Regular");
    }
    if (fontNumList == NULL) {
#ifdef DEBUG
        SoDebugError::post("SoText3::getFont",
                   "Couldn't find font Utopia-Regular!");
#endif
    }
    }

    numChars = 65536;  //Allow for all UCS-2 possibilities.
    sidesHaveTexCoords = FALSE;
    currentNodeId = 0; //guarantee UCS translation occurs first time.

    //sideDict and frontDict indicate if display lists exist for front,sides
    //outlineDict has pointer to outline.
    sideDict = new SbDict;
    frontDict = new SbDict;
    outlineDict = new SbDict;

    // Get profile info:
    const SoNodeList &profiles = SoProfileElement::get(state);
    addElement(state->getConstElement(
    SoProfileElement::getClassStackIndex()));
    addElement(state->getConstElement(
    SoProfileCoordinateElement::getClassStackIndex()));
    nProfileVerts = 0;
    if (profiles.getLength() > 0) {
    SoProfile *profileNode = (SoProfile *)profiles[0];
    profileNode->getVertices(state, nProfileVerts, profileVerts);
    } else {
    nProfileVerts = 2;
    profileVerts = new SbVec2f[2];
    profileVerts[0].setValue(0, 0);
    profileVerts[1].setValue(1, 0);
    }

    if (nProfileVerts > 1) {
    cosCreaseAngle = cos(SoCreaseAngleElement::get(state));
    addElement(state->getConstElement(
        SoCreaseAngleElement::getClassStackIndex()));
    int nSegments = (int) nProfileVerts - 1;

    // Figure out normals for profiles; there are twice as many
    // normals as segments.  The two normals for each segment endpoint
    // may be averaged with the normal for the next segment, depending
    // on whether or not the angle between the segments is greater
    // than the creaseAngle.
    profileNorms = new SbVec2f[nSegments*2];
    figureSegmentNorms(profileNorms, (int) nProfileVerts, profileVerts,
               cosCreaseAngle, FALSE);
    // Need to flip all the normals because of the way the profiles
    // are defined:
    int i;
    for (i = 0; i < nSegments*2; i++) {
        profileNorms[i] *= -1.0;
    }

    // Figure out S texture coordinates, which run along the profile:
    sTexCoords = new float[nProfileVerts];
    figureSegmentTexCoords(sTexCoords, (int) nProfileVerts,
                   profileVerts, FALSE);
    // And reverse them, so 0 is at the back of the profile:
    float max = sTexCoords[nProfileVerts-1];
    for (i = 0; i < nProfileVerts; i++) {
        sTexCoords[i] = max - sTexCoords[i];
    }
    } else {
    profileNorms = NULL;
    sTexCoords = NULL;
    }

    fonts->append(this);
}

////////////////////////////////////////////////////////////////////////
//
// Description:
//    Destructor
//
// Use: private

SoOutlineFontCache::~SoOutlineFontCache()
//
////////////////////////////////////////////////////////////////////////
{
    if (fontNumList) {
    if (flGetCurrentContext() != context) {
        flMakeCurrentContext(context);
    }

    // Free up cached outlines, display lists

    if (hasProfile()) {
        delete[] profileVerts;
        delete[] sTexCoords;
        delete[] profileNorms;
    }
    //Must free every outline in dictionary:

    outlineDict->applyToAll(freeOutline);

    // Only destroy the font library font if no other font caches
    // are using the same font identifier:
    // Must go through fontlist and destroy every font that isn't used
    // by any other cache.

    SbBool otherFonts = (fonts->getLength() > 1);
    SbDict *otherFontDict;
    if (otherFonts){
        otherFontDict = new SbDict;
        //Enter all the other fontnums into the dictionary:
        for (int i = 0; i< fonts->getLength(); i++) {
        SoOutlineFontCache *t = (SoOutlineFontCache *)(*fonts)[i];
        if ( t == this) continue;
        for (int j = 0; j< (t->fontNums->getLength()); j++){
            unsigned long key = (unsigned long)(*(t->fontNums))[j];
            otherFontDict->enter(key, NULL);
        }
        }
    }
    // Now destroy any fonts that don't appear in otherFontDict
    for (int i = 0; i < fontNums->getLength(); i++){
        void *value;
        if ( !otherFonts ||
            !otherFontDict->find((unsigned long)(*fontNums)[i], value)){
        flDestroyFont((FLfontNumber)(long)(*fontNums)[i]);
        }
    }
    if (otherFonts) delete otherFontDict;
    delete frontDict;
    delete sideDict;
    delete outlineDict;

    if (fontNumList)	delete [] fontNumList;
    if (fontNums)		delete fontNums;

    fonts->remove(fonts->find(this));
    }
}
////////////////////////////////////////////////////////////////////////
//
// Description:
//    Destroy this cache.  Called by unref(); frees up OpenGL display
//    lists.
//
// Use: protected, virtual

void
SoOutlineFontCache::destroy(SoState *)
//
////////////////////////////////////////////////////////////////////////
{
    // Pass in NULL to unref because this cache may be destroyed
    // from an action _other_ than GLRender:
    if (frontList) {
    frontList->unref(NULL);
    frontList = NULL;
    }
    if (sideList) {
    sideList->unref(NULL);
    sideList = NULL;
    }
    SoCache::destroy(NULL);
}

////////////////////////////////////////////////////////////////////////
//
// Description:
//    Returns the width of the specified line in the cache.
//
// Use: private

float
SoOutlineFontCache::getWidth(int line)
//
////////////////////////////////////////////////////////////////////////
{
    float total = 0.0;
    const char *chars = getUCSString(line);

    for (int i = 0; i < getNumUCSChars(line); i++) {
    SoFontOutline *outline = getOutline(chars+2*i);
    total += outline->getCharAdvance()[0];
    }

    return total;
}

////////////////////////////////////////////////////////////////////////
//
// Description:
//    Returns the 2D bounding box of the given character.
//
// Use: private

void
SoOutlineFontCache::getCharBBox(const char* c, SbBox2f &result)
//
////////////////////////////////////////////////////////////////////////
{
    result.makeEmpty();

    if (!fontNumList) return;

    SoFontOutline *outline = getOutline(c);

    for (int i = 0; i < outline->getNumOutlines(); i++) {
    for (int j = 0; j < outline->getNumVerts(i); j++) {
        result.extendBy(outline->getVertex(i,j));
    }
    }
}

////////////////////////////////////////////////////////////////////////
//
// Description:
//    Given a UCS character, return an outline for the character.  If, for
//    some reason, we can't get the outline, an 'identity' or 'null'
//    outline is returned.
//
// Use: private

SoFontOutline *
SoOutlineFontCache::getOutline(const char* c)
//
////////////////////////////////////////////////////////////////////////
{
    if (!fontNumList) {
    return SoFontOutline::getNullOutline();
    }
    unsigned char *uc = (unsigned char*)c;
    long key = (uc[0]<<8)|uc[1];
    void *value;
    if (!outlineDict->find(key, value)){

    FLoutline *flo = flUniGetOutline(fontNumList, (GLubyte*)c);
    if (flo == NULL) {
        value = (void*)SoFontOutline::getNullOutline();
    } else {
        value = (void*) new SoFontOutline(flo, fontSize);
        flFreeOutline(flo);
    }
    outlineDict->enter(key, value);
    }
    return (SoFontOutline*)value;
}

////////////////////////////////////////////////////////////////////////
//
// Description:
//    Given a character, return the correct amount to advance after
//    drawing that character.  Note:  if we ever wanted to deal with
//    kerning, we'd have to fix this...
//
// Use: private

SbVec2f
SoOutlineFontCache::getCharOffset(const char* c)
//
////////////////////////////////////////////////////////////////////////
{
    if (!fontNumList) return SbVec2f(0,0);

    return getOutline(c)->getCharAdvance();
}

////////////////////////////////////////////////////////////////////////
//
// Description:
//    Render the fronts of the given string.  The GL transformation
//    matrix is munged by this routine-- surround it by
//    PushMatrix/PopMatrix.
//
// Use: public, internal

void
SoOutlineFontCache::generateFrontChar(const char* c,
                      GLUtesselator *tobj)
//
////////////////////////////////////////////////////////////////////////
{
    if (!fontNumList) return;

    GLdouble v[3];

    tesselationError = FALSE;
    gluTessBeginPolygon(tobj, NULL);
    gluTessBeginContour(tobj);

    // Get outline for character
    SoFontOutline *outline = getOutline(c);
    int i;
    for (i = 0; i < outline->getNumOutlines(); i++) {

    // It would be nice if the font manager told us the type of
    // each outline...
    gluTessEndContour(tobj);
    gluTessBeginContour(tobj);

    for (int j = 0; j < outline->getNumVerts(i); j++) {
        SbVec2f &t = outline->getVertex(i,j);
        v[0] = t[0];
        v[1] = t[1];
        v[2] = 0.0;

        // Note: The third argument MUST NOT BE a local variable,
        // since glu just stores the pointer and only calls us
        // back at the gluEndPolygon call.
        gluTessVertex(tobj, v, &t);
    }
    }
    gluTessEndContour(tobj);
    gluTessEndPolygon(tobj);

    // If there was an error tesselating the character, just generate
    // a bounding box for the character:
    if (tesselationError) {
    SbBox2f charBBox;
    getCharBBox(c, charBBox);
    if (!charBBox.isEmpty()) {
        SbVec2f boxVerts[4];
        charBBox.getBounds(boxVerts[0], boxVerts[2]);
        boxVerts[1].setValue(boxVerts[2][0], boxVerts[0][1]);
        boxVerts[3].setValue(boxVerts[0][0], boxVerts[2][1]);

        gluTessBeginPolygon(tobj, NULL);
        gluTessBeginContour(tobj);

        for (i = 0; i < 4; i++) {
        v[0] = boxVerts[i][0];
        v[1] = boxVerts[i][1];
        v[2] = 0.0;
        gluTessVertex(tobj, v, &boxVerts[i]);
        }
        gluTessEndContour(tobj);
        gluTessEndPolygon(tobj);
    }
    }
}

////////////////////////////////////////////////////////////////////////
//
// Description:
//    Sets up for GL rendering.
//
// Use: internal

void
SoOutlineFontCache::setupToRenderFront(SoState *state)
//
////////////////////////////////////////////////////////////////////////
{
    otherOpen = SoCacheElement::anyOpen(state);
    if (!otherOpen && !frontList) {
    frontList = new SoGLDisplayList(state,
                    SoGLDisplayList::DISPLAY_LIST,
                    numChars);
    frontList->ref();
    }
    if (frontList) {
    // Set correct list base
    glListBase(frontList->getFirstIndex());
    frontList->addDependency(state);
    }
}

////////////////////////////////////////////////////////////////////////
//
// Description:
//    Sets up for GL rendering.
//
// Use: internal

void
SoOutlineFontCache::setupToRenderSide(SoState *state, SbBool willTexture)
//
////////////////////////////////////////////////////////////////////////
{
    otherOpen = SoCacheElement::anyOpen(state);
    if (!otherOpen && !sideList) {
    sideList = new SoGLDisplayList(state,
                    SoGLDisplayList::DISPLAY_LIST,
                    numChars);
    sideList->ref();
    sidesHaveTexCoords = willTexture;
    }
    if (sideList) {
    // Set correct list base
    glListBase(sideList->getFirstIndex());
    sideList->addDependency(state);
    }
}

////////////////////////////////////////////////////////////////////////
//
// Description:
//    Returns TRUE if a display lists exists for given character.
//    Tries to build a display list, if it can.
//
// Use: internal

SbBool
SoOutlineFontCache::hasFrontDisplayList(const char* c,
                    GLUtesselator *tobj)
//
////////////////////////////////////////////////////////////////////////
{
    // If we have one, return TRUE
    unsigned char *uc = (unsigned char*)c;
    long key = (uc[0]<<8) | uc[1];
    void *value;
    if (frontDict->find(key, value)) return TRUE;

    // If we don't and we can't build one, return FALSE.
    if (otherOpen) return FALSE;

    // Build one:
    glNewList(frontList->getFirstIndex()+key, GL_COMPILE);
    generateFrontChar(c, tobj);
    SbVec2f t = getOutline(c)->getCharAdvance();
    glTranslatef(t[0], t[1], 0.0);
    glEndList();

    frontDict->enter(key, value);

    return TRUE;
}
////////////////////////////////////////////////////////////////////////
//
// Description:
//    Returns TRUE if a display lists exists for given character.
//    Tries to build a display list, if it can.
//
// Use: internal

SbBool
SoOutlineFontCache::hasSideDisplayList(const char* c,
                       SideCB callbackFunc)
//
////////////////////////////////////////////////////////////////////////
{
    unsigned char *uc = (unsigned char*)c;
    long key = (uc[0]<<8) | uc[1];
    void *value;
    // If we have one, return TRUE
    if (sideDict->find(key, value)) return TRUE;

    // If we don't and we can't build one, return FALSE.
    if (otherOpen) return FALSE;

    // Build one:
    glNewList(sideList->getFirstIndex()+key, GL_COMPILE);

    glBegin(GL_QUADS);    // Render as independent quads:
    generateSideChar(c, callbackFunc);
    glEnd();

    SbVec2f t = getOutline(c)->getCharAdvance();
    glTranslatef(t[0], t[1], 0.0);
    glEndList();
    sideDict->enter(key, value);

    return TRUE;
}

////////////////////////////////////////////////////////////////////////
//
// Description:
//    Assuming that there are display lists built for all the
//    characters in given string, render them using the GL's CallLists
//    routine.
//
// Use: internal

void
SoOutlineFontCache::callFrontLists(int line)
//
////////////////////////////////////////////////////////////////////////
{
    const char *str = getUCSString(line);

    glCallLists(getNumUCSChars(line), GL_2_BYTES, (unsigned char*)str);
}

////////////////////////////////////////////////////////////////////////
//
// Description:
//    Assuming that there are display lists built for all the
//    characters in given line, render them using the GL's CallLists
//    routine.
//
// Use: internal

void
SoOutlineFontCache::callSideLists(int line)
//
////////////////////////////////////////////////////////////////////////
{
    const char *str = getUCSString(line);

    glCallLists(getNumUCSChars(line), GL_2_BYTES, (unsigned char*)str);
}

////////////////////////////////////////////////////////////////////////
//
// Description:
//    Assuming that there are not display lists built for all the
//    characters in given string, render the string.
//
// Use: internal

void
SoOutlineFontCache::renderFront(int line,
                GLUtesselator *tobj)
//
////////////////////////////////////////////////////////////////////////
{
    const char *str = getUCSString(line);
    unsigned char *ustr = (unsigned char*)str;

    void *value;
    for (int i = 0; i < getNumUCSChars(line); i++) {
    long key = ustr[2*i]<<8 | ustr[2*i+1];
    if (frontDict->find(key, value)) {
        glCallList(frontList->getFirstIndex()+key);
    }
    else {
        generateFrontChar(str+2*i, tobj);
        SbVec2f t = getOutline(str+2*i)->getCharAdvance();
        glTranslatef(t[0], t[1], 0.0);
    }
    }
}

////////////////////////////////////////////////////////////////////////
//
// Description:
//    Assuming that there are not display lists built for all the
//    characters in given line, render the line.
//
// Use: private internal

void
SoOutlineFontCache::renderSide(int line,
                   SideCB callbackFunc)
//
////////////////////////////////////////////////////////////////////////
{
    const char *str = getUCSString(line);
    unsigned char *ustr = (unsigned char*)str;
    void* value;
    for (int i = 0; i < getNumUCSChars(line); i++) {
    long key = (ustr[2*i]<<8)|ustr[2*i+1];
    if (sideDict->find(key, value)) {
        glCallList(sideList->getFirstIndex()+key);
    }
    else {
        glBegin(GL_QUADS);
        generateSideChar(str+2*i, callbackFunc);
        glEnd();

        SbVec2f t = getOutline(str+2*i)->getCharAdvance();
        glTranslatef(t[0], t[1], 0.0);
    }
    }
}
////////////////////////////////////////////////////////////////////////
//
// Description:
//    Convert MFString to UCS string, if necessary.
//
// Use: internal

void
SoOutlineFontCache::convertToUCS(uint32_t nodeid,
                   const SoMFString& strings)
//
////////////////////////////////////////////////////////////////////////
{
    if (nodeid == currentNodeId) return;
    currentNodeId = nodeid;

    //delete previously converted UCS string
    int i;
    for (i = 0; i< UCSStrings.getLength(); i++){
    delete [] (char*)UCSStrings[i];
    }
    UCSStrings.truncate(0);
    UCSNumChars.truncate(0);

    //make sure conversion code already set:
    if (conversionCode == NULL){
    conversionCode = iconv_open("UCS-2", "UTF-8");
    }

    if ( conversionCode == (iconv_t)-1 ){
#ifdef DEBUG
    SoDebugError::post("SoOutlineFontCache::convertToUCS",
        "Invalid UTF-8 to UCS-2 conversion");
#endif /*DEBUG*/
    return;
    }

    //for each line of text, allocate a sufficiently large buffer
    //An extra two bytes are allocated.
    for (i = 0; i< strings.getNum(); i++){
    UCSStrings[i] = new char[2*strings[i].getLength()+2];

    char* input = (char *)strings[i].getString();
    size_t inbytes = strings[i].getLength();
    size_t outbytes = 2*inbytes+2;
    char* output = (char*)UCSStrings[i];

    if ((iconv(conversionCode, &input, &inbytes, &output, &outbytes) == (size_t)-1)){
#ifdef DEBUG
        SoDebugError::post("SoOutlineFontCache::convertToUCS",
        "Error converting text to UCS-2");
#endif /*DEBUG*/

    }

    if (inbytes){
#ifdef DEBUG
        SoDebugError::post("SoOutlineFontCache::convertToUCS",
        "Incomplete conversion to UCS-2");
#endif /*DEBUG*/
        return;
    }

    UCSNumChars[i] = (void*)((2*strings[i].getLength()+2 - outbytes)>>1);

        int j;
        for (j = 0; j < getNumUCSChars(i); j++) {
            char* c = (char*)UCSStrings[i]+j*2;
            DGL_HTON_SHORT(SHORT(c), SHORT(c));
        }
    }

    return;

}
////////////////////////////////////////////////////////////////////////
//
// Description:
//   Copy info from the font library into a more convenient form.
//
// Use: internal

SoFontOutline::SoFontOutline(FLoutline *outline, float fontSize)
//
////////////////////////////////////////////////////////////////////////
{
    charAdvance = SbVec2f(outline->xadvance,
              outline->yadvance)*fontSize;
    numOutlines = outline->outlinecount;
    if (numOutlines != 0) {
    numVerts = new int[numOutlines];
    verts = new SbVec2f*[numOutlines];
    for (int i = 0; i < numOutlines; i++) {
        numVerts[i] = outline->vertexcount[i];
        if (numVerts[i] != 0) {
        verts[i] = new SbVec2f[numVerts[i]];
        for (int j = 0; j < numVerts[i]; j++) {
            verts[i][j] = SbVec2f(outline->vertex[i][j].x,
                      outline->vertex[i][j].y)*fontSize;
        }
        } else {
        verts[i] = NULL;
        }
    }
    } else {
    numVerts = NULL;
    verts = NULL;
    }
}

////////////////////////////////////////////////////////////////////////
//
// Description:
//   Destructor; free up outline storage
//
// Use: internal

SoFontOutline::~SoFontOutline()
//
////////////////////////////////////////////////////////////////////////
{
    for (int i = 0; i < numOutlines; i++) {
    if (numVerts[i] != 0)
        delete[] verts[i];
    }
    if (numOutlines != 0) {
    delete[] verts;
    delete[] numVerts;
    }
}

////////////////////////////////////////////////////////////////////////
//
// Description:
//   Get a do-nothing outline:
//
// Use: internal, static

SoFontOutline *
SoFontOutline::getNullOutline()
//
////////////////////////////////////////////////////////////////////////
{
    return new SoFontOutline;
}

////////////////////////////////////////////////////////////////////////
//
// Description:
//   Internal constructor used by getNullOutline
//
// Use: internal, static

SoFontOutline::SoFontOutline()
//
////////////////////////////////////////////////////////////////////////
{
    charAdvance = SbVec2f(0,0);
    numOutlines = 0;
    numVerts = NULL;
    verts = NULL;
}

////////////////////////////////////////////////////////////////////////
//
// Description:
//   Find the first and last points in the bevel-- that is where the
//   front and back of the character will be.
// Use:
//   internal

void
SoOutlineFontCache::getProfileBounds(float &firstZ, float &lastZ)
//
////////////////////////////////////////////////////////////////////////
{
    if (hasProfile()) {
    firstZ = -profileVerts[0][0];
    lastZ = -profileVerts[nProfileVerts-1][0];
    } else {
    firstZ = lastZ = 0;
    }
}

////////////////////////////////////////////////////////////////////////
//
// Description:
//   Get the 2D bounding box of the bevel.
//
//  Use:
//    internal

void
SoOutlineFontCache::getProfileBBox(SbBox2f &profileBox)
//
////////////////////////////////////////////////////////////////////////
{
    for (int i = 0; i < nProfileVerts; i++) {
    profileBox.extendBy(profileVerts[i]);
    }
}

////////////////////////////////////////////////////////////////////////
//
// Description:
//    Called by generateSide.  This generates the bevel triangles for
//    one character.
//
// Use: private

void
SoOutlineFontCache::generateSideChar(const char* c, SideCB callbackFunc)
//
////////////////////////////////////////////////////////////////////////
{
    if (!hasProfile()) return;

    // Get the outline of the character:
    SoFontOutline *outline = getOutline(c);

    for (int i = 0; i < outline->getNumOutlines(); i++) {
    // For each outline:

    int nOutline = outline->getNumVerts(i);

    SbVec2f *oVerts = new SbVec2f[nOutline];
    // Copy in verts so figureSegmentNorms can handle them..
    int j;
    for (j = 0; j < nOutline; j++) {
        oVerts[j] = outline->getVertex(i, j);
    }

    // First, figure out a set of normals for the outline:
    SbVec2f *oNorms = new SbVec2f[nOutline*2];
    figureSegmentNorms(oNorms, nOutline, oVerts, cosCreaseAngle, TRUE);

    // And appropriate texture coordinates:
    // Figure out T texture coordinates, which run along the
    // outline:
    float *tTexCoords = new float[nOutline+1];
    figureSegmentTexCoords(tTexCoords, nOutline, oVerts, TRUE);

    // Now, generate a set of triangles for each segment in the
    // outline.  A bevel of profiles is built at each point in the
    // outline; each profile must be flipped perpendicular to the
    // outline (x coordinate becomes -z), rotated to be the
    // average of the normals of the two adjoining segments at
    // that point, and translated to that point.  Triangles are
    // formed between consecutive bevels.
    // Normals are just taken from the 'pNorms' array, after being
    // rotated the appropriate amount.

    SbVec3f *bevel1 = new SbVec3f[nProfileVerts];
    SbVec3f *bevelN1 = new SbVec3f[(nProfileVerts-1)*2];
    SbVec3f *bevel2 = new SbVec3f[nProfileVerts];
    SbVec3f *bevelN2 = new SbVec3f[(nProfileVerts-1)*2];

    // fill out first bevel:
    fillBevel(bevel1, (int) nProfileVerts, profileVerts,
          outline->getVertex(i,0),
          oNorms[(nOutline-1)*2+1], oNorms[0*2]);

    SbVec3f *s1 = bevel1;
    SbVec3f *s2 = bevel2;

    for (j = 0; j < nOutline; j++) {
        // New normals are calculated for both ends of this
        // segment, since the normals may or may not be shared
        // with the previous segment.
        fillBevelN(bevelN1, (int)(nProfileVerts-1)*2, profileNorms,
               oNorms[j*2]);

        int j2 = (j+1)%nOutline;
        // fill out second bevel:
        fillBevel(s2, (int) nProfileVerts, profileVerts,
              outline->getVertex(i,j2),
              oNorms[j*2+1], oNorms[j2*2]);
        fillBevelN(bevelN2, (int)(nProfileVerts-1)*2, profileNorms,
               oNorms[j*2+1]);

        // And generate triangles between the two bevels:
        (*callbackFunc)((int) nProfileVerts, s1, bevelN1, s2, bevelN2,
                 sTexCoords, &tTexCoords[j]);

        // Swap bevel1/2 (avoids some recomputation)
        SbVec3f *t;
        t = s1; s1 = s2; s2 = t;
    }
    delete [] bevelN2;
    delete [] bevel2;
    delete [] bevelN1;
    delete [] bevel1;
    delete [] tTexCoords;
    delete [] oNorms;
    delete [] oVerts;
    }
}

////////////////////////////////////////////////////////////////////////
//
// Description:
//    Given a set of line segments, this figures out the normal at
//    each point in each segment.  It uses the creaseAngle passed in
//    to decide whether or not adjacent segments share normals.  The
//    isClosed flag is used to determine whether or not the first and
//    last points should be considered another segment.
//
//
// Use: private

void
SoOutlineFontCache::figureSegmentNorms(SbVec2f *norms, int nPoints,
                const SbVec2f *points,  float cosCreaseAngle,
                SbBool isClosed)
//
////////////////////////////////////////////////////////////////////////
{
    int nSegments;

    if (isClosed) nSegments = nPoints;
    else nSegments = nPoints-1;

    // First, we'll just make all the normals perpendicular to their
    // segments:
    int i;
    for (i = 0; i < nSegments; i++) {
    SbVec2f n;
    // This is 2D perpendicular, assuming profile is increasing in
    // X (which becomes 'decreasing in Z' when we actually use
    // it...) (note: if a profile isn't increasing in X, the
    // character will be inside-out, with the front face drawn
    // behind the back face, etc).
    SbVec2f v = points[(i+1)%nPoints] - points[i];
    n[0] = v[1];
    n[1] = -v[0];
    n.normalize();

    norms[i*2] = n;
    norms[i*2+1] = n;
    }
    // Now, figure out if the angle between any two segments is small
    // enough to average two of their normals.
    for (i = 0; i < (isClosed ? nSegments : nSegments-1); i++) {
    SbVec2f seg1 = points[(i+1)%nPoints] - points[i];
    seg1.normalize();
    SbVec2f seg2 = points[(i+2)%nPoints] - points[(i+1)%nPoints];
    seg2.normalize();

    float dp = seg2.dot(seg1);
    if (dp > cosCreaseAngle) {
        // Average the second normal for this segment, and the
        // first normal for the next segment:
        SbVec2f average = norms[i*2+1] + norms[((i+1)%nPoints)*2];
        average.normalize();
        norms[i*2+1] = average;
        norms[((i+1)%nPoints)*2] = average;
    }
    }
}

////////////////////////////////////////////////////////////////////////
//
// Description:
//    Given a set of line segments, this figures out the texture
//    coordinates for each vertex.  If the isClosed flag is TRUE,
//    an extra texture coordinate is calculated, AND the points are
//    traversed in reverse order.
//
// Use: private

void
SoOutlineFontCache::figureSegmentTexCoords(float *texCoords, int nPoints,
                const SbVec2f *points, SbBool isClosed)
//
////////////////////////////////////////////////////////////////////////
{
    float total = 0.0;

    int i;

    if (isClosed) {
    for (i = nPoints; i >= 0; i--) {
        texCoords[i] = total / getHeight();
        if (i > 0) {
        total += (points[i%nPoints] - points[i-1]).length();
        }
    }
    } else {
    for (int i = 0; i < nPoints; i++) {
        texCoords[i] = total / getHeight();
        if (i+1 < nPoints) {
        total += (points[i+1] - points[i]).length();
        }
    }
    }
}

////////////////////////////////////////////////////////////////////////
//
// Description:
//    Given a set of segments that make up a profile or bevel along
//    which we'll extrude the front face of the text, this routine
//    transforms the bevel from its default orientation (in the x-y
//    plane) to its correct position for a particular point on the
//    text outline, rotated and translated into position.  The
//    translation is the point on the outline, and the two normals
//    passed in are the normals for the segments adjoining that point.
//
// Use: private

void
SoOutlineFontCache::fillBevel(SbVec3f *result, int nPoints,
      const SbVec2f *points,
      const SbVec2f &translation,
      const SbVec2f &n1, const SbVec2f &n2)
//
////////////////////////////////////////////////////////////////////////
{
    // First, figure out a rotation for this bevel:
    SbVec2f n = n1+n2;
    n.normalize();

    // Now, for each point:
    for (int i = 0; i < nPoints; i++) {
    // This is really the 2D rotation formula,
    // x = x' cos(angle) - y' sin(angle)
    // y = x' sin(angle) + y' cos(angle)
    // Because of the geometry, cos(angle) is n[1] and sin(angle)
    // is -n[0], and x' is zero (the bevel always goes straight
    // back).
    result[i][0] = points[i][1] * n[0] + translation[0];
    result[i][1] = points[i][1] * n[1] + translation[1];
    result[i][2] = -points[i][0];
    }
}

////////////////////////////////////////////////////////////////////////
//
// Description:
//    Given a set of normals for a profile, this rotates the normals
//    from their default position (int the x-y plane) to the correct
//    orientation for a particular point on the texts outline.  The
//    normal passed in is the normal for one end of one of the
//    outline's segments.

void
SoOutlineFontCache::fillBevelN(SbVec3f *result, int nNorms,
      const SbVec2f *norms,
      const SbVec2f &n)
//
////////////////////////////////////////////////////////////////////////
{
    // Now, for each point:
    for (int i = 0; i < nNorms; i++) {
    // This is really the 2D rotation formula,
    // x = x' cos(angle) - y' sin(angle)
    // y = x' sin(angle) + y' cos(angle)
    // Because of the geometry, cos(angle) is n[1] and sin(angle)
    // is -n[0], and x' is zero (the bevel always goes straight
    // back).
    result[i][0] = norms[i][1] * n[0];
    result[i][1] = norms[i][1] * n[1];
    result[i][2] = -norms[i][0];
    }
}

////////////////////////////////////////////////////////////////////////
//
// Description:
//    Called by the GLU tesselator when there is an error
//
// Use: static, private
//
////////////////////////////////////////////////////////////////////////

#ifdef DEBUG
void
SoOutlineFontCache::errorCB(GLenum whichErr)
{
    SoDebugError::post("SoText3::errorCB", "%s", gluErrorString(whichErr));
    tesselationError = TRUE;
}
#else  /* DEBUG */
void
SoOutlineFontCache::errorCB(GLenum)
{
    tesselationError = TRUE;
}
#endif /* DEBUG */

////////////////////////////////////////////////////////////////////////
//
// Description:
//    function used to free up outline storage
//
// Use: static, private
//
////////////////////////////////////////////////////////////////////////

void SoOutlineFontCache::freeOutline(unsigned long, void* value)
{
    delete (FLoutline*)value;
}