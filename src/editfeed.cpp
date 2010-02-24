#include "editfeed.h"

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
#include <QDialog>
#include <QListWidget>
#include <QListWidgetItem>
#include "ui_probelinks.h"

namespace deliberate {

EditFeed::EditFeed ()
{
  Ui_FeedEdit::setupUi  (this);
  
  mQMgr = 0;
  ConnectButtons ();
  hide();
  loadingPic.load(":/loading.png");
  notloadingPic.load(":/notloading.png");
  
}

void 
EditFeed::Init (QNetworkAccessManager * pQMgr)
{
  mQMgr = pQMgr;
  if (mQMgr) {
    connect (mQMgr, SIGNAL(finished(QNetworkReply*)), 
        this, SLOT(CatchProbe(QNetworkReply*)));
  }
}

void
EditFeed::ConnectButtons ()
{
  connect (cancelButton,SIGNAL(clicked()), this, SLOT(Cancel()));
  connect (deleteButton, SIGNAL(clicked()), this, SLOT(Delete()));
  connect (doneButton, SIGNAL(clicked()), this, SLOT(Done()));
  connect (probeButton, SIGNAL (clicked()), this, SLOT (ProbeAgain()));
}

void
EditFeed::DisableButtons ()
{
  deleteButton->setEnabled(false);
  doneButton->setEnabled(false);
  probeButton->setEnabled(false);
}

void
EditFeed::EnableButtons ()
{
  cancelButton->setEnabled(true);
  deleteButton->setEnabled(true);
  if (mQMgr) {
    probeButton->setEnabled(true);
  }
  if (isgood) {
    doneButton->setEnabled(true);
  }
}


void
EditFeed::Save ()
{
  if (pFeed) {
    pFeed->SetSaved(true);
    Done(false);
  }
}

void
EditFeed::Delete ()
{
  if (pFeed) {
    pFeed->SetSaved(false);
    Done(true);
  }
}

void
EditFeed::Edit (QStandardItem *pItem)
{
   if (pItem == 0) {
      return;
   }
   pFeed = RssFeed::Reinterpret(pItem->data());
   pReturnItem = pItem;
   isgood = true;
   descriptionLineEdit->setPlainText (pFeed->Descr());
   nickLineEdit->setText (pFeed->Nick());
   titleLineEdit->setText (pFeed->Title());
   websiteLineEdit->setText (pFeed->WebLink());
   xmlLinkLineEdit->setText (pFeed->XmlLink());
   if (pFeed->Dir()) {
     loadStatus->setText(tr("Folder"));
     
   } else {
     loadStatus->setText("");
   }
   
   show();
}

void
EditFeed::EditFromQueue ()
{
  if (newXmlList.isEmpty()) {
    return;
  }
  QString newguy = newXmlList.takeFirst();
  EditNew (newguy);
}

void
EditFeed::EditNew (const QString newxml)
{
  if (mQMgr == 0) {
    return;
  }
  pFeed = 0;
  pReturnItem = 0;
  xmlLinkLineEdit->setText(newxml);
  descriptionLineEdit->setText("");
  nickLineEdit->setText("");
  titleLineEdit->setText("");
  websiteLineEdit->setText("");
  ProbeChannel (newxml);
  DisableButtons();
  show();
}

void
EditFeed::Done (bool erase)
{
  RssFeed *pRF(0);
  int result (Is_Changed);
  if (pReturnItem) {
    pRF = pFeed;
  } else {
    pReturnItem = new QStandardItem;
    pRF = new RssFeed;
    if (pReturnItem == 0 || pRF == 0) {
      return;
    }
    pReturnItem->setData (RssFeed::Reinterpret(pRF));
    result |= Is_New;
  }
  hide();
  pRF->SetDescr (descriptionLineEdit->toPlainText());
  pRF->SetNick (nickLineEdit->text());
  pRF->SetTitle (titleLineEdit->text());
  pRF->SetWebLink (websiteLineEdit->text());
  pRF->SetXmlLink (xmlLinkLineEdit->text());
  pRF->SetSaved(!erase);
  if (erase) {
    result |= Is_Delete;
  }
  emit FinishedEditing (pReturnItem, result);
  EditFromQueue();
}


void
EditFeed::Cancel ()
{
  pFeed = 0;
  hide();
  emit FinishedEditing (pReturnItem, Is_Cancelled);
  EditFromQueue();
}

void
EditFeed::ProbeChannel (const QString link)
{
  QNetworkReply * pProbeReply = mQMgr->get(QNetworkRequest(QUrl(link)));
  seenReply[pProbeReply] = false;
  loadStatus->setPixmap(loadingPic);
  isgood = false;
}

void
EditFeed::ProbeAgain ()
{
  if (mQMgr == 0) {
    return;
  }
  QString link = xmlLinkLineEdit->text();
  ProbeChannel (link);
}

void
EditFeed::PopulateFromRssDoc (QDomElement & el)
{
  static QString tag_title("title");
  static QString tag_link("link");
  static QString tag_description("description");
  QString t;
  QString weblink;
  QString title;
  QString description (tr("cannot find RSS feed data"));
  bool foundsomething(false);
  for (QDomElement child = el.firstChildElement();
       !child.isNull();
       child = child.nextSiblingElement()) {
    t = child.tagName();
    if (t == tag_title) {
      title = child.text();
      foundsomething = true;
    } else if (t == tag_link) {
      weblink = child.text();
      foundsomething = true;
    } else if (t == tag_description) {
      description = child.text();
    }
  }
  titleLineEdit->setText(title);
  websiteLineEdit->setText(weblink);
  descriptionLineEdit->setText(description);
  isgood = foundsomething;
  show();
}


void
EditFeed::PopulateFromAtomDoc (QDomElement & el)
{
  QString t;
  static QString tag_title("title");
  static QString tag_link("link");
  static QString tag_author("author");
  static QString tag_subtitle("subtitle");
  QString xmllink;
  QString weblink;
  QString title;
  QString author;
  QString description (tr("cannot find Atom feed data"));
  bool foundsomething(false);
  for (QDomElement child = el.firstChildElement();
       !child.isNull();
       child = child.nextSiblingElement()) {
    t = child.tagName();
    if (t == tag_title) {
      title = child.text();
      foundsomething = true;
    } else if (t == tag_link) {
      foundsomething |= ParseAtomLinkElem (child, xmllink, weblink);
    } else if (t == tag_author) {
      foundsomething |= ParseAtomAuthorElem (child, author);
    } else if (t == tag_subtitle) {
      description = child.text();
    }
  }
  titleLineEdit->setText(title);
  websiteLineEdit->setText(weblink);
  descriptionLineEdit->setText(description);
  isgood = foundsomething;
  show();
}

bool 
EditFeed::ParseAtomLinkElem (QDomElement & el, QString & xml, QString & web)
{
   QString relAttr = el.attribute("rel");
   bool ok(false);
   if (relAttr == "self") {
     xml = el.attribute("href");
     ok = true;
   } else {
     web = el.attribute("href");
     ok = true;
   }
   return ok;
}

bool
EditFeed::ParseAtomAuthorElem (QDomElement &el, QString & name)
{
  bool ok(false);
  for (QDomElement child = el.firstChildElement();
       !child.isNull();
       child = child.nextSiblingElement()) {
    if (child.tagName() == "name") {
      name = child.text();
      ok = name.length() > 0;
    }
  }
  return ok;
}

void
EditFeed::ProbeHtmlHead (QDomElement & head)
{
  QDomElement el;
  int numlinks(0);
  QStringList rssLinks;
  for (el = head.firstChildElement();
       !el.isNull();
       el = el.nextSiblingElement()) {
    if (el.tagName() == "link") {
      if (el.attribute("rel") == "alternate") {
        if (el.attribute("type") == "application/rss+xml") {
          QString lnk = el.attribute("href");
          numlinks++;
          rssLinks += lnk;
          
        }
      }
    }
  }
  if (numlinks > 0) {
    QDialog * askLinks = new QDialog;
    htmlLinkProbe.setupUi(askLinks);
    htmlLinkProbe.linkBox->addItems (rssLinks);
    askLinks->exec();
    QList<QListWidgetItem*> userWants = htmlLinkProbe.linkBox->selectedItems();
    int howmany = userWants.count();
    int dialogResult = askLinks->result();
    if (dialogResult != QDialog::Accepted) {
      howmany = 0;
    }
    QString wantThis;
    for (int i=0; i<howmany; i++) {
      wantThis = userWants[i]->text();
      newXmlList.append(wantThis);
    }
    delete askLinks;
    Cancel ();
  }
}


void
EditFeed::ProbeHtml (QDomElement & htmlEl)
{
  QDomElement el;
  for (el = htmlEl.firstChildElement(); 
       !el.isNull(); el = el.nextSiblingElement()) {
    if (el.tagName() == "head") {
      ProbeHtmlHead (el);
    }
  }
}



void
EditFeed::CatchProbe(QNetworkReply * reply)
{
  if (seenReply.count(reply) < 1) {
    return;
  }
  if (seenReply[reply]) {
#if DRSS_DEBUG
    QDEBUG () << " ignoring duplicate reply " << reply;
#endif
    return;
  }
  seenReply[reply] = true;
  QDomDocument doc;
  bool ok = doc.setContent (reply);
  reply->close();
  seenReply.erase(reply);
  reply->deleteLater();
  QDomElement root = doc.documentElement();
  QString tag = root.tagName();
  if (tag == "html") {
    ProbeHtml(root);
    return;
  }
  if (!ok) {
    descriptionLineEdit->setText(tr("cannot find Feed data"));
  } else {
    if (tag == "rss" || tag == "rdf:RDF") {  // RSS
      for (QDomElement rssel = root.firstChildElement();
           !rssel.isNull();
           rssel = rssel.nextSiblingElement()) {
        tag = rssel.tagName();
        if (tag == "channel") {
          PopulateFromRssDoc (rssel);
        } 
      }
    } else if (tag == "feed") { // Atom
      PopulateFromAtomDoc (root);
    } else if (tag == "dwml") { // NOAA weather format
      descriptionLineEdit->setText(tr("DWML decoding coming soon"));
    } else {  
      
      descriptionLineEdit->setText(tr("cannot decode Feed data with tag ") + tag);
    }
  }
  loadStatus->setPixmap(notloadingPic);
  EnableButtons();
  show();
}

}
