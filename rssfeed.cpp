#include "rssfeed.h"


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
#include "drssfileformat.h"
#include <QVariant>

namespace deliberate {



RssFeedPtrInt
RssFeed::Reinterpret (const RssFeed * p)
{
  return reinterpret_cast<RssFeedPtrInt>(p);
}

RssFeed*
RssFeed::Reinterpret (const QVariant v)
{
  RssFeed * f;
  f = reinterpret_cast<RssFeed*>(v.toULongLong());
  return f;
}

int RssFeed::FeedNum(1);

int RssFeed::type() { return 2000;}

RssFeed::~RssFeed ()
{
}

RssFeed::RssFeed()
:isSaved(false),
 isGood(false),
 isDir(false),
 isNewStory(false)
{
  FeedNum++;
}

RssFeed::RssFeed (const QString n)
:isSaved(false),
 isGood(false),
 isDir(false),
 isNewStory(false)
{
  nick = n;
}

RssFeed::RssFeed (const RssFeed &r)
: xmlLink(r.xmlLink),
 title(r.title),
 description(r.description),
 imgLink(r.imgLink),
 nick(r.nick),
 language(r.language),
 isSaved(r.isSaved),
 isGood(r.isGood),
 isDir(r.isDir),
 isNewStory(r.isNewStory)
{
}



RssFeed &
RssFeed::operator= (const RssFeed &other)
{
  xmlLink = other.xmlLink;
  webLink = other.webLink;
  title = other.title;
  description = other.description;
  imgLink = other.imgLink;
  nick = other.nick;
  language = other.language;
  isSaved = other.isSaved;
  isGood = other.isGood;
  isDir = other.isDir;
  isNewStory = other.isNewStory;
  return *this;
}

void
RssFeed::clear ()
{
   xmlLink="";
   webLink="";
   title="";
   description="";
   imgLink="";
   nick="";
   language="";
   isSaved=false;
   isGood=false;
}

void
RssFeed::StartHashList ()
{
  StoryHashListType::iterator it;
  for (it=storyHashList.begin(); it != storyHashList.end(); it++) {
    it->second.islive = false;
  }
}

void
RssFeed::CleanHashList ()
{
  StoryHashListType::iterator it;
  StoryHashListType::iterator del_it;
  it = storyHashList.begin(); 
  while (it != storyHashList.end() ) {
    if (!(it->second.islive)) {
      del_it = it;
      it++;
      storyHashList.erase (del_it);
    } else {
      it++;
    }
  }
}

void
RssFeed::AddStoryHash (const RssItem* pStory, QString hash,
                       bool & isnew, bool & isread)
{
  StorySignature sig;
  if (storyHashList.count(hash) < 1) {
    sig.islive = isnew;
    sig.pStory = const_cast<RssItem*>(pStory);
    sig.hash = hash;
    sig.haveread = isread;
  } else {
    sig = storyHashList[hash];
  }
  sig.haveread = isread;
  storyHashList[hash] = sig;
  return;
}

void
RssFeed::MarkAsRead (const QString storyhash, const bool readit)
{
  StorySignature sig = storyHashList[storyhash]; // if it wasn't there, it is now
  sig.haveread = readit;
  sig.islive = true;
  storyHashList[storyhash] = sig;
}

void
RssFeed::MarkAsLive (const QString storyhash, const bool islive)
{
  StorySignature sig = storyHashList[storyhash]; // if it wasn't there, it is now
  sig.islive = islive;
  storyHashList[storyhash] = sig;
}

bool
RssFeed::WasRead (const QString storyhash)
{
  if (storyHashList.count(storyhash) < 1) {
    return false;
  }
  return storyHashList[storyhash].haveread;
}


void
RssFeed::AppendTextElement (QDomDocument & dom, QDomElement &del, QString tag, QString text)
{
  QDomElement newEl = dom.createElement (tag);
  QDomText    txt   = dom.createTextNode (text);
  newEl.appendChild (txt);
  del.appendChild (newEl);
}

void
RssFeed::AppendSoloElement (QDomDocument &dom, QDomElement & del, QString tag,
                             QString attr1Name,  QString attr1Value,
                             QString attr2Name,  bool attr2Value)
{
  QDomElement newEl = dom.createElement (tag);
  newEl.setAttribute (attr1Name, attr1Value);
  QString yorn = (attr2Value ? "y" : "n");
  newEl.setAttribute (attr2Name, yorn);
  del.appendChild (newEl);
}

void
RssFeed::FillInDom (QDomDocument &dom, QDomElement & del)
{
   AppendTextElement (dom, del, DrssFF::TagString(DrssFF::t_language), Language());
   AppendTextElement (dom, del, DrssFF::TagString(DrssFF::t_xmlLink), XmlLink());
   AppendTextElement (dom, del, DrssFF::TagString(DrssFF::t_title), Title());
   AppendTextElement (dom, del, DrssFF::TagString(DrssFF::t_descr), Descr());
   AppendTextElement (dom, del, DrssFF::TagString(DrssFF::t_imgLink), ImageLink());
   AppendTextElement (dom, del, DrssFF::TagString(DrssFF::t_nick), Nick());
   AppendTextElement (dom, del, DrssFF::TagString(DrssFF::t_webLink), WebLink());
   StoryHashListType::iterator it;
   bool readit;
   for (it=storyHashList.begin(); it!= storyHashList.end(); it++) {    
     readit = it->second.haveread;
     AppendSoloElement (dom, del, DrssFF::TagString(DrssFF::t_storyhash), 
                              DrssFF::AttrName(DrssFF::a_hash), it->first,
                              DrssFF::AttrName(DrssFF::a_haveread), readit);
   }
}

void
RssFeed::ParseDom (QDomElement &feedEl)
{
  QDomElement el = feedEl.firstChildElement();
  if (el.isNull()) {
    return;
  }
  QString hash = feedEl.attribute("hash");
  SetHash(hash);
  QString sh;
  QString yorn;
  bool haveread;
  bool islive;
  while (!el.isNull()) {
    QString tag = el.tagName();
    DrssFF::FeedTag ftag = DrssFF::MatchTag(tag);
    switch (ftag) {
    case DrssFF::t_xmlLink:
      SetXmlLink (el.text());
      break;
    case DrssFF::t_webLink:
      SetWebLink (el.text());
      break;
    case DrssFF::t_title:
      SetTitle (el.text());
      break;
    case DrssFF::t_descr:
      SetDescr (el.text());
      break;
    case DrssFF::t_imgLink:
      SetImageLink (el.text());
      break;
    case DrssFF::t_nick:
      SetNick (el.text());
      break;
    case DrssFF::t_language:
      SetLanguage (el.text());
      break;
    case DrssFF::t_storyhash:
      sh = el.attribute(DrssFF::AttrName(DrssFF::a_hash));
      yorn = el.attribute(DrssFF::AttrName(DrssFF::a_haveread));
      haveread = (yorn == "y");
      islive = false;
      AddStoryHash (0, sh, islive, haveread);
      break;
    default:
      // don't know what this is, ignore it
      break;
    }
    el = el.nextSiblingElement();
  }
  SetSaved(true);
  SetDir(false);
}

void
RssFeed::Dump() const
{
#if DRSS_DEBUG
    QDEBUG () << " feed at XML   " << xmlLink;
    QDEBUG () << "         web   " << webLink;
    QDEBUG () << "         title " << title;
    QDEBUG () << "         descr " << description;
    QDEBUG () << "         Image " << imgLink;
    QDEBUG () << "         Nick  " << nick;
    QDEBUG () << "         isSaved " << isSaved;
    QDEBUG () << "         isGood  " << isGood;
    QDEBUG () << "         isDir   " << isDir;
#endif
}

}
