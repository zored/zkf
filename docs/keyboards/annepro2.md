# Anne Pro 2

Status: not at hand, revision unknown. Plan: Phase 5 in [plans.md](../../plans.md).

| | |
|---|---|
| Board | Anne Pro 2, 60%, two HT32 MCUs (keys + LEDs), Bluetooth |
| Framework | upstream QMK `annepro2/c15` or `annepro2/c18`; old fork branch `zkf_stable_ap2` is archived |
| LED firmware | [OpenAnnePro/AnnePro2-Shine](https://github.com/OpenAnnePro/AnnePro2-Shine) on the LED MCU |
| Flash tool | [OpenAnnePro/AnnePro2-Tools](https://github.com/OpenAnnePro/AnnePro2-Tools) (`annepro2_tools`) or [prifak/annepro2_flasher_c](https://github.com/prifak/annepro2_flasher_c); QMK Toolbox does not support it |
| Layout | not in ZKF yet |

## Enter bootloader (IAP)

- Unplug, hold `Esc`, plug in.
- With QMK and Shine installed: `LShift` + `RShift` + `B` (per the OpenAnnePro site, unverified).

## Flash

1. Main MCU: `annepro2_tools <qmk>.bin`.
2. LED MCU (Shine): `annepro2_tools --boot -t led <shine>.bin`.

## Specifics

- QMK on the main MCU drives LEDs through Shine over UART; stock Obins LED firmware does not understand QMK.
- Bluetooth keycodes: `KC_AP2_BT1..4`, `KC_AP2_BT_UNPAIR`, `KC_AP2_USB`.
