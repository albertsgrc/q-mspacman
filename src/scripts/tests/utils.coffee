assert = require 'assert'
_ = require 'lodash'
{ execSync } = require 'child_process'
readLineSync = require 'readline-sync'
{ stripColor } = require 'chalk'

logger = require './logger'
styler = require './styler'

DFL_DECIMAL_PLACES = 3

@normalizeError = (err) ->
    s = err.toString().replace("Error: ", "")
    s = s[...-1] if s[s.length - 1] is '\n'
    s

handleError = (error, {
                        exit = yes # Whether the process may exit on non-allowed error
                        allowedErrors = [] # List of allowed error codes,
                                           # returns null immediately if
                                           # error.code is in this array
                        noExitErrors = []
                        exitCode = 0  # if set to 0 defaults to (error.errno ? 1)
                        description = "" # String that will appear before the error text
                        printError = yes # Whether to print the error or not
                      } = {}) =>
    if (error.code? and error.code in allowedErrors) or
       (error.errno? and error.errno in allowedErrors) or
       (error.status? and error.status in allowedErrors)
        return null

    errorMsg =
        if description.length
            styler.id(description) + ": " + @normalizeError(error)
        else
            @normalizeError(error)

    logger.e errorMsg, showStack: no if printError

    exitCode = error.errno ? 1 if exitCode is 0
    process.exit exitCode if exit is yes and
                             not (error.code? and error.code in noExitErrors) and
                             not (error.errno? and error.errno in noExitErrors) and
                             not (error.status? and error.status in noExitErrors)

    error.toString = -> errorMsg
    error.isError = yes
    error


# Attemps to call a function with a given array of arguments.
# If the call throws a descriptive error is shown and the program exits unless
# otherwise specified
@attempt = (fn, args = [], options) =>
    assert _.isFunction(fn), "utils.attempt argument is not a function" # SANITY-CHECK

    args = [args] unless _.isArray args

    try
        result = fn args...
    catch error
        return handleError error, options

    result

@execSync = (command, options) ->
    options ?= {}
    options.encoding ?= 'utf-8'

    previousStderr = process.stderr.write
    stderr = ""

    process.stderr.write = (s) -> stderr += s.toString()

    try
        result = execSync command, options
    catch error
        process.stderr.write = previousStderr
        throw error

    process.stderr.write = previousStderr

    { stderr, stdout: result }

@askYesNo = (s) ->
    answer = readLineSync.question("#{s} (y/n) ")

    answer in ['y', 'yes', 'yep', 'ye', 'sure', 'si', 'course', 'of course']

@prettyDecimal = (value, decimal_places = DFL_DECIMAL_PLACES) ->
    rounder = Math.pow 10, decimal_places
    Math.round(value*rounder)/rounder

@getNonOcuppyingLength = (str) -> str.length - stripColor(str).length

COMPARTMENT_SEPARATOR = " | "

@compartimentedString = (spaces, values...) =>
    string = ""
    for { value, padKind = "End" }, i in values
        string += COMPARTMENT_SEPARATOR if i > 0 and values[i-1].value.length > 0
        padding = spaces[i] + @getNonOcuppyingLength(value)
        padding += COMPARTMENT_SEPARATOR.length if value.length is 0
        string += _['pad' + padKind](value, padding)

    string
