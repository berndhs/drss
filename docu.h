#ifndef DOCU_H
#define DOCU_H
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
#include <QtXml/QDomDocument>
#include <QtNetwork/QNetworkAccessManager>
#include <QtNetwork/QNetworkReply>
#include <QtCore/QString>
#include <QtGui/QApplication>
#include <QStandardItemModel>
#include <QAbstractItemModel>
#include <QModelIndex>
#include <QTimer>

#include <deque>
#include "newsbox.h"
#include "feedlist.h"
#include "editfeed.h"
#include "rssitem.h"
#include "version.h"
#include "drssconfig.h"
#include "textbox.h"
#include "storylist.h"
#include "prefedit.h"
#include "helpview.h"

namespace deliberate {



class Docu : public QObject {

Q_OBJECT

public:

  Docu (QApplication * papp, DRSSConfig & conf);
  void Show();
  void paintEvent (QPaintEvent *ev);
   
public slots:

  void ReceiveStoryList ();
  void ReceiveChanged (StoryList * sl);
  void EditDone (QStandardItem *pRF, const int result);
  void Shutdown ();
  void DoNewsLink();
  void DoNextStory();
  void DoPrevStory();
  void DoEditFeed();
  void DoNewFeed();
  void DoNewFolder();
  void DoMarkFeedRead ();
  void ShowVersion();
  void ShowHelp ();
  void ShowLicense ();
  void MailStory ();
  void SaveFeeds (bool useConfig = true);
  void SaveFeedsAs () { SaveFeeds (false); }
  void IsInteresting (const QModelIndex & index);
  void ClickedFeed (const QModelIndex & index);
  void DoThisStory (QListWidgetItem * item);
  void CheckPressed (const QModelIndex & index);
  void DoEditPrefs ();
  void ImportOPML ();
  void ImportDRSS ();
  void Update ();
  void ClearPeriodic ();
  void AskChanged (RssFeed * feed);
   
   
private:

  QApplication         *pApp;
  
  QTimer               updateT;
  QTimer               slowT;
  QTimer               saveT;
  DRSSConfig           config;
  PrefEdit             prefEditor;
  FeedList             feedList;
   
  
  StoryList             storyList;
  StoryList             newStoryList;
  StoryList::iterator   storyIt;
  
  map<StoryList*,RssFeed*>     checkForChanges;
  map<RssFeed*,StoryList*>     checking;

  QNetworkAccessManager * mQMgr;
  bool                    openingFeed;
  RssItem               * curStory;
  RssFeed               * curFeed;
  RssFeed               * oldFeed;
  
  QStandardItem         * interestingFeed;
  QStandardItem         * newFeed;
  EditFeed     editor;
  
  NewsBox   nb;
  bool      nbopen;
  
  HelpView    help;
  

  void ProbeChannel (const QString link, RssFeed &rl);
  void ConnectButtons ();
  void DoStory();
  bool Delete (QStandardItem *pItem);
  void LoadFeedModel ();
  bool OpenFeed (QStandardItem *pItem);
  void DitchFeed ();
  void MakeStoryModel ();
  void ClearStoryModel ();
  void AskChangedDeep (QStandardItem * pItem);
  StoryList::iterator FindStory (RssItem *st);
  
};

}
#endif
