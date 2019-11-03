//index.js
var ThreadJS = require('./build/Release/ThreadJS.node');
var Promise = require('promise');
var { serialize, deserialize } = require('v8');
global.serialize = serialize;
global.deserialize = deserialize;

var resolveFunctionDict = {};

process.on('SIGCHLD', () => {
    console.log('Received SIGCHLD');
    var finishedPid = ThreadJS.getFinishedPid();
    console.log("js finished pid =", finishedPid);
    result = ThreadJS.getResult(finishedPid);
    resolveFunctionDict[finishedPid](result);
});

var sleep = (milliseconds) => {
    return new Promise(resolve => setTimeout(resolve, milliseconds))
}



var Thread = function(work) {
    return new Promise((resolve, reject) => {
        var childPid = ThreadJS.makeThread(work);
        resolveFunctionDict[childPid] = resolve;
    });
};

var thread = Thread((resolve, reject) => {
    console.log("I'm running!");
    resolve("The quick brown fox thdiosdlkhsagoirsjklafd");
})
.then((result) => console.log("res", result));

var thread1 = Thread((resolve, reject) => {
    console.log("I'm running!");
    resolve("The quick brown fox thdiosdlkhsagoirsliasdhgslsjfgkskdghjskteopout;j;rwejijklafd");
})
.then((result) => {console.log("res", result); /* sleep(1000000).then(e => console.log("Yoo")) */;});


sleep(4000).then(() => console.log("Done"));

module.exports = ThreadJS;
