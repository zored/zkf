#!/usr/bin/env node
"use strict"

const fs = require('fs')
const ARRAY_GLUE = ',\n'
const INDENT = '  '
const _ = require('lodash')

function main() {
  const [,,keyboard = 'ergodox'] = process.argv
  const config = getConfig()
  const layersConfig = config.keyboards[keyboard].layers
  const layerStructure = flatternStructure(layersConfig.default)
  const keyFactory = new KeyFactory(config.keys, config.map)
  const layers = new LayerCollection(
    _.map(layersConfig, ({keys, lights}, name) => new Layer(name, keyFactory.createFromObject(keys), layerStructure, lights))
  )
  compileTemplate(layers, keyboard)
  console.log('Done!')
}

// Classes
class Key {
  constructor(name){
    this.name = name

    if (name !== null && name.match(/^(UC|DYN|ZKC)_/)) {
      this.noPrefix = true
    }
  }
  get codeName() {
    if (this.name == null) {
      return '_______'
    }

    return (this.noPrefix ? '' : 'KC_') + this.name.toUpperCase()
  }
  get tapCode() {
    return `register_win_code(${this.codeName});`
  }
  get releaseCode() {
    return `unregister_win_code(${this.codeName});`
  }
  get layerCodeName() {
    return this.codeName
  }
}

class EmojiKey extends Key {
  constructor (emoji) {
    super(emoji.codePointAt(0).toString(16).toUpperCase())
    this.emoji = emoji
  }

  get hex () {
    return this.name
  }

  get codeName() {
    return 'EMOJI_' + this.name
  }

  get layerCodeName () {
    return `X(${this.codeName}) /*${this.emoji}*/`
  }
}

class LayerCollection {
  constructor (layers) {
    this.layers = layers
  }

  get all () {
    return this.layers
  }

  get allWithLights () {
    return this.layers.filter(layer => layer.lights.length > 0)
  }

  get allKeys () {
    return this.layers.flatMap(layer => layer.keys)
  }
}

class Layer {
  constructor (name, keys, structure, lights) {
    this.name = name
    this.keys = keys || []
    this.structure = structure || null
    this._lights = lights || []
  }
  get codeName () {
    return 'LAYER_' + this.name.toUpperCase()
  }
  get lights () {
    return this._lights
  }
  get keyCodesString () {
    let i = 0

    const keyCodesString = _.trim(_.map(this.structure, (count, name) => {
      const keyCodes = this.keys.slice(i, i+count).map(key => key.layerCodeName)
      i += count

      return rtrim(`
/* ${name} */ ${keyCodes},
    `)
    }).join(''), ',')

    if (i !== this.keys.length) {
      throw new Error(`Layer '${this.name}' doesn't suit default layer structure: ${JSON.stringify(this.structure)}`)
    }

    return keyCodesString
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
    return this.tapAction.allActionsRecursive.concat(this.holdAction.allActionsRecursive)
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
  get layerCodeName () {
    return `TD(${super.layerCodeName})`
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
    const state = active ? this.codeName : 0

    return `
            dance_key_states[dance_key] = ${state};
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
    return this.uniqActions.map(action => `
          case ${action.codeName}:
            ${action.onDanceResetCode}
            break;
    `).join('')
  }
  get uniqActions () {
    return _.uniqBy(this.actions, 'name')
  }
  get manyDancesCode() {
    let action
    switch (this.manyDancesType) {
      case 'startLast':
        action = _.last(this.actions)
        if (!action) {
          return ''
        }

        return `
          ${action.onDanceCode}
        `

      case 'repeatFirst':
        action = _.first(this.actions)
        if (!action) {
          return ''
        }

        return `
          for (int i=0; i < state->count; i++) {
            ${action.onDanceCode}
            ${action.onDanceResetCode}
          }
        `
    }

    throw new Error(`Unknown tap dance action on many taps / holds: ${this.manyDancesType}`)
  }
  get childActions () {
    return this.actions
  }
}

// project
function getConfig() {
  const yaml = require('js-yaml')

  return yaml.safeLoad(fs.readFileSync('config.yaml', 'utf8'))
}

function glueEnum (items, initialValue = '1') {
  return _.uniq(items).map((item,index) => index == 0 ? `${item} = ${initialValue}` : item).join(ARRAY_GLUE + INDENT)
}

function rtrim (value) {
  return value.replace(/\s+$/, '')
}

function ltrim (value) {
  return value.replace(/^\s+/, '')
}

function getDanceTemplateData (layers) {
  const keys = layers.allKeys.filter(key => key instanceof DanceKey)
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
    count: actionNames.length,
    onDance,
    onDanceReset,
    actions
  }
}

function getUnicodeTemplateData (layers) {
  const keys = layers.allKeys.filter(key => key instanceof EmojiKey)
  const names = glueEnum(keys.map(key => key.codeName), 0)
  const map = keys.map(key => `[${key.codeName}] 0x${key.hex}, // ${key.emoji}`).join('\n')

  return {
    names,
    map,
  }
}

function getLayersTemplateData(layers) {
  const keys = layers.all.map(layer => `
[${layer.codeName}] = LAYOUT_ergodox(
  ${layer.keyCodesString}
)
  `).join(ARRAY_GLUE)

  const names = glueEnum(layers.all.map(layer => layer.codeName), 0)

  return {keys, names}
}

function getErgodoxTemplateData (layers, keyboard) {
  const lights = layers.allWithLights.map(layer => `
    case ${layer.codeName}:
      ` + layer.lights.map(light => `ergodox_right_led_on(${light});`).join(' ') + `
      break;
  `).join('')

  return {
    lights
  }
}

function compileTemplate (layers, keyboard) {
  const Mustache = require('Mustache')

  const result = Mustache.render(fs.readFileSync('keymap.c.mustache', 'utf8'), {
    dance: getDanceTemplateData(layers),
    unicode: getUnicodeTemplateData(layers),
    layers: getLayersTemplateData(layers),
    ergodox: getErgodoxTemplateData(layers, keyboard),
  })
  fs.writeFileSync('keymap.c', result)
}

// general
function flattern(item) {
  if (!(item instanceof Object) && !(item instanceof Array)) {
    return [item]
  }

  return _.flatMap(item, value => flattern(value))
}

function flatternStructure(item, structureKey, nextKey) {
  if (!(item instanceof Object) && !(item instanceof Array)) {
    return [structureKey]
  }

  if (nextKey) {
    if (structureKey) {
      structureKey += '-' + nextKey
    }
    else {
      structureKey = nextKey
    }
  }

  const structureKeys = Object
    .entries(item)
    .flatMap(([key,value]) => flatternStructure(value, structureKey, key))

  if (structureKey != null) {
    return structureKeys
  }

  return _.countBy(structureKeys)
}

// key

class KeyFactory {
  constructor(perKeyConfig, nameMap) {
    this.perKeyConfig = perKeyConfig
    this.nameMap = nameMap || {}
  }

  create(value) {
    const key = this._createFromScalar(value)

    return this._createKeysFromConfig(key) || this._createKeyByName(key) || key
  }

  createFromObject(keys) {
    return flattern(keys).map(key => this.create(key))
  }

  _createFromScalar(value) {
    if (value === null) {
      return new NullKey()
    }

    if (_.isNumber(value)) {
      value = value.toString()
    }

    if (_.isString(value)) {
      value = this.nameMap[value] || value

      return new Key(value)
    }

    throw new Error(`Undefined key value type: ${value}.`)
  }

  _createKeyByName(key) {
    const {name} = key
    if (typeof name !== 'string') {
      return null
    }

    return this._createLayerKey(name) || this._createEmojiKey(name) || null
  }

  _createEmojiKey (name) {
    const emojiPattern = /[\u{1f300}-\u{1f5ff}\u{1f900}-\u{1f9ff}\u{1f600}-\u{1f64f}\u{1f680}-\u{1f6ff}\u{2600}-\u{26ff}\u{2700}-\u{27bf}\u{1f1e6}-\u{1f1ff}\u{1f191}-\u{1f251}\u{1f004}\u{1f0cf}\u{1f170}-\u{1f171}\u{1f17e}-\u{1f17f}\u{1f18e}\u{3030}\u{2b50}\u{2b55}\u{2934}-\u{2935}\u{2b05}-\u{2b07}\u{2b1b}-\u{2b1c}\u{3297}\u{3299}\u{303d}\u{00a9}\u{00ae}\u{2122}\u{23f3}\u{24c2}\u{23e9}-\u{23ef}\u{25b6}\u{23f8}-\u{23fa}]/ug
    if (name.length !== 2) {
      return null
    }

    if (!emojiPattern.test(name)) {
      return null
    }

    return new EmojiKey(name)
  }

  _createLayerKey(name) {
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

  _createKeysFromConfig (key) {
    const perKeyConfig = this.perKeyConfig
    if (!perKeyConfig) {
      return null
    }

    const config = perKeyConfig[key.name]
    if (!(config instanceof Object)) {
      return null
    }

    switch (config.type) {
      case 'dance':
        const {tap = [], hold = []} = config
        const createActions = (stepsActions, manyDancesType) => new TapDanceAction(
          stepsActions.map(stepActions => this._createTapStepAction(stepActions)),
          manyDancesType
        )

        return new DanceKey(
          key.name,
          createActions(tap, 'repeatFirst'),
          createActions(hold, 'startLast')
        )
    }

    throw new Error(`Undefined .keys.${key.name} config type: ${config.type}`)
  }
  _createTapStepAction (stepActions) {
    const keys = stepActions.map(item => this.create(item))

    return new KeySequence(keys)
  }
}


main()