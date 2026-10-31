# Planck EZ

| | |
|---|---|
| Board | [Planck EZ](https://ergodox-ez.com/pages/planck), 4×12 ortholinear, STM32F303 |
| Framework | QMK via ZKF, target `planck/ez` |
| Firmware | `firmwares/planck.bin`: `./project.sh build planck` or `./project.sh download planck` |
| Flash tool | QMK Toolbox (STM32 DFU); legacy: `./project.sh flash planck` (`wally-cli`) |
| Layout | [layout viewer](https://zored.github.io/zkf/) → `planck`; source: `keyboards.planck` in [config.yaml](../../config.yaml) |

## Enter bootloader

- Hold `'` (`quoteDance`, hold → `emoji` layer), then press the bottom-left key (`doBootloader`).
- Or press the reset button on the bottom of the case.

## Flash

1. Open `firmwares/planck.bin` in QMK Toolbox.
2. Enter bootloader; Toolbox reports an STM32 DFU device.
3. Flash.

## Specifics

- Audio with clicky sounds, RGB keys, 2 LEDs for the active layer.
- `history` layer on `a` hold.
