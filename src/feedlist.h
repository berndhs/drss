#ifndef FEEDLIST_H
#define FEEDLIST_H


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

#include <map>
#include <QStandardItemModel>
#include <QString>
#include <QDomDocument>
#include "drssfileformat.h"
#include "rssfeed.h"

using namespace std;

namespace deliberate {

class FeedList : public QStandardItemModel   {

Q_OBJECT

public:


  FeedList();
  ~FeedList();
  
  void Init();
  void Load (QIODevice *dev);
  void LoadOpml (QIODevice *dev);
  void Save (QIODevice *dev);
  void clear();
  
  void CheckChanged (bool clear=false);
  void SlowCheckChanged (const QStandardItem * pItem);
  bool RecurseCheckChanged (const QStandardItem *pParent, 
                            bool doEmit = false, 
                            bool clear = false);
  void MarkReadDeep (QStandardItem * pRootItem);
  
  void Insert (RssFeed *f, QStandardItem * pItem = 0);
  void InsertNewDir (RssFeed *f);
  
  bool Delete (QStandardItem *f);
  
  QModelIndex indexFromItem (const QStandardItem *feed) 
     { return QStandardItemModel::indexFromItem (feed);}
  QStandardItem * itemFromIndex (const QModelIndex & ndx) 
     { return QStandardItemModel::itemFromIndex (ndx); }
     
  QModelIndex RootIndex () 
     { return indexFromItem (rootNode); }
    
  bool      Empty();
  
public slots:

signals:

  void QueryWeb (RssFeed *);

private:

  void Domify();
  void DomifyRecurseDir (QDomElement & dir, QStandardItem * pParent);
  
  void     ParseFeed (QDomElement &feedEl);
  void     RecurseDrssDir (QDomElement &el);
  void     LoadOpmlFeed (QDomElement &el);
  void     RecurseOpml  (QDomElement &el);
  void     PushNewDir   (QDomElement &el);
  void     PopDir       ();
  
  void     MakeBold (QStandardItem *pItem, bool bold = true);
  void     SetDirDeco (QStandardItem *pItem, bool isDir = true);
  
  QDomDocument     dom;
  
  QStandardItem   * rootNode;
  QStandardItem   * buildRoot;
  
  
};
}

#endif
