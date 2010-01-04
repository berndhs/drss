#include "aclock.h"
 
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
#include "drssdebug.h"


#include <stdio.h>
#include <iostream>

using namespace std;


namespace deliberate {



ADClock::ADClock(QWidget *parent)
 : QWidget(parent),
   mHours(12),
   pDigital(0),
   pAnalog(this)
{
}

 
 
void
ADClock::Start()
{
  QTimer *timer = new QTimer(this);
  connect(timer, SIGNAL(timeout()), this, SLOT(update()));
  timer->start(250);
}
 


void ADClock::paintEvent(QPaintEvent *)
{
  int hour, min, sec;
   
  QTime time = QTime::currentTime();
  hour = time.hour();
  min  = time.minute();
  sec = time.second();
  double dsec =  sec + time.msec()/1000.0;
  if (pAnalog) {
    static const QPoint hourHand[4] = {
        QPoint(7, 2),
        QPoint(0,10),
        QPoint(-7, 2),
        QPoint(0, -40)
    };
    static const QPoint minuteHand[4] = {
        QPoint(5, 2),
        QPoint(0,12),
        QPoint(-5, 2),
        QPoint(0, -70)
    };
    static const QPoint secondHand[4] = {
      QPoint(4, 0),
      QPoint(0, 15),
      QPoint(-4,0),
      QPoint(0,-80)
    };
    static const QPoint middleLine[4] = {
      QPoint(1,-86),
      QPoint(0,-83),
      QPoint(-1,-86),
      QPoint(0,-90)
    };

    QColor hourColor(127, 0, 127);
    QColor minuteColor(0, 127, 127, 191);
    QColor secondColor(218, 165, 32, 120);
    QColor blackColor (0 ,0 ,0 ,255);  

    int side = qMin(width(), height());
    
    double    hours_per_turn(mHours);
    double    minutes_per_turn(60.0);
    double    secs_per_turn(60.0);
    double deg_per_hour = 360.0/hours_per_turn;
    double deg_per_min  = 360.0/minutes_per_turn;
    double deg_per_sec  = 360.0/secs_per_turn;

    QPainter painter(pAnalog);
    painter.setRenderHint(QPainter::Antialiasing);
    painter.translate(width() / 2, height() / 2);
    painter.scale(side / 200.0, side / 200.0);

    painter.setPen(Qt::NoPen);
    painter.setBrush(hourColor);

    painter.save();
    painter.rotate(deg_per_hour * ((hour + min / minutes_per_turn)));
    painter.drawConvexPolygon(hourHand, 4);
    painter.restore();

    painter.setPen(hourColor);
    
    for (int i = 0; i < hours_per_turn; ++i) {
        painter.drawLine(88, 0, 96, 0);
        painter.rotate(deg_per_hour);
    }

    painter.setPen(Qt::NoPen);
    painter.setBrush(minuteColor);

    painter.save();
    painter.rotate(deg_per_min * min);  // jumpy minute hand
    painter.drawConvexPolygon(minuteHand, 4);
    painter.restore();

    painter.setPen(Qt::NoPen);
    painter.setBrush(secondColor);
    painter.save();
    double deg = deg_per_sec * dsec;
    painter.rotate(deg);
    painter.drawConvexPolygon(secondHand, 4);
    painter.setBrush(blackColor);
    painter.drawConvexPolygon(middleLine, 4);
    painter.restore();
  }
  if (pDigital) {
    char old_lcdtime[64];
    sprintf (old_lcdtime,"%02d:%02d:%02d",hour,min,sec);
    QString lcdtime(old_lcdtime);
    pDigital->display (lcdtime);
  }

}


}

