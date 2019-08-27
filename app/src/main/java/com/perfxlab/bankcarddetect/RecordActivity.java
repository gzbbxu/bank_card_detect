package com.perfxlab.bankcarddetect;

import android.graphics.Bitmap;
import android.graphics.ImageFormat;
import android.graphics.SurfaceTexture;
import android.hardware.Camera;
import android.support.v7.app.AppCompatActivity;
import android.os.Bundle;
import android.text.TextUtils;
import android.util.DisplayMetrics;
import android.util.Log;
import android.view.TextureView;
import android.view.View;
import android.widget.Button;
import android.widget.EditText;
import android.widget.FrameLayout;
import android.widget.ImageView;
import android.widget.TextView;

import com.perfxlab.bankcarddetect.Bean.DetectInfo;
import com.perfxlab.bankcarddetect.util.BitmapUtils;
import com.perfxlab.bankcarddetect.util.CameraUtil;
import com.perfxlab.bankcarddetect.util.TimeArray;
import com.perfxlab.bankcarddetect.util.ToastUtils;
import com.perfxlab.bankcarddetect.widget.IdenSuccsView;

import org.opencv.core.CvType;
import org.opencv.core.Mat;
import org.opencv.core.MatOfByte;
import org.opencv.core.Size;
import org.opencv.imgcodecs.Imgcodecs;
import org.opencv.imgproc.Imgproc;

import java.io.File;
import java.io.IOException;
import java.util.List;
import java.util.concurrent.ExecutorService;
import java.util.concurrent.Executors;

import us.pinguo.svideo.bean.VideoInfo;
import us.pinguo.svideo.interfaces.ICameraProxyForRecord;
import us.pinguo.svideo.interfaces.OnRecordListener;
import us.pinguo.svideo.interfaces.PreviewDataCallback;
import us.pinguo.svideo.interfaces.PreviewSurfaceListener;
import us.pinguo.svideo.interfaces.SurfaceCreatedCallback;
import us.pinguo.svideo.recorder.SAbsVideoRecorder;
import us.pinguo.svideo.recorder.SMediaCodecRecorder;

import static com.perfxlab.bankcarddetect.SocketManager.DRUG_BOX_DETECT_RESULT;
import static com.perfxlab.bankcarddetect.SocketManager.RESULT_CODE_JPG_DATA;


public class RecordActivity extends AppCompatActivity implements View.OnClickListener, TextureView.SurfaceTextureListener, OnRecordListener, Camera.PreviewCallback, SocketManager.MsgCallBack {
    private Camera mCamera;

    private SAbsVideoRecorder mRecorder;
    private PreviewDataCallback mCallback;
    private Camera.Size mPreviewSize;
    private ImageView mSwitchImg;
    private TextureView mTextureView;
    private TextView tv_detect_result;
    public int mCameraFacing = Camera.CameraInfo.CAMERA_FACING_BACK;
    Button btn_start_detect, btn_end_detect;
    private static String TAG = "SocketManager";
    private volatile boolean isStartDetect = false;
    private IdenSuccsView idenSuccsView;
    private String gDrugType = "default";
    private Button btn_ok;
    private volatile boolean isDeteSuccess = false;
    public static final int BANK_CARD_DETECT = 1000;
    public static final int DRUG_BOX_DETECT = 2000;

    private int currentType = DRUG_BOX_DETECT_RESULT;
    ExecutorService cachedThreadPool = Executors.newFixedThreadPool(2);
    EditText et_input;

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_record);
        mSwitchImg = findViewById(R.id.switch_camera);
        mSwitchImg.setOnClickListener(this);
        mTextureView = findViewById(R.id.textureview);
        btn_start_detect = findViewById(R.id.btn_start_detect);
        btn_end_detect = findViewById(R.id.btn_end_detect);
        idenSuccsView = findViewById(R.id.idenSuccView);
        tv_detect_result = findViewById(R.id.tv_detect_result);
        btn_start_detect.setOnClickListener(this);
        btn_end_detect.setOnClickListener(this);
        mTextureView.setSurfaceTextureListener(this);
        et_input = findViewById(R.id.et_input);
        btn_ok = findViewById(R.id.btn_ok);
        btn_ok.setOnClickListener(this);
        init();
    }


    private void init() {
        SocketManager.getInstance().addListener(this);
        ICameraProxyForRecord cameraProxyForRecord = new ICameraProxyForRecord() {

            @Override
            public void addSurfaceDataListener(PreviewSurfaceListener listener, SurfaceCreatedCallback callback) {

            }

            @Override
            public void removeSurfaceDataListener(PreviewSurfaceListener listener) {

            }

            @Override
            public void addPreviewDataCallback(PreviewDataCallback callback) {
                mCallback = callback;
            }

            @Override
            public void removePreviewDataCallback(PreviewDataCallback callback) {
                mCallback = null;
            }

            @Override
            public int getPreviewWidth() {
                return mPreviewSize.width;
            }

            @Override
            public int getPreviewHeight() {
                return mPreviewSize.height;
            }

            @Override
            public int getVideoRotation() {
                return mCameraFacing == Camera.CameraInfo.CAMERA_FACING_BACK ? 90 : 270;
            }
        };
        mRecorder = new SMediaCodecRecorder(this, cameraProxyForRecord);
        mRecorder.addRecordListener(this);
    }

    private Camera.Size getPreviewSize() {
        List<Camera.Size> sizeList = mCamera.getParameters().getSupportedPreviewSizes();
        for (int i = 0; i < sizeList.size(); i++) {
            Camera.Size size = sizeList.get(i);
//            Log.i("zhouke", "getPreviewSize >> size >> " + size.height + " >>> " + size.width);
            /*if (size.width == 640 || size.width == 960 || size.width == 1280) {
                return size;
            }*/
        }
        return sizeList.get(0);
    }

    private void openCamera() {
        if (mCamera != null) {
            return;
        }
        Camera.CameraInfo info = new Camera.CameraInfo();
        for (int k = 0; k < Camera.getNumberOfCameras(); k++) {
            Camera.getCameraInfo(k, info);
            if (info.facing == mCameraFacing) {
                mCamera = Camera.open(k);
                break;
            }
        }
        if (mCamera == null) {
            throw new RuntimeException("Can't open frontal camera");
        }
    }

    private void adjustPreviewSize() {
        DisplayMetrics displayMetrics = getResources().getDisplayMetrics();
        FrameLayout.LayoutParams params = (FrameLayout.LayoutParams) mTextureView.getLayoutParams();
        params.width = displayMetrics.widthPixels;
        params.height = (int) (mPreviewSize.width / (float) mPreviewSize.height * params.width);
        mTextureView.setLayoutParams(params);
        if (mCameraFacing == Camera.CameraInfo.CAMERA_FACING_FRONT) {
            mTextureView.setScaleX(-1);
        } else {
            mTextureView.setScaleX(1);
        }
    }

    private void startPreview(SurfaceTexture surfaceTexture) {
        openCamera();
        mCamera.setDisplayOrientation(90);
        Camera.Parameters parameters = mCamera.getParameters();
        mPreviewSize = getPreviewSize(); // 1280 720
   /*     mPreviewSize.width = 1280;
        mPreviewSize.height = 720;*/
        Log.i(TAG, "startPreview>>mPreviewSize  width : " + mPreviewSize.width + ">>> height : " + mPreviewSize.height);
        parameters.setPreviewSize(mPreviewSize.width, mPreviewSize.height);
        parameters.setPreviewFormat(ImageFormat.NV21);
        List<String> focusModes = parameters.getSupportedFocusModes();
        for (String s : focusModes) {
            if (Camera.Parameters.FOCUS_MODE_CONTINUOUS_PICTURE.equals(s)) {
                parameters.setFocusMode(Camera.Parameters.FOCUS_MODE_CONTINUOUS_PICTURE);
                break;
            }
        }
        mCamera.addCallbackBuffer(new byte[(int) (mPreviewSize.width * mPreviewSize.height * 1.5f)]);
        mCamera.setPreviewCallbackWithBuffer(this);
        mCamera.setParameters(parameters);
        adjustPreviewSize();
//        mCamera.setPreviewCallback(this);
        try {
            mCamera.setPreviewTexture(surfaceTexture);
        } catch (IOException e) {
            e.printStackTrace();
        }
        mCamera.startPreview();
    }


    @Override
    public void onClick(View v) {
        if (v == mSwitchImg) {
            switchCamera();
        } else if (v.getId() == btn_start_detect.getId()) {
            if (mRecorder != null) {
                isStartDetect = true;
                isDeteSuccess = false;
//                mRecorder.startRecord();
                idenSuccsView.hideIdenSucc();
                idenSuccsView.showHideIdenFailure(false, "");
                disEnableView(btn_start_detect);
            }
        } else if (v.getId() == btn_end_detect.getId()) {
            if (mRecorder != null) {
//                mRecorder.stopRecord();
                synchronized (RecordActivity.class) {
                    timeArray.rest();
                    frameIndex = 0;
                }

                isStartDetect = false;
                enableView(btn_start_detect);
                if (!isDeteSuccess) {
                    DetectInfo bankInfo = new DetectInfo();
                    bankInfo.type = currentType;
                    idenSuccsView.addQueue(bankInfo);
                }
            }
        } else if (v.getId() == btn_ok.getId()) {
            String drugsType = et_input.getText().toString();
            if (TextUtils.isEmpty(drugsType)) {
                gDrugType = "default";
            } else {
                gDrugType = drugsType;
            }
        }
    }

    public void enableView(View... views) {
        for (View view : views) {
            view.setEnabled(true);
        }
    }

    public void disEnableView(View... views) {
        for (View view : views) {
            view.setEnabled(false);
        }
    }

    private void switchCamera() {
        mCameraFacing = mCameraFacing == Camera.CameraInfo.CAMERA_FACING_BACK ?
                Camera.CameraInfo.CAMERA_FACING_FRONT : Camera.CameraInfo.CAMERA_FACING_BACK;
        if (mCamera != null) {
            mCamera.stopPreview();
            mCamera.release();
            mCamera = null;
        }
        openCamera();
        startPreview(mTextureView.getSurfaceTexture());
    }


    @Override
    public void onSurfaceTextureAvailable(SurfaceTexture surface, int width, int height) {
        startPreview(surface);
    }

    @Override
    public void onSurfaceTextureSizeChanged(SurfaceTexture surface, int width, int height) {

    }

    @Override
    public boolean onSurfaceTextureDestroyed(SurfaceTexture surface) {
        mCamera.stopPreview();
        mCamera.release();
        mCamera = null;
        return false;
    }

    @Override
    public void onSurfaceTextureUpdated(SurfaceTexture surface) {

    }

    @Override
    public void onRecordSuccess(VideoInfo videoInfo) {
        Log.i(TAG, "onRecordSuccess  >> " + videoInfo.getVideoPath());
    }

    @Override
    public void onRecordStart() {
        Log.i(TAG, "onRecordStart");
    }

    @Override
    public void onRecordFail(Throwable throwable) {
        Log.i(TAG, "onRecordFail " + Log.getStackTraceString(throwable));
    }

    @Override
    public void onRecordStop() {

    }

    @Override
    public void onRecordPause() {

    }

    @Override
    public void onRecordResume() {

    }

    private TimeArray timeArray = new TimeArray();
    private int frameIndex = 0;


    @Override
    public void openDetect(String drugName) {
        Log.i("接收到数据", "openDetect openDetect");
        isStartDetect =true;
        isDeteSuccess =false;
        gDrugType = drugName;

    }

    @Override
    public void onPreviewFrame(byte[] data, Camera camera) {

        Log.i("zhouke","===============================================");

        if (isStartDetect) {
            if (isDeteSuccess) {
                return;
            }
            if(currentType == DRUG_BOX_DETECT_RESULT){
                isStartDetect = false;
                isDeteSuccess = true;
            }
            cachedThreadPool.execute(new Runnable() {
                @Override
                public void run() {
                    Log.i("zhouke", "onPreviewFrame>> " + data.length + ">> isStartDetect " +isStartDetect +">> "+isDeteSuccess);
                    //计算和显示帧率
                   /* synchronized (RecordActivity.class) {
                        frameIndex++;
                        timeArray.newTime();
                        if (timeArray.count() % 10 == 0) {
                            String frame = String.format("[%d] %02.1f帧/秒", timeArray.count(), timeArray.rate());
                        } else if (timeArray.count() == 1) {
                        } else {
                            return;
                        }
                    }*/
                    byte[] det = new byte[data.length];
                    System.arraycopy(data, 0, det, 0, det.length);
                    Bitmap bitmap = CameraUtil.getPreviewBitmap(det, camera.getParameters().getPreviewFormat(), camera.getParameters().getPreviewSize());
                    int height = camera.getParameters().getPreviewSize().height;
                    int width = camera.getParameters().getPreviewSize().width;
                    Mat matSrc = new Mat();
                    org.opencv.android.Utils.bitmapToMat(bitmap, matSrc);
//            Mat matSrc =  Imgcodecs.imread("/sdcard/test/1.jpg");
//                    Log.i("zhouke", "bit map bitmapToMat " + matSrc.height() + " >> " + matSrc.width()); //1080 >> 1920
                    Size size = new Size();
                    size.height = 501;

                    float radio = (float) matSrc.height() / (float) size.height;


                    size.width = matSrc.width() / radio;
                    if (size.width % 2 != 0) {
                        size.width++;
                    }

//                    sz.width % 2 == 0 && sz.height % 3 == 0 && depth == 0
                    Log.i("zhouke", "mat size >> " + size.height + " >> " + size.width + ">> " + matSrc.depth());

                    Mat mat3 = new Mat(size, CvType.CV_8UC3);  //CV_8UC4


                    Imgproc.resize(matSrc, mat3, size);

                    Mat mat4 = new Mat(size, CvType.CV_8UC3);
                    Imgproc.cvtColor(mat3, mat4, Imgproc.COLOR_BGR2RGB);//116

                    MatOfByte matOfByte = new MatOfByte();
                    Imgcodecs.imencode(".jpg", mat4, matOfByte);
                    byte[] resultJpgArray = matOfByte.toArray();
                    Log.i("zhouke", "resultJpgArray: " + resultJpgArray.length + ">> ");
                    SocketManager.getInstance().writeToQueue(resultJpgArray, gDrugType);

                }
            });


        }


        if (mCallback != null) {
            long timeUs = System.nanoTime() / 1000;
            mCallback.onPreviewData(data, timeUs);
        }
        camera.addCallbackBuffer(data);
    }

    @Override
    protected void onPause() {
        super.onPause();
//        mRecorder.stopRecord();
    }

    @Override
    protected void onResume() {
        super.onResume();
//        mRecorder.resumeRecord();
    }

    @Override
    protected void onDestroy() {
        super.onDestroy();
        SocketManager.getInstance().removeListner();
        //可以根据合适的场景去调用free 。
//        SocketManager.getInstance().free();
    }

    @Override
    public void detectSuccess(String str, int msgType) {
        if (msgType == RESULT_CODE_JPG_DATA) {
            currentType = msgType;
            Bitmap bmp = BitmapUtils.base64ToBitmap(str);
            if (bmp != null && !isDeteSuccess) {
                isDeteSuccess = true;
                btn_end_detect.performClick();
                Log.i("zhouke", "检测成功 解析图片成功");
                File image = getExternalFilesDir("image");
                long time = System.currentTimeMillis();
                File fileResult = BitmapUtils.savePhotoToSDCard(bmp, image.getAbsolutePath(), time + ".jpg");
                ToastUtils.getInstanc(this).showToast("检测到结果:保留在 " + fileResult.getAbsolutePath());
                DetectInfo bankInfo = new DetectInfo();
                bankInfo.headPath = fileResult.getAbsolutePath();
                bankInfo.type = BANK_CARD_DETECT;//銀行卡
                idenSuccsView.addQueue(bankInfo);
            } else {
                Log.i("zhouke", "检测成功 解析图片失败");
//            ToastUtils.getInstanc(RecordActivity.this).showToast("bm == null");
            }
        } else if (msgType == DRUG_BOX_DETECT_RESULT) {
          /*  isDeteSuccess = true;
            currentType = msgType;*/
//            btn_end_detect.performClick();
            Log.i("接收到数据", "detectSuccess >> " + str);
            tv_detect_result.setText("detectSuccess >> " +System.currentTimeMillis() + ">> " +str);
//            ToastUtils.getInstanc(this).showToast(System.currentTimeMillis() + " > " + str);
        }

//        getExternalCacheDir();


    }

    @Override
    public void detectFailure(String failStr) {
        Log.i("zhouke", "detectFailure 没有结果");
      /*  DetectInfo bankInfo = new DetectInfo();
        idenSuccsView.addQueue(bankInfo);*/
    }


}
