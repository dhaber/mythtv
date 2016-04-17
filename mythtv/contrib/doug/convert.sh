#!/bin/bash
#
# My script to create hr.hdtv encodes.
#
# This script will perform the following tasks:
# 1. Analyze the original mpeg for
#       a. play length
#       b. audio size
#       c. frame speed (60fps/30fps)
# 2. Calculate final file size (350MB or 700MB)
# 3. Calculate final video size based on final file
#    size and audio size.
# 4. Encode.
#

#variables
overheadbytes=16
server_default=POT

#Functions
#Video Length Calculation
function VideoSizeCalc() {
#        audiosize=`pc $audiobitrate/1000*$totalseconds*0.1220703125/1024`
        audiosize=`echo "$audiobitrate/1000*$totalseconds*0.1220703125/1024" | bc`
        echo "Audio Size in MB = $audiosize"
#       audiooverhead=`pc $totalseconds*1000/64`
#       videooverhead=`pc 23.976*$totalseconds`
#       totaloverhead=`pc $audiooverhead+$videooverhead`
#       overheadsize=`pc $totaloverhead*$overheadbytes/1024**2`
#       overheadsize=`echo `${overheadsize:0:4}``
#       echo "(DD) Overhead = $overheadsize MB"
        totalseconds=`echo $totalseconds|sed -e 's/\.[0-9]*//'`
#       echo "(DD) Total Seconds = $totalseconds"
        if [ $totalseconds -lt 1800 ]; then
                totalsize=350
        elif [ $totalseconds -lt 3600 ]; then
                totalsize=700
        else
                echo "I don't know how to handle this length!"
                exit 1
        fi
        echo "Total Size = $totalsize"
#       videosize=`pc $totalsize-$overheadsize-$audiosize` #original formula
#        videosize=`pc $totalsize-$audiosize`    # new one since overhead doesn't
        videosize=`echo "$totalsize-$audiosize" | bc`    # new one since overhead doesn't

                                                #really matter its so small
        videosize=`echo `${videosize:0:3}``
        echo "Videosize = $videosize MB"
}


function FolderNameClean() {
        input=$1
        input=`echo "$input" | sed -e 's/.[Aa][Vv][Ii]//'`
        input=`echo "$input".XviD-MythTV`
        folder_name=$input
}

#test for arguements
if [[ -z "@ARGV" ]]; then
        echo "first arguement is the original mpg"
        echo "second arguement is the new folder name, will be used for filename"
        exit
else
        #Reassign variables
        if [[ ! -e $1 ]]; then
                echo "no such input mpg!"
                exit
        else
                original_name=$1
        fi
        folder_name=$2
        FolderNameClean $2
        file_name=`echo "$folder_name" | tr A-Z a-z`
        # display arguments
        echo "Original mpeg is: $original_name"
        echo "Folder name will be: $folder_name"
        echo "File name will be: $file_name"
fi

#Grab input file information
echo "Determining total file length..."
#totalseconds=`mencoder $original_name -ovc copy -nosound -o /dev/null -quiet 2>&1 | awk '/^Video stream:/{print $10+$10/$12}'`
timestring=`mythffmpeg -i /var/lib/mythtv/recordings2/4523_20110104075900.mpg 2>&1  | grep "Duration:" | cut -d " " -f 4 | sed s/,//`
hours=`echo "$timestring" | cut -d ":" -f 1`
mins=`echo "$timestring" | cut -d ":" -f 2`
secs=`echo "$timestring" | cut -d ":" -f 3`
totalseconds=`echo "($hours * 60 *60) + ($mins * 60) + $secs" | bc`
echo "total seconds = $totalseconds"
audiobitrate=`mplayer -vo null -ao null -frames 0 -identify "$original_name" 2>/dev/null | grep "ID_AUDIO_BITRATE" | sed -e 's/ID_AUDIO_BITRATE=0//' | grep "ID_AUDIO_BITRATE" | sed -e 's/ID_AUDIO_BITRATE=//'`
echo "audio bitrate = $audiobitrate"
videosize=`mythffmpeg -i /var/lib/mythtv/recordings2/4523_20110104075900.mpg 2>&1 | grep Video | cut -d " " -f 10`
#videowidth=`mplayer -vo null -ao null -frames 0 -identify "$original_name" 2>/dev/null | grep "ID_VIDEO_WIDTH" | sed -e 's/ID_VIDEO_WIDTH=//'`
videowidth=`echo "$videosize" | cut -d "x" -f 1`
echo "video width = $videowidth"
#videoheight=`mplayer -vo null -ao null -frames 0 -identify "$original_name" 2>/dev/null | grep "ID_VIDEO_HEIGHT" | sed -e 's/ID_VIDEO_HEIGHT=//'`
videoheight=`echo "$videosize" | cut -d "x" -f 2`
echo "video height = $videoheight"
videofps=`mplayer -vo null -ao null -frames 0 -identify "$original_name" 2>/dev/null | grep "ID_VIDEO_FPS" | sed -e 's/ID_VIDEO_FPS=//'`
videofps=`echo `${videofps:0:2}``
echo "FPS: $videofps"
#aspect=`pc $videowidth/$videoheight`
aspect=`echo "$videowidth/$videoheight" | bc`
aspect=`echo "$aspect" | tr -s [:digit:]`
echo "Aspect Ratio: $aspect"
#Calculate Video size
VideoSizeCalc
sleep 10
#videosize=`pc $videosize*1024`
videosize=`echo "$videosize*1024" | bc`
echo "Videosize = $videosize"
exit 0
if [[ -e "$original_name" ]]; then
        echo start `date +%H:%M`>$file_name.runtime
        if [[ "$videofps" -eq "59" ]]; then
                nice -n 17 mencoder $original_name -oac copy -ovc xvid -vf decimate=2:1000:1600:.001,scale=960:544 -ofps 24000/1001 -xvidencopts pass=1:vhq=0:me_quality=5:turbo:quant_type=mpeg:aspect=$aspect:max_bframes=0:bitrate=-$videosize -o /dev/null
                #
                nice -n 17 mencoder $original_name -oac copy -ovc xvid -vf decimate=2:1000:1600:.001,scale=960:544 -ofps 24000/1001 -xvidencopts pass=2:quant_type=mpeg:aspect=$aspect:max_bframes=0:bitrate=-$videosize -o $file_name.avi
        elif [[ "$videofps" -eq "29" ]]; then
                nice -n 17 mencoder $original_name -oac copy -ovc xvid -vf pullup,softskip,scale=960:544 -ofps 24000/1001 -xvidencopts pass=1:vhq=0:me_quality=5:turbo:quant_type=mpeg:aspect=$aspect:max_bframes=0:bitrate=-$videosize -o /dev/null
                #
                nice -n 17 mencoder $original_name -oac copy -ovc xvid -vf pullup,softskip,scale=960:544 -ofps 24000/1001 -xvidencopts pass=2:vhq=0:me_quality=5:quant_type=mpeg:aspect=$aspect:max_bframes=0:bitrate=-$videosize -o $file_name.avi
        fi
        echo stop `date +%H:%M`>>$file_name.runtime
        echo "Runtime"
        more $file_name.runtime
fi
exit
