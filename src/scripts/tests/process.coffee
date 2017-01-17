#!/usr/bin/env coffee

_ = require 'lodash'
fs = require 'fs'
summary = require 'summary'
queue = require 'queue'
{ exec } = require 'child_process'
os = require 'os'
http = require 'http'
request = require 'sync-request'
readline = require 'readline'

utils = require './utils'
log = require './logger'
styler = require './styler'

SRC_PATH = __dirname + "/../../"
DATA_PATH = SRC_PATH + "../data"
NN_PATH = DATA_PATH + "/neural-networks"
PACMAN_CMD = SRC_PATH + "pacman non_interactive=true"

available_threads = os.cpus().length

METERS = ['wins', 'completion']
URL = "http://mspacman.ml/methods/"

# Bom's: n_rounds_powerpill=55 initial_scatter_cycle_rounds=40 initial_chase_cycle_rounds=10

process.chdir(SRC_PATH)

Q = queue({ concurrency: available_threads })

fs.mkdirSync(DATA_PATH) unless fs.existsSync(DATA_PATH)

currentTasks = []

redo = (taskid) ->
    currentTasks.splice(currentTasks.indexOf(taskid), 1)
    try
        res = request('POST', URL + 'redoTask', { json: { _id: taskid } })
        log.i "Redo request for #{taskid} handed:\n#{JSON.stringify(res, null, 2)}"
    catch e
        log.e "Error while performing redo request for #{taskid}: #{e.toString()}"

done = (task, result) ->
    console.log task
    currentTasks.splice(currentTasks.indexOf(task._id), 1)
    try
        res = request('POST', URL + 'finishTask', { json: { _id: task._id, result, index: task.data.index, experimentId: task.data.experiment._id } })
        log.i "Finish request for #{task._id} handed:\n#{JSON.stringify(res, null, 2)}"
    catch e
        log.e "Error while performing done request for #{task._id}: #{e.toString()}"

perform = (task) ->
    argsString = _.toPairs(task.data.args).map((arg) -> if arg[1]? then arg.join('=') else '').join(' ')

    Q.push((finish) ->
        name = task.data.experiment.name
        command = PACMAN_CMD + " random_seed=#{Math.floor(Math.random()*10000000000)}" + (if argsString.length then " " else "") + argsString
        log.i "Experiment #{styler.id name}: Test ##{task.data.test}: Running #{styler.cmd command}..."

        exec(command + " > /dev/null", (error, stdout, stderr) ->
            if error
                log.e(error.toString())
                redo(task._id)
            else
                log.i "Experiment #{styler.id name}: Test ##{task.data.test}: Finished #{styler.cmd command}"

                result = JSON.parse(stderr)
                result.neural_network = fs.readFileSync(NN_PATH + "/#{result.neural_network}.txt", 'utf-8')

                done(task, result)

            processTask()

            finish()
        )
    )

processTask = ->
    try
        res = request('POST', URL + "processTask")
        res = JSON.parse(res.getBody('utf8'))
    catch e
        log.e "Error while performing process request: #{e.toString()}"
        return no

    if res?
        currentTasks.push(res._id)
        perform(res)
        yes


setInterval(->
    while currentTasks.length < available_threads and processTask()
        ""

    Q.start()
, 5000)


Q.start()

panic = ->
    while currentTasks.length
        redo(currentTasks[0])

rl = readline.createInterface({
  input: process.stdin,
  output: process.stdout
})

rl.on('line', (input) ->
    if input is "exit"
        panic()
        process.exit(0)
)
#process.on('exit', panic)
#process.on('SIGINT', panic)
#process.on('uncaughtException', panic)
