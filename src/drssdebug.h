#ifndef DRSSDEBUG_H
#define DRSSDEBUG_H

//
//  Copyright (C) 2009 - Bernd H Stramm 
//
// This program is distributed under the terms of 
// the GNU General Public License version 3 
//
// This software is distributed in the hope that it will be useful, 
// but WITHOUT ANY WARRANTY; without even the implied warranty 
// of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. 
//


#include <qapplication.h>
#include <iostream>

#if DRSS_DEBUG
#include <QDebug>
#define QDEBUG qDebug
#endif

namespace deliberate {

void UseMyOwnMsgHandler ();

void DRSSMessageOutput (QtMsgType type, const char* msg);


}
#endif
