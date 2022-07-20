#! /usr/bin/env node
const commands = require('./commands')
const yargs = require("yargs")

const usage = '\nUsage: wasmico <operation_name> <operation_args>'
const options = yargs
    .usage(usage)                                    
    .help(true)
    .locale('english')
    .argv

const commandFunctions = {
    'config': commands.configDevice,
    'devices': commands.showDevices,
    'select': commands.selectDevice,
    'upload': commands.uploadTask,
    'delete': commands.deleteTask,
    'start': commands.startTask,
    'stop': commands.stopTask,
    'pause': commands.pauseTask,
    'unpause': commands.unpauseTask,
    'showactive': commands.getActiveTaskDetails,
    'details': commands.getTaskDetails,
    'edit': commands.editTaskDetails,
    'heap': commands.getFreeHeapSize,
    'heapinfo': commands.getHeapInfo,

    'performance': commands.execPerformanceTest,
    'maxload': commands.execMaxLoadTest,
    '7threadsload': commands.exec7ThreadsLoadTest,
    'fullloadwithtimes': commands.execFullLoadTestWithTimes,
    'dynamic': commands.execDynamicTest,
}

const runCLI = () => {
    if (yargs.argv._[0] == null) {  
        yargs.showHelp()
        return
    }

    const commandFunction = commandFunctions[yargs.argv._[0]]
    if (!commandFunction) {
        yargs.showHelp()
        return
    }

    commandFunction(yargs.argv._)
}

runCLI()