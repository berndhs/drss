#ifndef HELPVIEW_H
#define HELPVIEW_H

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

#include "drssdebug.h"
#include <QWidget>
#include "ui_helpwin.h"

namespace deliberate {

class HelpView : public QWidget, public Ui_HelpView {

Q_OBJECT

public:

  HelpView();
  ~HelpView();
  
  void Show (QString urlString);
  
public slots:

  void DoClose ();
  void DoBack ();
  void DoForward ();
  
  void update ();
  
private:

  void ConnectButtons ();  

};



} // namespace

#endif

