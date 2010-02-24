#ifndef STORYLIST_H
#define STORYLIST_H


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

#include <deque>
#include "rssitem.h"
#include <QString>
#include <QObject>
#include <QNetworkReply>
#include <QDomElement>
#include <QDomDocument>


namespace deliberate {



typedef deque<RssItem*> StoryListType;
  
class StoryList : public QObject, public StoryListType {

Q_OBJECT


public:

  StoryList();
  StoryList (const StoryList & other);
  StoryList & operator=(const StoryList&  other);
  ~StoryList();
  
  void Clear (); // deep clear, delete things pointed to
  void ClearFlat ();
  void clear ();
  void CopyTo (StoryList &oher);
  
  RssItem * Item(const int i);

  QString HashValue();
  void Connect (QNetworkReply * expect);
  
public slots:

  void    CatchReply();
  
signals:

  void    Arrived (StoryList* sl);
  
private:

  QNetworkReply         * mOpenReply;
  QDomDocument            doc;

  void ParseDocument (QDomElement &doc);
  void RecurseElement (QDomElement &elem);
  void ParseDwmlElement (QDomElement &elem);
  void ParseRssStory (QDomElement &elem);
  void ParseAtomStory (QDomElement &elem);
  void ParseAtomAuthor (QDomElement &elem, QString & name);
};


}


#endif
