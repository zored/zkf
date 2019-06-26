# based on:
# https://github.com/jackhumbert/qmk_firmware_embeded

KEYBOARD ?= planck/ez
TARGET ?= $(KEYBODARD)_zored_$(KEYMAP_VERSION)

# Has no meaning with QMK as dependency:
KEYMAP :=

TOP_DIR := vendor/qmk_firmware

.DEFAULT_GOAL := all

all: $(TOP_DIR)/quantum/version.h

# Generate the version.h file
KEYMAP_VERSION ?= $(shell git describe --abbrev=6 --always --tags 2>/dev/null)
QMK_VERSION := $(shell git -C $(TOP_DIR) describe --abbrev=6 --always --tags  2>/dev/null)
BUILD_DATE := $(shell date +"%Y-%m-%d-%H:%M:%S")

$(TOP_DIR)/quantum/version.h:
	echo '#define QMK_VERSION "$(QMK_VERSION)"' > $@
	echo '#define QMK_BUILDDATE "$(BUILD_DATE)"' >> $@

apply-patch = patch -d $(TOP_DIR) --forward --no-backup-if-mismatch -r- -p1 -i $(CURDIR)/patches/$(1)
q.mk:
	git -C $(TOP_DIR) reset --hard >&2
	$(foreach patch, $(shell ls $(CURDIR)/patches/), $(call apply-patch,$(patch));)
	echo "include $(TOP_DIR)/build_keyboard.mk" > $@
	cd $(TOP_DIR) && make git-submodule

# The dark magic of GNU make.
# Presence of q.mk means that the target was executed and patches were applied
include q.mk
