const createDiv = (css, children) => {
  const div = document.createElement('div');
  children.forEach(c => div.appendChild(c));
  div.className = css;
  return div;
}

const createHeading = (depth, name) => {
   const h = document.createElement('h' + (depth + 1));
   h.innerText = name;
   return h;
}

window.onload = async () => {
  const config = await loadConfig();
  const byId = id => document.getElementById(id);
  const $keyboard = byId('keyboard');
  const $layer = byId('layer');
  const $mapping = byId('mapping');
  let hashInput = location.hash.length > 1 &&
    JSON.parse(decodeURIComponent(location.hash.substring(1)))
  const getInput = () => {
    const getValue = ($e, o) => {
      o = o || {};

      let value = null;
      if ($e.selectedIndex >= 0) {
        value = $e.options[$e.selectedIndex].value;
      }
      
      if (value && o[value]) {
        return value
      }

      if (value == 'all') {
        return value
      }


      const valueHolders = toArray(o);
      if (valueHolders.length === 0) {
        value = 'all'
      } else {
        value = valueHolders.map(([k]) => k)[0]; 
      }

      if (hashInput) {
        debugger
        value = hashInput[$e.id]
      }

      $e.value = value;
      return value;
    };

    const keyboards = config.keyboards
    const keyboard = getValue($keyboard, keyboards)

    const layers = keyboards[keyboard]?.layers;
    const layer = getValue($layer, layers);

    const mappings = keyboards[keyboard]?.mappings;
    const mapping = getValue($mapping, mappings);

    const getOption = (name, selected) => `
      <option value="${name}" ${selected ? 'selected="selected"' : ''} >${name}</option>
    `;

    const setOptions = ($e, configs, value) => {
      const data = toArray(configs);
      $e.style.display = (data.length === 0)
        ? 'none'
        : 'inline';

      data.unshift([['all']])
      if (!value && data.length) {
        value = 'all'
      }
      $e.innerHTML = data
        .map(([v]) => getOption(v, value === v));
    }

    setOptions($keyboard, keyboards, keyboard);
    setOptions($layer, layers, layer);
    setOptions($mapping, mappings, mapping);

    const result = {keyboard, layer, mapping};
    location.hash = JSON.stringify(result);
    return result;
  }
  const refresh = () => {
    const input = getInput();
    const filter = (o, k) => {
      if (k === 'all') {
        return o;
      }
      return {[k]: o[k]};
    };



    const content = wrap(
      createDiv('keyboards block', []),
      filter(config.keyboards, input.keyboard),
      (keyboard, keyboardName) => wrap(
        createDiv('keyboard block', [createHeading(1, keyboardName)]),
        filter(keyboard.layers, input.layer),
        (layer, layerName) => wrap(
          createDiv('layer block', [createHeading(2, layerName)]),
          layer.keys,
          (keys, keysName) => {
            const mapping = input.mapping && input.mapping !== 'all' && keyboard.mappings[input.mapping];
            return Number.isInteger(keysName)
              ? keyRowCallback(keys, keysName, mapping, config.keys)
              : keyBlockCallback(keys, keysName, keyboard + '-' + layerName, mapping, config.keys)
          }
        ),
      ),
    );

    byId('content').innerHTML = ''
    byId('content').appendChild(content)
  }
  refresh();
  hashInput = null;

  [$keyboard, $layer, $mapping].forEach($e => $e.addEventListener('change', refresh));
  return;
};

function toArray(o) {
  return Object.entries(o || {});
}

function keyBlockCallback(keys, blockName, idPrefix, mapping, dance) {
  return wrap(
    createDiv('keys-block block', [createHeading(3, blockName)]),
    keys,
    (r, n) => keyRowCallback(r, n, mapping, dance),
  );
}

function keyRowCallback(row, name, mapping, dance) {
  return wrap(
    createDiv('keys-row', []),
    row,
    key => getKey(key, mapping, dance),
  );
}

function getKey(key, mapping, danceKeys) {
  let $key = document.createTextNode((key || '~') + '');
  const danceKey = danceKeys[key];
  if (danceKey) {
     $key = createSpan('key-dance', 'Dance key', [
        document.createTextNode(danceKey.tap[[0]])
     ]);
     const info = [
       [danceKey.tap.slice(1), 'Tap'],
       [danceKey.hold, 'Hold'],
     ].flatMap(
       ([keys, name]) => (keys || []).map(
         (actions, i) => `- ${name} #${i+1}: ${actions.join(' + ')}`
       )
     ).join('\n') || 'Only one click action';
     $key.addEventListener('click', () => alert(info))
  }

  if (mapping) {
    const indexOne = mapping[0].indexOf(key)
    if (indexOne >= 0) {
      key = mapping[1][indexOne];
    }
  }

  return createSpan('key', key, [$key])
}

function createSpan(css, title, children) {
  const span = document.createElement('span')
  span.className = css;
  span.title = title;
  children.forEach(c => span.appendChild(c))
  return span;
}

function wrap(parent, child, map) {
  if (map !== undefined) {
    child = _.map(child, map);
  }
  const children = Array.isArray(child) ? child : [child];
  children.map(e => parent.appendChild(e));
  return parent;
}

async function loadConfig() {
  const response = await fetch('https://raw.githubusercontent.com/zored/zkf/master/config.yaml');
  const text = await response.text();
  return jsyaml.load(text);
}