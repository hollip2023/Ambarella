#!/bin/sh
#
# History:
#	2012/05/05 - [Jingyang Qiu] created file
#
# Copyright (c) 2015 Ambarella, Inc.
#
# This file and its contents ("Software") are protected by intellectual
# property rights including, without limitation, U.S. and/or foreign
# copyrights. This Software is also the confidential and proprietary
# information of Ambarella, Inc. and its licensors. You may not use, reproduce,
# disclose, distribute, modify, or otherwise prepare derivative works of this
# Software or any portion thereof except pursuant to a signed license agreement
# or nondisclosure agreement with Ambarella, Inc. or its authorized affiliates.
# In the absence of such an agreement, you agree to promptly notify and return
# this Software to Ambarella, Inc.
#
# THIS SOFTWARE IS PROVIDED "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES,
# INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF NON-INFRINGEMENT,
# MERCHANTABILITY, AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED.
# IN NO EVENT SHALL AMBARELLA, INC. OR ITS AFFILIATES BE LIABLE FOR ANY DIRECT,
# INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
# (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
# LOSS OF USE, DATA, OR PROFITS; COMPUTER FAILURE OR MALFUNCTION; OR BUSINESS
# INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
# CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
# ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
# POSSIBILITY OF SUCH DAMAGE.
#
config_dir=/sys/module/ambarella_config/parameters
board_chip=$(cat $config_dir/board_chip)
board_type=$(cat $config_dir/board_type)
board_rev=$(cat $config_dir/board_rev)

aoss_remount_root_ro()
{
	mkdir -p /mnt/tmpdev
	mount -t tmpfs tmpfs /mnt/tmpdev
	cp -a /dev/* /mnt/tmpdev
	mount -o bind /dev/pts /mnt/tmpdev/pts
	mount -o bind /mnt/tmpdev /dev
	mdev -s
	mount -o remount,ro /
	sync
	echo 3 > /proc/sys/vm/drop_caches
	free
}

aoss_clean_modules()
{
	module_check=$(lsmod | awk '{print $1}' | tail -n +2)
	for i in $module_check
	do
		rmmod "$i"
	done
}

aoss_i1()
{
	case "$board_type" in
		1)
		test2_cmd="-vwvga --lcd td043 --disable-csc --fb 0 -V1080p --hdmi --disable-csc --cs rgb --fb 0 --idle"
		#test2_cmd="-vwvga --lcd td043 --disable-csc --idle"
		;;
		2)
		test2_cmd="-vD480x800 --lcd 1p3831 --disable-csc --rotate --fb 0 -V1080p --hdmi --disable-csc --cs rgb --fb 0 --idle"
		#test2_cmd="-vwvga --lcd 1p3831 --disable-csc --idle"
		;;
		*)
		echo "$board_type"
		test2_cmd="-V720p --hdmi --idle"
		#test2_cmd="-V720p --hdmi -i0 -h720p"
		;;
	esac

	case "$board_rev" in
		*)
		echo "$board_rev"
		;;
	esac

	aoss_clean_modules
	aoss_remount_root_ro

	echo disk > /sys/power/state

	/etc/init.d/S11init start
	/usr/local/bin/init.sh --na
	/usr/local/bin/test2 "$test2_cmd"
	sd_empty=1
	while [ $sd_empty -ne 0 ]; do
		if [ -r /tmp/mmcblk0p1/Ducks.Take.Off.1080p.QHD.CRF25.x264-CtrlHD.mkv ]; then
			echo "SD ready"
			sd_empty=0
		else
			echo "SD empty"
		fi
		sleep 2
	done
	pbtest /tmp/mmcblk0p1/Ducks.Take.Off.1080p.QHD.CRF25.x264-CtrlHD.mkv
	#reboot
}

aoss_a5s()
{
	case "$board_type" in
		*)
		echo "$board_type"
		;;
	esac

	case "$board_rev" in
		*)
		echo "$board_rev"
		;;
	esac

	aoss_clean_modules
	aoss_remount_root_ro

	echo disk > /sys/power/state

	/usr/local/bin/init.sh --ov2710
	/usr/local/bin/test_encode -i0 -V480p --hdmi

	#/etc/init.d/S11init start
	#sleep 10
	#reboot
}

aoss_a2()
{
	case "$board_type" in
		*)
		echo "$board_type"
		;;
	esac

	case "$board_rev" in
		*)
		echo "$board_rev"
		;;
	esac

	aoss_clean_modules
	aoss_remount_root_ro

	echo disk > /sys/power/state

	/usr/local/bin/init.sh --mt9p031
	/usr/local/bin/test2 -i0 -v480p -h480p

	#/etc/init.d/S11init start
	#sleep 10
	#reboot
}

case "$board_chip" in
	7200)
	aoss_i1
	;;

	5100)
	aoss_a5s
	;;

	2000)
	aoss_a2
	;;

	*)
	echo "$board_chip"
	;;
esac

