//
// Created by J.C. on 2019/1/16.
//

#ifndef SHELL_JASSET_H
#define SHELL_JASSET_H

#include <sys/types.h>
#include <jni.h>

namespace JAsset{
    bool releaseEncryptFileIntoCache(AAsetManager* mgr,const char* fileName,mode_t  mode);

    char * getAssetManagerFromCtx(JNIEnv *pEnv, jobject pJobject);

}
#endif //SHELL_JASSET_H
