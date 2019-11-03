//index.js
const ThreadJS = require('./build/Release/ThreadJS.node');
var Promise = require('promise');

var makeThread = ThreadJS.makeThread;

const sleep = (milliseconds) => {
    return new Promise(resolve => setTimeout(resolve, milliseconds))
}

class Thread extends Promise {
    constructor(work) {
        super((reolve, reject) => {
            makeThread(work, resolve, reject);
        });
    }
}

var thread = new Thread((resolve, reject) => {
    console.log("I'm running!");
    sleep(5000).then(() => console.log("Done"));
});

module.exports = ThreadJS;