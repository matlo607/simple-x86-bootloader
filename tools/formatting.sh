#!/bin/bash

PROGNAME=${0##*/}

usage() { echo "Usage: ${PROGNAME} {loopback device} [-t|--type <hd|fd>] [-s|--sector-size <size of a sector in bytes>] {disk image}" 1>&2; exit 1; }

if [[ !($# -gt 0) ]]; then
    usage
fi

SHORTOPTS="hvt:s:"
LONGOPTS="help,version,disk-type:,sector-size:"

ARGS=$(getopt -s bash --options $SHORTOPTS  \
      --longoptions $LONGOPTS --name $PROGNAME -- "$@" )

if [ $? != 0 ]; then
    usage
fi

eval set -- "$ARGS"

VERBOSE=false
DISKTYPE="hd"
SECTORSIZE=512

while true; do
    case "$1" in
        -v | --verbose )
            VERBOSE=true;
            shift
            ;;

        -h | --help )
            usage
            shift
            ;;

        -s | --sector-size )
            SECTORSIZE="$2";
            shift 2
            ;;

        -t | --disk-type )
            DISKTYPE="$2";
            shift 2
            ;;

        -- )
            shift;
            break
            ;;

        * )
            break
            ;;
    esac
done

# Make sure only root can run the script
if [ "$(id -u)" != "0" ]; then
    echo "This script must be run as root" 1>&2
    exit 1
fi

disk_image=${*: -1:1}
if [[ !(-f ${disk_image}) ]]; then
    echo "${disk_image} does not exist or is not a regular file" 1>&2
    exit 1;
fi
disk_image_size=$(du -b ${disk_image} | awk '{print $1}')

loopback_device=$1
if [[ !(-e ${loopback_device}) ]]; then
    echo "${loopback_device} does not exist" 1>&2
    exit 1;
elif [[ !(-b $1) ]]; then
    echo "${loopback_device} is not a block device" 1>&2
    exit 1;
fi

if [[ ${DISKTYPE} == "hd" ]]; then
    H=16
    S=63
    C=`expr ${disk_image_size} / ${H} / ${S}`
elif [[ ${DISKTYPE} == "fd" ]]; then
    C=80
    H=2
    S=18
else
    echo "\`${DISKTYPE}\` is not a valid value for disk type, only possible values are \"hd\" and \"fd\"" 1>&2
    usage
fi

#echo "loopback device = ${loopback_device}"
#echo "disk type = $DISKTYPE"
#echo "disk image = ${disk_image}"

# get the FAT partition name and offset
#######################################
losetup ${loopback_device} ${disk_image}
if [ $? -ne 0 ]; then
    losetup -d ${loopback_device}
    losetup ${loopback_device} ${disk_image}
fi

FAT16_line=$(fdisk -c=nondos -u=sectors -C ${C} -H ${H} -S ${S} -b ${SECTORSIZE} -l ${loopback_device} | grep FAT16 | awk '{print $1,$3,$4}')

device_partition=$(echo ${FAT16_line} | awk '{print $1}')
device_partition_sector_start=$(echo ${FAT16_line} | awk '{print $2}')
device_partition_sector_end=$(echo ${FAT16_line} | awk '{print $3}')
device_partition_size=`expr \( ${device_partition_sector_end} - ${device_partition_sector_start} + 1 \) \* ${SECTORSIZE}`

losetup -d ${loopback_device}

# mount the FAT partition and format it
#######################################
echo "Mount partition ${device_partition} (begin=${device_partition_sector_start}, end=${device_partition_sector_end}), size=${device_partition_size} bytes"
#losetup --offset ${device_partition_sector_start} --sizelimit ${device_partition_size} ${loopback_device} ${disk_image}
losetup ${loopback_device} ${disk_image}
echo "Format partition ${device_partition} to FAT16"
mkdosfs -v -F 16 ${device_partition}
losetup -d ${loopback_device}

exit 0
