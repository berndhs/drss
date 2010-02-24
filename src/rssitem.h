#ifndef RSSITEM_H
#define RSSITEM_H


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

using namespace std;

namespace deliberate {

class RssItem  {

public:

    RssItem ();
    ~RssItem ();
    
    QString Title () { return mTitle; }
    QString Descr () { return mDescr; }
    QString Link ()  { return mLink; }
    QString Author (){ return mAuthor; }
    QString Guid ()  { return mGuid; }
    QString Date ()  { return mDate; }
    QString Hash ()  { return mHash; }
    bool    IsNew () { return isNew; }
    bool    IsRead (){ return haveRead; }
    int     Row()  { return row; }
    
    void ComputeHash () ;
    
    void SetTitle(QString t) { mTitle = t; }
    void SetDescr(QString d) { mDescr = d; }
    void SetLink (QString l) { mLink = l; }
    void SetAuthor (QString a) { mAuthor = a; }
    void SetGuid  (QString g) { mGuid = g; }
    void SetDate (QString d)  { mDate = d; }
    void SetRow (int r)      { row = r; }
    void SetIsNew (bool n)   { isNew = n; }
    void SetIsRead (bool r)  { haveRead = r; }
    
private:

  QString   mTitle;
  QString   mDescr;
  QString   mLink;
  QString   mAuthor;
  QString   mHash;
  QString   mGuid;
  QString   mDate;
  bool      isNew;
  bool      haveRead;
  int       row;
  

};

}


#endif
