const fs = require("fs")
const request = require('request')

const DEVICES_FILENAME = './devices/devices.txt'
const SELECTED_DEVICE_FILENAME = './devices/selectedDevice.txt'

const isValidIPAddress = ipAddress => {
    const regexExp = /^(([0-9]|[1-9][0-9]|1[0-9]{2}|2[0-4][0-9]|25[0-5])\.){3}([0-9]|[1-9][0-9]|1[0-9]{2}|2[0-4][0-9]|25[0-5])$/gi
    return regexExp.test(ipAddress)
}

const readDevicesToMap = () => {
    const map = new Map()
    const entries = fs.readFileSync(
        DEVICES_FILENAME, 
        {encoding: 'utf8', flag: 'r'}
    ).split('\n')
    for (const entry of entries) {
        if (entry == '') continue
        const [deviceName, deviceIP] = entry.split(' ')
        map.set(deviceName, deviceIP)
    }
    return map
}

const writeDevicesToFile = map => {
    fs.writeFileSync(DEVICES_FILENAME, '')  // clear file
    for (const [deviceName, deviceIP] of map) {
        fs.appendFileSync(
            DEVICES_FILENAME,
            `${deviceName} ${deviceIP}\n`
        )
    }
}

const printDevicesFromFile = () => {
    const entries = fs.readFileSync(
        DEVICES_FILENAME, 
        {encoding: 'utf8', flag: 'r'}
    ).split('\n')
    if (entries.size === 0) {
        console.log('No devices were registered!')
        return
    }
    for (const entry of entries) {
        if (entry == '') continue
        const [deviceName, deviceIP] = entry.split(' ')
        console.log(`Name: ${deviceName}; IP: ${deviceIP}`)
    }
}

const getSelectedDeviceIP = () => {
    return fs.readFileSync(
        SELECTED_DEVICE_FILENAME, 
        {encoding: 'utf8', flag: 'r'}
    )
}

const setSelectedDevice = deviceIP => {
    fs.writeFileSync(
        SELECTED_DEVICE_FILENAME, 
        deviceIP,
        {encoding: 'utf8', flag: 'w'}    
    )
}

const sendRequestToDevice = (url, method, args) => {
    const reqObj = {
        'method': method,
        'url': url,
        'timeout': 5000,
        'headers': {},
    }
    if (method == 'GET' && args) {
        reqObj['qs'] = args
    }
    if (method == 'POST' && args) {
        reqObj['formData'] = args
    }

    console.log(`Request time: ${getCurrentTime()}`)
    request(reqObj, (err, res) => {
        console.log(`Response time: ${getCurrentTime()}`)
        if (err) {
            console.log('An error ocurred, or the device is not available.')
            // console.error(err)
            return
        }
        console.log(JSON.parse(res.body))
    })
}

const sleep = ms => new Promise(resolve => { setTimeout(resolve, ms) })

const getCurrentTime = () => {
    const time = new Date()
    return time.getHours() + ":" + time.getMinutes() + ":" + time.getSeconds() + "." + time.getMilliseconds();
}

module.exports = {
    isValidIPAddress,
    readDevicesToMap,
    writeDevicesToFile,
    printDevicesFromFile,
    getSelectedDeviceIP,
    setSelectedDevice,
    sendRequestToDevice,
    sleep,
}