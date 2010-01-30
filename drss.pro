# //
# //  Copyright (C) 2009 - Bernd H Stramm 
# //
# // This program is distributed under the terms of 
# // the GNU General Public License version 3 
# //
# // This software is distributed in the hope that it will be useful, 
# // but WITHOUT ANY WARRANTY; without even the implied warranty 
# // of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. 
# //

CONFIG += qt app debug_and_release


QT += core gui xml network webkit

RESOURCES += drss.qrc

MAKEFILE = Make_Q

FORMS += drssmain.ui \
         feededit.ui \
         prefedit.ui \
         helpwin.ui \
         probelinks.ui \
         getstring.ui

TRANSLATIONS = drss_de.ts \
               drss_ro.ts
         
CONFIG(release, debug|release) {
   DEFINES += QT_NO_DEBUG
   DEFINES += QT_NO_DEBUG_OUTPUT
   DEFINES += DRSS_DEBUG=0
   QMAKE_CXXFLAGS_RELEASE -= -g
   TARGET = drss
}

CONFIG(debug, debug|release) {
   DEFINES += DRSS_DEBUG=1
   TARGET = drssd
}


SOURCES = \
          rssfeed.cpp \
          feedlist.cpp \
          docu.cpp \
          maindrss.cpp \
          drssconfig.cpp \
          editfeed.cpp \
          rssitem.cpp \ 
          version.cpp \
          newsbox.cpp \
          cmdoptions.cpp \
          textbox.cpp \
          drssdebug.cpp \
          storylist.cpp \
          prefedit.cpp \
          helpview.cpp \
          drssfileformat.cpp \
          aclock.cpp
         

HEADERS = docu.h \
          drssconfig.h \
          newsbox.h \
          feedindex.h \
          rssfeed.h \
          feedlist.h \
          editfeed.h \
          version.h \
          rssitem.h \
          cmdoptions.h \
          textbox.h \
          drssdebug.h \
          storylist.h \
          prefedit.h \
          helpview.h \
          drssfileformat.h \
          aclock.h


