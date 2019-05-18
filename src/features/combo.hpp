#include "ergodox_ez.h"
#include <vector>

using namespace std;

namespace ZFK {
  using Keycode = uint16_t;
  using Index = uint8_t;
  using QmkCombo = combo_t;

  class Combo {
    protected:
      vector<Keycode> input;
    public:
      Combo(vector<Keycode> input): input(input) {}
      void onkeydown(void) {}
      void onkeyup(void) {}
      vector<Keycode>& getInput(void) {
        return input;
      }
  };

  class KeycodeCombo: public Combo {
    private:
      Keycode output;
    public:
      KeycodeCombo(vector<Keycode>& input, Keycode output): Combo(input), output(output) {
        this->input = input;
      }
      void onkeyup (void) {
        tap_code(output);
      }
  };

  class ComboCollection {
    private:
      vector<Combo> combos = {};
    public:
      void add(Combo* combo) {
        combos.push_back(*combo);
      }
      void apply(void);
      QmkCombo* getQmkCombos() {
        vector<QmkCombo> qmkCombos = {};

        for (auto& combo: combos) {
          vector<Keycode> keysVector = combo.getInput();
          keysVector.push_back(COMBO_END);
          Keycode* keys = &keysVector[0];

          QmkCombo qmkCombo;
          qmkCombo.keys = keys;

          qmkCombos.push_back(qmkCombo);
        }

        return &qmkCombos[0];
      }
      int getCount() {
        return combos.size();
      }
      void processQmkEvent(Index index, bool pressed) {
        auto combo = combos[index];
        if (pressed) {
          combo.onkeydown();
          return;
        }

        combo.onkeyup();
      }
  };
}