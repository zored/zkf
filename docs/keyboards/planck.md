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

CLI (verified on a `0.34.7` build; `dfu-util` first prints `dfuERROR ... firmware is corrupt`, clears it and flashes, which is normal for STM32 DFU):

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
- Mouse keys tuned for smooth motion on a 240 Hz screen: a 1 px step every 4 ms (`MOUSEKEY_INTERVAL: 4`, `MOUSEKEY_MOVE_DELTA: 1`), top speed `26` px per tick, `200` ticks to reach it. Wheel: one line every 32 ms, half the shared 16 ms rate, same 2.9 s ramp (`MOUSEKEY_WHEEL_INTERVAL: 32`, `MOUSEKEY_WHEEL_TIME_TO_MAX: 90`). Upstream raised the default `MOUSEKEY_MOVE_DELTA` from 5 to 8, which made the old values jerky.
- `history` layer on `a` hold.
- A dance whose tap count reached its last action types the tap on release instead of waiting `TAPPING_TERM_TAP_DANCE` (270 ms): `tabDance` sends Tab at once, so `z` hold + Tab switches without delay.
- `navigation2` (tap `;`, then hold `;`) holds RGB, audio and OS-mode keys:
  - RGB: `q`/`w` brightness, `e`/`r` saturation, `f`/`g` animation speed (slow `32` on every boot); bottom row under Tab / Enter / `[` / `]` / last key: previous / next mode / toggle / hue down / hue up. Toggle switches the animation off with `LED_FLAG_NONE` and keeps the matrix enabled, so overlays still show; a matrix disabled by an older firmware is converted on boot.
  - Audio: `a` toggles clicks, `s`/`d` click pitch, `z` toggles music mode (keys play notes instead of typing until toggled off).
  - `i` Windows mode, `o` macOS mode (override until the next connect, see below); `y` powers the host off and `u` puts it to sleep.

## OS detection

- On every connect QMK fingerprints the host's USB descriptor requests (`OS_DETECTION_ENABLE`) and sets the OS mode and the unicode mode: macOS and iOS → macOS, Windows and Linux → Windows, unsure → the mode saved in EEPROM stays.
- The whole board flashes for 1 s: white for macOS, blue for Windows (`rgbOverlay.os` in [config.yaml](../../config.yaml)). No flash means the host was not recognised.
- KVM switching: `OS_DETECTION_KEYBOARD_RESET` resets the board when USB comes back while it stays powered, so detection runs again; `OS_DETECTION_SINGLE_REPORT` ignores the second report ARM Macs send minutes later.
- `i` / `o` on `navigation2` override the mode until the next connect.

## Steno (Plover)

- Protocol: Plover HID (`PLOVER_HID_ENABLE`); normal typing is not affected, so Plover can stay on.
- Enter: hold `;` and press the bottom-left key (`toggleSteno` on `navigation`): one key per hand. The bottom-left key leaves `steno`.
- Layout: number bar on the top row, `S T P H *` / `S K W R *` on the left, `* F P L T D` / `* R B G S Z` on the right, `A O` under Del / Backspace, `E U` under Tab / Enter. Trillo extras: `X1` / `X2` under Caps / left Shift (`Х-` / `Ц-`), `X3` left of `A` (`О`). Both LEDs on. Keys outside the steno chart are `KC_NO`, so nothing falls through to lower layers.
- Plover setup, English/Russian systems and the macOS input source: [plover.md](../plover.md). Verified with Plover 5.4.2 on macOS.
