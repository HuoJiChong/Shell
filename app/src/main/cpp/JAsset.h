//
// Created by J.C. on 2019/1/16.
//

#ifndef SHELL_JASSET_H
#define SHELL_JASSET_H

#include <sys/types.h>
#include <jni.h>

#include <android/asset_manager.h>
#include <android/asset_manager_jni.h>
#include <vm/Common.h>

namespace JAsset{
    AAssetManager * getAssetManagerFromCtx(JNIEnv *pEnv, jobject pJobject);

    bool releaseEncryptFileIntoMem(AAssetManager* mgr,char* filename, const u1 *& pMem,u4 &len);
//  read data from asset ,and decrypt ,and release into cache dir
    bool releaseEncryptFileIntoCache(AAssetManager* mgr,const char* fileName,mode_t  mode);
}
#endif //SHELL_JASSET_H
