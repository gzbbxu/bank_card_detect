//
// Created by zkk on 2019/3/28.
//

#ifndef FACERECOGNITION_WRITESERVICE_H
#define FACERECOGNITION_WRITESERVICE_H

#include <string>
#include <jni.h>
#include "SyncQueue.hpp"
#include <vector>
#include <mutex>
#include "Alog_pri.h"
using namespace std;
#define VERIFY_RESULT 0

namespace service {
    enum MsgType {
        imgWrite = 1,
        hertWrite = 2,
        bankCardWrite = 1000,
        drugBox = 2000,
    };

    class WriteService {

    public:

        JavaVM *mJvm;

        static mutex mMutex;
        static SyncQueue<char *> *syncQueue;
        static SyncQueue<char *> * detectResultQue;
        static long timeSecond;
        static WriteService *iNstance;
        static int perLen;

        static int sockFd;


        WriteService();

        void create_thread();

        void connectSocket();

        void writeString(std::vector<unsigned char> &vec, MsgType msgType,const  char * parameter,int parameterLen);

        void writeString(std::string &str, MsgType msgType);

        ~WriteService();

        void parseReceiveData(char *string, JNIEnv *env);




    private:


        int readString(string &str);

        //心跳检测
        void hertJumpCheck();

        static void *readConsume(void *pVoid);

        static void *writeConsume(void *pVoid);


        static void setCurrentime();

        char *buildData(const MsgType &msgType, const string &encodeStr) const;

        char *buildData(const MsgType &msgType, const unsigned char *pStr, int len) const;

        char *buildData(const MsgType &msgType, const unsigned char *pStr, int len,char* otherParmater,int otherParamaterLen) const;

        static void *acceptThread(void *pVoid);


        static void * receiveThread(void * pVoid);

        static void *sendThread(void * pVoid);

        static void jumpTimeCheck(long * time);
    };

}
#endif //FACERECOGNITION_WRITESERVICE_H
