//
// Created by zkk on 2019/4/2.
//
#ifndef WRITESERVICE_FACTORY_H
#define WRITESERVICE_FACTORY_H

#include <map>
#include "IWrite.h"
#include "WriteService.h"
#include "WriteImage.h"
#include "WriteHertJump.h"
#include "WriteYuv.h"

class WriteFactory {
public:
    static std::map<service::MsgType, IWrite *> mMap;

    static IWrite *getWrite(service::MsgType msgType) {
        IWrite *iWrite = NULL;
        if (mMap.find(msgType) != mMap.end()) {
            iWrite = mMap[msgType];
        } else {
            if (msgType == service::MsgType::imgWrite) {
                iWrite = new WriteImage(msgType);
            } else if (msgType == service::MsgType::hertWrite) {
                iWrite = new WriteHertJump(msgType);
            }else if(msgType == service::MsgType::bankCardWrite || msgType == service::MsgType::drugBox){
                iWrite = new WriteYuv(msgType);
            }
            mMap.insert(std::make_pair(msgType, iWrite));
        }
        return iWrite;
    }

    static void destroy() {
        std::map<service::MsgType, IWrite *>::iterator it;
        for (it = mMap.begin(); it != mMap.end();) {
            delete it->second;
            mMap.erase(it++);
        }
    }

private:

};

std::map<service::MsgType, IWrite *> WriteFactory::mMap = std::map<service::MsgType, IWrite *>();

#endif // WRITESERVICE_FACTORY_H