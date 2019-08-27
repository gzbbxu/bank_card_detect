//
// Created by yuanshuai on 2019/4/2.
//

#ifndef FACERECOGNITION_WRITEIMAGE_H
#define FACERECOGNITION_WRITEIMAGE_H

#include "IWrite.h"
#include "WriteService.h"

class WriteImage : public  IWrite {
public:
    WriteImage(service::MsgType msgType) {
        this->msgType = msgType;
    }
    virtual std::string encode(const unsigned char *Data, int DataByte);

    virtual std::string decode(const char *Data, int DataByte, int &OutByte);

    virtual   service::MsgType getType();

protected:


    /*  std::string encode(const unsigned char *Data, int DataByte);

      std::string decode(const char *Data, int DataByte, int &OutByte);*/
};


#endif //FACERECOGNITION_WRITEIMAGE_H
