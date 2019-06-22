"use strict";

function main() {
  const config = getConfig();
  const keys = config.keys
  const layoutKeys = flattern(config.keyboards.ergodox.layouts.default.keys)
    .map(key => createKey(key))
    .map(key => createAdvanedKeys(key, keys))

  console.log(layoutKeys)
}

class Key {
  constructor(name){
    this.name = name
  }
}

class DanceKey extends Key {
  constructor (name, tap, hold) {
    super(name)
    this.tap = tap
    this.hold = hold
  }
}

class NullKey extends Key {
  constructor(){
    super(null)
  }
}

class Actions {
}

class KeySequence extends Actions {
  constructor (keys) {
    super()
    this.keys = keys
  }
}

class TapDanceActions extends Actions {
  constructor (keySequences) {
    super()
    this.keySequences = keySequences
  }
}

// project
function getConfig() {
  const yaml = require('js-yaml')
  const fs = require('fs')

  return yaml.safeLoad(fs.readFileSync('config.yaml', 'utf8'));
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

function createAdvanedKeys (key, keys) {
  const config = keys[key.name]
  if (!(config instanceof Object)) {
    return key;
  }

  switch (config.type) {
    case 'dance':
      const {tap, hold} = config
      const createActions = (stepsActions) => new TapDanceActions(
        stepsActions.map(stepActions => createTapStepAction(stepActions))
      )

      return new DanceKey(key.name, createActions(tap), createActions(hold))
  }

  throw new Exception(`Undefined .keys.${key.name} config type: ${config.type}`)
}

function createTapStepAction (stepActions) {
  const keys = stepActions.map(item => createKey(item))

  return new KeySequence(keys)
}

main()