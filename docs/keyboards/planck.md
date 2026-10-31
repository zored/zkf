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

Verified with `v2.2.1`.

1. Install QMK Toolbox once: `brew install --cask qmk-toolbox`.
2. Open `firmwares/planck.bin` in QMK Toolbox; the MCU dropdown does not matter for STM32 DFU.
3. Enter bootloader; Toolbox logs `STM32 DFU device connected: STMicroelectronics STM32  BOOTLOADER (0483:DF11:2200)`.
4. Flash; the log ends with `Flash complete` and the board restarts by itself.

## Specifics

- Audio with clicky sounds, RGB keys, 2 LEDs for the active layer.
- `history` layer on `a` hold.
- `navigation2` (tap `;`, then hold `;`) holds RGB, audio and OS-mode keys:
  - RGB: `q`/`w` brightness, `e`/`r` saturation; bottom row under Tab / Enter / `[` / `]` / last key: previous / next mode / toggle / hue down / hue up.
  - Audio: `a` toggles clicks, `s`/`d` click pitch, `z` toggles music mode (keys play notes instead of typing until toggled off).
  - `i` Windows mode, `o` macOS mode; `y` powers the host off and `u` puts it to sleep.
