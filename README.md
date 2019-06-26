# ZKF
Zored Keyboard Firmwares — are keyboard firmwares based on [QMK](https://github.com/qmk/qmk_firmware). I have:
- [Ergodox](https://ergodox-ez.com).
- [Planck](https://ergodox-ez.com/pages/planck).

## Example
To build and flash Planck use this code:
```bash
./project.sh bf p
```

### How it works
- [YAML file](config.yaml) is a readable config.
- JavaScript code [compiles](compiler/README.md) config into keymap files.
- QMK (with [my patches](patches/)) builds firmware.
- Teensy flashes firmware into keyboard.

## TODO
- Compile combos with JS.
- Replace teensy with [wally](https://ergodox-ez.com/pages/wally).