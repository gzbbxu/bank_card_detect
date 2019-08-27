package com.perfxlab.bankcarddetect;

import android.util.Log;

import com.perfxlab.bankcarddetect.util.ToastUtils;
import com.perfxlab.bankcarddetect.wsmanager.WsManager;
import com.perfxlab.bankcarddetect.wsmanager.WsStatusListener;

import java.io.UnsupportedEncodingException;
import java.util.ArrayList;
import java.util.List;
import java.util.concurrent.ExecutorService;
import java.util.concurrent.Executors;
import java.util.concurrent.ThreadPoolExecutor;
import java.util.concurrent.TimeUnit;

import okhttp3.OkHttpClient;
import okhttp3.Response;
import okio.ByteString;

public class SocketManager {
    String TAG = "SocketManager";
    public static final int REQUEST_CODE = 1000;
    public static final int RESULT_CODE_NO_RESULT = REQUEST_CODE + 1;
    public static final int RESULT_CODE_JPG_DATA = REQUEST_CODE + 2;

    public static final int DRUG_BOX_REQUESTCODE = 2000;
    public static final int DRUG_BOX_NO_RESULT = DRUG_BOX_REQUESTCODE + 1;
    public static final int DRUG_BOX_DETECT_RESULT = DRUG_BOX_REQUESTCODE + 2;


    private static SocketManager socketManager;
    private WsManager wsManager;
    //    private String url = "ws://123.206.96.155:7651/card/";
//    private String url = "ws://123.206.96.155:7667/drug";
    private String url = "ws://123.206.96.155:7651/card_test";
    long t1 = 0;


    public List<MsgCallBack> listeners = new ArrayList<>();

    static {
        System.loadLibrary("native-lib");
    }

    public static SocketManager getInstance() {
        if (socketManager == null) {
            socketManager = new SocketManager();

        }
        return socketManager;
    }

    public void addListener(MsgCallBack msgCallBack) {
        listeners.add(msgCallBack);
    }

    public void removeListner() {
        listeners.clear();
    }

    public SocketManager() {
        connectWebSocket();

    }


    private WsStatusListener wsStatusListener = new WsStatusListener() {
        @Override
        public void onOpen(Response response) {
            Log.i(TAG, "WsManager-----服务器连接成功");
            ToastUtils.getInstanc(App.getAppInstance()).showToast("服务器连接成功");
        }

        @Override
        public void onMessage(String text) {
            Log.i(TAG, "WsManager-----onMessage:" + text);
        }

        @Override
        public void onMessage(ByteString bytes) {
            Log.i(TAG, "统计网络耗时:" + (System.currentTimeMillis() - t1));
            byte[] byteArray = bytes.toByteArray();
            parseReceive(byteArray);

        }

        @Override
        public void onReconnect() {
            Log.i(TAG, "WsManager-----onReconnect 服务器重连接中");
            ToastUtils.getInstanc(App.getAppInstance()).showToast("服务器正在连接中");
           /* tv_content.append(Spanny.spanText("服务器重连接中...\n", new ForegroundColorSpan(
                    ContextCompat.getColor(getBaseContext(), android.R.color.holo_red_light))));*/
        }

        @Override
        public void onClosing(int code, String reason) {
            Log.i(TAG, "WsManager-----onClosing 服务器连接关闭中");
            /*tv_content.append(Spanny.spanText("服务器连接关闭中...\n", new ForegroundColorSpan(
                    ContextCompat.getColor(getBaseContext(), android.R.color.holo_red_light))));*/
        }

        @Override
        public void onClosed(int code, String reason) {
            Log.i(TAG, "WsManager-----onClosed");
          /*  tv_content.append(Spanny.spanText("服务器连接已关闭\n", new ForegroundColorSpan(
                    ContextCompat.getColor(getBaseContext(), android.R.color.holo_red_light))));*/
        }

        @Override
        public void onFailure(Throwable t, Response response) {
            Log.i(TAG, "WsManager-----onFailure");
         /*   tv_content.append(Spanny.spanText("服务器连接失败\n", new ForegroundColorSpan(
                    ContextCompat.getColor(getBaseContext(), android.R.color.holo_red_light))));*/
        }
    };

    /**
     * A native method that is implemented by the 'native-lib' native library,
     * which is packaged with this application.
     */
    public native void startDetect();

    public native void writeToQueue(byte[] array, String parameter);


    public native void resize(byte[] bytes);

    public native void release();


    /**
     * 解析服务器返回的数据
     *
     * @param data
     */
    private native void parseReceive(byte[] data);

    void callBackParse(String str, int msgType) {
        if (msgType == RESULT_CODE_NO_RESULT) {
            //返回 没有结果的json 串
            failure(str);
        } else if (msgType == RESULT_CODE_JPG_DATA) {
            //返回图片 的base64
            success(str, msgType);
        } else if (msgType == DRUG_BOX_NO_RESULT) {
            failure(str);
        } else if (msgType == DRUG_BOX_DETECT_RESULT) {
            //藥盒檢測
            success(str, msgType);
        }
    }

    /**
     * 开启检测线程
     */
    void callBackOpenDetect(String drugName){
        for (MsgCallBack msgCallBack : listeners) {
            msgCallBack.openDetect(drugName);
        }
    }

    void success(String stringBase64, int type) {
        for (MsgCallBack msgCallBack : listeners) {
            msgCallBack.detectSuccess(stringBase64, type);
        }
    }


    void failure(String failureStr) {
        for (MsgCallBack msgCallBack : listeners) {
            msgCallBack.detectFailure(failureStr);
        }
    }

    void callBackSend(byte[] bytes) {
        Log.i(TAG, "callBack>>" + bytes.length);
//        if (wsManager.isWsConnected()) {
        t1 = System.currentTimeMillis();
        ByteString byteString = ByteString.of(bytes);
        wsManager.sendMessage(byteString);

    }

    private void connectWebSocket() {

        wsManager = new WsManager.Builder(App.getAppInstance())
                .client(
                        new OkHttpClient().newBuilder()
                                .pingInterval(15, TimeUnit.SECONDS)
                                .retryOnConnectionFailure(true)
                                .build())
                .needReconnect(true)
                .wsUrl(url)
                .build();
        wsManager.setWsStatusListener(wsStatusListener);
        wsManager.startConnect();
        startDetect();
    }

    public void free() {
        release();
        socketManager = null;
    }

    interface MsgCallBack {

        void detectSuccess(String base64Bitmap, int msgType);

        void detectFailure(String failStr);

        void openDetect(String drugName);
    }
}
