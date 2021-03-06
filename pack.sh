#!/bin/bash

NAME=drss
VERSION=0.3.5
PACKDIR=${HOME}/packaging/drss

makearchive.sh ${NAME}-${VERSION} master
cp ${NAME}-${VERSION}.tar.gz ${PACKDIR}
echo ${NAME} > ${PACKDIR}/pack-name
echo ${VERSION} > ${PACKDIR}/pack-version
ls -l ${PACKDIR}/${NAME}-${VERSION}.tar.gz
ls -l ${PACKDIR}/pack-*

if [ x$1 == "xmake" ]
then
  cd ${PACKDIR}
  make
fi
