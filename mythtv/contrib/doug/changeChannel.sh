#!/bin/bash

CHAN=`echo "$1" | sed 's/\(.\)/\1 /g'`

echo "key $CHAN enter" | nc localhost 6546
