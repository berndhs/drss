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

DIR_BIN=./bin
SYS_DEST_DIR=/usr/local/bin
INSTALL_SYS=yes
INSTALL_CMD="install -p"
ITEM_LIST="drss"

install_item () {
  ITEM=$1
  if [ -e $ITEM ]
  then
    ${INSTALL_CMD} $ITEM $DIR_BIN
    if [ $INSTALL_SYS == "yes" ]
    then
      ${INSTALL_CMD} $ITEM $SYS_DEST_DIR
      echo ${INSTALL_CMD} $ITEM $SYS_DEST_DIR
    fi
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
    INSTALL_SYS=yes
  else
    INSTALL_SYS=no
    echo "$D is not a directory"
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


