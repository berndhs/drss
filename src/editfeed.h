#ifndef EDITFEED_H
#define EDITFEED_H

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
#include <QDomElement>
#include <QNetworkReply>
#include <QNetworkAccessManager>
#include <QStandardItem>
#include <QUrl>
#include <map>
#include "ui_feededit.h"
#include "ui_probelinks.h"
#include "rssfeed.h"

namespace deliberate {

class EditFeed : public QDialog, public Ui_FeedEdit {

Q_OBJECT

public:

   enum Result {
       Is_None      = 0,
       Is_Changed   = 1,
       Is_Delete    = 2,
       Is_New       = 4,
       Is_Cancelled = 8,
   };
 
   EditFeed ();
   
   void Init (QNetworkAccessManager * pQMgr);
   
   void Edit (QStandardItem *pItem);
   void EditNew (const QString newxml);
                 
   
public slots:

  void Cancel();
  void Save();
  void Delete();
  void Done(bool erase = false);
  void CatchProbe (QNetworkReply * reply);
  void ProbeAgain ();
   
signals:

  void FinishedEditing (QStandardItem *f, const int result);
  
private:

  void ConnectButtons();
  void DisableButtons();
  void EnableButtons();
  
  void EditFromQueue ();
  void ProbeChannel (const QString link);
              
  void ProbeHtml (QDomElement & htmlEl, QUrl topUrl);
  void ProbeHtmlHead (QDomElement & head, QUrl topUrl);
  void PopulateFromRssDoc (QDomElement & el);
  void PopulateFromAtomDoc (QDomElement & el);
  bool ParseAtomLinkElem   (QDomElement & el, QString & xml, QString & web);
  bool ParseAtomAuthorElem (QDomElement & el, QString & name);
  
  QStandardItem * pReturnItem;
  RssFeed   * pFeed;
  RssFeed   * newFeed;
  QNetworkAccessManager * mQMgr;
  
  std::map <QNetworkReply*, bool> seenReply;
  
  QStringList          newXmlList;
  
  bool                 isgood;
  
  
  QPixmap              loadingPic;
  QPixmap              notloadingPic;
  
  Ui_LinkProbe            htmlLinkProbe;
};

}

#endif
