SUMMARY = "An interpreter of object-oriented scripting language"
DESCRIPTION = "Ruby is an interpreted scripting language for quick \
and easy object-oriented programming. It has many features to process \
text files and to do system management tasks (as in Perl). \
It is simple, straight-forward, and extensible. \
"
HOMEPAGE = "http://www.ruby-lang.org/"
SECTION = "devel/ruby"
LICENSE = "Ruby | BSD-2-Clause | BSD-3-Clause | GPLv2 | ISC | MIT"
LIC_FILES_CHKSUM = "file://COPYING;md5=5b8c87559868796979806100db3f3805 \
                    file://BSDL;md5=8b50bc6de8f586dc66790ba11d064d75 \
                    file://GPL;md5=b234ee4d69f5fce4486a80fdaf4a4263 \
                    file://LEGAL;md5=f260190bc1e92e363f0ee3c0463d4c7c \
                    "

DEPENDS = "ruby-native zlib openssl libyaml gdbm readline libffi"
DEPENDS:class-native = "openssl-native libyaml-native readline-native zlib-native"

SHRT_VER = "${@oe.utils.trim_version("${PV}", 2)}"
SRC_URI = "http://cache.ruby-lang.org/pub/ruby/${SHRT_VER}/ruby-${PV}.tar.gz \
           file://0001-extmk-fix-cross-compilation-of-external-gems.patch \
           file://0002-Obey-LDFLAGS-for-the-link-of-libruby.patch \
           "
UPSTREAM_CHECK_URI = "https://www.ruby-lang.org/en/downloads/"

inherit autotools ptest pkgconfig


# This snippet lets compiled extensions which rely on external libraries,
# such as zlib, compile properly.  If we don't do this, then when extmk.rb
# runs, it uses the native libraries instead of the target libraries, and so
# none of the linking operations succeed -- which makes extconf.rb think
# that the libraries aren't available and hence that the extension can't be
# built.

do_configure:prepend() {
    sed -i "s#%%TARGET_CFLAGS%%#$CFLAGS#; s#%%TARGET_LDFLAGS%%#$LDFLAGS#" ${S}/common.mk
    rm -rf ${S}/ruby/
}
