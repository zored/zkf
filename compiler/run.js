"use strict"

const fs = require('fs')
const ARRAY_GLUE = ',\n'
const INDENT = '  '
const _ = require('lodash')

function main() {
  const config = getConfig()
  const defaultLayer = config.keyboards.ergodox.layers.default.keys
  const layerStructure = flatternStructure(defaultLayer)
  const keyFactory = new KeyFactory(config.keys, config.map)
  const layerKeys = flattern(defaultLayer)
    .map(key => keyFactory.create(key))

  compileTemplate(layerKeys, layerStructure)

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
  get layerCodeName() {
    return this.codeName
  }
}

class Layer {
  constructor (name, keys, structure) {
    this.name = name
    this.keys = keys || []
    this.structure = structure
  }
  get codeName () {
    return 'LAYER_' + this.name.toUpperCase()
  }
  get keyCodesString () {
    let i = 0

    const keyCodesString = _.map(this.structure, (count, name) => {
      const keyCodes = this.keys.slice(i, i+count).map(key => key.layerCodeName)
      i += count

      return rtrim(`
/* ${name} */ ${keyCodes},
    `)
    }).join('')

    if (i !== this.keys.length) {
      throw new Exception(`Layer ${this.name} doesn't suit default layer structure.`)
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
      break
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
            dance_states[danceKey] = ${state}
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
            return
      `
    }).join('')

    return `
        switch (state->count) {
          ${actionsCode}

          default:
            ${this.manyDancesCode}
            return
        }
    `
  }
  get onDanceResetCode() {
    return this.actions.map(action => `
          case ${action.codeName}:
            ${action.onDanceResetCode}
            break
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

  return yaml.safeLoad(fs.readFileSync('config.yaml', 'utf8'))
}

function glueEnum (items, initialValue = '1') {
  return items.map((item,index) => index == 0 ? `${item} = ${initialValue}` : item).join(ARRAY_GLUE + INDENT)
}

function rtrim (value) {
  return value.replace(/\s+$/, '')
}

function ltrim (value) {
  return value.replace(/^\s+/, '')
}

function getDanceTemplateData (layerKeys) {
  const keys = layerKeys.filter(key => key instanceof DanceKey)
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
  const keys = layers.map(layer => `
[${layer.codeName}] = LAYOUT_ergodox(
  ${layer.keyCodesString}
)
  `).join(ARRAY_GLUE)

  const names = glueEnum(layers.map(layer => layer.codeName), 0)

  return {keys, names}
}

function compileTemplate (layerKeys, layerStructure) {
  const Mustache = require('Mustache')

  const result = Mustache.render(fs.readFileSync('keymap.c.mustache', 'utf8'), {
    dance: getDanceTemplateData(layerKeys),
    layers: getLayersTemplateData([new Layer('default', layerKeys, layerStructure)]),
  })
  fs.writeFileSync('keymap.new.c', result)
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

    throw new Exception(`Undefined key value type: ${value}.`)
  }

  _createKeyByName(key) {
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
        const {tap, hold} = config
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

    throw new Exception(`Undefined .keys.${key.name} config type: ${config.type}`)
  }
  _createTapStepAction (stepActions) {
    const keys = stepActions.map(item => this.create(item))

    return new KeySequence(keys)
  }
}


main()