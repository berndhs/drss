#include "prefedit.h"

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

namespace deliberate {

PrefEdit::PrefEdit ()
{
  setupUi (this);
  storyTextSize->setMinimum(DRSSConfig::minTextFactor);
  storyTextSize->setMaximum(DRSSConfig::maxTextFactor);
  hide();
  connect (okButton,SIGNAL(clicked()),this,SLOT(DoOk()));
  connect (cancelButton,SIGNAL(clicked()),this,SLOT(DoCancel()));
  connect (defaultsButton,SIGNAL(clicked()),this,SLOT(ResetToDefaults()));
  running = false;
}

void
PrefEdit::Reset ()
{
  if (pOrigConfig) {
    feedFileEdit->setText (tmpConfig.FeedListFile());
    saveOnExitCheck->setCheckState (State (tmpConfig.SaveOnExit()));
    storyTextSize->setValue (tmpConfig.StoryTextSize());
  }
}

void
PrefEdit::Edit (DRSSConfig * config)
{
  if (running) {
    return;
  }
  if (config == 0) {
    return;
  }
  pOrigConfig = config;
  tmpConfig = *pOrigConfig;
  Reset ();
  show ();
  running = true;
}

void
PrefEdit::ResetToDefaults ()
{
  tmpConfig.SetDefault ();
  Reset();
}

void
PrefEdit::DoCancel ()
{
  if (running) {
    pOrigConfig = 0;
    running = false;
    hide();
  }
}

void
PrefEdit::DoOk ()
{
  if (running) {
    if (pOrigConfig) {
      pOrigConfig->SetFeedListFile(feedFileEdit->text());
      pOrigConfig->SetSaveOnExit (IsChecked (saveOnExitCheck->checkState()));
      pOrigConfig->SetStoryTextSize (storyTextSize->value());
      pOrigConfig->Write();
      pOrigConfig->SetChanged (true);
      pOrigConfig = 0;
    }
    running = false;
    hide();
  }
}


} // namespace

