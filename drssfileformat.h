#ifndef DRSSFILEFORMAT_H
#define DRSSFILEFORMAT_H

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

namespace deliberate {

class DrssFF {

public:

   enum FeedTag { t_none, 
                  t_drssFeedList,
                  t_feed,
                  t_language,
                  t_xmlLink, 
                  t_webLink,
                  t_title, 
                  t_descr,
                  t_imgLink,
                  t_nick,
                  t_dir,
                  t_guid,
                  t_storyhash,
                  t_Last
                  };
                  
  enum FeedAttr { a_none,
                  a_hash,
                  a_name,
                  a_haveread,
                  a_Last
                  };
                  
  static QString TagString (FeedTag t);
  static QString AttrName  (FeedAttr a);
  static FeedTag MatchTag (QString t);
  static void Init();

private:

  static QString theTagString[t_Last];
  static QString theAttrName[a_Last];
  static bool    Initialized;
  
  

};


}


#endif

