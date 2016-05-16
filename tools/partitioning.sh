#!/bin/bash

PROGNAME=${0##*/}

usage() { echo "Usage: ${PROGNAME} {loopback device} [-t|--type <hd|fd>] [-s|--sector-size <size of a sector in bytes>] [-d|--disk-size <disk size in number of sectors>] {-o|--output <disk image>} {bootsector code} {bootloader}" 1>&2; exit 1; }

if [[ !($# -gt 0) ]]; then
    usage
fi

SHORTOPTS="hvt:s:d:o:"
LONGOPTS="help,version,disk-type:,sector-size:,disk-size:output:"

ARGS=$(getopt -s bash --options $SHORTOPTS  \
      --longoptions $LONGOPTS --name $PROGNAME -- "$@" )

if [ $? != 0 ]; then
    usage
fi

eval set -- "$ARGS"

VERBOSE=false
SECTORSIZE=512
DISKTYPE="hd"
DISKSIZE=131072
disk_image=

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

        -d | --disk-size )
            DISKSIZE="$2";
            shift 2
            ;;

        -o | --output )
            disk_image="$2";
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

if [[ -z ${disk_image} ]]; then
    echo "The output file of the disk image has not been specified" 1>&2
    exit 1;
fi

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
    C=`expr ${DISKSIZE} / \( $H \* $S \)`
elif [[ ${DISKTYPE} == "fd" ]]; then
    DISKSIZE=2880
    H=2
    S=18
    C=80
else
    echo "\`${DISKTYPE}\` is not a valid value for disk type, only possible values are \"hd\" and \"fd\"" 1>&2
    usage
fi

boot_sector_code=${*: -2:1}
if [[ !(-f ${boot_sector_code}) ]]; then
    echo "${boot_sector_code} does not exist or is not a regular file" 1>&2
    exit 1;
fi

bootloader_code=${*: -1:1}
if [[ !(-f ${bootloader_code}) ]]; then
    echo "${bootloader_code} does not exist or is not a regular file" 1>&2
    exit 1;
fi

#echo "loopback device = ${loopback_device}"
#echo "sector size = $SECTORSIZE bytes"
#echo "disk type = $DISKTYPE"
#echo "disk size = $DISKSIZE sectors"
#echo "disk image = ${disk_image}"
#echo "boot sector code = ${boot_sector_code}"
#echo "bootloader code = ${bootloader_code}"

cat ${boot_sector_code} ${bootloader_code} /dev/zero | dd of=${disk_image} bs=${SECTORSIZE} count=${DISKSIZE}
losetup ${loopback_device} ${disk_image}
if [ $? -ne 0 ]; then
    losetup -d ${loopback_device}
    losetup ${loopback_device} ${disk_image}
fi


# fdisk commands (main menu)
fdisk_cmd_quit_without_saving='q'
fdisk_cmd_print_partition_table='p'
fdisk_cmd_add_new_partition='n'
fdisk_cmd_toggle_bootable_flag='a'
fdisk_primary_partition='p'
fdisk_cmd_change_type='t'
fdisk_cmd_write_table_and_exit='w'

# fdisk commands (expert menu)
fdisk_cmd_enter_expert_menu='x'
fdisk_cmd_return_to_main_menu='r'
fdisk_cmd_change_disk_identifier='i'
fdisk_cmd_print_rawdata_partition_table='d'

# disk properties (hard disks only)
part_n1='1'
part_n1_first_sector='2048'
part_n1_last_sector=`expr \( ${DISKSIZE} - ${part_n1_first_sector} \) / 2`
part_n1_type='6' # FAT16
part_n2='2'
part_n2_first_sector=`expr ${part_n1_last_sector} + 1`
part_n2_last_sector=`expr ${DISKSIZE} - 1`
part_n2_type='80' # Old Minix
part_n3='3'
part_n4='4'
new_disk_identifier='0xdeadcafe'


fdisk -c=nondos -u=sectors -C ${C} -H ${H} -S ${S} -b ${SECTORSIZE} ${loopback_device} <<EOF
${fdisk_cmd_enter_expert_menu}
${fdisk_cmd_change_disk_identifier}
${new_disk_identifier}
${fdisk_cmd_return_to_main_menu}
${fdisk_cmd_add_new_partition}
${fdisk_primary_partition}
${part_n1}
${part_n1_first_sector}
${part_n1_last_sector}
${fdisk_cmd_add_new_partition}
${fdisk_primary_partition}
${part_n2}
${part_n2_first_sector}
${part_n2_last_sector}
${fdisk_cmd_change_type}
${part_n1}
${part_n1_type}
${fdisk_cmd_change_type}
${part_n2}
${part_n2_type}
${fdisk_cmd_toggle_bootable_flag}
${part_n1}
${fdisk_cmd_enter_expert_menu}
${fdisk_cmd_print_rawdata_partition_table}
${fdisk_cmd_return_to_main_menu}
${fdisk_cmd_print_partition_table}
${fdisk_cmd_write_table_and_exit}
EOF

losetup -d ${loopback_device}

exit 0
