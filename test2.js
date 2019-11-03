var Promise = require("promise");
var Thread = require("./index.js");
var now = require("performance-now")

function isPrime(n) {
    if(n <= 2) {
        return true;
    }
    var sqrt = Math.sqrt(n);
    for(var i = 2; i <= sqrt; i++) {
        if(n % i == 0) {
            return false;
        }
    }
    return true;
}

function getPrimesInRange(start, end, log) { // [inclusive, exclusive]
    if(log) {
        console.log("getPrimesInRange",start, end);
    }
    var primes = [];
    for(var n = start; n < end; n++) {
        if(log) {
            console.log("primes:", n);
        }
        if(isPrime(n)) {
            primes.push(n);
        }
    }
    if(log) {
        console.log("primes: done");
    }
    return primes;
}

console.log("Let's make a thread");
Thread((resolve) => {
    var primes = getPrimesInRange(0, 10);
    resolve(primes);
}).then((primes) => {
    console.log("Calculated some primes in different thread!", primes);
    process.exit();
});

console.log("Now the thread is running");

var wait = (new Promise(resolve => setTimeout(resolve, 100000))).then(() => {
    // var allPrimes = primes.concat(secondPrimes);
    console.log("Done");
});