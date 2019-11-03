/* cppsrc/main.cpp */
#include <napi.h>
#include <iostream>
#include <sys/types.h> 
#include <unistd.h> 



using namespace Napi;
using namespace std;

Value add(const CallbackInfo& info) {
    Env env = info.Env();
    double sum = 0;
    for(int i = 0; i < info.Length(); i++) {
        if(info[i].IsNumber()) {
            sum += info[i].ToNumber().DoubleValue();
        } else {
            // fail
        }
    }
    napi_value result;
    napi_status status = napi_create_double(env, sum, &result);
    return Value(env, result);
}

void childResolve(const CallbackInfo& info) {

}

void childReject(const CallbackInfo& info) {
    
}

Value makeThread(const CallbackInfo& info) {
    Env env = info.Env();
    if(info.Length() == 3 && info[0].IsFunction() && info[1].IsFunction() && info[2].IsFunction()) {
        int pid = fork();
        if(pid == 0) { // child
            napi_value args[] = {
                (napi_value)Function::New(env, childResolve),
                (napi_value)Function::New(env, childReject)
            };
            Value val = info[0].As<Function>().Call(2, args);
            
            exit(0);
        } else {
            napi_value res;
            napi_create_uint32(env, pid, &res);
            return Value(env, res);
        }
    } else {
        //fail
    }
}

Object Init(Env env, Object exports) {
    exports.Set(String::New(env, "add"), Function::New(env, add));
    exports.Set(String::New(env, "makeThread"), Function::New(env, makeThread));
    return exports;
}

NODE_API_MODULE(ThreadJS, Init)