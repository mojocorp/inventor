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
 |   $Revision: 1.1 $
 |
 |   Classes:
 |      SoFont
 |
 |   Author(s)          : Thad Beier
 |
 ______________  S I L I C O N   G R A P H I C S   I N C .  ____________
 _______________________________________________________________________
 */

#include <Inventor/actions/SoCallbackAction.h>
#include <Inventor/actions/SoGLRenderAction.h>
#include <Inventor/actions/SoGetBoundingBoxAction.h>
#include <Inventor/actions/SoPickAction.h>
#include <Inventor/elements/SoFontNameElement.h>
#include <Inventor/elements/SoFontSizeElement.h>
#include <Inventor/elements/SoOverrideElement.h>
#include <Inventor/nodes/SoFont.h>

static std::map<SbName, SbString> s_font_path_map;

SO_NODE_SOURCE(SoFont);

////////////////////////////////////////////////////////////////////////
//
// Description:
//    This initializes the SoFont class.
//
// Use: internal

void
SoFont::initClass()
//
////////////////////////////////////////////////////////////////////////
{
    SO__NODE_INIT_CLASS(SoFont, "Font", SoNode);

    // Enable elements for appropriate actions:
    SO_ENABLE(SoGLRenderAction, SoFontNameElement);
    SO_ENABLE(SoGLRenderAction, SoFontSizeElement);
    SO_ENABLE(SoPickAction, SoFontNameElement);
    SO_ENABLE(SoPickAction, SoFontSizeElement);
    SO_ENABLE(SoGetBoundingBoxAction, SoFontNameElement);
    SO_ENABLE(SoGetBoundingBoxAction, SoFontSizeElement);
    SO_ENABLE(SoCallbackAction, SoFontNameElement);
    SO_ENABLE(SoCallbackAction, SoFontSizeElement);

    // In PDF, the following 14 fonts are defined:
    // Courier (Regular, Oblique, Bold, Bold Oblique)
    // Helvetica (Regular, Oblique, Bold, Bold Oblique)
    // Symbol
    // Times (Roman, Italic, Bold, Bold Oblique)
    // ITC Zapf Dingbats

#if defined(SB_OS_WIN)
    s_font_path_map["Courier"] = "c:\\Windows\\Fonts\\cour.ttf";
    s_font_path_map["Courier-Regular"] = "c:\\Windows\\Fonts\\cour.ttf";
    s_font_path_map["Courier-Bold"] = "c:\\Windows\\Fonts\\courbd.ttf";
    s_font_path_map["Courier-Oblique"] = "c:\\Windows\\Fonts\\couri.ttf";
    s_font_path_map["Courier-Bold Oblique"] = "c:\\Windows\\Fonts\\courbi.ttf";

    s_font_path_map["Helvetica"] = "c:\\Windows\\Fonts\\arial.ttf";
    s_font_path_map["Helvetica-Regular"] = "c:\\Windows\\Fonts\\arial.ttf";
    s_font_path_map["Helvetica-Bold"] = "c:\\Windows\\Fonts\\arialbd.ttf";
    s_font_path_map["Helvetica-Oblique"] = "c:\\Windows\\Fonts\\ariali.ttf";
    s_font_path_map["Helvetica-Bold Oblique"] =
        "c:\\Windows\\Fonts\\arialbi.ttf";

    s_font_path_map["Symbol"] = "c:\\Windows\\Fonts\\symbol.ttf";

    s_font_path_map["Times-Roman"] = "c:\\Windows\\Fonts\\times.ttf";
    s_font_path_map["Times-Bold"] = "c:\\Windows\\Fonts\\timesbd.ttf";
    s_font_path_map["Times-Oblique"] = "c:\\Windows\\Fonts\\timesi.ttf";
    s_font_path_map["Times-Bold Oblique"] = "c:\\Windows\\Fonts\\timesbi.ttf";

    s_font_path_map["Arial Unicode"] = "c:\\Windows\\Fonts\\ARIALUNI.TTF";

#elif defined(SB_OS_MACOS)
    s_font_path_map["Courier"] = "/Library/Fonts/Courier New.ttf";
    s_font_path_map["Courier-Regular"] = "/Library/Fonts/Courier New.ttf";
    s_font_path_map["Courier-Oblique"] =
        "/Library/Fonts/Courier New Italic.ttf";
    s_font_path_map["Courier-Bold"] = "/Library/Fonts/Courier New Bold.ttf";
    s_font_path_map["Courier-Bold Oblique"] =
        "/Library/Fonts/Courier New Bold Italic.ttf";

    s_font_path_map["Helvetica"] = "/Library/Fonts/Arial.ttf";
    s_font_path_map["Helvetica-Regular"] = "/Library/Fonts/Arial.ttf";
    s_font_path_map["Helvetica-Oblique"] = "/Library/Fonts/Arial Italic.ttf";
    s_font_path_map["Helvetica-Bold"] = "/Library/Fonts/Arial Bold.ttf";
    s_font_path_map["Helvetica-Bold Oblique"] =
        "/Library/Fonts/Arial Bold Italic.ttf";

    s_font_path_map["Symbol"] = "/System/Library/Fonts/Symbol.ttf";

    s_font_path_map["Times-Roman"] = "/Library/Fonts/Times New Roman.ttf";
    s_font_path_map["Times-Oblique"] =
        "/Library/Fonts/Times New Roman Italic.ttf";
    s_font_path_map["Times-Bold"] = "/Library/Fonts/Times New Roman Bold.ttf";
    s_font_path_map["Times-Bold Oblique"] =
        "/Library/Fonts/Times New Roman Bold Italic.ttf";

    s_font_path_map["Arial Unicode"] = "/Library/Fonts/Arial Unicode.ttf";

#else
    s_font_path_map["Courier"] = "/usr/share/fonts/X11/Type1/c0419bt_.pfb";
    s_font_path_map["Courier-Regular"] =
        "/usr/share/fonts/X11/Type1/c0419bt_.pfb";
    s_font_path_map["Courier-Oblique"] =
        "/usr/share/fonts/X11/Type1/c0419bt_.pfb";
    s_font_path_map["Courier-Bold"] = "/usr/share/fonts/X11/Type1/c0583bt_.pfb";
    s_font_path_map["Courier-Bold Oblique"] =
        "/usr/share/fonts/X11/Type1/c0419bt_.pfb";

    s_font_path_map["Helvetica"] =
        "/usr/share/fonts/truetype/msttcorefonts/Arial.ttf";
    s_font_path_map["Helvetica-Regular"] =
        "/usr/share/fonts/truetype/msttcorefonts/Arial.ttf";
    s_font_path_map["Helvetica-Oblique"] =
        "/usr/share/fonts/truetype/msttcorefonts/Arial_Italic.ttf";
    s_font_path_map["Helvetica-Bold"] =
        "/usr/share/fonts/truetype/msttcorefonts/Arial_Bold.ttf";
    s_font_path_map["Helvetica-Bold Oblique"] =
        "/usr/share/fonts/truetype/msttcorefonts/Arial_Bold_Italic.ttf";

    s_font_path_map["Symbol"] =
        "/usr/share/fonts/truetype/?ttf-symbol-replacement/"
        "?symbol-replacement.ttf/?symbol.ttf";

    s_font_path_map["Times-Roman"] =
        "/usr/share/fonts/truetype/msttcorefonts/Times_New_Roman.ttf";
    s_font_path_map["Times-Oblique"] =
        "/usr/share/fonts/truetype/msttcorefonts/Times_New_Roman_Italic.ttf";
    s_font_path_map["Times-Bold"] =
        "/usr/share/fonts/truetype/msttcorefonts/Times_New_Roman_Bold.ttf";
    s_font_path_map["Times-Bold Oblique"] =
        "/usr/share/fonts/truetype/msttcorefonts/"
        "Times_New_Roman_Bold_Italic.ttf";

    s_font_path_map["Arial Unicode"] =
        "/usr/share/fonts/truetype/ttf-dejavu/DejaVuSans.ttf";
#endif
}

////////////////////////////////////////////////////////////////////////
//
// Description:
//    Constructor
//
// Use: public

SoFont::SoFont()
//
////////////////////////////////////////////////////////////////////////
{
    SO_NODE_CONSTRUCTOR(SoFont);
    SO_NODE_ADD_FIELD(name, (SoFontNameElement::getDefault()));
    SO_NODE_ADD_FIELD(size, (SoFontSizeElement::getDefault()));
    isBuiltIn = TRUE;
}

////////////////////////////////////////////////////////////////////////
//
// Description:
//    Destructor
//
// Use: private

SoFont::~SoFont()
//
////////////////////////////////////////////////////////////////////////
{}

////////////////////////////////////////////////////////////////////////
//
// Description:
//    Clean-up.
// Use: public, static

void
SoFont::finishClass()
//
////////////////////////////////////////////////////////////////////////
{
    s_font_path_map.clear();
}

////////////////////////////////////////////////////////////////////////
//
// Description:
//    Handles actions
//
// Use: extender

void
SoFont::doAction(SoAction *action)
//
////////////////////////////////////////////////////////////////////////
{
    SoState *state = action->getState();

    if (!name.isIgnored() && !SoOverrideElement::getFontNameOverride(state)) {
        if (isOverride()) {
            SoOverrideElement::setFontNameOverride(state, this, TRUE);
        }
        SoFontNameElement::set(state, this, name.getValue().getString());
    }

    if (!size.isIgnored() && !SoOverrideElement::getFontSizeOverride(state)) {
        if (isOverride()) {
            SoOverrideElement::setFontSizeOverride(state, this, TRUE);
        }
        SoFontSizeElement::set(state, size.getValue());
    }
}

////////////////////////////////////////////////////////////////////////
//
// Description:
//    Does callback action thing.
//
// Use: extender

void
SoFont::callback(SoCallbackAction *action)
//
////////////////////////////////////////////////////////////////////////
{
    SoFont::doAction(action);
}

////////////////////////////////////////////////////////////////////////
//
// Description:
//    Does GL render action.
//
// Use: extender

void
SoFont::GLRender(SoGLRenderAction *action)
//
////////////////////////////////////////////////////////////////////////
{
    SoFont::doAction(action);
}

////////////////////////////////////////////////////////////////////////
//
// Description:
//    Does pick action...
//
// Use: extender

void
SoFont::pick(SoPickAction *action)
//
////////////////////////////////////////////////////////////////////////
{
    SoFont::doAction(action);
}

////////////////////////////////////////////////////////////////////////
//
// Description:
//    Does get bounding box action...
//
// Use: extender

void
SoFont::getBoundingBox(SoGetBoundingBoxAction *action)
//
////////////////////////////////////////////////////////////////////////
{
    SoFont::doAction(action);
}

////////////////////////////////////////////////////////////////////////
//
// Description:
//    Returns the font path
//
// Use: public
SbString
SoFont::getFontFileName(const SbName &fontName)
//
////////////////////////////////////////////////////////////////////////
{
    if (!s_font_path_map[fontName].isEmpty()) {
        return s_font_path_map[fontName];
    }
#ifdef DEBUG
    SoDebugError::post("SoFontNameElement::getFontFileName",
                       "Couldn't find font %s, replacing with %s",
                       fontName.getString(),
                       SoFontNameElement::getDefault().getString());
#endif
    return SoFontNameElement::getDefault().getString();
}
////////////////////////////////////////////////////////////////////////
//
// Description:
//    Register a font
//
// Use: public
void
SoFont::addFontFileName(const SbName &fontName, const SbString &fontPath)
//
////////////////////////////////////////////////////////////////////////
{
    s_font_path_map[fontName] = fontPath;
}
