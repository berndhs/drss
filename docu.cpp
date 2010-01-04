#include "docu.h"
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
#include <QtNetwork/QNetworkRequest>
#include <QtCore/QUrl>
#include <QtCore/QString>
#include "drssdebug.h"
#include <QDesktopServices>
#include <QFile>
#include <QFileDialog>
#include <iostream>
#include "newsbox.h"
#include "aclock.h"
#include "textbox.h"
#include "drssdebug.h"

using namespace std;
namespace deliberate {


Docu::Docu (QApplication *papp, DRSSConfig & conf)
:pApp(papp),
 config(conf)
{
  nbopen = false;
  interestingFeed = 0;
  newFeed = 0;
  oldFeed = 0;
  curStory = 0;
  curFeed = 0;
  storyList.Clear ();
  help.hide();
  
  ConnectButtons();
  nb.ShowLoading();
  QFile feedFile(config.FeedListFile());
  feedFile.open(QFile::ReadOnly);
  feedList.Init();
  feedList.Load(&feedFile);
  feedFile.close();
  nb.SetTreeModel(&feedList);
  
  mQMgr = new QNetworkAccessManager;
  editor.Init (mQMgr);
  nb.ShowLoading(false);
  openingFeed = false;
  
  connect (&updateT, SIGNAL(timeout()),
            this,SLOT( Update ()));
  connect (&slowT,SIGNAL(timeout()),
            this,SLOT( ClearPeriodic ()));
  connect (&saveT, SIGNAL(timeout()),
            this, SLOT( SaveFeeds ()));
            
  updateT.setParent (this);
  updateT.start (2100);
  slowT.setParent (this);
  slowT.start (10000);
  saveT.setParent (this);
  saveT.start (5*60*1000); // 5 minutes
  
  nb.Reconfigure (config);
}


void
Docu::ConnectButtons ()
{
  connect (nb.actionExit, SIGNAL(triggered()),this,SLOT ( Shutdown ()));
  connect (nb.actionExit1, SIGNAL(triggered()),this,SLOT ( Shutdown ()));
  connect (nb.exitButton, SIGNAL(clicked()),this,SLOT ( Shutdown ()));
  connect (nb.importOPML, SIGNAL(triggered()),this,SLOT ( ImportOPML ()));
  connect (nb.importDRSS, SIGNAL( triggered()),this,SLOT ( ImportDRSS ()));
  connect (nb.linkButton, SIGNAL (clicked()),this,SLOT ( DoNewsLink ()));
  connect (nb.editFeed, SIGNAL(triggered()),
              this, SLOT ( DoEditFeed ()));
  connect (nb.editNewFeed, SIGNAL(triggered()),
              this, SLOT ( DoNewFeed ()));
  connect (nb.editNewFolder, SIGNAL(triggered()),
              this, SLOT ( DoNewFolder ()));
  connect (nb.actionFeedRead, SIGNAL (triggered()),
              this, SLOT ( DoMarkFeedRead ()));
  connect (nb.nextStoryButton, SIGNAL(clicked()), 
              this, SLOT ( DoNextStory ()));
  connect (nb.backStoryButton, SIGNAL(clicked()), 
              this, SLOT ( DoPrevStory ()));
  connect (nb.mailButton, SIGNAL (clicked()),
              this, SLOT ( MailStory ()));
  connect (nb.showAbout_DRSS, SIGNAL(triggered()),
              this, SLOT ( ShowVersion ()));
  connect (nb.actionSave_Feed_List, SIGNAL(triggered()),
              this, SLOT ( SaveFeeds ()));
  connect (nb.actionSaveAs, SIGNAL(triggered()),
              this, SLOT ( SaveFeedsAs ()));
  connect (nb.editPrefs, SIGNAL (triggered()),
              this, SLOT ( DoEditPrefs ()));
  connect (nb.actionNext, SIGNAL (triggered()),
              this, SLOT (DoNextStory ()));
  connect (nb.actionPrevious, SIGNAL (triggered()),
              this, SLOT (DoPrevStory ()));
  connect (nb.actionMail, SIGNAL (triggered()),
              this, SLOT ( MailStory ()));
  connect (nb.feedTree,SIGNAL(doubleClicked(const QModelIndex &)),
              this, SLOT ( ClickedFeed (const QModelIndex &)));
  connect (nb.feedTree,SIGNAL(pressed(const QModelIndex &)),
              this, SLOT ( CheckPressed (const QModelIndex &)));
  connect (nb.feedTree, SIGNAL(clicked(const QModelIndex &)),
              this, SLOT ( IsInteresting (const QModelIndex &)));
  connect (nb.storyView, SIGNAL(itemClicked(QListWidgetItem *)),
              this, SLOT ( DoThisStory (QListWidgetItem *)));
  connect (nb.actionUser_Manual, SIGNAL (triggered()),
              this, SLOT ( ShowHelp ()));   
  connect (nb.actionLicense, SIGNAL (triggered()),
              this, SLOT ( ShowLicense ()));           
              
  connect (&editor, SIGNAL( FinishedEditing (QStandardItem *, const int)),
              this, SLOT( EditDone (QStandardItem *, const int)));

  connect (&feedList,SIGNAL( QueryWeb (RssFeed*)),
                this,SLOT( AskChanged (RssFeed*)));
             
  connect (&nb,SIGNAL( ImDone ()),this,SLOT( Shutdown ()));
}

void
Docu::Update ()
{
  if (config.Changed()) {
    nb.Reconfigure (config);
    config.SetChanged (false);
  }
  feedList.CheckChanged();
  nb.update();
}

void
Docu::ClearPeriodic ()
{
  feedList.CheckChanged (true);
}

void
Docu::DoEditPrefs ()
{
  prefEditor.Edit(&config);
}

void
Docu::SaveFeeds (bool useConfig)
{
  QString filename("");
  if (useConfig) {
    filename = config.FeedListFile();
  }
  if (!useConfig || filename.length() == 0) {
    filename = QFileDialog::getSaveFileName (&nb, tr("Save Feed List File"),
                  config.FeedListFile(),
                  tr("XML files (*.xml);; All files (*)"));
  }
  if (filename.length() == 0) {
    return;
  }
  QFile saveFile(filename);
  saveFile.open (QFile::WriteOnly);
  feedList.Save (&saveFile);
  saveFile.close();
}

void
Docu::Show()
{
  nb.show();
}

bool
Docu::OpenFeed (QStandardItem *pItem)
{
  bool ok(true);
  QString url;
  
  openingFeed = true;
  interestingFeed = pItem;
  RssFeed * pFeed;
  if (interestingFeed) {
    pFeed = RssFeed::Reinterpret(interestingFeed->data());
    nb.SetNick (pFeed->Nick());
    url = pFeed->XmlLink();
 
    QNetworkReply * openReply = mQMgr->get(QNetworkRequest(QUrl(url)));

    connect (&newStoryList, SIGNAL(Arrived(StoryList*)), 
         this, SLOT(ReceiveStoryList()));
    newStoryList.Connect(openReply);
    nb.ShowLoading(true);
    if (curFeed) {                   // assuming they read it
      QString h = storyList.HashValue();
      curFeed->SetHash(h);
    }
    oldFeed = curFeed;
    curFeed = pFeed;
  }
  return ok;
}

void
Docu::AskChangedDeep (QStandardItem *pItem)
{
  feedList.SlowCheckChanged (pItem);
}

void
Docu::AskChanged (RssFeed * feed)
{
  if (feed == 0) {
    return;
  }
  if (feed->Dir()) {
    return;
  }
  if (feed->Recently()) {
    return;
  }
  if (checking.count(feed) > 0) {
    return;
  }
  QString xmlUrl = feed->XmlLink();
  StoryList * stories = new StoryList;
  if (stories == 0) {
    return;
  }
  
  checking[feed] = stories;
  checkForChanges[stories] = feed;
  nb.SetBusy(checking.size());
  QNetworkReply * reply = mQMgr->get(QNetworkRequest(QUrl(xmlUrl)));
  stories->Connect (reply);
  connect (stories, SIGNAL(Arrived(StoryList*)),
           this,SLOT(ReceiveChanged(StoryList*)));
}

void
Docu::ReceiveChanged (StoryList * sl)
{
  if (sl == 0) {
    return;
  }
  disconnect (sl,SIGNAL(Arrived(StoryList*)),
      this,SLOT(ReceiveChanged(StoryList*)));
  if (checkForChanges.count(sl) < 1) {
     return;
  }
  RssFeed * feed = checkForChanges[sl];
  QString oldHash = feed->Hash();
  QString newHash = sl->HashValue();
  bool  hasChanged = oldHash != newHash;
  if (hasChanged) {
    feed->SetNewStory (true);
  }
  checkForChanges.erase(sl);
  checking.erase(feed);
  nb.SetBusy (checking.size());
}

void
Docu::ShowVersion ()
{
  ShowVersionWindow();
}


void
Docu::ShowHelp ()
{
  help.Show("qrc:/helpman.html");
}

void
Docu::ShowLicense ()
{
  help.Show("qrc:/LICENSE.txt");
}


void
Docu::DoEditFeed ()
{
  editor.Edit (interestingFeed);
}

void
Docu::MailStory ()
{
  if (curStory) {
    QString subject = tr("Re:") + curStory->Title();
    QString body;
    QString intro;
    intro = tr("on ") + curFeed->Title() + "\r\n"
                  + tr("story") +  "<a href=\"" 
                  + curStory->Link() + "\">"
                  + curStory->Title() + "</a>\r\n";
    QString auth = curStory->Author();
    if (auth.length() > 0) {
      intro.append(tr(" by ") + auth + "\r\n");
    }
    body = intro + curStory->Descr();
    QString url = "mailto:?subject=" + subject
                 + "&body=" + body;
    QDesktopServices::openUrl (QUrl(url));
  }
}

void
Docu::DoNewFeed ()
{
  TextBox getXml (&nb);
  getXml.SetTitle (tr("New RSS/Atom Xml Feed"));
  getXml.SetLabel (tr("Enter or Paste Link"));
  getXml.SetDefault ("");
  getXml.exec();
  QString newxml = getXml.GetText();
  if (newxml.length() > 0) {
    editor.EditNew (newxml);
  }
}

void
Docu::DoNewFolder ()
{
  RssFeed * newFolder = new RssFeed;
  if (newFolder == 0) {
    return;
  }
  TextBox getFolder (&nb);
  getFolder.SetTitle (tr("New Folder"));
  getFolder.SetLabel (tr("Enter or Paste Folder Name"));
  QString defaultname(tr("New Folder"));
  getFolder.SetDefault (defaultname);
  getFolder.exec();
  QString newname = getFolder.GetText();
  if (getFolder.WasGood()) {
    if (newname.length() < 1) {
      newname = defaultname;
    }
    newFolder->SetNick(newname);
    feedList.InsertNewDir(newFolder);
  }
}

void
Docu::CheckPressed (const QModelIndex & index)
{
  Qt::MouseButtons buttons = QApplication::mouseButtons();
  if (buttons & Qt::RightButton) {
    IsInteresting(index);
    DoEditFeed();
  }
}

void
Docu::ImportOPML ()
{
  QString opmlName = QFileDialog::getOpenFileName(&nb,
     tr("Open OPML File"));
  QFile opmlfile(opmlName);
  bool oktoread = opmlfile.open(QFile::ReadOnly);
  if (oktoread) {
    feedList.LoadOpml(&opmlfile);
  }
  opmlfile.close();
  nb.SetTreeModel(&feedList);
}

void
Docu::ImportDRSS ()
{
  QString drssName = QFileDialog::getOpenFileName(&nb,
     tr ("Open DRSS File"));
     
  QFile drssfile (drssName);
  bool oktoread = drssfile.open (QFile::ReadOnly);
  if (oktoread) {
    feedList.Load(&drssfile);
  }
  drssfile.close();
  nb.SetTreeModel(&feedList);
}

void
Docu::EditDone (QStandardItem *pItem, const int result)
{
  bool changed =    (result & EditFeed::Is_Changed) != 0;
  bool deleteFeed = (result & EditFeed::Is_Delete) != 0;
  bool isNew      = (result & EditFeed::Is_New) != 0;
  bool cancelled =  (result & EditFeed::Is_Cancelled) != 0;
  if (cancelled) {
    return;
  }
  bool gone(false);
  RssFeed * pRF (0);  
  if (pItem) {
    pRF = RssFeed::Reinterpret(pItem->data());
  }
  if (pRF && isNew) {
    feedList.Insert (pRF, pItem);
  }
  if (pRF && (changed || isNew) ) {
    AskChanged (pRF);
    nb.update();
  }
  if (pItem && deleteFeed) {
    gone = Delete (pItem);
  }
  if (changed && !gone) {
    pItem->setText(pRF->Nick());
  }
  QModelIndex rootndx = feedList.RootIndex();
  bool isexpanded(false);
  if (rootndx.isValid()) {
    isexpanded = nb.feedTree->isExpanded(rootndx);
  }
  nb.SetTreeModel(0);
  nb.SetTreeModel(&feedList);
  if (rootndx.isValid()) {
    nb.feedTree->setExpanded(rootndx,isexpanded);
  }
  if (changed || isNew) {
    SaveFeeds();
  }
}


bool
Docu::Delete (QStandardItem *pItem) 
{
  if (pItem == 0) {
    return false;
  }
  QModelIndex mindex = feedList.indexFromItem(pItem);
  int row = mindex.row();
  RssFeed * pRF = RssFeed::Reinterpret(pItem->data());
  if (pRF) {
    if (pRF == curFeed) {
      DitchFeed();
    }
      // delete children
    int nrows = pItem->rowCount();
    for (int r=0; r<nrows; r++) {
      Delete (pItem->child(r));
    }
    delete pRF;
    pItem->setData(0);
  }
  QStandardItem *parent = pItem->parent();
  if (parent) {
    parent->removeRow(row);  
  }  
  return true;
}

void
Docu::ClickedFeed (const QModelIndex & index)
{
  if (openingFeed) {  // busy opening already
    return; 
  }
  QStandardItem * sitem = feedList.itemFromIndex(index);
  RssFeed * pRF = RssFeed::Reinterpret(sitem->data());
  if (pRF && !pRF->Dir()) {
    OpenFeed (sitem);
  }
}

void
Docu::IsInteresting (const QModelIndex & index)
{
  QStandardItem * sitem = feedList.itemFromIndex(index);
  RssFeed * pRF = RssFeed::Reinterpret(sitem->data());
  if (pRF) {
    interestingFeed = sitem;
    if (pRF->Dir()) {
      AskChangedDeep (sitem);
    } else {
      AskChanged(pRF);
    }
  }
}

void
Docu::DoMarkFeedRead ()
{
  feedList.MarkReadDeep (interestingFeed);
}

void
Docu::DoNewsLink ()
{
  if (curStory) {
    QDesktopServices::openUrl (QUrl(curStory->Link()));
  }
}

void
Docu::MakeStoryModel ()
{
  StoryList::iterator stit = newStoryList.begin();
  nb.storyView->clear();
  int row(0);
  bool islive (true);
  bool isread (false);
  if (curFeed) {
    curFeed->StartHashList ();
  }
  while (stit != newStoryList.end()) {
    RssItem *pRI = *stit;
    if (pRI) {
      if (curFeed) {
        pRI->ComputeHash ();
        islive =true;
        curFeed->MarkAsLive (pRI->Hash(), true);
        isread = curFeed->WasRead (pRI->Hash());
        pRI->SetIsNew (islive);
        pRI->SetIsRead (isread);
      }
      QString title = pRI->Title();
      nb.storyView->addItem(title);
      nb.MarkStoryBold (row, !isread);
      pRI->SetRow(row);
    }
    stit++; row++;
  }
  if (curFeed) {
    curFeed->CleanHashList ();
  }
}

void
Docu::ClearStoryModel ()
{
  nb.storyView->clear();
}

void  
Docu::ReceiveStoryList ()
{
  disconnect (&newStoryList, SIGNAL(Arrived(StoryList*)), 
       this, SLOT(ReceiveStoryList()));
  MakeStoryModel();
  QString oldhash = storyList.HashValue();
  storyList.Clear();
  newStoryList.CopyTo(storyList);
  newStoryList.ClearFlat();
  storyIt = storyList.begin();
  DoStory();
  nb.ShowLoading(false);
  openingFeed = false;
  if (oldFeed) {
    oldFeed->SetHash (oldhash);
    oldFeed->SetNewStory (false);
    oldFeed = curFeed;
  }
  
}

void
Docu::DitchFeed ()  // after current feed deleted
{
  disconnect (&newStoryList, SIGNAL(Arrived(StoryList*)), 
       this, SLOT(ReceiveStoryList()));
  storyList.Clear();
  newStoryList.Clear();
  interestingFeed = 0;
  storyIt = storyList.end();
  nb.ShowLoading (false);
  ClearStoryModel ();
  openingFeed = false;
  oldFeed = 0;
  curFeed = 0;
  DoStory();
}

void 
Docu::DoStory ()
{
  if (storyIt != storyList.end()) {
    RssItem * pStory = *storyIt;
    curStory = pStory;
    
    if (pStory) {
      int r = pStory->Row();
      nb.SetTitle (pStory->Title(),pStory->Author());
      nb.SetDescr (pStory->Descr());
      pStory->SetIsRead(true);
      nb.storyView->setCurrentRow(r);
      nb.MarkStoryBold (r, false);
      if (curFeed) {
        curFeed->MarkAsRead (pStory->Hash(), true);
      }
    }
  }
}


void
Docu::DoThisStory (QListWidgetItem * item)
{
  if (item == 0) {
    return;
  }
  int row = nb.storyView->row(item);
  RssItem * pStory = storyList.Item(row);
  if (pStory) {
    curStory = pStory;
    nb.SetTitle (pStory->Title());
    nb.SetDescr (pStory->Descr());
    storyIt = FindStory(pStory);
    pStory->SetIsRead (true);
    nb.MarkStoryBold (row, false);
    if (curFeed) {
      curFeed->MarkAsRead (pStory->Hash(), true);
    }
  }
}

void
Docu::DoNextStory ()
{
  if (storyIt != storyList.end()) {
    storyIt++;
    DoStory();
  }
}

void
Docu::DoPrevStory ()
{
  if (storyIt != storyList.begin()) {
    storyIt--;
    DoStory();
  }
}

StoryList::iterator 
Docu::FindStory (RssItem * st)
{
  StoryList::iterator stit = storyList.begin();
  while (stit != storyList.end()) {
    if (*stit == st) {
      return stit;
    }
    stit++;
  }
  return storyList.end();
}

void
Docu::Shutdown ()
{
  if (config.SaveOnExit()) {
    SaveFeeds (true);
  }
  DitchFeed ();
  pApp->quit();
}



}
