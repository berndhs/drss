#include "version.h"
#include <string>
#include <iostream>
#include <QMessageBox>
#include <QTimer>

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

  std::string ProgramVersion("0.3.5");

  std::string ProgramName("drss");
  std::string copyright ("Copyright (C) 2009,2010 Bernd H Stramm");
  
  std::string Version ()
  { 
    return std::string(ProgramName + " Version " 
                   + ProgramVersion + " " + std::string(__DATE__) + " "
		       + std::string(__TIME__)
		       + std::string("\r\n")
		       + copyright);
  }

  void ShowVersionWindow ()
  {
    QString versionMessage(Version().c_str());
    QMessageBox box;
    box.setText (versionMessage);
    QTimer::singleShot(30000,&box,SLOT(accept()));
    box.exec();
  }
  
  void CLIVersion ()
  {
    std::cout << Version() << std::endl;
  }
  
  std::string MyName()
  { 
    return ProgramName;
  }
  
  std::string VersionMajor ()
  {
    QString v = QString::fromStdString (ProgramVersion);
    QStringList parts = v.split (".");
    if (parts.isEmpty()) {
      return "0";
    }
    return parts.first().toStdString();
  }
  
  std::string VersionMinor ()
  {
    QString v = QString::fromStdString (ProgramVersion);
    QStringList parts = v.split (".");
    if (parts.isEmpty()) {
      return "0";
    }
    parts.removeFirst();
    if (parts.isEmpty ()) {
      return "0";
    }
    return parts.first().toStdString();
  }
  
  std::string VersionPatch ()
  {
    QString v = QString::fromStdString (ProgramVersion);
    QStringList parts = v.split (".");
    if (parts.isEmpty()) {
      return "0";
    }
    return parts.last().toStdString();
  }

}
