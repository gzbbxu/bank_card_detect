//
// Created by yuanshuai on 2019/4/2.
//

#ifndef FACERECOGNITION_WRITEHERTJUMP_H
#define FACERECOGNITION_WRITEHERTJUMP_H

#include "IWrite.h"

class WriteHertJump : public  IWrite {
public:
    WriteHertJump(service::MsgType msgType) {
        this->msgType = msgType;
    }
    virtual std::string encode(const unsigned char *Data, int DataByte);

    virtual std::string decode(const char *Data, int DataByte, int &OutByte);

    virtual   service::MsgType getType();
private:
};


#endif //FACERECOGNITION_WRITEHERTJUMP_H
