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
| `keyboards.<kb>.osDetection` | `true`: QMK OS detection (`OS_DETECTION_ENABLE`, `OS_DETECTION_KEYBOARD_RESET`, `OS_DETECTION_SINGLE_REPORT`) picks the OS mode on every connect |
| `keyboards.<kb>.hostState` | `true`: Raw HID link to the [host helper](../docs/host.md) (`RAW_ENABLE`, `DEFERRED_EXEC_ENABLE`): input source and volume from the host, absolute RU/EN switching |
| `keyboards.<kb>.rgbOverlay` | RGB matrix overlays (`DEFERRED_EXEC_ENABLE`). Also makes `RM_TOGG` toggle `LED_FLAG_NONE` instead of disabling the matrix. `os: {ms, macos: [r, g, b], windows: [r, g, b]}` colours the whole board on connect, needs `osDetection`. `layout: {ms, colors: [[r, g, b]...], other}` colours the whole board on an input source change, by `--layouts` index; needs `hostState`. `volume: {ms, first, count, color, muted, none}` draws the volume as a bar on LEDs `first`..`first + count - 1` (all `muted` when muted, all `none` for an output without volume); needs `hostState` |
| `keyboards.<kb>.combos` | combos on a fixed letter-pair grid (`we rt yu io / sd fg hj kl / vb nm`) |
| `keyboards.ymd09.mappings` | before/after strings: `ru` phonetic Russian, `en_fast` / `ru_fast` alternate layouts |
| `keys.<name>Dance` | tap dances: `tap: [[seq]...]`, `hold: [[seq]...]`, indexed by tap count |
| `dance_enemies` | two groups of mod dances: holding one group forces the other group to tap |
| `map.keys` | aliases to QMK codes; Cyrillic letters map to their ЙЦУКЕН positions |
| `map.prefixes` | codes that skip the `KC_` prefix |

Parts behind a disabled flag emit no code.

Key names: `doXxx` → custom keycode `DO_XXX`, `holdX` / `toggleX` → `MO()` / `TG()`, an emoji char → unicodemap `X()`.

## Template features

- **OS mode** (macOS / Windows): `UC_MAC` / `UC_WINC` switch it, restored from the unicode mode at boot. With `osDetection` the host OS sets it on connect (macOS, iOS → macOS; Windows, Linux → Windows; unsure → keeps the saved mode) together with the unicode mode; the keys override it until the next connect. On macOS Ctrl↔Gui swap and media prev/next become rewind/fast-forward.
- **RGB overlays** (`rgbOverlay`): drawn in `rgb_matrix_indicators_advanced_user` over the running animation on an LED range, expired by `defer_exec`.
- **Host state** (`hostState`): `raw_hid_receive` keeps the layout index and volume from the helper, each unknown 15 s after its last report; protocol in [docs/host.md](../docs/host.md).
- **Absolute language**: `doLayoutEn` / `doLayoutRu` ask the helper for layout index 0 / 1 (no-op if active); without `hostState` or with an unknown layout they act as `doNextLanguage`.
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
