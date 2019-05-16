#include <array>
#include <features/combo.hpp>

using namespace ZFK;

{
	ComboCollection* combos = new ComboCollection();
	combos->add(new KeycodeCombo({KC_Y, KC_U}, KC_ESC));
	combos->apply();
}