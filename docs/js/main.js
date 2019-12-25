window.onload = async () => {
  const config = await loadConfig();
  document.getElementById('content').innerHTML = wrap(
    '<div class="keybaords"><h2>My keybaords</h2>',
    '</div>',
    config.keyboards,
    (keyboard, name) => wrap(
      `<div class="keybaord"><h3>Keybaord ${name}</h2>`,
      '</div>',
      keyboard.layers,
      (layer, name) => wrap(
        `<div class="layer"><h4>Layer ${name}</h3>`,
        '</div>',
        layer.keys,
        (keys, name) => Number.isInteger(name)
          ? keys.map(getMapKeyRowCallback)
          : wrap(
            `<div class="keys-block"><h5>Keys ${name}</h4>`,
            '</div>',
            keys,
            getMapKeyRowCallback,
          ),
      ),
    ),
  );
};

function getMapKeyRowCallback() {
  return row => wrap(
    '<div class="keys-row">',
    '</div>',
    row,
    key => `<span class="key" title="${key}">${((key || '~') + '').substring(0, 3)}</span>`,
  );
}

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