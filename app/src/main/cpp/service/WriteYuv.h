//
// Created by yuanshuai on 2019/4/22.
//

#ifndef BANKCARDDETECT_WRITEYUV_H
#define BANKCARDDETECT_WRITEYUV_H

#include "IWrite.h"

class WriteYuv : public IWrite {
public:
    WriteYuv(service::MsgType msgType) {
        this->msgType = msgType;
    }

    virtual std::string encode(const unsigned char *Data, int DataByte);

    virtual std::string decode(const char *Data, int DataByte, int &OutByte);

    virtual service::MsgType getType();

protected:
};


#endif //BANKCARDDETECT_WRITEYUV_H
