# Zored's QMK work

What I built on top of [QMK](https://github.com/qmk/qmk_firmware) and where it lives.

Two layers:

1. **ZKF** (this repo) — a YAML DSL + Node compiler that generates QMK keymaps, plus a Docker build wrapper.
2. **QMK fork** — [zored/qmk_firmware](https://github.com/zored/qmk_firmware), a single tap-dance core fix. Upstream has had an equivalent fix since 2022.

Plus two boards outside this pipeline: DAO44 (ZMK) and Anne Pro 2 (old QMK community fork).

## 1. ZKF wrapper

### Pipeline

```
config.yaml ──node compiler/run.js──▶ keymap.c / config.h / rules.mk
     │                                  ├─▶ vendor/qmk_firmware/keyboards/<kb>/keymaps/zored/
     │                                  └─▶ example/<kb>/   (committed copy for review)
     └─ fetched at runtime by docs/ (GitHub Pages layout viewer)

vendor/qmk_firmware (fork, branch zkf_stable_planck) ──make <kb>:zored──▶ firmwares/<kb>.{hex,bin}
```

`project.sh` runs everything in Docker: `node:12.4.0-alpine` for the compiler, `qmkfm/qmk_firmware` (frozen 2021 image, amd64 only) for `make`.

### config.yaml DSL

| Key | Meaning |
|---|---|
| `keyboards.<kb>.layers.<name>` | `keys` (rows or split blocks like `left`, `right-thumb`), `lights` (LED indices on), `enableCombos` |
| `keyboards.<kb>.settings.config` | becomes `config.h` (`#undef`/`#define` pairs); custom knobs: `TURBO_*`, `APP_SWITCH_TIMEOUT`, `MOUSEKEY_SLOW/FAST_SPEED`, `TAPPING_TERM_TAP_DANCE` |
| `keyboards.<kb>.settings.rules` | becomes `rules.mk`; Planck and YMD09 inherit ErgoDox's via YAML anchors |
| `keyboards.<kb>.combos` | combos on a fixed letter-pair grid (`we rt yu io / sd fg hj kl / vb nm`) |
| `keyboards.ymd09.mappings` | before/after strings: `ru` phonetic Russian, `en_fast` / `ru_fast` alternate layouts |
| `keys.<name>Dance` | tap dances: `tap: [[seq]...]`, `hold: [[seq]...]`, indexed by tap count |
| `dance_enemies` | two groups of mod dances: holding one group forces the other group to tap |
| `map.keys` | aliases to QMK codes; Cyrillic letters map to their ЙЦУКЕН positions |
| `map.prefixes` | codes that skip the `KC_` prefix |

Naming rules in `compiler/run.js`: `doXxx` → custom keycode `DO_XXX`, `holdX` / `toggleX` → `MO()` / `TG()`, an emoji char → unicodemap `X()`.

### Custom features (compiler/template/zored_keymap.c)

- **OS mode** `zored_os` (macOS / Windows): `UC_M_OS` / `UC_M_WC` switch it, auto-detected from unicode mode at boot. On macOS Ctrl↔Gui are swapped and media prev/next become rewind/fast-forward.
- **`DO_*` actions**, OS-aware: copy, paste, close, screenshot, next language, find begin/end, launch Enpass / terminal (ConEmu / iTerm), type my email and login, `->`, `=>`, `!=`, `_`, `&`, emoji panel, history back/forward, bootloader.
- **One-shot mods**: `DO_ONE_SHOT_CTRL/ALT/GUI/SHIFT`.
- **Turbo button** `DO_ONE_SHOT_TURBO`: the next key auto-repeats while held.
- **App / window / tab switcher**: holds Alt/Cmd + Tab with a timeout.
- **Mouse speed**: `DO_MOUSE_SLOW/FAST`.
- **Unicode emoji** via unicodemap; emoji dances (`happyDance`, `sadDance`, …).
- **Tap-dance engine**: home-row mods, flush on many taps, `dance_enemies`.
- **Combos**, enabled per layer in `layer_state_set_user`.
- **Leader**: `U` types login, `P` opens Enpass, `S` takes a screenshot.
- **Layer LEDs** on ErgoDox and Planck.
- **Mappings** (YMD09): `DO_NEXT_MAPPING` cycles letter layouts and switches OS language.

### Per keyboard

| Board | QMK target | Notes |
|---|---|---|
| ErgoDox EZ | `ergodox_ez` | `LAYOUT_ergodox`, 3 LEDs; only board with game / gameMouse / turbo layers |
| Planck EZ | `planck/ez` | `LAYOUT_planck_grid`, 2 LEDs, audio + clicky, RGB |
| YMD09 | `ymdk/ymd09` | 9 keys; every key is a tap/hold dance into a layer; RGB, mappings; no leader, no combos, no LEDs |

Details per board: [docs/keyboards/](docs/keyboards/).

## 2. QMK fork: zored/qmk_firmware

| Branch | Base (upstream) | My change | Status |
|---|---|---|---|
| `zkf_stable_planck` | `ae46e6a`, 2020-02-27 | tap-dance fix (4 commits) | **used by `project.sh sync`** |
| `zkf` (tag `zkf-1`) | `c868f93`, 2021-02-21 | same fix, unconditional | abandoned ("Use old commit") |
| `zkf_stable_ap2` | OpenAnnePro community branch, 2021 | same fix ported | Anne Pro 2 |
| `zored` | `01fb06a`, 2019-06-26 | old ErgoDox keymap inside QMK, `ACTION_TAP_DANCE_DOUBLE_TIME` | obsolete |

### The tap-dance fix

Files: `quantum/quantum.c`, `quantum/process_keycode/process_tap_dance.{c,h}`.

`process_record_quantum()` resolves the keycode once with the old layer state. Then `preprocess_tap_dance()` force-finishes an interrupted dance, whose `on_dance_finished` may call `layer_on()`. The rest of the quantum pipeline (`process_record_user`, quantum keycodes, other TD keys) still saw the stale keycode, so the next key typed fast after a layer dance came from the wrong layer.

Fix: `preprocess_tap_dance()` returns `bool` (a dance was actually interrupted), and `process_record_quantum()` then re-reads `keycode = get_record_keycode(record)`.

**Upstream has the same fix**: [qmk/qmk_firmware#17935](https://github.com/qmk/qmk_firmware/pull/17935) (`ca0c128`, 2022-10-03), covered by `tests/tap_dance/tap_dance_layers/` (`*Fast*` cases).

### History of patches/

Before Feb 2021 the fix lived in `patches/` here (removed in `8533f5f`, "Use qmk branch instead of patches"). Earlier (2019) patches added `TOP_DIR` build support, `ACTION_TAP_DANCE_DOUBLE_TIME` and C++ keymaps; all dropped by 2020. The repo started from Aleksei Bavshin's C++ layout generator (2019-01).

### Still useful?

Nothing unique. `ACTION_TAP_DANCE_DOUBLE_TIME` → upstream `TAPPING_TERM_PER_KEY` + `get_tapping_term(TD(x))`. The ErgoDox keymap is generated by ZKF now.

## 3. Outside the pipeline

- **DAO44** — split BLE board ([yumagulovrn/dao-choc-ble](https://github.com/yumagulovrn/dao-choc-ble)), nRF52840, ZMK. Config: [zored/dao-zmk-config@zored](https://github.com/zored/dao-zmk-config/tree/zored) on ZMK fork `ftc/zmk@mouse-ftc` (mouse keys). See [docs/keyboards/dao44.md](docs/keyboards/dao44.md).
- **Anne Pro 2** — branch `zkf_stable_ap2`. Upstream QMK supports it since 2022-03 (`keyboards/annepro2/c15|c18`). See [docs/keyboards/annepro2.md](docs/keyboards/annepro2.md).

Known issues and next steps: [plans.md](plans.md).
