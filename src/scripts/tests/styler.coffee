chalk = require 'chalk256'
assert = require 'assert'

{ TERM } = process.env
supports256 = TERM? and /^xterm-256(?:color)?/.test(TERM)

styles256 =
    error: chalk("#e74c3c")
    warn: chalk("#f39c12")
    info: chalk("#00d7ff")
    verbose: chalk("#5fffd7")

    cmd: chalk.underline
    id: chalk.italic
    value: chalk.bold
    unit: chalk("#e4e4e4").italic

    okay: chalk("#87ff00")
    normal: chalk("#f1c40f")
    bad: chalk("#9b59b6")

    superNice: chalk("#00afff").bgBlack
    veryGood: chalk("#00ffff").bgBlack
    nice: chalk("#00ffd7").bgBlack
    good: chalk("#5fff87").bgBlack
    ok: chalk("#87ff00").bgBlack
    regular: chalk("#ffff00").bgBlack
    insuficient: chalk("#ff8700").bgBlack
    veryBad: chalk("#ff005f").bgBlack


styles16 =
    error: chalk.red
    warn: chalk.yellow
    info: chalk.blue
    verbose: chalk.cyan

    cmd: chalk.underline
    id: chalk.italic
    value: chalk.bold
    unit: chalk.grey.italic

    okay: chalk.green
    normal: chalk.white
    bad: chalk.yellow

    superNice: chalk.blue.bgBlack
    veryGood: chalk.blue.bgBlack
    nice: chalk.cyan.bgBlack
    good: chalk.green.bgBlack
    ok: chalk.green.bgBlack
    regular: chalk.yellow.bgBlack
    insuficient: chalk.red.bgBlack
    veryBad: chalk.red.bgBlack

# SANITY-CHECKS
assert(Object.keys(styles256).length is Object.keys(styles16).length,
       "256 and 16 styles don't have the same properties")
assert(prop of styles256, "Style #{prop} missing in 256") for prop of styles16

module.exports = if supports256 then styles256 else styles16

# Testing code
#module.exports[style].log("Style #{style}") for style of styles16
