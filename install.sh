#!/bin/sh
# 
# Local Install Script for DRSS
#
# //
# //  Copyright (C) 2010 - Bernd H Stramm
# //
#
# // This program is distributed under the terms of 
# // the GNU General Public License version 3 
# //
# // This software is distributed in the hope that it will be useful, 
# // but WITHOUT ANY WARRANTY; without even the implied warranty 
# // of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. 
# //

SYS_DEST_DIR=/usr/local/bin
INSTALL_CMD="install -p"
ITEM_LIST="drss"

install_item () {
  ITEM=$1
  if [ -e $ITEM ]
  then
    echo "Trying: " ${INSTALL_CMD} $ITEM $SYS_DEST_DIR
    ${INSTALL_CMD} $ITEM $SYS_DEST_DIR
  else
    echo "cannot find " $ITEM " to install"
  fi
}


if [ $# -gt 0 ]
then
  D=$1
  if [ -d $D ]
  then
    SYS_DEST_DIR=$D
  else
    echo "cannot install in $D:  not a directory"
    exit 1
  fi
fi


if [ ! -d $DIR_BIN ]
then
  mkdir $DIR_BIN
fi

echo "Installing in $SYS_DEST_DIR"
for i in $ITEM_LIST ; do
  install_item $i
  done


