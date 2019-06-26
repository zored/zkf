#/usr/bin/env bash
set -ex

keyboard=ergodox_ez
case $2 in
  planck|p|2)
    keyboard=planck/ez
    ;;
esac

init-docker () {
  local machine=${DOCKER_MACHINE:-default}
  docker-machine start $machine || true
  eval $(docker-machine env $machine) || true
}


node_image=node:12.4.0-alpine
qmk_image=qmkfm/qmk_firmware
#qmk_image=zored/alebastr-qmk-whitefox-keymap
sync_file=q.mk

case $1 in
 build|b)
  if [[ ! -e $sync_file ]]; then
    $0 sync
  fi
  init-docker
  docker run --rm -v "/$PWD:/build" --workdir=//build $node_image compiler/run.js $keyboard
  export KEYBOARD=$keyboard
  docker run --rm -v "/$PWD:/build" --workdir=//build $qmk_image make
 ;;

 docker-build|d)
  init-docker
  docker build \
    --tag zored/zkf:latest \
    --file ./docker/Dockerfile \
    ./docker
 ;;

 sync|s)
  init-docker
  docker run --rm -v "/$PWD:/build" --workdir=//build/compiler $node_image yarn install --no-bin-links
  docker pull $qmk_image

  echo 'Updating QMK library'
  rm -f $sync_file
  make $_
 ;;

 teensy|t)
  mkdir -p vendor/teensy
  pushd $_
  git clone https://github.com/zored/teensy_loader_cli . || true
  git checkout zored || true
  OS=${2:-WINDOWS} make # MACOSX
  popd
 ;;

 flash|f)
  echo 'Retrieving HEX file.'
  hex=$(ls *${2}*.hex)
  count=$(echo $hex | wc -l)
  if [[ $count != '1' ]]; then
    echo "Found $count HEX files. Specify version: $hex"
  fi

  mcu='atmega32u4'
  teensy_vendor=./vendor/teensy/teensy_loader_cli
  if [[ -e $teensy_vendor ]]; then
    teensy="$teensy_vendor --mcu=$mcu -v"
  else
    # wget https://www.pjrc.com/teensy/teensy_loader_cli_windows.zip -O teensy.zip
    # unzip $_ -d .
    teensy="/d/zored/downloads/teensy_loader_cli.exe -mmcu=$mcu"
  fi

  cat <<'TEXT'

===========

ENTER BOOTLOADER ON YOUR ERGODOX
- Either push a little button on top-right.
- Or hold SPACE + B on reconnect.
- Press Bootloader button (`ZKC_BTL`)

TEXT

  $teensy -w $hex
 ;;

 build-and-flash|bf)
  rm -f .build/*.hex
  $0 build $2
  rm *.hex
  cp .build/*.hex build.hex
  $0 flash
 ;;

 *)
  echo Unknown parameters.
  exit 1
 ;;
esac

