#!/bin/sh

localedir="${1}"
set -- fr es
for lang in "${@}"
do
    installdir="${DESTDIR}/${localedir}/${lang}/LC_MESSAGES"
    mkdir -p "${installdir}"
    msgfmt "${MESON_SOURCE_ROOT}/i18n/po/${lang}.po" -o "${installdir}/vendr.mo"
done