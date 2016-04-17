#!/bin/sh

xfs_db -c frag -r /dev/sdb1
xfs_db -c frag -r /dev/sda6

