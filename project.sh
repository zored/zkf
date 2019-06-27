#/usr/bin/env bash
# Contains helper code for easy work.
set -e

keyboard=ergodox_ez
case $2 in
  planck|p|2)
    keyboard=planck/ez
    ;;
esac

pre-run () {
  local machine=${DOCKER_MACHINE:-default}
  docker-machine start $machine &> /dev/null || true
  eval $(docker-machine env $machine) || true
}

run () {
  pre-run
  local image=$1
  shift
  docker run --rm \
    -v "/$PWD:/build" \
    --workdir=//build \
    $image \
    //bin/sh -c "$*"
}

if [[ "$OSTYPE" == "darwin"* ]]; then
  export OS=MACOSX
elif [[ "$OSTYPE" == "cygwin" ]] || [[ "$OSTYPE" == "msys" ]] || [[ "$OSTYPE" == "win32" ]]; then
  export OS=WINDOWS
else
  echo 'Could not guess your OS.'
  exit 1
fi

export QMK_DIR=vendor/qmk_firmware
teensy_rel=vendor/teensy_${OS}
node_image=node:12.4.0-alpine
qmk_image=qmkfm/qmk_firmware


case $keyboard in
  planck/ez)
    firmware_source=$QMK_DIR/planck_ez_zored.bin
    firmware=firmwares/planck.bin
    ;;
  ergodox_ez)
    firmware_source=$QMK_DIR/ergodox_ez_zored.hex
    firmware=firmwares/ergodox_ez.hex
    ;;
esac
#qmk_image=zored/alebastr-qmk-whitefox-keymap

case $1 in
 build|b)
  [[ -e $QMK_DIR ]] || $0 sync
  run $node_image node compiler/run.js $keyboard

  echo "Building firmware..."
  run $qmk_image "cd $QMK_DIR && make $keyboard:zored"
  mv $firmware_source $firmware
 ;;

 sync|s)
  echo "Install keymap compiler."
  run $node_image "\
    cd compiler/ &&\
    yarn install &&\
    ./node_modules/eslint/bin/eslint.js --fix run.js \
  "

  echo "Get and patch QMK."
  git submodule update --init --recursive
  for patch in patches/*; do
    patch -d $QMK_DIR --forward --no-backup-if-mismatch -r- -p1 -i $PWD/$patch || true
  done
  run $qmk_image "cd $QMK_DIR && make git-submodule"

  rm -rf $teensy_rel
  mkdir -p $_
  pushd $_
  case $OS in
    WINDOWS)
      wget https://www.pjrc.com/teensy/teensy_loader_cli_windows.zip -O teensy.zip
      unzip $_
      rm $_
      ;;
    MACOSX)
      git clone https://github.com/zored/teensy_loader_cli . || true
      git checkout zored || true
      make
      ;;
  esac
  popd
 ;;

 flash|f)
  cat <<'TEXT'

===========

ENTER BOOTLOADER ON YOUR ERGODOX
- Either push a little button on top-right.
- Or hold SPACE + B on reconnect.
- Press Bootloader button (`ZKC_BTL`)

TEXT

  case $OS in
    WINDOWS)
      # flasher="$teensy_rel/teensy_loader_cli.exe -mmcu=$mcu -w"
      flasher="'/c/Program Files (x86)/Wally/wally-cli.exe'"
      ;;
    MACOSX)
      flasher="$teensy_rel/teensy_loader_cli --mcu=atmega32u4 -v -w"
      ;;
  esac

  eval $flasher $firmware
 ;;

 build-and-flash|bf)
  $0 b $2
  $0 f $2
 ;;

 build-all|ba)
  for keyboard in ergodox planck; do
    $0 b $keyboard
  done
  ;;

 *)
  echo Unknown parameters.
  exit 1
 ;;
esac

