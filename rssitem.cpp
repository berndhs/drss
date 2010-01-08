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

/** Note that we do NOT include the Date() field in the hash,
  * since we are interested in changed in content, not date.
  */
  
void
RssItem::ComputeHash ()
{
  QCryptographicHash tmpHash (QCryptographicHash::Sha1);
  tmpHash.addData (mTitle.toUtf8());
  tmpHash.addData (mAuthor.toUtf8());
  tmpHash.addData (mDescr.toUtf8());
  QByteArray result = tmpHash.result();
  mHash = result.toHex();
}

}
