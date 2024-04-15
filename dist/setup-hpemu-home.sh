#!/usr/bin/env sh

DOTHPEMU=${DOTHPEMU:-.config/hpemu}

ROM=${ROM:-gxrom-r}

mkdir -p ~/"${DOTHPEMU}"

[ -e ~/"${DOTHPEMU}"/rom ] && rm ~/"${DOTHPEMU}"/rom
ln -s @PREFIX@/share/hpemu/ROMs/"$ROM" ~/"${DOTHPEMU}"/rom
cd ~/"${DOTHPEMU}"
