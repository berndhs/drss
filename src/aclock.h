
#ifndef ACLOCK_H
#define ACLOCK_H

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

 /*
 **
 ** Parts Copyright (C) 2004-2007 Trolltech ASA. All rights reserved.
 **
 */
 
#include <QtGui>
#include <QWidget>


namespace deliberate {


class ADClock : public QWidget
{
     Q_OBJECT

 public:
     ADClock(QWidget *parent = 0);
   
     int hours( int h = 12);
     void SetDigital (QLCDNumber *pD) { pDigital = pD; }
     void SetAnalog  (QLabel *pA) { pAnalog = pA;}
     void paintEvent(QPaintEvent *event);
     void Start ();
     
 public slots:
 

 protected:

 private:
     
     int mHours;
     QLCDNumber * pDigital;
     QWidget     * pAnalog;
     QObject     * myself;
 };

}
 #endif
