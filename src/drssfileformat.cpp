
#include "drssfileformat.h"

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

bool DrssFF::Initialized(false);

QString DrssFF::theTagString[];
QString DrssFF::theAttrName[];

void
DrssFF::Init ()
{
  int tg;
  
  for (tg = t_Last-1; tg > t_none; tg--) {
    theTagString[tg] = QString("<");
  }
  theTagString[t_none]    = QString("");
  theTagString[t_feed]    = QString("feed");
  theTagString[t_language] = QString("language");
  theTagString[t_drssFeedList] = QString("drssfeedlist");
  theTagString[t_xmlLink] = QString("xmlUrl");
  theTagString[t_webLink] = QString("webUrl");
  theTagString[t_title]   = QString("title");
  theTagString[t_descr]   = QString("description");
  theTagString[t_imgLink] = QString("imageUrl");
  theTagString[t_nick]    = QString("nick");
  theTagString[t_dir]     = QString("folder");
  theTagString[t_storyhash] = QString("storysig");
  theTagString[t_guid]    = QString("guid");
  
  int at;
  for (at = a_none; at < a_Last; at++) {
    theAttrName[at] = "<";
  }
  theAttrName[a_none] = QString("");
  theAttrName[a_hash] = "hash";
  theAttrName[a_name] = "name";
  theAttrName[a_haveread] = "readit";
  Initialized = true;
}

QString
DrssFF::TagString (FeedTag t)
{
  if (!Initialized) { Init(); }
  return theTagString[t];
}

QString
DrssFF::AttrName (FeedAttr a)
{
  if (!Initialized) { Init(); }
  return theAttrName[a];
}


DrssFF::FeedTag
DrssFF::MatchTag (QString t)
{
  int tg;
  if (!Initialized) {
    Init();
  }
  for (tg = DrssFF::t_Last-1; tg > DrssFF::t_none; tg--) {
    if (DrssFF::theTagString[tg] == t) {
       return static_cast<FeedTag>(tg);
    }
  }
  return t_none;
}

}
