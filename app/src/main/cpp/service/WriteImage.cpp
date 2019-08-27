//
// Created by yuanshuai on 2019/4/2.
//

#include "WriteImage.h"
#include "Base64Util.hpp"


std::string WriteImage::encode(const unsigned char *Data, int DataByte) {
    return Base64Util::encode(Data, DataByte);
}

std::string WriteImage::decode(const char *Data, int DataByte, int &OutByte) {
    return Base64Util::decode(Data, DataByte, OutByte);
}

service::MsgType WriteImage::getType() {
    return this->msgType;
}


