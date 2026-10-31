# ErgoDox EZ

| | |
|---|---|
| Board | [ErgoDox EZ](https://ergodox-ez.com), split, Teensy-compatible ATmega32U4 |
| Framework | QMK via ZKF, target `ergodox_ez` |
| Firmware | `firmwares/ergodox_ez.hex`: `./project.sh build ergodox_ez` or `./project.sh download ergodox_ez` |
| Flash tool | QMK Toolbox (HalfKay); legacy: `./project.sh flash ergodox_ez` (`wally-cli`) |
| Layout | [layout viewer](https://zored.github.io/zkf/) → `ergodox_ez`; source: `keyboards.ergodox_ez` in [config.yaml](../../config.yaml) |

## Enter bootloader

- Hold `'` (`quoteDance`, hold → `emoji` layer), then press the bottom-left key of the left half (`doBootloader`).
- Or press the reset button with a paperclip.

## Flash

1. Open `firmwares/ergodox_ez.hex` in QMK Toolbox.
2. Enter bootloader; Toolbox reports a HalfKay device.
3. Flash.

## Specifics

- Only board with `game`, `gameNumbers`, `gameMouse`, `gameMirror` layers and the turbo button.
- 3 LEDs show the active layer (`lights` per layer).
- Combos and leader are on.

## Steno (Plover)

- Protocol: Plover HID (`PLOVER_HID_ENABLE`); normal typing is not affected, so Plover can stay on.
- Enter: hold `;` and press the top inner key of the left half (`toggleSteno` on `navigation`). The same key leaves `steno`.
- Trillo extras: `X1` / `X2` on the outer pinky column of the home and Shift rows (`Х-` / `Ц-`), `X3` left of `A` in the bottom row (`О`).
- LEDs: all three on. Keys outside the steno chart are `KC_NO`, so nothing falls through to lower layers.
- Plover setup, English/Russian systems and the macOS input source: [plover.md](../plover.md). Not verified on hardware yet.
