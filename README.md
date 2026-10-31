# ZKF
Zored Keyboard Firmwares — are [simply configured](config.yaml) keyboard firmwares based on [QMK](https://github.com/qmk/qmk_firmware).

[Github page is here](https://zored.github.io/zkf/): layout viewer and [workflow](https://zored.github.io/zkf/workflow.htm).

## Keyboards
| Keyboard | Framework | Docs |
|---|---|---|
| [ErgoDox EZ](https://ergodox-ez.com) | QMK via ZKF | [ergodox_ez.md](docs/keyboards/ergodox_ez.md) |
| [Planck EZ](https://ergodox-ez.com/pages/planck) | QMK via ZKF | [planck.md](docs/keyboards/planck.md) |
| [YMD09](https://github.com/qmk/qmk_firmware/tree/master/keyboards/ymdk/ymd09) | QMK via ZKF | [ymd09.md](docs/keyboards/ymd09.md) |
| [DAO44](https://github.com/yumagulovrn/dao-choc-ble) | [ZMK](https://github.com/zored/dao-zmk-config/tree/zored) | [dao44.md](docs/keyboards/dao44.md) |
| Anne Pro 2 | QMK (planned) | [annepro2.md](docs/keyboards/annepro2.md) |

Supports Windows and macOS.

Host helper: [zkf-host](docs/host.md) tells the keyboard the input source and the volume (absolute RU/EN switching, RGB overlays).

Steno: ErgoDox and Planck have a Plover HID layer, see [plover.md](docs/plover.md).

## Example
```bash
./project.sh build-and-flash planck
```

### How it works
- [YAML file](config.yaml) is a readable config.
- JavaScript code [compiles](compiler/README.md) config into keymap files.
- QMK ([my fork](https://github.com/zored/qmk_firmware/tree/zkf_stable_planck)) builds firmware in Docker.
- The host flashes firmware into the keyboard.

Roadmap: [plans.md](plans.md).

## Additional info
- To make Russian layout on MacOS same with Windows run `./project.sh provision-host` and choose `Русская BG46` as Russian input source.
