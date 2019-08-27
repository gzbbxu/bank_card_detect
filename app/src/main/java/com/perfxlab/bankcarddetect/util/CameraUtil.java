package com.perfxlab.bankcarddetect.util;

import android.graphics.Bitmap;
import android.graphics.BitmapFactory;
import android.graphics.Matrix;
import android.graphics.Rect;
import android.graphics.YuvImage;
import android.hardware.Camera;

import java.io.ByteArrayOutputStream;

public class CameraUtil {
    public static  Bitmap getPreviewBitmap(byte[] bytes, int format, Camera.Size size) {
        return getPreviewBitmap(bytes, format, size.width, size.height);
    }

    public static Bitmap getPreviewBitmap(byte[] bytes, int format, int width, int height) {

        YuvImage yuv = new YuvImage(bytes, format, width, height, null);

        ByteArrayOutputStream jpgStream = new ByteArrayOutputStream();
        yuv.compressToJpeg(new Rect(0, 0, width, height), 100, jpgStream);

        byte[] jpgByte = jpgStream.toByteArray();
        Bitmap bitmap =  BitmapFactory.decodeByteArray(jpgByte, 0, jpgByte.length);

        Matrix matrix = new Matrix();
        matrix.preRotate(90);

        bitmap = Bitmap.createBitmap(bitmap ,0,0, bitmap .getWidth(), bitmap
                .getHeight(),matrix,true);
        return bitmap;

    }
}
