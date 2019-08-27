#include <jni.h>
#include <string>
#include "WriteService.h"
#include <omp.h>
/*#include "opencv/cv.h"
#include "opencv2/opencv.hpp"*/

#define LOGE(...)  __android_log_print(ANDROID_LOG_ERROR, "perfxlab_native", __VA_ARGS__);
JavaVM *jvm;
jmethodID parseMethod;
jmethodID callbackId;
jobject obj;
jmethodID openDetect;
service::WriteService *writeService = nullptr;

extern "C" JNIEXPORT void JNICALL
Java_com_perfxlab_bankcarddetect_SocketManager_startDetect(
        JNIEnv *env,
        jobject thiz/* this */) {
    int my_rank = omp_get_thread_num();
    int max_thread = omp_get_max_threads();
    LOGE("--------------------startDetectbbbbbbbaa------------------- >> %d ,%d ",my_rank,max_thread);
    std::string hello = "Hello from C++";
    env->NewStringUTF(hello.c_str());
    jclass clazz = env->GetObjectClass(thiz);
    parseMethod = env->GetMethodID(clazz, "callBackParse", "(Ljava/lang/String;I)V");
    callbackId = env->GetMethodID(clazz, "callBackSend", "([B)V");
    openDetect = env->GetMethodID(clazz,"callBackOpenDetect","(Ljava/lang/String;)V");
    obj = env->NewGlobalRef(thiz);
    writeService = new service::WriteService;
    writeService->connectSocket();
    return;
}

extern "C" JNIEXPORT void JNICALL
Java_com_perfxlab_bankcarddetect_SocketManager_writeToQueue(JNIEnv *env, jobject thiz,
                                                            jbyteArray array,jstring otherParameter) {
    int len = env->GetArrayLength(array);
    LOGE("--------------------writeToQueue------------------- >> ");
    vector<unsigned char> vec(len);
    jbyte *byte = env->GetByteArrayElements(array, NULL);
    memcpy(&vec[0], byte, len);
    const char*  parameter = env->GetStringUTFChars(otherParameter,NULL);
    int parameterLen = strlen(parameter);
    LOGE("--------------------otherParameter------------------- >> %d  %s",parameterLen,parameter);
    writeService->writeString(vec, service::MsgType::drugBox,parameter,parameterLen);
    env->ReleaseStringUTFChars(otherParameter,parameter);
}

extern "C" JNIEXPORT void JNICALL
Java_com_perfxlab_bankcarddetect_SocketManager_parseReceive(JNIEnv *env, jobject thiz,
                                                            jbyteArray receData) {
    LOGE("--------------------parseReceive------------------- >>  %d ",
            env->GetArrayLength(receData));

    jbyte *arrayBody = env->GetByteArrayElements(receData, 0);

    char *dest = (char *) arrayBody;
    writeService->parseReceiveData(dest, env);
    env->ReleaseByteArrayElements(receData, arrayBody, 0);
    LOGE("--------------------parseReceive end------------------- >>");
}

extern "C" JNIEXPORT void JNICALL
Java_com_perfxlab_bankcarddetect_SocketManager_resize(JNIEnv *env, jobject thiz,
                                                      jbyteArray receData) {

}


extern "C" JNIEXPORT void JNICALL
Java_com_perfxlab_bankcarddetect_SocketManager_release(JNIEnv *env, jobject thiz) {


    env->DeleteGlobalRef(obj);
    delete writeService;
    writeService = nullptr;

    /*  LOGE("--------------------yuv2rgb------------------- >>");
  //    cv::imencode(".jpg", mat, buff_jpg, param);
      jbyte *yuvJbyte = env->GetByteArrayElements(yuvData, NULL);
      int size = env->GetArrayLength(yuvData);
  //    cv::imdecode()
      char *data = reinterpret_cast<char *>(yuvJbyte);
      std::vector<char> vec_data(&data[0], &data[0] + size);

      cv::Mat mat2 = cv::imdecode(vec_data, 1);

      mat2.rows;
      mat2.cols;
      LOGE("--------------------yuv2rgb row %d  colos %d------------------- >>",mat2.rows,mat2.cols);
      env->ReleaseByteArrayElements(yuvData, yuvJbyte, 0);*/


}


jint JNI_OnLoad(JavaVM *vm, void *reserved) {
    jvm = vm;
    JNIEnv *env = NULL;
    jint result = -1;
    if (vm->GetEnv((void **) &env, JNI_VERSION_1_4) != JNI_OK) {
        goto bail;
    }
    result = JNI_VERSION_1_4;
    bail:
    return result;
}

//

