//
// Created by yuanshuai on 2019/3/28.
//

#ifndef FACERECOGNITION_COMMON_H
#define FACERECOGNITION_COMMON_H

#ifdef __cplusplus
extern "C"
{
#endif // __cplusplus

#define ERR_EXIT(m)  \
    do \
    { \
        perror(m); \
        exit(EXIT_FAILURE); \
    }while(0)

#ifdef __cplusplus
}
#endif // __cplusplus

#endif //FACERECOGNITION_COMMON_H
