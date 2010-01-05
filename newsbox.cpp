#include "newsbox.h"
#include "drssdebug.h"
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


namespace deliberate {

QString NewsBox::failedMessage ("Story Load Failure");

NewsBox::NewsBox ()
{

  setupUi(this);
  clockWidget->Start();
  loadingPic.load(":/loading.png");
  notloadingPic.load(":/notloading.png");
  ShowAnalog (true);
  busy = 0;
  busyCount->show();
  busyCount->setText("");
  
  
  updateT.setParent (this);
  connect (&updateT,SIGNAL(timeout()), this, SLOT(update()));
  updateT.start (250);
}

void
NewsBox::Reconfigure (DRSSConfig & config)
{
  ShowAnalog  (config.Analog());
  float scale = float(config.StoryTextSize()) / 100.0;
  storyBrowser->setTextSizeMultiplier(scale);
}


void
NewsBox::quit ()
{
   emit ImDone();
}

void
NewsBox::update()
{
  ShowBusy();
  QMainWindow::update();
}

void 
NewsBox::SetTreeModel (QAbstractItemModel *pm)
{
  feedTree->setModel(pm); 
}
 

void
NewsBox::ShowLoading (bool loading)
{
  if (loading) {
    loadingLabel->setPixmap(loadingPic);
  } else {
    loadingLabel->setPixmap(notloadingPic);
  }
}


void
NewsBox::ShowAnalog (bool show)
{
  if (show) {
    clockWidget->show();
  } else {
    clockWidget->hide();
  }
}

void
NewsBox::ShowBusy ()
{
  if (busy>0) {
    busyCount->setText(QString::number(busy));
  } else {
    busyCount->setText("");
  }
}

void
NewsBox::SetTitle (QString t, QString auth, QString date)
{
  QString title_with_author;
  if (auth == "") {
     title_with_author = t;
  } else {
     title_with_author = t + tr(", by ") + auth;
  }
  if (date.length() > 0) {
    title_with_author += tr(" , ") + date;
  }
  storyTitle->setHtml(title_with_author);
}

void
NewsBox::SetDescr (QString d)
{
  try {
    storyBrowser->setHtml(d);
  } catch (...) {
    storyBrowser->setHtml (failedMessage);
  }
}

void
NewsBox::SetNick (QString n)
{
  nickLine->setHtml(n);
}

void
NewsBox::MarkStoryBold (const int row, const bool isbold)
{ 
  QListWidgetItem * it = storyView->item(row);
  if (it) {
    QFont f =  it->font();
    f.setBold(isbold);
    it->setFont(f);
  }
}

void
NewsBox::Clear ()
{
  SetDescr ("");
  SetTitle ("","","");
  SetNick ("");
}

}
