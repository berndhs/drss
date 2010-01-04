#ifndef NEWSBOX_H
#define NEWSBOX_H
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
#include <QObject>
#include <QWidget>
#include <QTimer>
#include <QMainWindow>
#include <QResizeEvent>

#include "ui_drssmain.h"
#include "aclock.h"
#include "drssconfig.h"

namespace deliberate {


class NewsBox : public QMainWindow, public Ui_DRSSMain {

Q_OBJECT

public:

   NewsBox ();
   
   void SetTitle (QString t, QString auth = "");
   void SetDescr (QString d);
   void SetNick  (QString n);
   void SetTreeModel (QAbstractItemModel *pm);
   
   void Clear ();
   
   void ShowLoading (bool loading = true);
   void ShowDigital (bool show = true);
   void ShowAnalog  (bool show = true);
   
   void MarkStoryBold (const int row, const bool isbold);
   
   void Reconfigure (DRSSConfig & config);
   
   void ShowBusy ();
   void SetBusy (int b) { busy = b; }
   
public slots:

   void quit();
   void update ();
   
signals:

  void ImDone();
   
private:

  QTimer               updateT;
  bool                 digital;
  QPixmap              loadingPic;
  QPixmap              notloadingPic;
  int                  busy;
  
  static QString       failedMessage;

};

}


#endif
