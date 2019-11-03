
var Promise = require("promise");
var Thread = require("./index.js");
var now = require("performance-now")


const SINGLE_THREADED = false;
const maxPrime = 10000;

function isPrime(n) {
    if(n == 2) {
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

var start;
var end;
var all;

if(SINGLE_THREADED) { // calculate in 1 thread
    start = now()
    primes = getPrimesInRange(0, maxPrime);
    end = now()
    console.log("Calculated", primes.length, "primes in", Math.round((end-start)*100)/100,"milliseconds.");
    process.exit();
} else { // calculate in two threads
    start = now();
    var splitPoint = Math.round(maxPrime / 2);
    var thread = Thread((resolve, reject) => {
        var myprimes = getPrimesInRange(0, splitPoint);
        resolve(myprimes);
    })
    var promise = new Promise((resolve, reject) => {
        var myprimes = getPrimesInRange(splitPoint, maxPrime);
        resolve(myprimes);
    })
    all = Promise.all([thread, promise]).then(res => {
        //console.log("IN ALL");
        end = now();
        console.log("Calculated", res[0].length + res[1].length, "primes in", Math.round((end-start)*100)/100,"milliseconds.");
        process.exit();
    }, (err) => {
        console.log(err);
    })
    // primes = getPrimesInRange(splitPoint, 0);
}

var wait = (new Promise(resolve => setTimeout(resolve, 100000))).then(() => {
    // var allPrimes = primes.concat(secondPrimes);
    console.log("Done");
});