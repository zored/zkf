import ctypes
import sys
from threading import Thread

from plover import log
from plover.registry import registry

SHORTCUT_MODIFIERS = 1 | 16
LATIN_SYSTEM = "English Stenotype"
NON_LATIN_SYSTEM = "Russian Trillo"


def _steps(sequence):
    return sequence if isinstance(sequence[0], tuple) else (sequence,)


def _fix_trillo_implicit_hyphens():
    try:
        from plover_russian_trillo import system
    except ImportError:
        return
    letters = [key.strip("-") for key in system.KEYS]
    system.IMPLICIT_HYPHEN_KEYS = tuple(
        key for key in system.IMPLICIT_HYPHEN_KEYS if letters.count(key.strip("-")) == 1
    )


_fix_trillo_implicit_hyphens()


class LayoutFix:
    def __init__(self, engine):
        self._engine = engine
        self._layout = engine._keyboard_emulation._layout
        self._layout_class = type(self._layout)
        self._original = self._layout_class._update_layout
        layout_module = sys.modules[self._layout_class.__module__]
        self._carbon = layout_module.carbon
        self._ascii_capable_key = layout_module.kTISPropertyInputSourceIsASCIICapable
        self._carbon.CFBooleanGetValue.argtypes = [ctypes.c_void_p]
        self._carbon.CFBooleanGetValue.restype = ctypes.c_bool

    def start(self):
        original = self._original
        on_layout = self._on_layout

        def _update_layout(layout):
            original(layout)
            layout._char_to_key_sequence = {
                char: sequence
                for char, sequence in layout._char_to_key_sequence.items()
                if not any(mod & SHORTCUT_MODIFIERS for _, mod in _steps(sequence))
            }
            on_layout()

        self._layout_class._update_layout = _update_layout
        Thread(target=self._layout._update_layout, daemon=True).start()

    def stop(self):
        self._layout_class._update_layout = self._original
        self._layout._update_layout()

    def _on_layout(self):
        system_name = LATIN_SYSTEM if self._input_source_is_latin() else NON_LATIN_SYSTEM
        log.info("zkf_layout_fix: h=%s, system=%s", self._layout.char_to_key_sequence("h"), system_name)
        try:
            registry.get_plugin("system", system_name)
        except KeyError:
            log.error("zkf_layout_fix: system `%s` is not installed", system_name)
            return
        if self._engine["system_name"] != system_name:
            self._engine.config = {"system_name": system_name}

    def _input_source_is_latin(self):
        source = self._carbon.TISCopyCurrentKeyboardInputSource()
        try:
            value = self._carbon.TISGetInputSourceProperty(source, self._ascii_capable_key)
            return bool(value) and self._carbon.CFBooleanGetValue(value)
        finally:
            self._carbon.CFRelease(source)
