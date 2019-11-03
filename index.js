//index.js
var ThreadJS = require('./build/Release/ThreadJS.node');
var Promise = require('promise');
var { serialize, deserialize } = require('v8');
global.serialize = serialize;
global.deserialize = deserialize;

process.on('SIGINT', () => {

});

var sleep = (milliseconds) => {
    return new Promise(resolve => setTimeout(resolve, milliseconds))
}

class Thread extends Promise {
    constructor(work) {
        super((resolve, reject) => {
            ThreadJS.makeThread(work, resolve, reject);
        });
    }
}

var thread = new Thread((resolve, reject) => {
    console.log("I'm running!");
    resolve(1000);
});


sleep(1000).then(() => console.log("Done"));

module.exports = ThreadJS;