/* cppsrc/main.cpp */
#include <napi.h>
#include <iostream>
#include <sys/types.h> 
#include <sys/stat.h> 
#include <unistd.h> 
#include <fcntl.h> 
#include <assert.h>
#include <unordered_map>

using namespace Napi;
using namespace std;

int sockets[2];


void pushBuff(uint8_t *data, size_t len){
    /*
        write len and data
    */
    assert(sockets != nullptr);
    close(sockets[0]);
    if (write(sockets[1], &len, sizeof(size_t)) < 0)
        perror("writing message1");
    if (write(sockets[1], data, len) < 0)
        perror("writing message2");
    close(sockets[1]);

    cout << "ending " << getppid() << endl;
    /*
        Data is written
        End process and send SIGCHILD
    */
    raise(SIGCHILD);
    cout << "Shouldn't run ever" << endl;
}

Value getResult(const CallbackInfo& info) {
    Env env = info.Env();
    /*
        Get serialize function
    */
    Function deserialize = env.Global().Get("deserialize").As<Function>();

    /*
        Read len and data from pipe
    */
    assert(sockets != nullptr);
    close(sockets[1]);
    size_t len;
    if(read(sockets[0], &len, sizeof(size_t)) < 0)
        perror("writing message5");
    uint8_t* data = (uint8_t*) malloc(len);
    if (read(sockets[0], data, len) < 0)
        perror("writing message6");
    close(sockets[0]);

    /*
        Package and deserialize
    */
    Buffer<uint8_t> buf = Buffer<uint8_t>::New(env, data, len);
    napi_value napiBuf = (napi_value) buf;
    Value deserializedData = deserialize.Call(1, &napiBuf);
    return deserializedData;
}

void childResolve(const CallbackInfo& info) {
    cout << "childresolve" << endl;
    Env env = info.Env();
    Function serialize = env.Global().Get("serialize").As<Function>();
    if(info.Length() == 1) {
        napi_value data = (napi_value) info[0];
        Value serializedData = serialize.Call(1, &data);
        assert(serializedData.IsBuffer());
        Buffer<uint8_t> buf = serializedData.As<Buffer<uint8_t>>();
        size_t len = buf.Length();
        uint8_t* bufData = buf.Data();
        /* cout << "len = " << len << endl;
        for(int i = 0; i < len; i++) {
            cout << (int) bufData[i] << " ";
        }
        cout << endl; */
        pushBuff(bufData, len);
    } else {
        assert(false);
    }
}

void childReject(const CallbackInfo& info) {
    
}

Value makeThread(const CallbackInfo& info) {
    cout << "make thread"<<endl;
    Env env = info.Env();
    if(info.Length() == 1 && info[0].IsFunction()) {
        if (pipe(sockets) < 0) {
            perror("opening stream socket pair");
            exit(10);
        }
        pid_t pid = fork();
        if(pid == 0) { // child
            cout <<"child " << pid <<endl;
            pid = getpid();
            napi_value args[] = {
                (napi_value)Function::New(env, childResolve),
                (napi_value)Function::New(env, childReject)
            };
            info[0].As<Function>().Call(2, args);
        } else { // parent
            cout << "parent " << pid << endl;
            napi_value res;
            napi_create_uint32(env, pid, &res);
            return Value(env, res); 
        }
    } else {
        assert(false);
        return env.Undefined(); 
    }
}



Object Init(Env env, Object exports) {
    exports.Set(String::New(env, "makeThread"), Function::New(env, makeThread));
    exports.Set(String::New(env, "getResult"), Function::New(env, getResult));
    return exports;
}

NODE_API_MODULE(ThreadJS, Init)
