//
// Created by zkk on 2019/3/28.
//

#include "WriteService.h"
#include "SocketServer.h"
#include <unistd.h>
#include <jni.h>
#include "SyncQueue.hpp"
#include <arpa/inet.h>
#include <vector>
#include <fcntl.h>
#include <socket/commsocket.h>
#include "WriteFactory.hpp"

using namespace service;

#define LOG_TAG "LOG_TAG_Base64Write"
int WriteService::sockFd = -1;
int WriteService::perLen = sizeof(int32_t) * 2;
extern jobject obj;
extern jmethodID callbackId;
extern jmethodID parseMethod;
extern JavaVM *jvm;
extern jmethodID openDetect;
SyncQueue<char *> *WriteService::syncQueue = NULL;
SyncQueue<char *> *WriteService::detectResultQue = NULL;

long WriteService::timeSecond = 0;
mutex WriteService::mMutex;
WriteService *WriteService::iNstance = NULL;


void WriteService::setCurrentime() {
    timespec time;
    clock_gettime(CLOCK_REALTIME, &time);
    timeSecond = time.tv_sec;
}


void WriteService::writeString(std::string &data, MsgType msgType) {
    //todo
    IWrite *writer = WriteFactory::getWrite(msgType);
    if (writer->getType() == hertWrite) {
        char *rPdata = buildData(msgType, data);
        syncQueue->Put(rPdata);
    }
}

void WriteService::writeString(std::vector<unsigned char> &vect, MsgType msgType,const  char * parameter,int parameterLen) {

    char *bufferOrigin = new char[vect.size()]; //构造源数据指针
    memcpy(bufferOrigin, &vect[0], vect.size() * sizeof(char));
    const unsigned char *dest = (const unsigned char *) bufferOrigin;
    IWrite *writer = WriteFactory::getWrite(msgType);

    if (writer->getType() == imgWrite) {
        string encodeStr = writer->encode(dest, vect.size());
        //test save
/*           struct timespec now;
           clock_gettime(CLOCK_MONOTONIC, &now);
           char path[255] = {0};
           sprintf(path, "/sdcard/test/aaa%ld", now.tv_nsec);
           int fd = open(path, O_CREAT | O_WRONLY);
           write(fd, encodeStr.c_str(), encodeStr.length());
           close(fd);*/


        char *rPdata = buildData(msgType, encodeStr);
        syncQueue->Put(rPdata);
    } else if (writer->getType() == hertWrite) {
        //todo
    } else if (writer->getType() == bankCardWrite ) {
        char *rPdata = buildData(msgType, dest, vect.size());
        syncQueue->Put(rPdata);
    }else if(writer->getType() == drugBox){
        ALOGE("drugBox ======11111 ");
        char *rPdata = buildData(msgType, dest, vect.size(), const_cast<char *>(parameter), parameterLen);
        ALOGE("drugBox ======222");
        syncQueue->Put(rPdata);
    }
    delete []bufferOrigin;

//    syncQueue->Put(pStr);
}

char *WriteService::buildData(const MsgType &msgType, const string &encodeStr) const {
    char *rPdata = new char[perLen + encodeStr.size()]; //最终构造的数据
    int32_t *plen = (int32_t *) rPdata;
    *plen = encodeStr.size(); //长度
    int32_t *ptype = reinterpret_cast<int32_t *>(rPdata + perLen / 2);
    *ptype = msgType;//保留字段 type
    memcpy(rPdata + perLen, encodeStr.c_str(), encodeStr.size());
    return rPdata;
}

char *WriteService::buildData(const MsgType &msgType, const unsigned char *pData, int len) const {
    char *rPdata = new char[perLen + len]; //最终构造的数据
    int32_t *plen = (int32_t *) rPdata;
    *plen = len; //长度
    int32_t *ptype = reinterpret_cast<int32_t *>(rPdata + perLen / 2);
    *ptype = msgType;//保留字段 type
    memcpy(rPdata + perLen, pData, len);
    return rPdata;
}
char *WriteService::buildData(const MsgType &msgType, const unsigned char *pData, int len,char* otherParmater,int otherParamaterLen) const{

//    int writeSize = perLen+perLen/2 + _localLen+perLen/2;


    int classNmaeLen = sizeof(int32_t);
//    length,type,classNameLen,图片数据，className真实数据
//    int writeSize = perLen+perLen/2 + _localLen+perLen/2;
//    char *rPdata = new char[perLen + classNmaeLen+len+otherParamaterLen]; //最终构造的数据
    char *rPdata = new char[perLen + perLen/2 +len + otherParamaterLen];
    int32_t *plen = (int32_t *) rPdata;
    *plen = len+otherParamaterLen; //长度
    int32_t *ptype = reinterpret_cast<int32_t *>(rPdata + perLen / 2);
    *ptype = msgType;//保留字段 type

    int32_t  *pParamater = reinterpret_cast<int32_t *>(rPdata + perLen);// ptype+ classNmaeLen;
    *pParamater = otherParamaterLen;
    memcpy(rPdata+perLen+classNmaeLen,pData,len);
    memcpy(rPdata+perLen+classNmaeLen+len,otherParmater,otherParamaterLen);
    ALOGE("build Data 图片长度%d 药盒长度 %d  前四个字节长度 %d  总长度 %d 总長度 %d",len,otherParamaterLen,*plen,(perLen + classNmaeLen+len+otherParamaterLen),(perLen + perLen/2 +len + otherParamaterLen));

    return rPdata;
}

void *WriteService::acceptThread(void *pVoid) {
    pthread_detach(pthread_self());

    int 	listenfd;
    int wait_seconds = 5;
    int ret = 0;
    ret = sckServer_init(8001, &listenfd);
    if (ret != 0)
    {
        ALOGE("sckServer_init() err:%d \n", ret);
        return NULL;
    }
    ALOGE("acceptThread run " );
    while(1){
        if (!syncQueue) {
            break;
        }

        int connfd = 0;
//        ALOGE("acceptThread run2222 " );
        ret = sckServer_accept(listenfd, &connfd,  wait_seconds);
        if (ret == Sck_ErrTimeOut)
        {
//            ALOGE("timeout....sckServer_accept\n");
            continue;
        }
        ALOGE("有新的连接 %d ",connfd );
        pthread_t receivepid;
        pthread_create(&receivepid, NULL, receiveThread, reinterpret_cast<void *>(connfd));
    }
    pthread_exit(0);
    return NULL;
}

void * WriteService::sendThread(void *pVoid){
    pthread_detach(pthread_self());
    long connfd = reinterpret_cast<long>(pVoid);
    int ret;
     while (1){
         char *_pstr = NULL;
         if(!detectResultQue){
             break;
         }
         ALOGE("解析队列1111111 ");
         detectResultQue->Take(_pstr);
         ALOGE("解析队列222222");
         if(_pstr){
             int len = strlen(_pstr);
             ret = sckServer_send(connfd, reinterpret_cast<unsigned char *>(_pstr), len+1, 6);
             if (ret != 0)
             {
                 ALOGE("解析队列3333 sckServer_send() ERROR :%d \n", ret);
             }
             ALOGE("解析队列 后，接着发送数据 >> %s  %d  ret =%d",_pstr,len,ret);
             free(_pstr);
             if(ret<0){
                break;
             }
         }else{
             ALOGE("解析队列444 空值");
             break;
         }
     }
    ALOGE("sendThread ============= closed");
    close(connfd);
    pthread_exit(0);
    return NULL;
}


void * WriteService::receiveThread(void *pVoid){
    pthread_detach(pthread_self());
//    int * pconnfd = static_cast<int *>(pVoid);
    long receTimeSecond;
    jumpTimeCheck(&receTimeSecond);
    JNIEnv *env = NULL;
    jint result = -1;
    if ((result = jvm->GetEnv((void **) &env, JNI_VERSION_1_4)) != JNI_OK) {
        ALOGI("get env failed %d ", result);
    }

    jvm->AttachCurrentThread(&env, NULL);

    long connfd = reinterpret_cast<long>(pVoid);

    pthread_t sendtid;
    pthread_create(&sendtid, NULL, sendThread, reinterpret_cast<void *>(connfd));

    ALOGE("receiveThread connfd = %d ",connfd);
    int ret = 0;
    int wait_seconds = 5;
    //接收到了之後
    while(1){
        unsigned char 	recvbuf[1024] ={0};
        int 	recvbuflen = 1024;
        ALOGE("接收到数据 ");

        ret = sckServer_rev(connfd,recvbuf,&recvbuflen,5);
        ALOGE("接收到数据  ");
        if(ret ==Sck_ErrTimeOut){
            ALOGE("func sckServer_rev error %d ",ret);
//           todo 心
            long currTime=0;
            jumpTimeCheck(&currTime);
            if(currTime-receTimeSecond>=60){
                ALOGE("接收到数据 心跳检测 时间判断，如果1min 内 ，没有任何响应，认为客户端关闭了 ");
                break;
            }else{
                continue;
            }


        }else if(ret == Sck_ErrPeerClosed){
            ALOGE("func sckServer_rev error %d ",ret);
            break;
        }
        ALOGE("接收到数据>> %s  %d ",recvbuf,recvbuflen);
        //openDetect
        char *_pstr = NULL;
        if(!detectResultQue){
            break;
        }

        if(strcmp(reinterpret_cast<const char *>(recvbuf), "ping")==0){
            ALOGE("接收到数据 接受到心跳包");
            ret = sckServer_send(connfd, reinterpret_cast<unsigned char *>(recvbuf), recvbuflen, 6);
            if (ret != 0)
            {
                ALOGE("接收到数据 返回心跳包 sckServer_send() ERROR :%d \n", ret);
            }
            ALOGE("接受到数据，返回心跳包>> %s  %d  ret =%d",recvbuf,recvbuflen,ret);
        }else{
            ALOGE("接收到数据 调用java 函数 开启调用线程 %s ",recvbuf);
            jstring  drugNmae = env->NewStringUTF(reinterpret_cast<const char *>(recvbuf));
            env->CallVoidMethod(obj,openDetect,drugNmae);
        }
        jumpTimeCheck(&receTimeSecond);

    }

    if(detectResultQue){
        detectResultQue->Put(0);
    }
    ALOGE("receiveThread ============= closed");
    close(connfd);
    jvm->DetachCurrentThread();
    pthread_exit(0);
    return NULL;
}

void WriteService::jumpTimeCheck(long * jumpTimeCheck) {
    timespec time;
    clock_gettime(CLOCK_REALTIME, &time);
    *jumpTimeCheck = time.tv_sec;
}


void timer(int sig) {
    if (SIGALRM == sig) {
//        printf("timer\n");
        ALOGI("timer ");
        timespec time;
        clock_gettime(CLOCK_REALTIME, &time);
        long time_sec = time.tv_sec;
        if (time_sec - WriteService::timeSecond > HERT_JUMP) {
            //距离上一次发送成功时间大于30s
            //发送心跳包。并重新计时

            char temp[1] = {'\0'};
            string str(temp);
            if (WriteService::iNstance != nullptr) {
                ALOGI("发送心跳包 %d ,%d", hertWrite, sizeof(temp));
                WriteService::iNstance->writeString(str, hertWrite);
            }

//            WriteService::writeString(temp,sizeof(temp),hertWrite);
        }
        alarm(HERT_JUMP);       //重新继续定时1s
    }
    return;
}

void WriteService::hertJumpCheck() {
    signal(SIGALRM, timer); //注册安装信号
    alarm(HERT_JUMP);       //触发定时器
}


int WriteService::readString(string &str) {
    return 0;
}


WriteService::WriteService() {
    iNstance = this;
    syncQueue = new SyncQueue<char *>(100);
    detectResultQue = new SyncQueue<char *>(1);
}

void WriteService::connectSocket() {
    create_thread();
    hertJumpCheck();
}

WriteService::~WriteService() {

    iNstance = nullptr;
    std::unique_lock<std::mutex> lock(mMutex);
    alarm(0);
    delete syncQueue;
    syncQueue = NULL;
    delete detectResultQue;
    detectResultQue = NULL;
    WriteFactory::destroy();
}

void WriteService::parseReceiveData(char *dest, JNIEnv *env) {
    int *plen = reinterpret_cast<int *>(dest);
    int len = *plen;
    int realLen = ntohl(len);

    int *pType = reinterpret_cast<int *>(dest + perLen / 2);

    int type = *pType;
    int realType = ntohl(type);

    char *buf = new char[realLen + 1];
    memset(buf, 0, realLen + 1);
    memcpy(buf, (dest + perLen), realLen);
    ALOGI("parseReceiveData %d  , %d  , %s", realLen, realType ,buf);
    jstring verifyJson = env->NewStringUTF(buf); //识别结果json 串
    env->CallVoidMethod(obj, parseMethod, verifyJson, realType);
    if(detectResultQue->PutResult(buf)==-1){
        //对列 没有加进去
        delete[] buf;
    }
//    delete[] buf; 这里不释放

//    len = ntohl(len);
//    type = ntohl(type);
}

void *WriteService::readConsume(void *pVoid) {
    ALOGI("读线程%d  , %d ", gettid(), pVoid);
    JNIEnv *env = NULL;
    jint result = -1;
    if ((result = jvm->GetEnv((void **) &env, JNI_VERSION_1_4)) != JNI_OK) {
        ALOGI("get env failed %d ", result);
    }

    jvm->AttachCurrentThread(&env, NULL);
    while (1) {

//        string str;
        ALOGI("读线程 while %d,%d ", result, perLen / 2);
        if (!syncQueue) {
            break;
        }
        unsigned int len = 0;
        int ret = readFromServer(&len, perLen / 2);//读取包头 4个字节
        ALOGI("读线程 长度 读到了 %d , %d", ret, len);
        if (ret == -1) {
            ALOGI("读线程 读取包头长度 ERROR");
            sleep(10);
            continue;
        } else if (ret < perLen / 2) {
            ALOGI("读线程 读取包头长度 client close ");
            sleep(10);
            continue;
        }
        unsigned int type = 0;

        ret = readFromServer(&type, perLen / 2);
        ALOGI("读线程 类型 读到了 %d , %d", ret, type);
        if (ret == -1) {
            ALOGI("读线程 读取包头类型 ERROR");
            sleep(10);
            continue;
        } else if (ret < perLen / 2) {
            ALOGI("读线程 读取包头类型 client close ");
            sleep(10);
            continue;
        }
        len = ntohl(len);
        type = ntohl(type);
        char *buf = new char[len + 1];
        memset(buf, 0, len + 1);
        ret = readFromServer(buf, len);
        if (ret == -1) {
            ALOGI("读线程 读取实际数据 ERROR");
            delete[] buf;
            sleep(10);
            continue;
        } else if (ret < len) {
            ALOGI("读线程 读取实际数据 client close ");
            delete[] buf;
            sleep(10);
            continue;
        }
        ALOGI("读线程 读取的长度为 %d ,类型为 %d ret = %d ,数据为 %s ", len, type, ret, buf);
        jstring verifyJson = env->NewStringUTF(buf); //识别结果json 串
//        env->CallVoidMethod(obj, callbackId, verifyJson, VERIFY_RESULT);
        env->DeleteLocalRef(verifyJson);
        delete[] buf;

    }

    ALOGI("读线程2222");
    jvm->DetachCurrentThread();

//    sockfd = connectServer();
    return NULL;

}

void *WriteService::writeConsume(void *pVoid) {
    pthread_detach(pthread_self());
    ALOGI("写线程%d ", gettid());
    JNIEnv *env = NULL;
    jint result = -1;
    if ((result = jvm->GetEnv((void **) &env, JNI_VERSION_1_4)) != JNI_OK) {
        ALOGI("get env failed %d ", result);
    }

    jvm->AttachCurrentThread(&env, NULL);
    while (1) {
        ALOGI("写线程 从队列中取数据 ");
        char *_pstr = NULL;
        if (!syncQueue) {
            break;
        }
        syncQueue->Take(_pstr);
        if (_pstr) {

            int *_pdatalen = reinterpret_cast<int *>(_pstr);
            int _localLen = *_pdatalen;
//            ALOGI("写数据成功长度111>>dataLen %d type %c, %c %c %d,",_localLen,_pstr[_localLen+12],_pstr[_localLen+13]);
            int netlen = htonl(_localLen);
            *_pdatalen = netlen;
            //类型
            int32_t *_ptype = reinterpret_cast<int32_t *>(_pstr + perLen / 2);
            int _local_msgtype = *_ptype;//当前消息类型 本地字节序
            *_ptype = htonl(_local_msgtype);
            if(_local_msgtype==MsgType::drugBox){
                int32_t  * pClassName = reinterpret_cast<int32_t *>(_pstr + perLen);
                int _local_ClassNameLen  = *pClassName;
                *pClassName = htonl(_local_ClassNameLen);
                ALOGE("drugBox333 %d %d",_local_ClassNameLen,_localLen);
            }

            jbyte *by = (jbyte *) _pstr;
            int writeSize = perLen+perLen/2 + _localLen;
            jbyteArray jarray = env->NewByteArray(writeSize);
            env->SetByteArrayRegion(jarray, 0, writeSize, by);
            env->CallVoidMethod(obj, callbackId, jarray); //写数据
//            env->ReleaseIntArrayElements(jarray,)
            env->DeleteLocalRef(jarray);

            setCurrentime();
            free(_pstr);
            _pstr = nullptr;
        }
    }

    jvm->DetachCurrentThread();
    ALOGE("writeConsume closed =========");
    pthread_exit(0);//pthread_exit时自动会被释放
    return NULL;
}

void WriteService::create_thread() {
    pthread_t writepid;
    pthread_t readpid;
    ALOGI("pthread_create %d ", jvm);
    signal(SIGPIPE, SIG_IGN);
//    pthread_create(&readpid, NULL, readConsume, NULL);

    pthread_create(&writepid, NULL, writeConsume, NULL);

    pthread_t accetpid;
    pthread_create(&accetpid, NULL, acceptThread, NULL);

}





