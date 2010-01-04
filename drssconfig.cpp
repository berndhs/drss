#include "drssconfig.h"
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
#include <QFile>
#include <QDomElement>
#include <QDesktopServices>

namespace deliberate {

QString DRSSConfig::doctypetag("drssconfig");
QString DRSSConfig::filetag   ("feedfile");
QString DRSSConfig::anatag   ("analogclock");
QString DRSSConfig::soetag   ("saveonexit");
QString DRSSConfig::ststag   ("storytextsize");
QString DRSSConfig::boolattr ("yesorno");
int     DRSSConfig::maxTextFactor (10000);
int     DRSSConfig::minTextFactor (10);

DRSSConfig::DRSSConfig()
{
  SetDefault();
}

DRSSConfig::DRSSConfig (const DRSSConfig &cfg)
:feedListFile(cfg.feedListFile),
 configFile(cfg.configFile),
 analog(cfg.analog),
 saveonexit(cfg.saveonexit),
 running(false),
 storytextsize(cfg.storytextsize)
{
  changed = true;
}

DRSSConfig &
DRSSConfig::operator= (const DRSSConfig & cfg)
{
  feedListFile = cfg.feedListFile;
  configFile = cfg.configFile;
  analog = cfg.analog;
  saveonexit = cfg.saveonexit;
  running = false;
  storytextsize = cfg.storytextsize;
  changed = true;
  return *this;
}

void
DRSSConfig::SetDefault ()
{
  QString homedir = QDesktopServices::storageLocation(QDesktopServices::HomeLocation);
  feedListFile = homedir + "/.drss_feeds.xml";
  configFile = homedir + "/.drss_cfg.xml";
  analog = true;
  saveonexit = true;
  storytextsize = 75;
  changed = false;
}

void
DRSSConfig::Read ()
{
  QFile cfgfile(ConfigFile());
  cfgfile.open(QFile::ReadOnly);
  QDomDocument dom;
  dom.setContent (&cfgfile);
  cfgfile.close();
  
  QDomElement doc = dom.documentElement();
  if (doc.tagName() != doctypetag) {
    return;
  }
  for (QDomElement el = doc.firstChildElement();
       !el.isNull();
       el = el.nextSiblingElement()) {
    if (el.tagName() == filetag) {
      if (el.hasAttribute("name")) {
        SetFeedListFile(el.attribute("name"));
      }
    } else if (el.tagName () == anatag) {
      SetAnalog (BoolOption(el,boolattr));
    } else if (el.tagName () == soetag) {
      SetSaveOnExit (BoolOption(el,boolattr));
    } else if (el.tagName () == ststag) {
      int sz = el.attribute("percent").toInt();
      if (sz < minTextFactor) { sz = minTextFactor; }
      if (sz > maxTextFactor) { sz = maxTextFactor; }
      SetStoryTextSize (sz);
    }
  }
}

bool
DRSSConfig::BoolOption (const QDomElement & el,  
                        const QString     & attr)
{
  QString yesorno = el.attribute(attr).toLower();
  QString yes = QString("yes").toLower();
  bool y = (yesorno == yes);
  return y;
}

void
DRSSConfig::Write ()
{
  QFile cfgfile(ConfigFile());
  cfgfile.open(QFile::WriteOnly);
  QDomDocument dom;
  QString emptyDoc ("<?xml version=\"1.0\" encoding=\"utf-8\"?>");
  emptyDoc.append ("\n");
  emptyDoc.append ("<" + doctypetag + " version=\"1.0\">\n");
  emptyDoc.append ("</" + doctypetag + ">\n");
  dom.setContent (emptyDoc);
  QDomElement root = dom.documentElement();
  QDomElement el;
  
  el = dom.createElement (filetag);
  el.setAttribute ("name", FeedListFile());
  root.appendChild (el);
  
  QString yesorno;
  
  el = dom.createElement (anatag);
  yesorno = Analog() ? "yes" : "no";
  el.setAttribute (boolattr, yesorno);
  root.appendChild (el);
  
  el = dom.createElement (soetag);
  yesorno = SaveOnExit() ? "yes" : "no";
  el.setAttribute (boolattr, yesorno);
  root.appendChild (el);
  
  el = dom.createElement (ststag);
  int sz = StoryTextSize();
  el.setAttribute("percent",QString::number(sz));
  root.appendChild (el);
  
  QByteArray bytes;
  bytes = dom.toByteArray();
  cfgfile.write(bytes);
  cfgfile.close();
}

}
