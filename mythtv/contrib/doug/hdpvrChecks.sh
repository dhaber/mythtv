#!/bin/sh

ls -las /dev/hdpvr*
ls -las /dev/fw*
echo "hdpvr0 brightness check"
v4l2-ctl -d /dev/hdpvr0 -c brightness=134
echo "hdpvr1 brightness check"
v4l2-ctl -d /dev/hdpvr1 -c brightness=134
plugreport
