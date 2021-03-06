#!/usr/bin/env bash
# Contains helper code for easy work.
set -ex

keyboards="ergodox_ez planck/ez ymdk/ymd09 annepro2/c18"
case $2 in
  planck|planck/ez|p|2)
    keyboard=planck/ez
    ;;
  ymd09|ymdk/ymd09|y|3)
    keyboard=ymdk/ymd09
    ;;
  annepro2|annepro2/c18|ap2|4)
    keyboard=annepro2/c18
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


AP2_DIR='vendor/AnnePro2-Tools.git'
AP2_SHINE='firmwares/annepro2-shine.bin'
AP2_QMK_DIR=vendor/ap2_qmk_firmware
export QMK_DIR=vendor/qmk_firmware
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
  annepro2/c18)
    firmware_source=$AP2_QMK_DIR/annepro2_c18_zored.bin
    firmware_filename=annepro2.bin
    flash_with=ap2
    ;;
esac
firmware=firmwares/$firmware_filename
mkdir -p firmwares/

case $1 in
 build|b) ##
  $0 transpile $keyboard

  echo "Building firmware..."
  $0 make "$QMK_DIR" "${keyboard}:zored"
  mv $firmware_source $firmware
 ;;

 make)
  run $qmk_image "cd $2 && make $3"
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

  case $2 in
    ci) ;;
    *) SYNC_QMK=Y SYNC_AP2=Y ;;
  esac

  if [[ "$SYNC_QMK" = "Y" ]]; then
    echo "Clone QMK with submodules."
    if [[ ! -f "$QMK_DIR/Makefile" ]]; then
      git clone -b zkf_stable_planck --single-branch --recurse-submodules https://github.com/zored/qmk_firmware.git $QMK_DIR
    fi
  fi

  if [[ "$SYNC_AP2" = "Y" ]]; then
    echo "Clone AnnePro2 QMK with submodules"
    if [[ ! -f "$AP2_QMK_DIR/Makefile" ]]; then
      git clone --recurse-submodules https://github.com/OpenAnnePro/qmk_firmware.git "$AP2_QMK_DIR"
      #git checkout keyboard-annepro2
    fi

    echo "Clone and compile AnnePro2 shine"
    AP2_SHINE_DIR=vendor/ap2_shine
    if [[ ! -f "$AP2_SHINE_DIR/readme.md" ]]; then
    git clone https://github.com/OpenAnnePro/annepro2-shine.git --recursive "$AP2_SHINE_DIR"
    fi
    if [[ ! -f "$AP2_SHINE" ]]; then
      $0 make $AP2_SHINE_DIR C18
      ls $AP2_SHINE_DIR/
      ls $AP2_SHINE_DIR/build
      ls $AP2_SHINE_DIR/build/C18
      mv $AP2_SHINE_DIR/build/C18/annepro2-shine-C18.bin $AP2_SHINE
    fi
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
    ap2)
      AP2="$AP2_DIR/target/release/annepro2_tools"
      if [[ ! -f "$AP2" ]]; then
        git clone git@github.com:OpenAnnePro/AnnePro2-Tools.git "$AP2_DIR"
        pushd "$AP2_DIR"
        RUSTFLAGS=-Awarnings cargo build --release
        popd
      fi

      case $3 in
        shine)
          $AP2 --boot -t led $AP2_SHINE
          ;;
        *)
          while ! $AP2 $firmware; do
            sleep 3
          done
          $0 $1 $2 shine
          ;;
      esac
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
  ./project.sh sync ci
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

