# Plover steno

ErgoDox EZ and Planck EZ have a `steno` layer that speaks Plover HID. Board-specific keys: [ergodox_ez.md](keyboards/ergodox_ez.md#steno-plover), [planck.md](keyboards/planck.md#steno-plover).

## Start

In Claude Code, `/steno-init` walks through these steps and checks each one.

1. Install [Plover](https://www.openstenoproject.org/plover/) 5.1+ (verified with 5.4.2 on macOS).
2. Leave Plover running: the HID interface is separate from the keyboard, so normal typing is not affected.
3. Optional Russian: Plover → Plugins Manager → install `plover-russian-trillo`, restart Plover.
4. Quit Plover and run:
   ```bash
   ./project.sh plover-setup
   ```
   It installs the `plover-zkf` plugin from [os/plover](../os/plover) and updates `plover.cfg`:
   - machine **Plover HID**, output enabled on start (`auto_start`: without it Plover starts with output off, strokes reach the paper tape but type nothing);
   - enables the `zkf_layout_fix` extension (input source fix, system by input source, Trillo fixes);
   - prepends [english.json](../os/plover/dictionaries/english.json) to the English dictionaries;
   - writes the Russian Trillo section: [trillo.json](../os/plover/dictionaries/trillo.json) on top of the Trillo dictionaries, Trillo keys mapped to Plover HID.
5. Start Plover, enter the `steno` layer on the keyboard.

## Systems

The `zkf_layout_fix` extension picks the system from the macOS input source on Plover start and on every input source change: a Latin (ASCII-capable) source such as ABC selects English, any other (RussianWin) selects Russian Trillo. The switch strokes override it until the next input source change.

| System | Keys | Switch to the other |
|---|---|---|
| English Stenotype | standard steno chart | `R*UFS` → Russian Trillo |
| Russian Trillo | standard chart plus `Х-` `Ц-` (extra pinky column, `X1`/`X2`) and `О` (extra left thumb key, `X3`) | `ХЦ` (both extra pinky keys) → English |

Trillo maps Tréal keys: `Ё` is the upper `*`, `*` the lower one (both star columns work), numbers `1-`..`0-` are `#2`..`#B`. The plugin ships only dictionaries (roots, prefixes, suffixes), no lessons.

Trillo 0.0.2 predates Plover 5: its implicit hyphen keys include `Н-`, whose letter also belongs to `-Н`, and Plover 5 refuses to load it (`invalid implicit_hyphen_keys: some letters are not unique`). `zkf_layout_fix` drops `Н-` from that list on import. 67 prefix/suffix entries write `*Ё` instead of `Ё*` (keys out of steno order, which Plover rejects as invalid steno); `zkf_layout_fix` swaps them while loading Trillo's dictionaries, none of them collides with an existing entry.

## macOS input source

Stock Plover reads the active input source and types each character with the first key combination that produces it. With RussianWin, Latin letters exist only on the Cmd layer, so `h` becomes `Cmd+H` and hides the front app. The `zkf_layout_fix` extension drops Cmd and Ctrl combinations from that table, so such characters go out as Unicode text instead: steno works with any input source.

Without the extension, keep the ABC input source while stenoing.
