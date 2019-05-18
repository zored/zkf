extern "C" {
#include QMK_KEYBOARD_H
}

#include <util/new.cpp>

#include <iterator>
#include <vector>
#include <features/combo.hpp>

using namespace ZFK;

void zored_main(void) {
	ComboCollection* combos = new ComboCollection();
	vector<Keycode> input1 {{KC_Y},{KC_U}};
	combos->add(new KeycodeCombo(input1, KC_ESC));
	//combos->apply();
}

zored_main();