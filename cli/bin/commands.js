const utils = require('./utils')
const fs = require('fs')

const configDevice = args => {
    if (args.length < 3) {
        console.log('Need to specify device name and device IP!')
        return
    }
    const deviceName = args[1]
    const deviceIP = args[2]
    if (!utils.isValidIPAddress(deviceIP)) {
        console.log('The specified IP address is not valid!')
        return
    }
    const map = utils.readDevicesToMap()
    map.set(deviceName, deviceIP)
    utils.writeDevicesToFile(map)
    console.log(`Successfully added device "${deviceName}" with IP ${deviceIP}`)
}

const showDevices = args => {
    utils.printDevicesFromFile()
}

const selectDevice = args => {
    if (args.length < 2) {
        console.log('Need to specify device name!')
        return
    }
    const deviceName = args[1]
    const map = utils.readDevicesToMap()
    if (!map.has(deviceName)) {
        console.log('Device was not registed!')
        return
    }
    const deviceIP = map.get(deviceName)
    utils.setSelectedDevice(deviceIP)
    console.log(`Successfully selected device "${deviceName}" with IP ${deviceIP}`)
}

const uploadTask = args => {
    if (args.length < 2) {
        console.log('Need to specify filename!')
        return
    }
    const deviceIP = utils.getSelectedDeviceIP()
    if (!deviceIP) {
        console.log('No device was selected yet!')
        return
    }
    const filepath = `./files/${args[1]}`
    if (!fs.existsSync(filepath)) {
        console.log('The specified file does not exist!')
        return
    }
    const formData = {
        'wasmfile': {
            'value': fs.createReadStream(filepath),
            'options': {
                'filename': args[1],
                'contentType': null
            }
        },
    }
    if (args.length >= 3) formData['reservedStackSize'] = args[2]
    if (args.length >= 4) formData['reservedInitialMemory'] = args[3]
    if (args.length >= 5) formData['memoryLimit'] = args[4]

    utils.sendRequestToDevice(
        `http://${deviceIP}/task/upload`,
        'POST',
        formData,
    )
}

const deleteTask = args => {
    if (args.length < 2) {
        console.log('Need to specify filename!')
        return
    }
    const deviceIP = utils.getSelectedDeviceIP()
    if (!deviceIP) {
        console.log('No device was selected yet!')
        return
    }
    const filename = args[1]

    utils.sendRequestToDevice(
        `http://${deviceIP}/task/delete`,
        'POST',
        { 'filename': filename },
    )
}

const startTask = args => {
    if (args.length < 2) {
        console.log('Need to specify filename!')
        return
    }
    const deviceIP = utils.getSelectedDeviceIP()
    if (!deviceIP) {
        console.log('No device was selected yet!')
        return
    }
    const filename = args[1]

    utils.sendRequestToDevice(
        `http://${deviceIP}/task/start`,
        'POST',
        { 'filename': filename },
    )
}

const stopTask = args => {
    if (args.length < 2) {
        console.log('Need to specify filename!')
        return
    }
    const deviceIP = utils.getSelectedDeviceIP()
    if (!deviceIP) {
        console.log('No device was selected yet!')
        return
    }
    const filename = args[1]

    utils.sendRequestToDevice(
        `http://${deviceIP}/task/stop`,
        'POST',
        { 'filename': filename },
    )
}

const pauseTask = args => {
    if (args.length < 2) {
        console.log('Need to specify filename!')
        return
    }
    const deviceIP = utils.getSelectedDeviceIP()
    if (!deviceIP) {
        console.log('No device was selected yet!')
        return
    }
    const filename = args[1]

    utils.sendRequestToDevice(
        `http://${deviceIP}/task/pause`,
        'POST',
        { 'filename': filename },
    )
}

const unpauseTask = args => {
    if (args.length < 2) {
        console.log('Need to specify filename!')
        return
    }
    const deviceIP = utils.getSelectedDeviceIP()
    if (!deviceIP) {
        console.log('No device was selected yet!')
        return
    }
    const filename = args[1]

    utils.sendRequestToDevice(
        `http://${deviceIP}/task/unpause`,
        'POST',
        { 'filename': filename },
    )
}

const getTaskDetails = args => {
    const deviceIP = utils.getSelectedDeviceIP()
    if (!deviceIP) {
        console.log('No device was selected yet!')
        return
    }

    utils.sendRequestToDevice(
        `http://${deviceIP}/task/details`,
        'GET',
    )
}

const editTaskDetails = args => {
    if (args.length < 2) {
        console.log('Need to specify filename!')
        return
    }
    const deviceIP = utils.getSelectedDeviceIP()
    if (!deviceIP) {
        console.log('No device was selected yet!')
        return
    }
    const params = { 'filename': args[1] }
    if (args.length >= 3) formData['reservedStackSize'] = args[2]
    if (args.length >= 4) formData['reservedInitialMemory'] = args[3]
    if (args.length >= 5) formData['memoryLimit'] = args[4]

    utils.sendRequestToDevice(
        `http://${deviceIP}/task/edit`,
        'POST',
        params,
    )
}

const getActiveTaskDetails = args => {
    const deviceIP = utils.getSelectedDeviceIP()
    if (!deviceIP) {
        console.log('No device was selected yet!')
        return
    }

    utils.sendRequestToDevice(
        `http://${deviceIP}/task/active`,
        'GET',
    )
}

const getFreeHeapSize = args => {
    const deviceIP = utils.getSelectedDeviceIP()
    if (!deviceIP) {
        console.log('No device was selected yet!')
        return
    }

    utils.sendRequestToDevice(
        `http://${deviceIP}/heap`,
        'GET',
    )
}

const getHeapInfo = args => {
    const deviceIP = utils.getSelectedDeviceIP()
    if (!deviceIP) {
        console.log('No device was selected yet!')
        return
    }

    utils.sendRequestToDevice(
        `http://${deviceIP}/heapinfo`,
        'GET',
    )
}

const execPerformanceTest = async args => {
    const deviceIP = utils.getSelectedDeviceIP()
    if (!deviceIP) {
        console.log('No device was selected yet!')
        return
    }

    const filename = 'performance.wasm'
    for (let i = 1; i <= 20; i++) {
        console.log(`Starting task #${i}`)
        startTask(['start', filename])
        await utils.sleep(5000)
        console.log(`Stopping task #${i}`)
        stopTask(['stop', filename])
        await utils.sleep(2000)
    }
}

const execMaxLoadTest = async args => {
    const deviceIP = utils.getSelectedDeviceIP()
    if (!deviceIP) {
        console.log('No device was selected yet!')
        return
    }

    const filename = 'max_load_test.wasm'
    for (let i = 1; ; i++) {
        console.log(`Starting task #${i}`)
        startTask(['start', filename])
        await utils.sleep(2000)
    }
}

const exec7ThreadsLoadTest = async args => {
    const deviceIP = utils.getSelectedDeviceIP()
    if (!deviceIP) {
        console.log('No device was selected yet!')
        return
    }
    for (let i = 1; i <= 7; i++) {
        const filename = `7threads_test${i}.wasm`
        console.log(`Starting task #${i}`)
        startTask(['start', filename])
        await utils.sleep(1000)
    }

    await utils.sleep(10000)
    getHeapInfo('heapinfo')
}

const execFullLoadTestWithTimes = async args => {
    const MAX_NUM_THREADS = 19  // maximum number of threads for the full_load_test task

    const deviceIP = utils.getSelectedDeviceIP()
    if (!deviceIP) {
        console.log('No device was selected yet!')
        return
    }
    for (let i = 1; i <= MAX_NUM_THREADS; i++) {
        const filename = `full_load_test.wasm`
        console.log(`Starting task #${i}`)
        startTask(['start', filename])
        await utils.sleep(2000)
    }
}

const execDynamicTest = async args => {
    if (args.length < 2) {
        console.log('Need to specify what dynamic test to run (1, 2 or 3)!')
        return
    }
    const deviceIP = utils.getSelectedDeviceIP()
    if (!deviceIP) {
        console.log('No device was selected yet!')
        return
    }
    const dynamicTest = parseInt(args[1])
    switch (dynamicTest) {
        case 1:
            for (let i = 1; i <= 5; i++) {
                console.log(`Task Upload #${i}`)
                uploadTask(['upload', `dynamic${i}.wasm`, 6000, 0, 2048])
                await utils.sleep(2000)
                console.log(`Task Creation #${i}`)
                startTask(['start', `dynamic${i}.wasm`])
                await utils.sleep(2000)
            }
            for (let i = 1; i <= 5; i++) {
                console.log(`Task Stoppage #${i}`)
                stopTask(['stop', `dynamic${i}.wasm`])
                await utils.sleep(2000)
            }
            break
        
        // in this case, upload all 5 files first manually, and then run the dynamic test
        case 2:
            console.log(`Task Upload #1`)
            uploadTask(['upload', `dynamic1.wasm`, 6000, 0, 2048])
            await utils.sleep(2000)
            console.log("Task Creation #1")
            startTask(['start', 'dynamic1.wasm'])
            await utils.sleep(2000)
            console.log(`Task Upload #2`)
            uploadTask(['upload', `dynamic2.wasm`, 6000, 0, 2048])
            await utils.sleep(2000)
            console.log("Task Creation #2")
            startTask(['start', 'dynamic2.wasm'])
            await utils.sleep(2000)
            console.log("Task Stoppage #1")
            stopTask(['stop', 'dynamic1.wasm'])
            await utils.sleep(2000)
            console.log(`Task Upload #3`)
            uploadTask(['upload', `dynamic3.wasm`, 6000, 0, 2048])
            await utils.sleep(2000)
            console.log("Task Creation #3")
            startTask(['start', 'dynamic3.wasm'])
            await utils.sleep(2000)
            console.log(`Task Upload #4`)
            uploadTask(['upload', `dynamic4.wasm`, 6000, 0, 2048])
            await utils.sleep(2000)
            console.log("Task Creation #4")
            startTask(['start', 'dynamic4.wasm'])
            await utils.sleep(2000)
            console.log("Task Stoppage #3")
            stopTask(['stop', 'dynamic3.wasm'])
            await utils.sleep(2000)
            console.log("Task Stoppage #4")
            stopTask(['stop', 'dynamic4.wasm'])
            await utils.sleep(2000)
            console.log(`Task Upload #5`)
            uploadTask(['upload', `dynamic5.wasm`, 6000, 0, 2048])
            await utils.sleep(2000)
            console.log("Task Creation #5")
            startTask(['start', 'dynamic5.wasm'])
            await utils.sleep(2000)
            console.log("Task Stoppage #2")
            stopTask(['stop', 'dynamic2.wasm'])
            await utils.sleep(2000)
            console.log("Task Stoppage #5")
            stopTask(['stop', 'dynamic5.wasm'])
            await utils.sleep(2000)
            break
        
        // in this case, upload the first 3 files manually, and then run the dynamic test 
        case 3:
            console.log("Task Creation #1")
            startTask(['start', 'dynamic1.wasm'])
            await utils.sleep(2000)
            console.log("Task Creation #2")
            startTask(['start', 'dynamic2.wasm'])
            await utils.sleep(2000)
            console.log("Task Pause #1")
            pauseTask(['pause', 'dynamic1.wasm'])
            await utils.sleep(2000)
            console.log("Task Creation #3")
            startTask(['start', 'dynamic3.wasm'])
            await utils.sleep(2000)
            console.log("Task Stoppage #2")
            stopTask(['stop', 'dynamic2.wasm'])
            await utils.sleep(2000)
            console.log("Task Pause #3")
            pauseTask(['pause', 'dynamic3.wasm'])
            await utils.sleep(2000)
            console.log("Task Unpause #1")
            unpauseTask(['unpause', 'dynamic1.wasm'])
            await utils.sleep(2000)
            console.log("Task Stoppage #1")
            stopTask(['stop', 'dynamic1.wasm'])
            await utils.sleep(2000)
            console.log("Task Unpause #3")
            unpauseTask(['unpause', 'dynamic3.wasm'])
            await utils.sleep(2000)
            console.log("Task Stoppage #3")
            stopTask(['stop', 'dynamic3.wasm'])
            await utils.sleep(2000)
            break

        default:
            console.log("Dynamic test not supported!")
            break
    }
}

module.exports = {
    configDevice,
    showDevices,
    selectDevice,
    uploadTask,
    deleteTask,
    startTask,
    stopTask,
    pauseTask,
    unpauseTask,
    getTaskDetails,
    editTaskDetails,
    getActiveTaskDetails,
    getFreeHeapSize,
    getHeapInfo,

    execPerformanceTest,
    execMaxLoadTest,
    exec7ThreadsLoadTest,
    execFullLoadTestWithTimes,
    execDynamicTest,
}