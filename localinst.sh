#!/bin/sh

PROGRAM=drss

DEST=$HOME/bin

install --compare $PROGRAM $DEST

ls -l $DEST/$PROGRAM
