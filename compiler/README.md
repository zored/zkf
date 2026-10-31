# ZKF compiler
- Compiles keymap files:
    + `keymap.c` with basic behavor (from [template](template/zored_keymap.c)).
    + `config.h` is header file with code definitions.
    + `rules.mk` is Makefile with parameters.
- Puts these files in `vendor/qmk_firmware/keyboards/<kb>/keymaps/zored/` and a copy in `example/<kb>/`.

Entrypoint is [here](run.js).

## config.yaml

| Key | Meaning |
|---|---|
| `keyboards.<kb>.layers.<name>` | `keys` (rows or split blocks like `left`, `right-thumb`), `lights` (LED indices on), `enableCombos` |
| `keyboards.<kb>.settings.config` | becomes `config.h`; custom knobs: `TURBO_*`, `APP_SWITCH_TIMEOUT`, `MOUSEKEY_SLOW/FAST_SPEED`, `TAPPING_TERM_TAP_DANCE` |
| `keyboards.<kb>.settings.rules` | becomes `rules.mk`; Planck and YMD09 inherit ErgoDox's via YAML anchors |
| `keyboards.<kb>.combos` | combos on a fixed letter-pair grid (`we rt yu io / sd fg hj kl / vb nm`) |
| `keyboards.ymd09.mappings` | before/after strings: `ru` phonetic Russian, `en_fast` / `ru_fast` alternate layouts |
| `keys.<name>Dance` | tap dances: `tap: [[seq]...]`, `hold: [[seq]...]`, indexed by tap count |
| `dance_enemies` | two groups of mod dances: holding one group forces the other group to tap |
| `map.keys` | aliases to QMK codes; Cyrillic letters map to their ЙЦУКЕН positions |
| `map.prefixes` | codes that skip the `KC_` prefix |

Key names: `doXxx` → custom keycode `DO_XXX`, `holdX` / `toggleX` → `MO()` / `TG()`, an emoji char → unicodemap `X()`.

## Template features

- **OS mode** (macOS / Windows): `UC_M_OS` / `UC_M_WC` switch it, auto-detected from unicode mode at boot. On macOS Ctrl↔Gui swap and media prev/next become rewind/fast-forward.
- **`DO_*` actions**, OS-aware: copy, paste, close, screenshot, next language, find begin/end, launch Enpass / terminal, type email and login, `->`, `=>`, `!=`, `_`, `&`, emoji panel, history back/forward, bootloader.
- **One-shot mods**: `DO_ONE_SHOT_CTRL/ALT/GUI/SHIFT`.
- **Turbo**: `DO_ONE_SHOT_TURBO`, the next key auto-repeats while held.
- **App / window / tab switcher**: holds Alt/Cmd + Tab with a timeout.
- **Mouse speed**: `DO_MOUSE_SLOW/FAST`.
- **Tap-dance engine**: home-row mods, flush on many taps, `dance_enemies`.
- **Combos**, enabled per layer in `layer_state_set_user`.
- **Leader**: `U` types login, `P` opens Enpass, `S` takes a screenshot.
- **Layer LEDs** on ErgoDox and Planck.
- **Mappings** (YMD09): `DO_NEXT_MAPPING` cycles letter layouts and switches OS language.
