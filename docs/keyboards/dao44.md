# DAO44

| | |
|---|---|
| Board | [DAO choc BLE](https://github.com/yumagulovrn/dao-choc-ble), split wireless, nRF52840 |
| Framework | ZMK, fork [ftc/zmk@mouse-ftc](https://github.com/ftc/zmk/tree/mouse-ftc) (mouse keys) |
| Config repo | [zored/dao-zmk-config@zored](https://github.com/zored/dao-zmk-config/tree/zored) (not ZKF) |
| Firmware | `dao_left-zmk.uf2`, `dao_right-zmk.uf2`: built in Docker by `./run.sh image flow` in the config repo |
| Flash tool | none: copy `.uf2` to the `NRF52BOOT` drive (`./run.sh flash left <dir>` automates it on macOS) |
| Layout | [dao.keymap](https://github.com/zored/dao-zmk-config/blob/zored/config/boards/arm/dao/dao.keymap) |

## Enter bootloader

Slide power to `OFF`, connect USB-C, press `RESET` twice. The `NRF52BOOT` drive appears.

## Flash

1. Copy `dao_left-zmk.uf2` to `NRF52BOOT` on the left half; the drive ejects itself ("File Transfer Error" is expected).
2. Repeat for the right half with `dao_right-zmk.uf2`.

## Pair halves

Power both halves off, then on, then press `RESET` once on both at the same time.

## Specifics

- `settings_reset` firmware (`nice_nano_v2`) is built too: flash it to clear Bluetooth bonds.
- Upstream FAQ: [docs/FAQ.md](https://github.com/zored/dao-zmk-config/blob/zored/docs/FAQ.md).
