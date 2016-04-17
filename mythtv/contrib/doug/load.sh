#!/bin/sh

ps -eo pid,ppid,state,cmd |    awk '$3 ~ /[RD]/ { print $0 }'
