#include "storylist.h"
#include <QCryptographicHash>

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


namespace deliberate {


StoryList::StoryList()
:mOpenReply(0)
{
  clear();
}

StoryList::~StoryList ()
{
  for (StoryListType::iterator it = begin();
      it != end();
      it++) {
    if (*it) {
      delete *it;
    }
  }
  StoryListType::clear();
}

StoryList &
StoryList::operator= (const StoryList & other)
{
  StoryListType::operator=(other);
  doc = other.doc;
  mOpenReply = 0;
  return *this;
}

void
StoryList::clear ()
{
  StoryListType::clear();
}

RssItem *
StoryList::Item (const int i)
{
  if (i>=0 && static_cast<int>(size()) > i) {
    return this->operator[](i);
  }
  return 0;
}

void
StoryList::CopyTo (StoryList & other)
{
  StoryListType::iterator it;
  for (it = begin();  it != end(); it++) {
    other.push_back(*it);
  }
}

QString 
StoryList::HashValue ()
{
  QCryptographicHash hash(QCryptographicHash::Sha1);
  
  StoryListType::iterator it = begin();
  while (it != end()) {
    RssItem * pStory = *it;
    if (pStory) {
      pStory->ComputeHash();
      hash.addData(pStory->Hash().toUtf8());
    }
    it++;
  }
  return hash.result().toHex();
}

void
StoryList::Connect (QNetworkReply * expect)
{
  mOpenReply = expect;
  connect (mOpenReply, SIGNAL(finished()),this,SLOT(CatchReply()));
}

void
StoryList::CatchReply ()
{

  disconnect (mOpenReply,SIGNAL(finished()), this, SLOT(CatchReply()));
  QString msg;
  int line(-43);
  bool ok = doc.setContent (mOpenReply, &msg, &line);
  if (ok)  {
     QDomElement docElem = doc.documentElement();
     ClearFlat ();
     ParseDocument (docElem);  
  }
  emit Arrived (this);
  doc.clear();
  mOpenReply->close();
}


void
StoryList::ParseDocument (QDomElement & doc)
{
  QString tag = doc.tagName();
  if (tag == "rss" || tag == "rdf:RDF") {           // RSS
    RecurseElement (doc);
  } else if (tag == "feed") {   // Atom
    RecurseElement (doc);
  } else if (tag == "dwml") {   // DWML weather/geological
    ParseDwmlElement (doc);
  }
}

void
StoryList::ParseDwmlElement (QDomElement & doc)
{
  RssItem *pI = new RssItem;
  pI->SetTitle ("unknown DWML entry");
  pI->SetDescr ("decoding of DWML feeds coming soon");
  
  push_back (pI);
}


void
StoryList::ParseRssStory (QDomElement & elem)
{
  static QString tag_title("title");
  static QString tag_descr("description");
  static QString tag_link("link");
  static QString tag_creator("dc:creator");
  static QString tag_guid("guid");
  QDomElement e = elem.firstChildElement();
  RssItem * pI(0);
  while (!e.isNull()) {
    QString t = e.tagName();
    if (t == tag_title) {
      if (pI == 0) {
        pI = new RssItem;
      }
      pI->SetTitle(e.text());
    } else if (t == tag_descr) {
      if (pI == 0) {
         pI = new RssItem;
       }
       pI->SetDescr (e.text());
    } else if (t == tag_link) {
       if (pI == 0) {
         pI = new RssItem;
       }
       pI->SetLink(e.text());
    } else if (t == tag_creator) {
       if (pI == 0) {
         pI = new RssItem;
       }
       pI->SetAuthor (e.text());
    } else if (t == tag_guid) {
      if (pI == 0) {
        pI = new RssItem;
      }
      pI->SetGuid (e.text());
    }
    e = e.nextSiblingElement();
  }
  if (pI) {
    push_back(pI);
  }
}

void
StoryList::ParseAtomAuthor (QDomElement & elem, QString & name)
{
  QString tag_name("name");
  for (QDomElement child = elem.firstChildElement();
      !child.isNull();
      child = child.nextSiblingElement()) {
    QString t = child.tagName();
    if (t == tag_name) {
      name = child.text();
    }
  }
}

void
StoryList::ParseAtomStory (QDomElement & elem)
{
  static QString tag_title("title");
  static QString tag_descr("content");
  static QString tag_link("link");
  static QString tag_author("author");
  static QString tag_id ("id");
  
  QDomElement e = elem.firstChildElement();
  RssItem * pI(0);
  while (!e.isNull()) {
    QString t = e.tagName();
    if (t == tag_title) {
      if (pI == 0) {
        pI = new RssItem;
      }
      pI->SetTitle(e.text());
    } else if (t == tag_descr) {
      if (pI == 0) {
         pI = new RssItem;
       }
       pI->SetDescr (e.text());
    } else if (t == tag_link) {
       if (e.hasAttribute("href")) {
         if (pI == 0) {
           pI = new RssItem;
         }
         pI->SetLink(e.attribute("href"));
       }
    } else if (t == tag_author) {
      QString authorname;
      ParseAtomAuthor (e,authorname);
      if (pI == 0) {
        pI = new RssItem;
      }
      pI->SetAuthor(authorname);
    } else if (t == tag_id) {
      if (pI == 0) {
        pI = new RssItem;
      }
      pI->SetGuid (e.text());
    }
    e = e.nextSiblingElement();
  }
  if (pI) {
    push_back(pI);
  }
}

void
StoryList::RecurseElement (QDomElement & elem)
{
  static QString tag_item("item");
  static QString tag_entry("entry");
  
  QDomElement e = elem.firstChildElement();
  int nitems(0);
  while(!e.isNull()) {
    nitems++;
    QString t = e.tagName();
    if (t == tag_item) {
      ParseRssStory (e);
    } else if (t == tag_entry){
      ParseAtomStory (e);
    } else {   
      RecurseElement (e);
    }
    e = e.nextSiblingElement();
  }
}


void
StoryList::Clear ()
{
  StoryListType::iterator it;
  it = begin();
  while (it != end()) {
    //delete *it;
    it++;
  }
  clear();
}

void
StoryList::ClearFlat ()
{
  clear();
}


}
