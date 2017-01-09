_ = require 'lodash'
fs = require 'fs'
summary = require 'summary'
queue = require 'queue'
{ exec } = require 'child_process'
os = require 'os'

utils = require './utils'
log = require './logger'
styler = require './styler'

SRC_PATH = __dirname + "/../../"
DATA_PATH = "../data"
EXPERIMENTS_PATH = "#{DATA_PATH}/experiments/"
PACMAN_CMD = "pacman non_interactive=true"

available_threads = os.cpus().length

N_REPETITIONS = 0.5*available_threads

DFL_T_STUDENT_TEST_CONFIDENCE = 0.05

METERS = ['wins', 'completion']

# Bom's: n_rounds_powerpill=55 initial_scatter_cycle_rounds=40 initial_chase_cycle_rounds=10

process.chdir(SRC_PATH)

Q = queue({ concurrency: available_threads })

fs.mkdirSync(DATA_PATH) unless fs.existsSync(DATA_PATH)
fs.mkdirSync(EXPERIMENTS_PATH) unless fs.existsSync(EXPERIMENTS_PATH)

test = (args, name, cb) ->
    count = 0
    results = {}
    results[meter] = [0...N_REPETITIONS] for meter in METERS
    results.execId = [0...N_REPETITIONS]

    argsString = _.toPairs(args).map((arg) -> arg.join('=')).join(' ')
    for times in [0...N_REPETITIONS]
        do (times) ->
            command = PACMAN_CMD + " random_seed=#{Math.floor(Math.random()*10000000000)}" + (if argsString.length then " " else "") + argsString
            Q.push((finish) ->
                log.i "Experiment #{styler.id name}: Test ##{times+1}: Running #{styler.cmd command}..."

                exec(command + " > /dev/null", (error, stdout, stderr) ->
                    if error
                        log.e(error.toString(), exit: yes)
                    else
                        log.i "Experiment #{styler.id name}: Test ##{times+1}: Finished #{styler.cmd command}"

                    result = JSON.parse(stderr)
                    results[meter][times] = result[meter] for meter in METERS
                    results.execId[times] = result.neural_network[2..]

                    if ++count is N_REPETITIONS
                        results[meter] = summary(results[meter]) for meter in METERS
                        results[key] = { mean: v.mean(), max: v.max(), min: v.min(), sd: v.sd(), summary: v } for key, v of results when key isnt 'execId'
                        cb(results)

                    finish()
                )
            )

    null

runExperiment = (name, { variableArgs = {}, constantArgs = {} }, cb) ->
    count = 0

    variableArgsKeys = Object.keys(variableArgs)
    if variableArgsKeys.length > 0
        lengthFirst = variableArgs[variableArgsKeys[0]].length
    else
        lengthFirst = 1

    if lengthFirst is 0
        log.e "Experiment #{styler.id name}: Variable argument #{styler.id key} doesn't any value"

    for key, value of variableArgs
        if value.length isnt lengthFirst
            log.e "Experiment #{styler.id name}: Variable argument #{styler.id key} doesn't have required #{styler.id lengthFirst} # of values", exit: yes

    summaries = [0...lengthFirst]

    for i in [0...lengthFirst]
        do (i) ->
            args = _.cloneDeep(constantArgs)
            args[key] = variableArgs[key][i] for key in variableArgsKeys
            test(args, name, (summary) ->
                summary.args = args;
                summaries[i] = summary

                cb(summaries) if ++count is lengthFirst
            )

    null


experiments = [
    {
        name: "EvaluationPrecision"
        active: no
        variableArgs:
            test_statistics_precision: [5, 10, 25, 50, 100, 150, 200, 250, 300, 400, 500]
    }
    {
        name: "TestAttribute"
        active: no
        variableArgs:
            nn_evaluation_attribute: ["completion", "wins"]
        constantArgs:
            test_statistics_precision: 400
    }
    {
        name: "EvaluationPrecisionNarrow"
        active: no
        variableArgs:
            test_statistics_precision: [200, 300, 325, 350, 375, 400, 425, 450, 475, 500]
    }
    {
        name: "Plays"
        active: no
        variableArgs:
            plays: [5000, 7500, 10000,12500, 15000, 17500, 20000, 22500]
        foreach:
            name: "LearningRate"
            active: yes
            variableArgs:
                learning_rate: [0.00005, 0.000075, 0.0001, 0.0002, 0.0003, 0.0004, 0.0005]
    }
    {
        name: "Plays"
        active: yes
        variableArgs:
            plays: [50000, 10000]
        foreach:
            name: "HiddenNeurons"
            variableArgs:
                n_hidden_neurons: [50, 100, 200, 500, 1000, 2000]
    }
    {
        name: "SmartExploration"
        active: no
        variableArgs:
            smart_exploration: [1, 0]
        constantArgs:
            plays: 20000
    }
    {
        name: "SafeDistanceMargin"
        active: no
        variableArgs:
            safe_distance_margin: [0, 1, 2, 3, 4, 5, 6]
        constantArgs:
            plays: 20000
    }
    {
        name: "MaxIntersectionDistance"
        active: no
        variableArgs:
            max_intersection_distance: [1, 2, 3, 4, 5]
        constantArgs:
            plays: 20000
            n_hidden_neurons: 130
    }
    {
        name: "AnnealingMinProgression"
        active: no
        variableArgs:
            exploration_annealing_min_progression: [0, 0.05, 0.1, 0.15, 0.2, 0.25, 0.3]
        constantArgs:
            plays: 20000
            exploration_strategy: 'annealing'
    }
    {
        name: "AnnealingMaxProgression"
        active: no
        variableArgs:
            exploration_annealing_max_progression: [0.8, 0.85, 0.9, 0.95, 0.975, 0.99, 1]
        constantArgs:
            plays: 20000
            exploration_strategy: 'annealing'
    }
    {
        name: "ExplorationEpsilon"
        active: no
        variableArgs:
            exploration_epsilon: [0.05, 0.1, 0.15, 0.2, 0.25, 0.3, 0.35, 0.4, 0.5]
        constantArgs:
            plays: 20000
            exploration_strategy: 'epsilon'
    }
    {
        name: 'ExplorationEpsilonStopProgression'
        active: no
        variableArgs:
            exploration_epsilon_stop_progression: [0.8, 0.85, 0.9, 0.95, 0.975, 0.99, 1]
        constantArgs:
            plays: 20000
            exploration_strategy: 'epsilon'
    }
    {
        name: 'RewardPill'
        active: no
        variableArgs:
            reward_pill: [1, 3, 6, 9, 12, 15, 20, 25, 30, 35]
        constantArgs:
            plays: 7500
    }
    {
        name: 'RewardPowerpill'
        active: no
        variableArgs:
            reward_powerpill: [1, 2, 3, 4, 5, 6, 8, 11, 15, 20]
        constantArgs:
            plays: 7500
    }
    {
        name: 'RewardKillGhost'
        active: no
        variableArgs:
            reward_kill_ghost: [0, 1, 2, 4, 6, 10, 20]
        constantArgs:
            plays: 7500
    }
    {
        name: 'RewardWin'
        active: no
        variableArgs:
            reward_win: [0, 10, 20, 30, 40, 50, 60, 80, 120, 200]
        constantArgs:
            plays: 7500
    }
    {
        name: 'RewardLose'
        active: no
        variableArgs:
            reward_lose: [-100, -200,-250, -300, -350, -400, -450, -500, -600, -800]
        constantArgs:
            plays: 7500
    }
    {
        name: 'RewardReverse'
        active: no
        variableArgs:
            reward_reverse: [0, -1, -2, -4, -6, -8, -15, -20]
        constantArgs:
            plays: 7500
    }
    {
        name: 'RewardStep'
        active: no
        variableArgs:
            reward_step: [-1, -2, -3, -4, -5, -8, -11, -15, -20]
        constantArgs:
            plays: 7500
    }
    {
        name: "Rewards"
        active: no
        variableArgs:
            reward_pill: [12, 9]
            reward_powerpill: [6, 11]
            reward_kill_ghost: [0, 6]
            reward_win: [50, 60]
            reward_lose: [-350, -350]
            reward_reverse: [0, -1]
            reward_step: [-5, -4]
    }
]

treatForEach = (experiment, name, constantArgs) ->
    if experiment.foreach?
        variableArgsKeys = Object.keys(experiment.variableArgs)
        if variableArgsKeys.length > 0
            nDifferentValues = experiment.variableArgs[variableArgsKeys[0]].length
            for i in [0...nDifferentValues]
                newConstantArgs = _.cloneDeep(constantArgs)
                newName = name
                for key, value of experiment.variableArgs
                    newName = newName + "[#{key}=#{value[i]}]"
                    newConstantArgs[key] = value[i]
                newName = newName + ":"
                treatForEach(experiment.foreach, newName, newConstantArgs)
    else
        newExperiment = _.cloneDeep(experiment)
        newName = name + experiment.name
        newExperiment.name = newName
        newExperiment.constantArgs = constantArgs
        newExperiment.active = yes
        experiments.push(newExperiment)


for experiment in experiments when experiment.foreach? and experiment.active
    treatForEach(experiment, experiment.name, experiment.constantArgs ? {})

_.remove(experiments, (exp) -> exp.foreach)

for experiment in experiments when experiment.active
    do (experiment) ->
        runExperiment(experiment.name, { variableArgs: experiment.variableArgs, constantArgs: experiment.constantArgs },
            (summaries) ->
                json = { experiment, summaries }
                jsonString = JSON.stringify(json, null, 4)
                console.log jsonString
                fs.writeFileSync("#{DATA_PATH}/experiments/#{experiment.name}-#{Date.now()}.json", jsonString)
        )

Q.start()
