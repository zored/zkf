[![Build Status](https://travis-ci.org/zored/zkf.svg?branch=master)](https://travis-ci.org/zored/zkf)

# ZKF
Zored Keyboard Firmwares — are [simply configured](config.yaml) keyboard firmwares based on [QMK](https://github.com/qmk/qmk_firmware). I have:
- [Ergodox](https://ergodox-ez.com).
- [Planck](https://ergodox-ez.com/pages/planck).

Supports Windows and macOS.

## Example
```bash
./project.sh build-and-flash planck
```

### How it works
- [YAML file](config.yaml) is a readable config.
- JavaScript code [compiles](compiler/README.md) config into keymap files.
- QMK (with [my patches](patches/)) builds firmware.
- Teensy flashes firmware into keyboard.
