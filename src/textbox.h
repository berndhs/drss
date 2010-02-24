#ifndef TEXTBOX_H
#define TEXTBOX_H
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

#include "ui_getstring.h"

namespace deliberate {

  class TextBox : public QDialog, public Ui_getStringDialog {

Q_OBJECT

  public:
 
    TextBox (QWidget *parent);
    ~TextBox ();

    void SetLabel (QString lbl);
    void SetDefault (QString def);
    void SetTitle (QString t);
    QString GetText ();
    bool WasGood () { return wasOk; }

    void exec();
    
  public slots:
  
    void MarkGood() { wasOk = true; }
    void MarkBad () { wasOk = false; }

  private:

    QString theValue;
    bool    wasOk;

};

}

#endif
