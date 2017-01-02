#! /usr/bin/env coffee

fs = require 'fs'
path = require 'path'

FOLDER_PATH = '../data/neural-networks/';

files = fs.readdirSync FOLDER_PATH

if files.length is 0
  console.error "No neural networks available. Run ./pacman"

argmax = (l, f) ->
  maxarg = l[0]
  maxval = f l[0]

  for elem in l[1..] when (val = f(elem)) > maxval
    maxval = val
    maxarg = elem

  maxarg

console.log argmax(files, (file) -> fs.statSync(path.join(FOLDER_PATH, file)).ctime).replace('.txt', '')