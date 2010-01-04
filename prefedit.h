#ifndef PREFEDIT_H
#define PREFEDIT_H

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

#include <QDialog>
#include "drssconfig.h"
#include "ui_prefedit.h"


namespace deliberate {

class PrefEdit : public QDialog, public Ui_PrefEditForm {

Q_OBJECT

public:

  PrefEdit ();
  
  void Edit (DRSSConfig * config);

public slots:

  void DoOk ();
  void DoCancel ();
  void ResetToDefaults ();
  
private:

  DRSSConfig  *pOrigConfig;
  DRSSConfig  tmpConfig;
  bool        running;
  
  static Qt::CheckState State (bool chkd) 
     { return (chkd ? Qt::Checked : Qt::Unchecked); }
  static bool IsChecked (Qt::CheckState chkst) 
     { return chkst == Qt::Checked; }
     
  void Reset();

};

}

#endif
