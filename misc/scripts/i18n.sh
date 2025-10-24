#!/usr/bin/env bash

langs=(fr de es)


potfile=.build/vendr.pot
podir=i18n/po

mkdir -p $(dirname ${potfile})
mkdir -p ${podir}

sources=($(find src -name \*.cpp -o -name \*.hpp -! -name toml.hpp -! -name args.hxx -! -name i18n.hpp))

xgettext --keyword=_ --keyword=_n:1,2 -o ${potfile} ${sources[@]}

for language in ${langs[@]}
do
    pofile=${podir}/${language}.po
    if [ -r ${pofile} ]
    then
        msgmerge --update ${pofile} ${potfile}
    else
        gt_args=(--input=${potfile}
                 --locale=${language}.UTF-8
                 --output=${pofile}
                 --no-translator)
    
        msginit ${gt_args[@]}
    fi

done