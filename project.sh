#/usr/bin/env bash
set -ex

init-docker () {
  local machine=${DOCKER_MACHINE:-default}
  docker-machine start $machine || true
  eval $(docker-machine env $machine) || true
}

export KEYMAP_VERSION=$(git describe --abbrev=6 --always --tags 2>/dev/null)
export TARGET=ergodox_ez_zored_${KEYMAP_VERSION}

NODE_IMAGE=node:12.4.0-alpine
QMK_IMAGE=qmkfm/qmk_firmware
# QMK_IMAGE=zored/alebastr-qmk-whitefox-keymap
SYNC_FILE=q.mk

case $1 in
 build|b)
  if [[ ! -e $SYNC_FILE ]]; then
    $0 sync
  fi
  required_files='visualizer.c'
  touch $required_files
  init-docker
  docker run --rm -v "/$PWD:/build" --workdir=//build $NODE_IMAGE compiler/run.js
  docker run --rm -v "/$PWD:/build" --workdir=//build $QMK_IMAGE make
  rm -f $required_files
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
  docker run --rm -v "/$PWD:/build" --workdir=//build/compiler $NODE_IMAGE yarn install
  docker pull $QMK_IMAGE

  cd compiler
  yarn install
  cd -
  echo 'Updating QMK library'
  rm -f $SYNC_FILE
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
  $0 build
  $0 flash $2
 ;;

 *)
  echo Unknown parameters.
  exit 1
 ;;
esac

