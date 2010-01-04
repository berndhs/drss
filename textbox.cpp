#include "textbox.h"


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

namespace deliberate {

  TextBox::TextBox (QWidget *parent)
          :theValue(""),
           wasOk(false)
  {
    setupUi(this);
    connect (buttonBox,SIGNAL(accepted()),this,SLOT(MarkGood()));
    connect (buttonBox,SIGNAL(rejected()),this,SLOT(MarkBad()));
  }

  TextBox::~TextBox ()
  {
  }

  void
  TextBox::SetLabel (QString lbl)
  {
    boxLabel->setText(lbl);
  }

  void
  TextBox::SetDefault (QString def)
  {
    textValueBox->setText(def);
  }
  
  void
  TextBox::SetTitle (QString t)
  {
    setWindowTitle(t);
  }

  QString
  TextBox::GetText ()
  {
    return textValueBox->text();
  }

  void
  TextBox::exec ()
  {
    this->QDialog::exec();
  }

} // namespace
