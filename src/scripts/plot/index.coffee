ChartjsNode = require 'chartjs-node'
fs = require 'fs'
stream = require 'stream'
colorMap = require 'colormap'

DATA_PATH = __dirname + "/../../../data/"
EXPERIMENTS_PATH = DATA_PATH + "experiments/"
PLOTS_PATH = DATA_PATH + "plots/"

STATISTICS = ['mean', 'max', 'sd']
MEASURES = ['wins']

fs.mkdirSync(PLOTS_PATH) unless fs.existsSync(PLOTS_PATH)

createChart = (experimentFiles, yMeasurement, yStatistic, plotName, xAttribute) ->
    experimentFiles = [experimentFiles] unless Array.isArray(experimentFiles)

    colors = colorMap({ colormap: 'plasma', nshades: Math.max(experimentFiles.length, 50), format: 'hex', alpha: 1 }).reverse()

    datasets = [0...experimentFiles.length].map(-> { lineTension: 0.25 })
    data = null
    firstData = null

    for experimentFile, i in experimentFiles
        json = fs.readFileSync(EXPERIMENTS_PATH + experimentFile, { encoding: 'utf-8' })
        data = JSON.parse(json)

        firstData = data if i is 0

        xLength = data.summaries.length

        xAttribute ?= Object.keys(data.experiment.variableArgs)[0]

        datasets[i].label = if experimentFiles.length > 1 then experimentFile[experimentFile.indexOf('[')..experimentFile.lastIndexOf(']')] else ""

        yValues = [0...xLength]

        for test, j in data.summaries
            yValues[j] = test[yMeasurement][yStatistic]

        datasets[i].data = ({ x: data.experiment.variableArgs[xAttribute][j], y: yValues[j] } for j in [0...xLength])
        datasets[i].borderColor = colors[Math.round((i/Math.max((experimentFiles.length-1), 1))*(colors.length-1))]

    xLabels = ((value[i] for key, value of firstData.experiment.variableArgs).join(':') for i in [0...xLength])
    xAxisName = Object.keys(firstData.experiment.variableArgs).join(':')
    yAxisName = "#{yMeasurement}:#{yStatistic}"

    chartNode = new ChartjsNode(800, 800)

    chartNode.drawChart({
        type: 'line',
        data: {
            labels: xLabels,
            datasets
        },
        options: {
            legend: {
                display: experimentFiles.length > 1
            }
            scales: {
                yAxes: [{
                    ticks: {
                        beginAtZero: true
                    },
                    scaleLabel: {
                        display: true,
                        labelString: yAxisName
                        fontSize: 14
                        fontStyle: 'bold'
                        fontColor: 'black'
                    }
                }],
                xAxes: [{
                    scaleLabel: {
                        display: true,
                        labelString: xAxisName
                        fontSize: 14
                        fontStyle: 'bold'
                        fontColor: 'black'
                    }
                }]
            },
            tooltips: {
                mode: 'label'
            }
        }
    })
    .then(-> chartNode.getImageBuffer('image/png'))
    .then((buffer) -> chartNode.getImageStream('image/png'))
    .then((streamResult) -> chartNode.writeImageToFile('image/png', PLOTS_PATH + plotName + "-" + yAxisName.replace(':', '_') + ".png"))
    .then(-> console.log "Plot saved");

experimentSets = {}

for file in fs.readdirSync(DATA_PATH + 'experiments')
    name = file.split(/\[[^\]]+\]/gi).join('').replace(/-\d+\.json/, '')
    experimentSets[name] ?= []
    experimentSets[name].push(file)

parameterArray = (experimentFile) -> experimentFile.match(/\[[^=]+=(\d+)\]/g).map((x) -> Number(x[1...-1].split('=')[1]))

for k, array of experimentSets
    experimentSets[k].sort((a, b) ->
            pa = parameterArray(a)
            pb = parameterArray(b)

            for elem, i in a
                if pa[i] > pb[i]
                    return 1
                else if pa[i] < pb[i]
                    return -1
            0
        )

console.log experimentSets

for name, experimentSet of experimentSets
    for measure in MEASURES
        for statistic in STATISTICS
            createChart(experimentSet, measure, statistic, name)
