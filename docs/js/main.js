window.onload = async () => {
  const config = await loadConfig()
  document.getElementById('content').innerHTML = wrap(
    '<div class="keybaords"><h2>My keybaords</h2>',
    '</div>',
    config.keybords,
    (keyboard, name) => wrap(
      `<div class="keybaord"><h2>Keybaord ${name}<h2>`,
      '</div>',
      keyboard.layers,
      (layer, name) => wrap(
        `<div class="layer"><h3>Layer ${name}</h3>`,
        '</div>',
        layer.keys,
        (keys, name) => wrap(
          '<div class="keys-block">`<h4>Keys ${name}</h4>',
          '</div>',
          keys,
          row => wrap(
            '<div class="keys-row">',
            '</div>',
            row,
            key => wrap(
              '<span class="key">',
              '</span>',
              key,
            ),
          ),
        ),
      ),
    ),
  );
};

function wrap(left, right, inside, map) {
  if (map !== undefined) {
    inside = _.map(inside, map);
  }

  if (Array.isArray(inside)) {
    inside = inside.join('');
  }
  return left + inside + right;
}

async function loadConfig() {
  const response = await fetch('https://raw.githubusercontent.com/zored/zkf/master/config.yaml');
  const text = await response.text();
  return jsyaml.load(text);
}