package com.perfxlab.bankcarddetect.wsmanager;

import java.io.Serializable;

public class DetectResult implements Serializable {
    private int code;
    private String name;
    private int msgType;
    public boolean isSuccess(){
        return code==0?true:false;
    }
    public int getCode() {
        return code;
    }

    public void setCode(int code) {
        this.code = code;
    }

    public String getName() {
        return name;
    }

    public void setName(String name) {
        this.name = name;
    }

    public int getMsgType() {
        return msgType;
    }

    public void setMsgType(int msgType) {
        this.msgType = msgType;
    }


}
