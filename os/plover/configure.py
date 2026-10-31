import configparser
import json
import sys
from pathlib import Path

from plover.oslayer.config import CONFIG_FILE

DICTIONARIES = Path(__file__).resolve().parent / "dictionaries"
EXTENSION = "zkf_layout_fix"
ENGLISH = "System: English Stenotype"
TRILLO = "System: Russian Trillo"
TRILLO_ASSETS = [
    f"asset:plover_russian_trillo:dictionaries/{name}.json"
    for name in ("trillo_user", "trillo_suffixes", "trillo_prefixes", "trillo_roots")
]
TRILLO_PLOVER_HID = {
    "1-": "#2", "2-": "#3", "3-": "#4", "4-": "#5", "5-": "#6",
    "6-": "#7", "7-": "#8", "8-": "#9", "9-": "#A", "0-": "#B",
    "Х-": "X1", "Ц-": "X2", "Ф-": "S1-", "С-": "S2-", "Т-": "T-",
    "П-": "K-", "В-": "P-", "К-": "W-", "М-": "H-", "Р-": "R-",
    "Н-": "A-", "Е-": "O-", "Ё": ["*1", "*3"], "*": ["*2", "*4"], "О": "X3",
    "-А": "-E", "-И": "-U", "-С": "-F", "-Р": "-R", "-Т": "-P",
    "-Л": "-B", "-В": "-L", "-К": "-G", "-М": "-T", "-Н": "-S",
    "-Й": "-D", "-Ъ": "-Z", "no-op": "#1",
}


def entry(path):
    return {"enabled": True, "path": str(path)}


def with_first(dictionaries, first):
    return [first] + [d for d in dictionaries if d["path"] != first["path"]]


config = configparser.RawConfigParser()
if not config.read(CONFIG_FILE, encoding="utf-8"):
    sys.exit(f"Plover config not found: {CONFIG_FILE}")
for section in ("Plugins", ENGLISH, TRILLO):
    if not config.has_section(section):
        config.add_section(section)

extensions = set(json.loads(config.get("Plugins", "enabled_extensions", fallback="[]")))
config.set("Plugins", "enabled_extensions", json.dumps(sorted(extensions | {EXTENSION})))

english = json.loads(config.get(ENGLISH, "dictionaries", fallback="[]"))
if not english:
    sys.exit(f"[{ENGLISH}] has no dictionaries: open Plover once and check its config")
config.set(ENGLISH, "dictionaries", json.dumps(with_first(english, entry(DICTIONARIES / "english.json"))))

trillo = json.loads(config.get(TRILLO, "dictionaries", fallback="[]")) or [entry(p) for p in TRILLO_ASSETS]
config.set(TRILLO, "dictionaries", json.dumps(with_first(trillo, entry(DICTIONARIES / "trillo.json"))))
config.set(TRILLO, "keymap[plover hid]", json.dumps(
    [[action, keys if isinstance(keys, list) else [keys]] for action, keys in TRILLO_PLOVER_HID.items()]
))

with open(CONFIG_FILE, "w", encoding="utf-8") as f:
    config.write(f)
print(f"Updated {CONFIG_FILE}")
