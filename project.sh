#!/usr/bin/env bash
# Contains helper code for easy work.
set -ex

keyboards="ergodox_ez planck/ez ymdk/ymd09 dao44"
case $2 in
  planck|planck/ez|p|2)
    keyboard=planck/ez
    ;;
  ymd09|ymdk/ymd09|y|3)
    keyboard=ymdk/ymd09
    ;;
  dao|dao44|d|4)
    keybaord=dao44
    ;;
  *)
    keyboard=ergodox_ez
    ;;
esac

pre_run () {
  # Already running:
  docker ps &> /dev/null && return

  local machine=${DOCKER_MACHINE:-default}
  docker-machine start $machine &> /dev/null || true
  eval $(docker-machine env $machine) || true
}

run () {
  pre_run
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
  if which wally-cli; then
    wally="wally-cli"
  else
    wally="/usr/local/bin/wally-cli"
  fi
elif [[ "$OSTYPE" == "cygwin" ]] || [[ "$OSTYPE" == "msys" ]] || [[ "$OSTYPE" == "win32" ]]; then
  export OS=WINDOWS
  wally="'/c/Program Files (x86)/Wally/wally-cli.exe'"
else
  wally=''
fi

export QMK_DIR=vendor/qmk_firmware DAO_ZMK_DIR=vendor/dao_zmk
node_image=node:12.4.0-alpine
qmk_image=qmkfm/qmk_firmware
images="$node_image $qmk_image"


flash_with=wally
case $keyboard in
  planck/ez)
    firmware_source=$QMK_DIR/planck_ez_zored.bin
    firmware_filename=planck.bin
    ;;
  ergodox_ez)
    firmware_source=$QMK_DIR/ergodox_ez_zored.hex
    firmware_filename=ergodox_ez.hex
    ;;
  ymdk/ymd09)
    firmware_source=$QMK_DIR/ymdk_ymd09_zored.hex
    firmware_filename=ymd09.hex
    flash_with=dfu
    ;;
  dao44)
    ;;
esac
firmware=firmwares/$firmware_filename

case $1 in
 build|b) ##
  $0 transpile $keyboard
  if [ "$firmware_source" = '' ]; then
    exit 0
  fi
  echo "Building firmware..."
  run $qmk_image "cd $QMK_DIR && make $keyboard:zored"
  mv $firmware_source $firmware
 ;;

 transpile|t) ##
  [[ -e $QMK_DIR ]] || $0 sync
  echo "Transpiling JS to C..."
  run $node_image node compiler/run.js $keyboard
 ;;

 lint|l) ##
  run $node_image 'cd ./compiler/ && node_modules/eslint/bin/eslint.js --fix run.js'
 ;;

 sync|s) ##
  echo "Install keymap compiler."
  run $node_image 'yarn install --no-bin-links --cwd=compiler'

  echo "Clone QMK with submodules."
  if [[ ! -f "$QMK_DIR/Makefile" ]]; then
    git clone -b zkf_stable_planck --single-branch --recurse-submodules https://github.com/zored/qmk_firmware.git $QMK_DIR
    rm -rf $QMK_DIR/.git
  fi

  echo "Checking firmware flasher presence..."
  if [[ $wally != '' ]] && [[ ! -e $wally ]]; then
    echo "Install Wally ($wally):"
    echo "https://ergodox-ez.com/pages/wally"
    exit 1
  fi
 ;;

 download|d) ## Download firmware.
  echo "Downloading latest release."
  version=${version:-$(git describe --abbrev=0)}
  link=https://github.com/zored/zkf/releases/download/${version}/${firmware_filename}
  wget $link -O $firmware
  ;;

 wally-build|wb) ##
   mkdir -p vendor/wally
   cd $_
   git pull || git clone git@github.com:zsa/wally.git .
   if [[ "$OS" = 'MACOSX' ]]; then
     brew install libusb dfu-programmer
   fi
   go install github.com/wailsapp/wails/cmd/wails@v1.16.6
   wails build
   ;;

 provision-host)
  case $OS in
    MACOSX)
      sudo cp ./os/macos/Library/Keyboard\ Layouts/ /Library/Keyboard\ Layouts/*
      ;;
  esac
  ;;

 flash|f)
  cat <<'TEXT'

===========

ENTER BOOTLOADER ON YOUR KEYBOARD
- Either push a little reset button.
- Or hold SPACE + B on reconnect.
- Press Bootloader button (`ZKC_BTL`)

TEXT
  case $flash_with in
    dfu)
      run="dfu-programmer atmega32u4"
      while ! $run get bootloader-version; do
        sleep 1
      done
      $run erase --force
      $run flash $firmware
      $run reset || true
      ;;
    wally)
      if [[ $wally = '' ]]; then
        echo 'No wally defined (could not guess OS?).'
        exit 1
      fi
      flasher=$wally
      eval $flasher $firmware
      ;;
    *)
      echo "Unknown flash type: $flash_with"
      exit 1
      ;;
  esac
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
  for keyboard in $keyboards; do
    $0 build $keyboard
  done
  ;;

 transpile-all|ta)
  for keyboard in $keyboards; do
    $0 transpile $keyboard
  done
  ;;

 ci) ##
  ./project.sh sync
  ./project.sh build-all
  ./project.sh prune-images
  ;;

 prune-images)
  skip=""
  for i in $images; do
    skip="$skip $(docker image ls $i -q)"
  done

  docker stop -t 1 $(docker ps -q)

  for i in $(docker image ls -q); do
    for s in $skip; do
      if [[ "$i" == "$s" ]]; then
        i=""
        break
      fi
    done

    if [[ "$i" = "" ]]; then
      continue;
    fi

    docker image rm -f $i || echo 'Could not stop image'
  done
  ;;
 *)
  echo Unknown parameters.
  exit 1
 ;;
esac

