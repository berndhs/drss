#include "feedlist.h"


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
#include <QCryptographicHash>

namespace deliberate {



FeedList::FeedList()
:rootNode(0)
{
}


void
FeedList::Init ()
{
  clear();
  rootNode = new QStandardItem;
  rootNode->setText(tr("Feeds"));
  RssFeed * dummy(0);
  rootNode->setData(RssFeed::Reinterpret(dummy));
  appendRow(rootNode);
  buildRoot = rootNode;
}

FeedList::~FeedList ()
{
  clear();
  rootNode = 0;
}

void
FeedList::MakeBold (QStandardItem * pItem, bool bold)
{
  if (pItem) {
    QFont font = pItem->font();
    font.setBold(bold);
    pItem->setFont(font);
  }
}

void 
FeedList::SetDirDeco (QStandardItem * pItem, bool isDir)
{
  if (pItem) {
    QFont font = pItem->font();
    font.setUnderline (isDir);
    pItem->setFont (font); 
  }
}

void
FeedList::Save (QIODevice *dev)
{
   dom.clear();
   Domify();   // copy feed map into DOM format
   QByteArray bytes;
   bytes = dom.toByteArray();
   dev->write(bytes);
}

void
FeedList::LoadOpml (QIODevice *dev)
{
  QDomDocument domopml;
  QString errmsg;
  int badline(0);
  bool ok = domopml.setContent (dev,&errmsg, &badline);
  buildRoot = rootNode;
  if (ok) {
    QDomElement docElem = domopml.documentElement();
    QString tag = docElem.tagName();
    QDomElement el = docElem.firstChildElement();
    while (!el.isNull()) {
      RecurseOpml(el);
      el = el.nextSiblingElement();
    }
  }
}

void
FeedList::PopDir ()
{
  buildRoot = buildRoot->parent();
}

void
FeedList::RecurseDrssDir (QDomElement &el)
{
  PushNewDir (el);
  for (QDomElement child = el.firstChildElement();
       !child.isNull();
       child = child.nextSiblingElement()) {
     QString t = child.tagName();
     if (t == DrssFF::TagString(DrssFF::t_feed)) {
       ParseFeed (child);
     } else if (t == DrssFF::TagString(DrssFF::t_dir)) {
       RecurseDrssDir (child);
     }
  }
  PopDir();
}

void
FeedList::RecurseOpml (QDomElement &el)
{
   if (el.tagName() == "outline" && el.attribute("type") == QString("rss")) {
     LoadOpmlFeed (el);
   } else {
     QDomElement child = el.firstChildElement();
     while (!child.isNull()) {
       RecurseOpml (child);
       child = child.nextSiblingElement();
     }
   }
}

void
FeedList::LoadOpmlFeed (QDomElement &el)
{
   QString nul("");
   RssFeed *pRF = new RssFeed;
   if (pRF == 0) { return; }
   pRF->SetTitle(el.attribute("title",nul));
   pRF->SetXmlLink(el.attribute("xmlUrl",nul));
   pRF->SetWebLink(el.attribute("htmlUrl",nul));
   pRF->SetDescr(el.attribute("description",nul));
   pRF->SetDir(false);
   Insert (pRF);
}

void
FeedList::InsertNewDir (RssFeed * f)
{
  if (f == 0) {
    return;
  }
  QStandardItem * pItem = new QStandardItem;
  if (pItem == 0) {
    return;
  }
  f->SetTitle(f->Nick());
  f->SetDir(true);
  f->SetGood(true);
  f->SetSaved(true);
  pItem->setText(f->Nick());
  pItem->setData(RssFeed::Reinterpret(f));
  pItem->setDropEnabled(true);
  SetDirDeco(pItem);
  rootNode->appendRow(pItem);
}

void
FeedList::PushNewDir (QDomElement &el)
{
  QString name = el.attribute("name");
  RssFeed *pDir = new RssFeed;
  QStandardItem * pItem = new QStandardItem;
  if (pDir == 0) {
    return;
  }
  pDir->SetDir(true);
  if (name.length() < 1) {
    name = tr("anonymous");
  }
  pDir->SetTitle(name);
  pDir->SetNick(name);
  pDir->SetGood(true);
  pDir->SetSaved(true);
  pItem->setText(name);
  pItem->setData(RssFeed::Reinterpret(pDir));
  pItem->setDropEnabled(true);
  SetDirDeco (pItem);
  buildRoot->appendRow(pItem);
  buildRoot = pItem;
}

void
FeedList::Insert (RssFeed * f, QStandardItem * pItem)
{
  if (f) {
    if (f->Title().length() == 0) {
      f->SetTitle(f->XmlLink());
    }
    if (f->Nick().length() == 0) {
      f->SetNick(f->Title());
    }
    if (!pItem) {
      pItem = new QStandardItem;
    }
    if (!pItem) {
      return;
    }
    f->SetGood(true);
    f->SetSaved(true);
    pItem->setData(RssFeed::Reinterpret(f));
    pItem->setText(f->Nick());
    pItem->setDropEnabled(false);
    if (f->NewStory()) {
      MakeBold (pItem);
    }
    buildRoot->appendRow(pItem);
  }
}


void
FeedList::Load (QIODevice *dev)
{
  dom.clear();
  QString errmsg;
  int     badline(0);
  bool ok = dom.setContent(dev, &errmsg, &badline);
  buildRoot = rootNode;
  QString t;
  if (ok) {
    QDomElement docElem = dom.documentElement();
    QString tag = docElem.tagName();
    if (tag == DrssFF::TagString(DrssFF::t_drssFeedList)) {
      if (docElem.hasAttribute("version")) {
        QString version = docElem.attribute("version");
        QDomElement el = docElem.firstChildElement();
        while (!el.isNull()) {
           t = el.tagName();
           if (t == DrssFF::TagString(DrssFF::t_feed)) {
             ParseFeed (el);
           } else if (t == DrssFF::TagString(DrssFF::t_dir)) {
             RecurseDrssDir (el);
           }
           el = el.nextSiblingElement();
        }
      }
    }
  }
}

void
FeedList::ParseFeed (QDomElement & feedEl)
{
  RssFeed *pRF = new RssFeed;
  if (pRF == 0) {
    return;
  }
  pRF->ParseDom (feedEl);
  Insert(pRF);
}

void
FeedList::Domify ()
{
   QString emptyDoc("<?xml version=\"1.0\" encoding=\"utf-8\"?>");
   emptyDoc.append("\n");
   emptyDoc.append ("<" + DrssFF::TagString(DrssFF::t_drssFeedList));
   emptyDoc.append(" version=\"1.0\">\n");
   emptyDoc.append("</" + DrssFF::TagString(DrssFF::t_drssFeedList)+ ">\n");
   dom.setContent(emptyDoc);
   if (rootNode == 0) {
     return;
   }
   QDomElement root = dom.documentElement();
   QDomElement el;
   QStandardItem * pRF(0);
   RssFeed * pData;
   int nrows = rootNode->rowCount();
   for (int row = 0; row < nrows; row++) {
     pRF = rootNode->child(row);
     pData = RssFeed::Reinterpret(pRF->data());
     QString t = (pData->Dir() ? DrssFF::TagString(DrssFF::t_dir) 
                               : DrssFF::TagString(DrssFF::t_feed));
     el = dom.createElement (t);
     if (pData->Dir()) {
       el.setAttribute (DrssFF::AttrName(DrssFF::a_name),pData->Nick());
       DomifyRecurseDir (el,pRF);
     } else {
       // fill in from pRF
       pData->FillInDom (dom,el);
     }
     el.setAttribute(DrssFF::AttrName(DrssFF::a_hash),pData->Hash());
     root.appendChild(el);
   }
}

void
FeedList::CheckChanged (bool clear)
{
  RecurseCheckChanged(rootNode, false, clear);
}


void
FeedList::SlowCheckChanged (QStandardItem * pItem)
{
  RecurseCheckChanged (pItem, true);
}

bool
FeedList::RecurseCheckChanged (QStandardItem * pParent, bool doEmit, bool clear)
{
  QStandardItem * pItem(0);
  RssFeed * pData;
  QFont font;
  bool  childChanged; 
  bool  thisChanged;
  bool  somethingChanged(false);
  int nrows = pParent->rowCount();
  
  QCryptographicHash parentHash(QCryptographicHash::Sha1);

  if (nrows > 0) { 
    for (int row = 0; row < nrows; row++) {
      pItem = pParent->child(row);
      pData = RssFeed::Reinterpret(pItem->data());
      if (pData) {
        childChanged = false;
        if (pData->Dir()) {
          childChanged = RecurseCheckChanged (pItem, doEmit, clear);
          pData->SetNewStory (childChanged);        
        } else {
          if (clear && ! doEmit) {
            pData->SetRecently(false);
          }
          if (doEmit && ! pData->Recently()) {
            emit QueryWeb(pData);
          }
        }
        thisChanged = pData->NewStory();
        MakeBold (pItem,thisChanged);
        somethingChanged |= thisChanged;
        if (clear) {
          parentHash.addData (pData->Hash().toUtf8());
        }
      }
    }
  }
  pData = RssFeed::Reinterpret(pParent->data());
  if (pData && clear && somethingChanged) {
    QString hash = parentHash.result().toHex();
    pData->SetHash (hash);
    pData->SetNewStory (somethingChanged);
  }
  return somethingChanged;
}

void
FeedList::DomifyRecurseDir (QDomElement & dir, QStandardItem * pParent)
{
  if (pParent == 0) {
    return;
  }
  int nrows = pParent->rowCount();
  QDomElement el;
  QStandardItem * pChild;
  RssFeed       * pChildData;
  for (int row=0; row< nrows; row++) {
     pChild = pParent->child(row);
     if (!pChild) {
       continue;
     }
     pChildData = RssFeed::Reinterpret(pChild->data());
     QString t = (pChildData->Dir() ? DrssFF::TagString(DrssFF::t_dir) 
                                    : DrssFF::TagString(DrssFF::t_feed));
     el = dom.createElement (t);
     if (pChildData->Dir()) {
       el.setAttribute (DrssFF::AttrName(DrssFF::a_name),pChildData->Nick());
       DomifyRecurseDir (el, pChild);
     } else {
       pChildData->FillInDom (dom, el);
     }
     el.setAttribute(DrssFF::AttrName(DrssFF::a_hash),pChildData->Hash());
     dir.appendChild(el);
  }
}


void
FeedList::clear()
{
  QStandardItemModel::clear();
  dom.clear();
}


bool
FeedList::Delete (QStandardItem *f)
{
 bool foundit(false);
 
 QModelIndex mndx = indexFromItem (f);
 if (mndx.isValid()) {
   RssFeed * pRF = RssFeed::Reinterpret(f->data());
   delete pRF;
   rootNode->removeRow(mndx.row());
   
   return true;
 }
 return foundit;
}

}
