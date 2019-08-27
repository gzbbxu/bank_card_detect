//
// Created by yuanshuai on 2019/4/2.
//

#ifndef FACERECOGNITION_IWRITE_H
#define FACERECOGNITION_IWRITE_H

#include "string"
#include "WriteService.h"


class IWrite {
protected:
      service::MsgType msgType;
public:

    virtual std::string encode(const unsigned char *Data, int DataByte)=0;

    virtual std::string decode(const char *Data, int DataByte, int &OutByte) = 0;

    virtual   service::MsgType getType() = 0;
};

#endif //FACERECOGNITION_IWRITE_H