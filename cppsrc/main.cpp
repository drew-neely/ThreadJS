/* cppsrc/main.cpp */
#include <napi.h>
#include <iostream>
#include <sys/types.h> 
#include <unistd.h> 



using namespace Napi;
using namespace std;


void childResolve(const CallbackInfo& info) {
    Env env = info.Env();
    Function serialize = env.Global().Get("serialize").As<Function>();
    if(info.Length() == 1) {
        napi_value data = (napi_value) info[0];
        Value serializedData = serialize.Call(1, &data);
        assert(serializedData.IsBuffer());
        Buffer<uint8_t> buf = serializedData.As<Buffer<uint8_t>>();
        size_t len = buf.Length();
        uint8_t* bufData = buf.Data();
        cout << "len = " << len << endl;
        for(int i = 0; i < len; i++) {
            cout << (int) bufData[i] << " ";
        }
        cout << endl;
    } else {
        assert(false);
    }
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
        } else { // parent
            napi_value res;
            napi_create_uint32(env, pid, &res);
            return Value(env, res); 
        }
    } else {
        //fail
    }
}

// void registerSerial(const CallbackInfo& info) {
//     Env env = info.Env();
//     if(info.Length() == 3 && info[0].IsFunction() && info[1].IsFunction()) {
//         serialize = (Function*) malloc(sizeof(Function));
//         deserialize = (Function*) malloc(sizeof(Function));
//         *serialize = info[0].As<Function>();
//         *deserialize = info[1].As<Function>();
//         napi_value data = (napi_value) info[2];
//         Value serializedData = serialize->Call(1, &data);
//         cout << "rs data buf = " << serializedData.IsBuffer() << endl;
//         cout << "buf length = " << serializedData.As<Buffer<char>>().Length() << endl;
//     } else {
//         cout << info.Length() << endl;
//         cout << info[0].IsFunction() << endl;
//         cout << info[1].IsFunction() << endl;
//         assert(false); // incorrect call to registerSerial
//     }
// }

Object Init(Env env, Object exports) {
    exports.Set(String::New(env, "makeThread"), Function::New(env, makeThread));
    // exports.Set(String::New(env, "registerSerial"), Function::New(env, registerSerial));
    return exports;
}

NODE_API_MODULE(ThreadJS, Init)