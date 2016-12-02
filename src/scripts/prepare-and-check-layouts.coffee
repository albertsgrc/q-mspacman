fs = require 'fs'

LAYOUTS_PATH = "#{__dirname}/../layouts/"
layouts = fs.readdirSync(LAYOUTS_PATH)

VALID_CHARACTERS = [' ', '%', 'P', 'G', 'o', '.']

countChar = (str, char) -> str.split(char).length - 1

checks = [
    ["Maze has no rows", (layout) -> layout.split('\n').length is 0]
    ["Maze has no columns", (layout) -> layout.split('\n')[0].length is 0]
    ["Not all rows have same size", (layout) -> rows = layout.split('\n'); rows.some((r) -> console.log r if r.length isnt rows[0].length; r.length isnt rows[0].length)]
    ["More than one pacman", (layout) -> countChar(layout, 'P') > 1]
    ["No pacman", (layout) -> countChar(layout, 'P') is 0]
    ["Invalid character", (layout) -> (c for c in layout when c isnt '\n').some((c) -> c not in VALID_CHARACTERS)]
    ["More than 4 ghosts", (lauout) -> countChar(layout, 'G') > 4]
]

error = (layoutPath, text) ->
    console.error "#{layoutPath}: #{text}"
    process.exit(1)

checkLayout = (layout, layoutPath) ->
    for [errorMsg, isNotOkay] in checks
        if isNotOkay layout
            error layoutPath, errorMsg

for layoutPath, i in layouts
    fullPath = LAYOUTS_PATH + layoutPath
    layout = fs.readFileSync(fullPath, 'utf-8').trimRight()

    firstLine = layout[...layout.indexOf('\n')].split(' ')
    hasSizes = no
    if firstLine.length is 2
        [rows, cols] = [parseInt(firstLine[0]), parseInt(firstLine[1])]
        if not isNaN(rows) and not isNaN(cols)
            hasSizes = true
            layout = layout[layout.indexOf('\n') + 1..]

    checkLayout(layout, layoutPath)

    layoutRows = layout.split('\n')
    rowsReal = layoutRows.length
    colsReal = layoutRows[0].length

    if hasSizes
        if rowsReal isnt rows or colsReal isnt cols
            error layoutPath, "The indicated sizes are not correct"
    else
        layout = "#{rowsReal} #{colsReal}\n#{layout}"
        fs.writeFileSync(fullPath, layout)
