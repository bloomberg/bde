#!/bin/sh

SUBST="s;@TARGET@;\""$1"\";"
#echo $SUBST
cat wrapper.c.prototype | sed -e $SUBST > $2
