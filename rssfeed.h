#ifndef RSSFEED_H
#define RSSFEED_H

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

#include <QString>
#include <QMetaType>
#include <QDomElement>
#include <map>
#include "rssitem.h"

namespace deliberate {

typedef qulonglong RssFeedPtrInt;

class RssFeed  {


public:
   RssFeed ();
   RssFeed (const QString n);
   RssFeed (const RssFeed& r);
   ~RssFeed ();
   
   static RssFeedPtrInt   Reinterpret (const RssFeed* p);
   static RssFeed*        Reinterpret (const QVariant v);
   
   void StartHashList();
   void CleanHashList();
   void AddStoryHash (const RssItem * pStory, QString hash, 
                     bool & isnew, bool & isread);
   void MarkAsRead (const QString storyhash, const bool readit);
   void MarkAsLive (const QString storyhash, const bool islive);
   bool WasRead    (const QString storyhash);
   void MarkAllRead (const bool readit = true);
   
   #if DRSS_DEBUG
   void DumpHashList();
   #endif
   
   QString XmlLink ()                 { return xmlLink; }
   QString WebLink ()                 { return webLink; }
   QString Title ()                   { return title; }
   QString Descr ()                   { return description; }
   QString ImageLink ()               { return imgLink; }
   QString Nick ()                    { return nick; }
   QString Language ()                { return language; }
   QString Hash()                     { return hash; }
   
   void   FillInDom (QDomDocument &dom, QDomElement &del);
   void   ParseDom (QDomElement & feedEl);
   
   void   SetXmlLink (QString x)      { xmlLink = x; }
   void   SetWebLink (QString w)      { webLink = w; }
   void   SetTitle (QString t)        { title = t; }
   void   SetDescr (QString d)        { description = d; }
   void   SetImageLink ( QString il)  { imgLink = il; }
   void   SetNick (QString n)         { nick = n; }
   void   SetLanguage (QString l)     { language = l;}
   void   SetHash (QString h)         { hash = h; }
   
   bool   Saved () { return isSaved; }
   bool   Good  () { return isGood; }
   bool   Dir   () { return isDir; }
   bool   NewStory (){return isNewStory; }
   bool   Recently () { return recently; }
   
   void   SetSaved (bool s) { isSaved = s; }
   void   SetGood  (bool g) { isGood = g; }
   void   SetDir   (bool d) { isDir = d; }
   void   SetNewStory (bool c) { isNewStory = c; }
   void   SetRecently (bool r) { recently = r;}
   
   void   clear();
   void   Dump() const;
   
   RssFeed & operator = (const RssFeed &other);
   
   

private:

   class StorySignature {
   public:
      StorySignature ()
        :pStory (0), hash (""), islive (false), haveread(false)
        {}
      StorySignature (RssItem *pS, QString h, bool s, bool r)
         :pStory (pS),hash (h),islive (s), haveread (r)
         {}
      StorySignature (const StorySignature &other)
         :pStory (other.pStory),
          hash (other.hash),
          islive (other.islive),
          haveread (other.haveread)
          {}
      StorySignature & operator= (const StorySignature &other)
         { pStory = other.pStory;
           hash = other.hash;
           islive = other.islive;
           haveread = other.haveread;
           return *this;
           }
      RssItem * pStory;
      QString   hash;
      bool      islive;
      bool      haveread;
   };
   
   typedef map<QString, StorySignature> StoryHashListType;

   QString     xmlLink;
   QString     webLink;
   QString     title;
   QString     description;
   QString     imgLink;
   QString     nick;
   QString     language;
   QString     hash;
   
   StoryHashListType   storyHashList;
   
   bool        isSaved;
   bool        isGood;
   bool        isDir;
   bool        isNewStory;
   bool        recently;
   
   static int FeedNum;
   
   static int type() ;
   
   void AppendTextElement (QDomDocument &dom, QDomElement &del, QString tag, QString text);
   void AppendSoloElement (QDomDocument &dom, QDomElement &del, QString tag, 
                           QString attr1Name, QString attr1Value,
                           QString attr2Name, bool attr2Value);
   

};



}



#endif
