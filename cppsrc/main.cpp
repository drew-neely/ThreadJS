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

unordered_map<pid_t, int*> socketMap; 
int pidBuf[2];
char * myfifo = "/tmp/myfifo";
int fd;


void pushBuff(uint8_t *data, size_t len){
    cout << "pushBuff" << endl;
    pid_t pid = getpid();
    cout << "pushBuff pid = " << pid << endl;
    int* sockets = socketMap[pid];
    assert(sockets != nullptr);
    close(sockets[0]);
    if (write(sockets[1], &len, sizeof(size_t)) < 0)
        perror("writing message1");
    if (write(sockets[1], data, len) < 0)
        perror("writing message2");
    close(sockets[1]);
    cout << "wrote data" << endl;
    // close(pidBuf[0]);
    // int fd = open(myfifo, O_WRONLY);
    cout << "opened" << endl;
    write(fd, &pid, sizeof(pid_t));
    cout << "wrote pid: " << pid << endl;
    /* if (write(pidBuf[1], &pid, sizeof(pid_t)) < 0)
        perror("writing message3");
    close(pidBuf[1]); */
    cout << "Dying" << endl;
    exit(0);
}

Value getFinishedPid(const CallbackInfo& info){
    Env env = info.Env();
    // int fd = open(myfifo, O_RDONLY);
    pid_t pid;
    cout << "HEREER" <<endl;
    read(fd, (void*)&pid, sizeof(pid_t));
    cout << "getFinishedPid pid = " << pid << endl;
    /* close(pidBuf[1]);
    pid_t pid;
    if(read(pidBuf[0], &pid, sizeof(pid_t)) < 0)
        perror("writing message4");
    close(pidBuf[0]); */
    napi_value res;
    napi_create_uint32(env, pid, &res);
    return Value(env, res); 
}

Value getResult(const CallbackInfo& info) {
    Env env = info.Env();
    Function deserialize = env.Global().Get("deserialize").As<Function>();
    if(info.Length() == 1 && info[0].IsNumber()) {
        uint32_t pid = info[0].ToNumber().Uint32Value();
        cout << "pid = " << pid << endl;
        int* sockets = socketMap[pid];
        assert(sockets != nullptr);
        close(sockets[1]);
        size_t len;
        if(read(sockets[0], &len, sizeof(size_t)) < 0)
            perror("writing message5");
                    

        uint8_t* data = (uint8_t*) malloc(len);
        if (read(sockets[0], data, len) < 0)
            perror("writing message6");
        close(sockets[0]);
        Buffer<uint8_t> buf = Buffer<uint8_t>::New(env, data, len);
        napi_value napiBuf = (napi_value) buf;
        Value deserializedData = deserialize.Call(1, &napiBuf);
        return deserializedData;
    } else {
        assert(false);
    }
}

void childResolve(const CallbackInfo& info) {
    cout << "childResolve" << endl;
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
    cout << "makeThread" << endl;
    Env env = info.Env();
    if(info.Length() == 1 && info[0].IsFunction()) {
        int* sockets = (int*) malloc(2*sizeof(int));
        if (pipe(sockets) < 0) {
            perror("opening stream socket pair");
            exit(10);
        }
        pid_t pid = fork();
        if(pid == 0) { // child
            pid = getpid();
            socketMap[pid] = sockets;
            napi_value args[] = {
                (napi_value)Function::New(env, childResolve),
                (napi_value)Function::New(env, childReject)
            };
            Value val = info[0].As<Function>().Call(2, args);
        } else { // parent
            socketMap[pid] = sockets;
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
    /* if (pipe(pidBuf) < 0) {
        perror("opening stream socket pair");
        exit(10);
    } */
    mkfifo(myfifo, 7777);
    fd = open(myfifo, O_RDWR|O_SYNC);

    exports.Set(String::New(env, "makeThread"), Function::New(env, makeThread));
    exports.Set(String::New(env, "getResult"), Function::New(env, getResult));
    exports.Set(String::New(env, "getFinishedPid"), Function::New(env, getFinishedPid));
    // exports.Set(String::New(env, "registerSerial"), Function::New(env, registerSerial));
    return exports;
}

NODE_API_MODULE(ThreadJS, Init)
