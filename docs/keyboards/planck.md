# Planck EZ

| | |
|---|---|
| Board | [Planck EZ Glow](https://ergodox-ez.com/pages/planck), 4×12 ortholinear, STM32F303, per-key RGB |
| Framework | QMK `0.34.7` via ZKF, target `zsa/planck_ez/glow` (`v2.2.1` used the 2020 fork's `planck/ez`) |
| Firmware | `firmwares/planck.bin`: `./project.sh build planck`; previous builds in `firmwares/previous/`; `v2.2.1` rollback: `firmwares/planck_v2.2.1.bin` |
| Flash tool | `./project.sh flash planck` (`dfu-util` bundled in QMK Toolbox) or the QMK Toolbox app |
| Layout | [layout viewer](https://zored.github.io/zkf/) → `planck`; source: `keyboards.planck` in [config.yaml](../../config.yaml) |

## Enter bootloader

- Hold `'` (`quoteDance`, hold → `emoji` layer), then press the bottom-left key (`doBootloader`).
- Or press the reset button on the bottom of the case.

## Flash

Install QMK Toolbox once: `brew install --cask qmk-toolbox`. The Toolbox app is verified on `v2.2.1` and `0.34.7` builds.

CLI (not yet verified end to end: the first run lost the device to an open Toolbox):

1. Quit QMK Toolbox: it claims the DFU device first (`LIBUSB_ERROR_ACCESS`); the script refuses to run while it is open.
2. `./project.sh flash planck`: waits for `0483:df11`, then runs `dfu-util -a 0 -d 0483:df11 -s 0x08000000:leave -D firmwares/planck.bin`.
3. Enter bootloader.

QMK Toolbox:

1. Open `firmwares/planck.bin`; the MCU dropdown does not matter for STM32 DFU.
2. Enter bootloader; Toolbox logs `STM32 DFU device connected: STMicroelectronics STM32  BOOTLOADER (0483:DF11:2200)`.
3. Flash; the log ends with `Flash complete` and the board restarts by itself.

Upstream builds report USB `3297:C6CE` (ZSA) instead of the fork's ID, and start with fresh EEPROM settings.

## Specifics

- Audio with clicky sounds, off on every boot (`clicky_off()` in `keyboard_post_init_user`); RGB matrix keys (`RM_*`); 2 LEDs for the active layer.
- Mouse keys tuned for smooth motion on a 240 Hz screen: a 1 px step every 4 ms (`MOUSEKEY_INTERVAL: 4`, `MOUSEKEY_MOVE_DELTA: 1`), top speed `26` px per tick, `200` ticks to reach it. Upstream raised the default `MOUSEKEY_MOVE_DELTA` from 5 to 8, which made the old values jerky.
- `history` layer on `a` hold.
- `navigation2` (tap `;`, then hold `;`) holds RGB, audio and OS-mode keys:
  - RGB: `q`/`w` brightness, `e`/`r` saturation; bottom row under Tab / Enter / `[` / `]` / last key: previous / next mode / toggle / hue down / hue up.
  - Audio: `a` toggles clicks, `s`/`d` click pitch, `z` toggles music mode (keys play notes instead of typing until toggled off).
  - `i` Windows mode, `o` macOS mode; `y` powers the host off and `u` puts it to sleep.
