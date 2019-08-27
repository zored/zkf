#!/usr/bin/env bash
# Contains helper code for easy work.
set -ex

keyboard=ergodox_ez
case $2 in
  planck|p|2)
    keyboard=planck/ez
    ;;
esac

pre-run () {
  # Already running:
  docker ps &> /dev/null && return

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
  wally="/usr/local/bin/wally-cli"
elif [[ "$OSTYPE" == "cygwin" ]] || [[ "$OSTYPE" == "msys" ]] || [[ "$OSTYPE" == "win32" ]]; then
  export OS=WINDOWS
  wally="'/c/Program Files (x86)/Wally/wally-cli.exe'"
else
  wally=''
fi

export QMK_DIR=vendor/qmk_firmware
node_image=node:12.4.0-alpine
qmk_image=qmkfm/qmk_firmware


case $keyboard in
  planck/ez)
    firmware_source=$QMK_DIR/planck_ez_zored.bin
    firmware_filename=planck.bin
    ;;
  ergodox_ez)
    firmware_source=$QMK_DIR/ergodox_ez_zored.hex
    firmware_filename=ergodox_ez.hex
    ;;
esac
firmware=firmwares/$firmware_filename
#qmk_image=zored/alebastr-qmk-whitefox-keymap

case $1 in
 build|b)
  [[ -e $QMK_DIR ]] || $0 sync
  run $node_image node compiler/run.js $keyboard

  echo "Building firmware..."
  run $qmk_image "cd $QMK_DIR && make $keyboard:zored"
  mv $firmware_source $firmware
 ;;

 upgrade|u)
  git submodule update --remote
  $0 s
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

  echo "Checking firmware flasher presence..."
  if [[ $wally != '' ]] && [[ ! -e $wally ]]; then
    echo "Install Wally ($wally):"
    echo "https://ergodox-ez.com/pages/wally"
    exit 1
  fi
 ;;

 download|d)
  echo "Downloading latest release."
  version=${version:-$(git describe --abbrev=0)}
  link=https://github.com/zored/zkf/releases/download/${version}/${firmware_filename}
  wget $link -O $firmware
  ;;

 wally-build|wb)
   mkdir -p vendor/wally
   cd $_
   git pull || git clone git@github.com:zsa/wally.git .
   if [[ "$OS" = 'MACOSX' ]]; then
     brew install libusb
   fi
   go build -o wally-cli cli/main.go
   ;;

 flash|f)
  if [[ $wally = '' ]]; then
    echo 'No wally defined (could not guess OS?).'
    exit 1
  fi
  cat <<'TEXT'

===========

ENTER BOOTLOADER ON YOUR ERGODOX
- Either push a little button on top-right.
- Or hold SPACE + B on reconnect.
- Press Bootloader button (`ZKC_BTL`)

TEXT

  flasher=$wally

  eval $flasher $firmware
 ;;

 download-and-flash|df)
  $0 d $2
  $0 f $2
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

