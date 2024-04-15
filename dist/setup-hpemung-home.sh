#!/usr/bin/env sh

DOTHPEMUNG=${DOTHPEMUNG:-.config/hpemung}

ROM=${ROM:-gxrom-r}

mkdir -p ~/"${DOTHPEMUNG}"

[ -e ~/"${DOTHPEMUNG}"/rom ] && rm ~/"${DOTHPEMUNG}"/rom
ln -s @PREFIX@/share/hpemung/ROMs/"$ROM" ~/"${DOTHPEMUNG}"/rom
cd ~/"${DOTHPEMUNG}"
