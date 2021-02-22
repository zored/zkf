#!/bin/sh
# Contains helper code for easy work.
set -ex

keyboards="ergodox_ez planck/ez ymdk/ymd09"
case $2 in
  planck|planck/ez|p|2)
    keyboard=planck/ez
    ;;
  ymd09|ymdk/ymd09|y|3)
    keyboard=ymdk/ymd09
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
esac
firmware=firmwares/$firmware_filename

case $1 in
 build|b) ##
  $0 transpile $keyboard

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

 upgrade|u) ## Update submodules.
  git submodule update --remote
  $0 s
 ;;

 sync|s) ##
  echo "Install keymap compiler."
  run $node_image 'yarn install --no-bin-links --cwd=compiler'

  echo "Get and patch QMK."
  for d in vendor vendor/qmk_firmware; do
    (
      cd $d 
      ls -lAc .
      git status
    ) || true
  done
  git submodule update --init --recursive || git submodule sync --recursive
  run $qmk_image "cd $QMK_DIR && make git-submodule"

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
   go build -o wally-cli cli/main.go
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

 *)
  echo Unknown parameters.
  exit 1
 ;;
esac

