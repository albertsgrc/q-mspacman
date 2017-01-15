#! /usr/bin/env coffee

fs = require 'fs'

FOLDER_PATH = __dirname +  '/../../data/neural-networks/';

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

console.log argmax(files, (file) -> Number(file[file.lastIndexOf('-')+1...file.lastIndexOf('.')])).replace('.txt', '')
