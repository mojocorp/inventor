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
 |   $Revision: 1.2 $
 |
 |   Classes:
 |	SoDetailList
 |
 |   Author(s)		: Nick Thompson
 |
 ______________  S I L I C O N   G R A P H I C S   I N C .  ____________
 _______________________________________________________________________
 */

#include <Inventor/lists/SoDetailList.h>
#include <Inventor/details/SoDetail.h>

////////////////////////////////////////////////////////////////////////
//
// Description:
//    Copy constructor.
//
// Use: public

SoDetailList::SoDetailList(const SoDetailList &l) : SbPList(l) {
    // We need to copy the details, since we delete them when we
    // truncate the list
    for (int i = 0; i < getLength(); i++)
        (*(const SbPList *)this)[i] = (void *)(*this)[i]->copy();
}

////////////////////////////////////////////////////////////////////////
//
// Description:
//    Delete all details on the list from one with given index on,
//    inclusive.
//
// Use: public

void
SoDetailList::truncate(int start)
//
////////////////////////////////////////////////////////////////////////
{
    int i;

    for (i = start; i < getLength(); i++) {
        if ((*this)[i]) {
            delete (*this)[i];
        }
    }

    SbPList::truncate(start);
}

////////////////////////////////////////////////////////////////////////
//
// Description:
//    Copies list, making copy instances of each detail in list.
//
// Use: public

void
SoDetailList::copy(const SoDetailList &l)
//
////////////////////////////////////////////////////////////////////////
{
    truncate(0);

    int num = l.getLength();
    for (int i = 0; i < num; i++) {
        SoDetail *detail = l[i];
        if (detail)
            append(detail->copy());
    }
}

////////////////////////////////////////////////////////////////////////
//
// Description:
//    Sets an element of a list, deleting old entry.
//
// Use: public

void
SoDetailList::set(int i, SoDetail *detail)
//
////////////////////////////////////////////////////////////////////////
{
    if ((*this)[i] != NULL)
        delete (*this)[i];

    (*(const SbPList *)this)[i] = (void *)detail;
}