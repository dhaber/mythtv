#!/bin/bash
#MythIPod v.1 http://www.chiefhacker.com/                                                 
#Copyright 2007 Chris Lorenzo et al                  
#This software is licensed under the CC-GNU LGPL <http://creativecommons.org/licenses/LGPL/2.1/> 
#Additionally modified 2007 Doug Johnson

if [ ! $# == 2 ]; then
  echo "Usage: $0 directory file"
  exit
fi

directory="$1";
file="$2";

#Get the video information
#video_info=`mplayer -vo null -ao null -frames 10 -identify "${directory}/${file}" 2>/dev/null`

#echo $video_info
#exit

#Command line grep sucks, but we make due...
aspect=`echo $video_info | egrep -oe "Movie-Aspect is [0-9:.]+" | egrep -o "[0-9:.]+"`
framerate=`echo $video_info | egrep -oe "[0-9.]+ fps" | egrep -o "[0-9.]+"`

# set resolution by aspect ratio

if [ "$aspect" == "1.78:1" ]
then
  width=432
  height=240
  vbitrate=480k
else
  width=480
  height=360
  vbitrate=378k
fi

width=720
height=480
vbitrate=9080k
abitrate=128k

# Figure out which AAC we have
AAC=$(ffmpeg -v 2>&1|sed "s/--enable-/\n/g"|grep aac|awk '{print $1}')
if [ "x${AAC}" = "xlibfaac" ]
then
	ACODEC="-acodec libfaac"
else
	ACODEC="-acodec aac"
fi

# Create the MP4

#ffmpeg -i "${directory}/${file}" $ACODEC -ab ${abitrate} -ac 2 -s ${width}x${height} -vcodec mpeg4 -b ${vbitrate} -flags +aic+mv4+trell -mbd 2 -cmp 2 -subcmp 2 -g 250 -maxrate 512k -bufsize 2M -title "${file}" "${directory}/${file}.mp4" 
#ffmpeg -i "${directory}/${file}" $ACODEC -ab ${abitrate} -ac 2 -s ${width}x${height} -vcodec mpeg4 -b ${vbitrate} -mbd 2 -cmp 2 -subcmp 2 -g 250 -maxrate 512k -bufsize 2M -title "${file}" "${directory}/${file}.mp4" 

# UPDATED - For newer builds of ffmpeg, trell and title are depreciated - use this instead:
#ffmpeg -i "${directory}/${file}" $ACODEC -ab ${abitrate} -ac 2 -s ${width}x${height} -vcodec mpeg4 -b ${vbitrate} -flags +aic+mv4 -trellis 1 -mbd 2 -cmp 2 -subcmp 2 -g 250 -maxrate 512k -bufsize 2M -metadata title="${file}" "${directory}/${file}.mp4"
# UPDATED 2 - If you get an error like "libfaac doesn't support this output format" try resampling audio input to 44100Hz:
ffmpeg -i "${directory}/${file}" $ACODEC -ab ${abitrate} -ac 2 -ar 44100 -s ${width}x${height} -b ${vbitrate} -flags +aic+mv4 -trellis 1 -mbd 2 -cmp 2 -subcmp 2 -g 250 -maxrate 512k -bufsize 2M -metadata title="${file}" "${directory}/${file}.mp4"


