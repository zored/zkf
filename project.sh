#/usr/bin/env bash
set -ex

init-docker () {
  local machine=${DOCKER_MACHINE:-default}
  docker-machine start $machine || true
  eval $(docker-machine env $machine) || true
}

case $1 in
 build|b)
  init-docker
  required_files='visualizer.c'
  touch $required_files
  docker run --rm -v "/$PWD:/build" zored/alebastr-qmk-whitefox-keymap make || true
  rm -f $required_files
  ## mv ergodox_ez_zored.hex $hex
 ;;

 docker-build|d)
  init-docker
  docker build \
    --tag zored/alebastr-qmk-whitefox-keymap:latest \
    --file ./docker/Dockerfile \
    ./docker
 ;;

 sync|s)
  echo 'Updating QMK library'
  rm -f q.mk
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
  count=$(echo $hex | wc --lines)
  if [[ $count != '1' ]]; then
    echo "Found $count HEX files. Specify version. $hex"
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
  $0 flash
 ;;

 *)
  echo Unknown parameters.
  exit 1
 ;;
esac

