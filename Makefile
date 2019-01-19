# based on:
# https://github.com/jackhumbert/qmk_firmware_embeded

KEYBOARD := whitefox

KEYMAP :=

TOP_DIR := qmk_firmware

TARGET ?= whitefox-alebastr-$(QMK_VERSION)+$(KEYMAP_VERSION)

# Generate the version.h file
QMK_VERSION := $(shell cd $(TOP_DIR) && git describe --abbrev=6 --always --tags 2>/dev/null || date +"%Y-%m-%d-%H:%M:%S")
BUILD_DATE := $(shell date +"%Y-%m-%d-%H:%M:%S")
$(shell echo '#define QMK_VERSION "$(QMK_VERSION)"' > $(TOP_DIR)/quantum/version.h)
$(shell echo '#define QMK_BUILDDATE "$(BUILD_DATE)"' >> $(TOP_DIR)/quantum/version.h)
KEYMAP_VERSION := $(shell git describe --abbrev=6 --always 2>/dev/null)

# Apply embedded qmk_firmware patches
patch = $(shell patch -d $(TOP_DIR) -N -r- -p1 -i ../patches/$(1) >/dev/null)
$(call patch,0001-makefile-changes-to-support-a-parent-directory.patch)
$(call patch,0002-add-top_dir-in-the-correct-place.patch)
$(call patch,0003-fix-tests.patch)
$(call patch,0004-More-TOP_DIR-fixes.patch)

include qmk_firmware/build_keyboard.mk
