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

UI_DIR = tmp/ui
MOC_DIR = tmp/moc

RESOURCES += drss.qrc

MAKEFILE = MakeDRSS

FORMS += ui/drssmain.ui \
         ui/feededit.ui \
         ui/prefedit.ui \
         ui/helpwin.ui \
         ui/probelinks.ui \
         ui/getstring.ui

TRANSLATIONS = drss_de.ts \
               drss_ro.ts
         
INCLUDEPATH += src

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
          src/rssfeed.cpp \
          src/feedlist.cpp \
          src/docu.cpp \
          src/maindrss.cpp \
          src/drssconfig.cpp \
          src/editfeed.cpp \
          src/rssitem.cpp \ 
          src/version.cpp \
          src/newsbox.cpp \
          src/cmdoptions.cpp \
          src/textbox.cpp \
          src/drssdebug.cpp \
          src/storylist.cpp \
          src/prefedit.cpp \
          src/helpview.cpp \
          src/drssfileformat.cpp \
         

HEADERS = src/docu.h \
          src/drssconfig.h \
          src/newsbox.h \
          src/feedindex.h \
          src/rssfeed.h \
          src/feedlist.h \
          src/editfeed.h \
          src/version.h \
          src/rssitem.h \
          src/cmdoptions.h \
          src/textbox.h \
          src/drssdebug.h \
          src/storylist.h \
          src/prefedit.h \
          src/helpview.h \
          src/drssfileformat.h \


