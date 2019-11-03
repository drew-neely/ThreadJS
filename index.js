//index.js
var ThreadJS = require('./build/Release/ThreadJS.node');
var Promise = require('promise');
var { serialize, deserialize } = require('v8');
global.serialize = serialize;
global.deserialize = deserialize;

var resolveFunction;

process.on('SIGCHLD', () => {
    console.log('Received SIGCHLD');
    result = ThreadJS.getResult();
    resolveFunction(result);
});

var sleep = (milliseconds) => {
    return new Promise(resolve => setTimeout(resolve, milliseconds))
}

var Thread = function(work) {
    return new Promise((resolve, reject) => {
        resolveFunction = resolve;
        var childPid = ThreadJS.makeThread(work);
    });
};

var thread = Thread((resolve, reject) => {
    resolve(90);
}).then(console.log);

console.log("Made Thread");

sleep(1000).then(() => console.log("DONE"));

module.exports = Thread;
