
var Promise = require("promise");
var Thread = require("./index.js");
var now = require("performance-now")


const SINGLE_THREADED = false;
const maxPrime = 20;

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

var fistPrimes = [];
var secondPrimes = [];
var start;
var end;

if(SINGLE_THREADED) { // calculate in 1 thread
    start = now()
    primes = getPrimesInRange(0, maxPrime);
    end = now()
} else { // calculate in two threads
    start = now();
    var splitPoint = Math.round(maxPrime / 2);
    var thread = Thread((resolve, reject) => {
        var myprimes = getPrimesInRange(0, splitPoint, true);
        resolve(myprimes);
    }).then((p) => {
        console.log("IN THEN");
        console.log(p);
        firstPrimes = p;
        end = now();
        console.log("HERE");
    });
    secondPrimes = getPrimesInRange(splitPoint, maxPrime, true);
    console.log("RUNNING MAIN");
    // primes = getPrimesInRange(splitPoint, 0);
}

var wait = (new Promise(resolve => setTimeout(resolve, 2000))).then(() => {
    // var allPrimes = primes.concat(secondPrimes);
    console.log("Calculated", primes.length, "primes in", Math.round((end-start)*100)/100,"milliseconds.");
});