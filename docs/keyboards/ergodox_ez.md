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
- Enter: toggle `navigation`, then press the top inner key of the left half (`toggleSteno`). Press it again to leave `steno`; `navigation` stays on until `toggleNavigation`.
- LEDs: all three on. Keys outside the steno chart are `KC_NO`, so nothing falls through to lower layers.
- Plover: machine "Plover HID" (built in since Plover 5.1, earlier versions need the `plover-machine-hid` plugin). Not verified on hardware yet.
- macOS input source must be ABC (English) while stenoing: with a Russian layout Plover finds Latin letters only on the Cmd layer and sends `Cmd+H` for `h`, which hides the front app.
