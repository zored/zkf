#!/usr/bin/env bash
# Contains helper code for easy work.
set -ex

keyboards="ergodox_ez/base zsa/planck_ez/glow ymdk/ymd09"
case $2 in
  planck|zsa/planck_ez/glow|p|2)
    keyboard=zsa/planck_ez/glow
    ;;
  ymd09|ymdk/ymd09|y|3)
    keyboard=ymdk/ymd09
    ;;
  *)
    keyboard=ergodox_ez/base
    ;;
esac

run () {
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

export QMK_DIR=vendor/qmk_firmware
qmk_repo=https://github.com/qmk/qmk_firmware.git
qmk_ref=0.34.7
node_image=node:22-alpine
qmk_image=ghcr.io/qmk/qmk_cli
images="$node_image $qmk_image"


flash_with=wally
case $keyboard in
  zsa/planck_ez/glow)
    firmware_source=$QMK_DIR/zsa_planck_ez_glow_zored.bin
    firmware_filename=planck.bin
    flash_with=dfu-util
    ;;
  ergodox_ez/base)
    firmware_source=$QMK_DIR/ergodox_ez_base_zored.hex
    firmware_filename=ergodox_ez.hex
    ;;
  ymdk/ymd09)
    firmware_source=$QMK_DIR/ymdk_ymd09_zored.hex
    firmware_filename=ymd09.hex
    flash_with=dfu
    ;;
esac
firmware=firmwares/$firmware_filename

case $1 in
 build|b) ##
  $0 transpile $keyboard

  echo "Building firmware..."
  run $qmk_image "cd $QMK_DIR && make $keyboard:zored"
  if [[ -f $firmware ]] && ! cmp -s $firmware_source $firmware; then
    mkdir -p firmwares/previous
    cp $firmware firmwares/previous/$(date +%Y%m%d-%H%M%S)-$firmware_filename
  fi
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
  if [[ ! -e "$QMK_DIR" ]]; then
    git clone --branch $qmk_ref --depth 1 --recurse-submodules --shallow-submodules $qmk_repo $QMK_DIR
  fi
  actual_ref=$(git -C $QMK_DIR describe --tags --exact-match HEAD)
  if [[ "$actual_ref" != "$qmk_ref" ]]; then
    echo "$QMK_DIR is at '$actual_ref', expected '$qmk_ref': delete it and rerun sync."
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
      sudo cp ./os/macos/Library/Keyboard\ Layouts/* /Library/Keyboard\ Layouts/
      ;;
  esac
  ;;

 flash|f)
  cat <<'TEXT'

===========

ENTER BOOTLOADER ON YOUR KEYBOARD
- Press the bootloader key (`doBootloader`, see docs/keyboards/).
- Or push the reset button.

TEXT
  case $flash_with in
    dfu-util)
      dfu_util="/Applications/QMK Toolbox.app/Contents/Resources/dfu-util"
      if pgrep -xq "QMK Toolbox"; then
        echo "Quit QMK Toolbox first: it claims the DFU device (LIBUSB_ERROR_ACCESS)."
        exit 1
      fi
      until "$dfu_util" -l | grep -qi 0483:df11; do
        sleep 1
      done
      "$dfu_util" -a 0 -d 0483:df11 -s 0x08000000:leave -D $firmware
      ;;
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

