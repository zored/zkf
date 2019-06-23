"use strict";

const fs = require('fs')
const ARRAY_GLUE = ',\n'
const INDENT = '  '

function main() {
  const config = getConfig();
  const keys = config.keys
  const layerKeys = flattern(config.keyboards.ergodox.layers.default.keys)
    .map(key => createKey(key))
    .map(key => createAdvanedKey(key, keys))

  compileTemplate(layerKeys)

  console.log('Done!')
}

// Classes
class Key {
  constructor(name){
    this.name = name
  }
  get codeName() {
    if (this.name == null) {
      return '_______'
    }
    return 'KC_' + this.name.toUpperCase()
  }
  get tapCode() {
    return `register_win_code(${this.codeName});`
  }
  get releaseCode() {
    return `unregister_win_code(${this.codeName});`
  }
}

class Layer {
  constructor (name, keys) {
    this.name = name
    this.keys = keys || []
  }
  get codeName () {
    return 'LAYER_' + this.name.toUpperCase()
  }
  get keyCodes () {
    return this.keys.map(key => key.codeName)
  }
  toString() {
    return this.codeName
  }
}

class LayerToggleKey extends Key {
  constructor (layer) {
    super('TOGGLE_' + layer)
    this.layer = layer
  }
  get codeName() {
    return `TG(${this.layer.codeName})`
  }
  get tapCode() {
    return `layer_invert(${this.layer.codeName});`
  }
  get releaseCode() {
    return ``
  }
}

class LayerHoldKey extends Key {
  constructor (layer) {
    super('HOLD_' + layer)
    this.layer = layer
  }
  get codeName() {
      return `MO(${this.layer.codeName})`
  }

  get tapCode() {
    return `layer_on(${this.layer.codeName});`
  }
  get releaseCode() {
    return `layer_off(${this.layer.codeName});`
  }
}

class DanceKey extends Key {
  constructor (name, tapAction, holdAction) {
    super(name)
    this.tapAction = tapAction
    this.holdAction = holdAction
  }
  get codeName() {
    return 'DANCE_' + super.codeName
  }
  get actions () {
    return this.tapAction.allActionsRecursive.concat(this.holdAction.allActionsRecursive);
  }
  get onDanceCode () {
    return `
    case ${this.codeName}:
      if (state->pressed) {
        // Hold actions:
        ${this.holdAction.onDanceCode}
      }
      else {
        // Tap actions:
        ${this.tapAction.onDanceCode}
      }
      break;
    `
  }
  get onDanceResetCode () {
    return `
        ${this.holdAction.onDanceResetCode}
        ${this.tapAction.onDanceResetCode}
    `
  }
}

class NullKey extends Key {
  constructor(){
    super(null)
  }
}

class Action {
  constructor(id) {
    this.id = id
  }
  get codeName() {
    if (this.id === null) {
      return null
    }
    return 'ACTION_' + this.id.toUpperCase()
  }
  get childActions () {
    return []
  }
  get allActionsRecursive () {
    return [this].concat(this.childActions.flatMap(child => child.allActionsRecursive))
  }
}

class KeySequence extends Action {
  constructor (keys, id) {
    super(id)
    this.keys = keys
    if (this.id == null) {
      const glue = '__'
      this.id = 'SEQ' + glue + this.keys.map(key => key.name).join(glue)
    }
  }
  get onDanceCode() {
    return ltrim(this.keys.map(key => rtrim(`
            ${key.tapCode}
    `)).join('')) + this.onDanceStateCode(true)
  }
  get onDanceResetCode() {
    return ltrim(this.keys.map(key => rtrim(`
            ${key.releaseCode}
    `)).join('')) + this.onDanceStateCode(false)
  }
  onDanceStateCode(active) {
    const state = active ? this.codeName : 0;

    return `
            dance_states[danceKey] = ${state};
    `
  }
}

class TapDanceAction extends Action {
  constructor (actions, manyDancesType) {
    super(null)
    this.actions = actions
    this.manyDancesType = manyDancesType
  }
  get onDanceCode() {
    let count = 0
    const actionsCode = this.actions.map(action => {
      count++

      return `
          case ${count}:
            ${action.onDanceCode}
            return;
      `
    }).join('')

    return `
        switch (state->count) {
          ${actionsCode}

          default:
            ${this.manyDancesCode}
            return;
        }
    `
  }
  get onDanceResetCode() {
    return this.actions.map(action => `
          case ${action.codeName}:
            ${action.onDanceResetCode}
            break;
    `).join('')
  }
  get manyDancesCode() {
    let action
    switch (this.manyDancesType) {
      case 'startLast':
        action = this.actions[this.actions.length - 1]

        return `
          ${action.onDanceCode}
        `

      case 'repeatFirst':
        action = this.actions[0]
        return `
          for (int i=0; i < state->count; i++) {
            ${action.onDanceCode}
            ${action.onDanceResetCode}
          }
        `
    }

    throw new Exception(`Unknown tap dance action on many taps / holds: ${this.manyDancesType}`)
  }
  get childActions () {
    return this.actions
  }
}

// project
function getConfig() {
  const yaml = require('js-yaml')

  return yaml.safeLoad(fs.readFileSync('config.yaml', 'utf8'));
}

function glueEnum (items) {
  return items.map((item,index) => index == 0 ? `${item} = 1` : item).join(ARRAY_GLUE + INDENT)
}

function rtrim (value) {
  return value.replace(/\s+$/, '')
}

function ltrim (value) {
  return value.replace(/^\s+/, '')
}

function getDanceTemplateData (layerKeys) {
  const keys = layerKeys.filter(key => key instanceof DanceKey);
  const names = glueEnum(keys.map(key => key.codeName))
  const onDance = keys.map(key => key.onDanceCode).join('')
  const onDanceReset = ltrim(keys.map(key => key.onDanceResetCode).join(''))
  const actionNames = glueEnum(keys
    .flatMap(key => key.actions)
    .map(key => key.codeName)
    .filter(key => key != null)
  )
  const actions = keys.map(key => `[${key.codeName}] = DANCE_MODIFIER()`).join(ARRAY_GLUE + INDENT)

  return {
    names,
    actionNames,
    count: keys.length,
    onDance,
    onDanceReset,
    actions
  }
}

function getLayersTemplateData(layers) {
  return layers.map(layer => `
[${layer.codeName}] = LAYOUT_ergodox(
  ${layer.keyCodes.join(',')}
)
  `).join(ARRAY_GLUE)
}

function compileTemplate (layerKeys) {
  const Mustache = require('Mustache')

  const result = Mustache.render(fs.readFileSync('keymap.c.mustache', 'utf8'), {
    dance: getDanceTemplateData(layerKeys),
    layers: getLayersTemplateData([new Layer('default', layerKeys)]),
  });
  fs.writeFileSync('keymap.new.c', result)
}

// general
function flattern(item) {
  if (!(item instanceof Object) && !(item instanceof Array)) {
    return [item]	
  }

  const callback = ([,value]) => flattern(value)

  return Object.entries(item).flatMap(callback)
}

// key

function createKey (key) {
  if (key == null) {
    return new NullKey()
  }

  if (Number.isInteger(key)) {
    key = key.toString()
  };

  return new Key(key)
}

function createAdvanedKey (key, keys) {
  return createKeysFromConfig(key, keys) || createKeyByName(key) || key
}

function createKeyByName(key) {
  const {name} = key
  if (typeof name !== 'string') {
    return null
  }

  const match = name.match(/^(?<type>toggle|hold)(?<layer>[A-Z0-9].+)$/)
  if (match == null) {
    return null
  }

  const {type, layer} = match.groups
  const layerObject = new Layer(layer)

  switch (type) {
    case 'hold':
      return new LayerHoldKey(layerObject)
    case 'toggle':
      return new LayerToggleKey(layerObject)
  }
}

function createKeysFromConfig (key, keys) {
  if (!keys) {
    return null
  }

  const config = keys[key.name]
  if (!(config instanceof Object)) {
    return null
  }

  switch (config.type) {
    case 'dance':
      const {tap, hold} = config
      const createActions = (stepsActions, manyDancesType) => new TapDanceAction(
        stepsActions.map(stepActions => createTapStepAction(stepActions)),
        manyDancesType
      )


      return new DanceKey(
        key.name,
        createActions(tap, 'repeatFirst'),
        createActions(hold, 'startLast')
      )
  }

  throw new Exception(`Undefined .keys.${key.name} config type: ${config.type}`)
}

function createTapStepAction (stepActions) {
  const keys = stepActions.map(item => createAdvanedKey(createKey(item)))

  return new KeySequence(keys)
}

main()