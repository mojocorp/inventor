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
 * Copyright (C) 1990-93   Silicon Graphics, Inc.
 *
 _______________________________________________________________________
 ______________  S I L I C O N   G R A P H I C S   I N C .  ____________
 |
 |
 |   $Revision: 1.1 $
 |
 |   Description:
 |	This is the SoLineHighlightRenderAction class, used to highlight
 |   the selection by drawing the selected shapes in wireframe.
 |
 |   Classes	: SoLineHighlightRenderAction
 |
 |   Author(s)	: David Mott
 |
 ______________  S I L I C O N   G R A P H I C S   I N C .  ____________
 _______________________________________________________________________
 */

#pragma once

#include <Inventor/actions/SoGLRenderAction.h>

class SbColor;
class SoSeparator;
class SoLightModel;
class SoBaseColor;
class SoDrawStyle;
class SoTexture2;

/// Selection highlight style.
/// \ingroup Interaction
/// <tt>SoLineHighlightRenderAction</tt> is a render action which
/// renders the specified scene graph, then renders each selected object
/// again in wireframe.
/// Selected objects are specified by the first <tt>SoSelection</tt>
/// node in the scene to which this action is applied.
/// If there is no renderable geometry in a selected object,
/// no highlight is rendered for that object.
/// A highlight render action can be passed to
/// the <b>setGLRenderAction()</b> method of <tt>SoXtRenderArea</tt>
/// to have an affect on scene graphs.
/// \sa SoBoxHighlightRenderAction, SoGLRenderAction, SoSelection,
/// SoXtRenderArea, SoDrawStyle, SoInteraction
class INVENTOR_API SoLineHighlightRenderAction : public SoGLRenderAction {
    SO_ACTION_HEADER(SoLineHighlightRenderAction);

  public:
    /// Constructor which takes no parameters. This sets up a dummy
    /// viewport region. If passed to a render area, the render area
    /// will correctly set the region. Otherwise, setViewportRegion()
    /// should be called before applying this action.
    SoLineHighlightRenderAction();

    /// Constructor which takes the normal SoGLRenderAction parameters.
    SoLineHighlightRenderAction(const SbViewportRegion &viewportRegion);

    /// Destructor
    virtual ~SoLineHighlightRenderAction();

    /// This renders the passed scene graph, and also renders each selected
    /// object in wireframe, as specified by the first SoSelection node found
    /// in the scene graph.
    virtual void apply(SoNode *node);

    /// Applies action to the graph defined by a path.
    /// These simply invoke the parent class apply() methods.
    /// These do NOT highlight the path, whether selected or not.
    virtual void apply(SoPath *path);
    /// Applies action to the graph defined by a path list.
    /// These simply invoke the parent class apply() methods.
    /// These do NOT highlight the path, whether selected or not.
    virtual void apply(const SoPathList &pathList, SbBool obeysRules = FALSE);

    /// This provides a convenient mechansim for turning highlights off
    /// or back on. When FALSE is passed, subsequent calls to
    /// apply()
    /// SoLineHLRenderActApply()
    /// render the scene graph without rendering highlights.
    /// The application is responsible for forcing a redraw of the
    /// scene after changing this state. The default visibility is on.
    void setVisible(SbBool b) { hlVisible = b; }

    /// Returns whether highlights will be rendered or not.
    SbBool isVisible() const { return hlVisible; }

    /// Set the color of the highlight.
    /// The application is responsible for forcing a redraw of the
    /// scene to see the affects of this change.
    void setColor(const SbColor &c);

    /// Get the color of the highlight. Default is red \e (1,0,0).
    const SbColor &getColor();

    /// Set the line pattern of the highlight.
    /// The pattern of bits in the passed variable specifies the pattern of the
    /// line. See SoDrawStyle for more information. The application is
    /// responsible for forcing a redraw of the scene to see the affects of this
    /// change.
    void setLinePattern(unsigned short pattern);

    /// Get the line pattern of the highlight. Default is solid, 0xffff.
    /// The pattern of bits in the passed variable specifies the pattern of the
    /// line. See SoDrawStyle for more information.
    unsigned short getLinePattern();

    /// Set the line width of the highlight.
    /// The application is responsible for forcing a redraw of the
    /// scene to see the affects of this change.
    void setLineWidth(float width);

    /// Set and get the line width of the highlight. Default is 3.
    float getLineWidth();

    SoINTERNAL
  public:
    static void initClass();
    static void finishClass();

  protected:
    // Nodes which comprise the local highlight graph
    SoSeparator * localRoot;
    SoLightModel *lightModel;
    SoBaseColor * baseColor;
    SoDrawStyle * drawStyle;
    SoTexture2 *  texture;

    SbBool hlVisible;

  private:
    void constructorCommon();
};

