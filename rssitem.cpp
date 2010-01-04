#include "rssitem.h"

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

RssItem::RssItem()
{
}

RssItem::~RssItem()
{
}

void
RssItem::ComputeHash ()
{
  QCryptographicHash tmpHash (QCryptographicHash::Sha1);
  tmpHash.addData (mTitle.toUtf8());
  tmpHash.addData (mAuthor.toUtf8());
  tmpHash.addData (mDescr.toUtf8());
  tmpHash.addData (mGuid.toUtf8());
  QByteArray result = tmpHash.result();
  mHash = result.toHex();
}

}
