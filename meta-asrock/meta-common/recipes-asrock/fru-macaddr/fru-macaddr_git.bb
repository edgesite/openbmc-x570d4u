SUMMARY = "Script to apply MAC address from ASRock Rack FRU EEPROMs"
LICENSE = "Apache-2.0"
LIC_FILES_CHKSUM = "file://${COMMON_LICENSE_DIR}/Apache-2.0;md5=89aea4e17d99a7cacdbeed46a0096b10"

SRC_URI = "file://apply-fru-macaddr.sh file://apply-fru-macaddr.service"
S = "${WORKDIR}"

inherit systemd
inherit obmc-phosphor-systemd

DEPENDS += "systemd"
RDEPENDS:${PN} += "bash"

SYSTEMD_PACKAGES = "${PN}"
SYSTEMD_SERVICE:${PN} += "apply-fru-macaddr.service"

do_install:append(){
    install -d ${D}${sbindir} ${D}${systemd_unitdir}/system/
    install -m 0755 ${WORKDIR}/apply-fru-macaddr.sh ${D}${sbindir}
    install -m 0644 ${WORKDIR}/apply-fru-macaddr.service ${D}${systemd_unitdir}/system/
    if [ -e ${WORKDIR}/fru-macaddr ]; then
        install -d ${D}${sysconfdir}/default
        install -m 0644 ${WORKDIR}/fru-macaddr ${D}${sysconfdir}/default
    fi
}
