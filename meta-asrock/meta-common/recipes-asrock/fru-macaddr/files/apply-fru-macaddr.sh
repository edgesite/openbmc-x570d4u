#!/bin/sh

set -eu

[ -e /etc/default/fru-macaddr ] || exit 0
. /etc/default/fru-macaddr

progname="$(basename $0)"
log() { logger -t "$progname" "$*"; }

# produces seven bytes as space-separated hex digit pairs -- first six
# are the MAC address, the last is a zero-sum checksum
get_mac_bytes()
{
	dd if="$1" bs=1 count=7 skip="$2" 2>/dev/null \
		| hexdump -e '6/1 "%02x " " %02x\n"'
}

checksum_valid()
{
	sum=0
	for b in $*; do
		sum=$((sum + 0x$b))
	done
	return $(((sum % 0x100) != 0))
}

frubytes="$(get_mac_bytes "$FRUMAC_FILE" "$FRUMAC_OFFSET")"

if ! checksum_valid "$frubytes"; then
	log "Error: invalid checksum in FRU MAC address"
	exit 1
fi

frumac="${frubytes% *}" # strip off last byte (checksum), leaving just the address
frumac="${frumac// /:}" # format appropriately (colon-separated)
curmac="$(cat /sys/class/net/$FRUMAC_DEV/address)"

ubootmac="$(fw_printenv -n ethaddr 2>/dev/null || :)"

if [ "$ubootmac" != "$frumac" ]; then
	fw_setenv ethaddr "$frumac"
	if [ -n "$ubootmac" ]; then
		log "u-boot ethaddr had bogus address! (Corrected from $ubootmac to $frumac from FRU EEPROM)"
	else
		log "Initialized unset u-boot ethaddr to $frumac from FRU EEPROM"
	fi
fi

if [ "$curmac" != "$frumac" ]; then
	ip link set dev "$FRUMAC_DEV" addr "$frumac"
	log "Applied FRU MAC address $frumac to $FRUMAC_DEV"
fi

curname="$(hostname)"
machine="$(. /etc/os-release && echo "$OPENBMC_TARGET_MACHINE")"
macname="$machine-${frumac//:/}"

if [ "$curname" = "$machine" ]; then
	hostnamectl set-hostname "$macname"
	log "Set hostname to $macname"
fi
