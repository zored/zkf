CPPFLAGS += -std=c++17 -fno-exceptions -Isrc -Ivendor -Ivendor/avr-stl/include
SRC += \
	src/main.cpp

# https://docs.qmk.fm/#/config_options?id=the-rulesmk-file
BOOTMAGIC_ENABLE = full
COMBO_ENABLE = yes
COMMAND_ENABLE = no
KEY_LOCK_ENABLE = yes
LEADER_ENABLE = yes
RGBLIGHT_ENABLE = no
STENO_ENABLE = yes
TAP_DANCE_ENABLE = yes
UNICODE_ENABLE = no
UNICODEMAP_ENABLE = yes