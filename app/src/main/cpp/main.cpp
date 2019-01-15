//
// Created by J.C. on 2019/1/1.
//

#include "jni.h"

int fromNative(JNIEnv* env,jobject obj)
{
    return 1;
}

JNIEXPORT jint JNICALL JNI_OnLoad(JavaVM* vm,void* reser)
{

    JNIEnv* env = nullptr;
    if(vm->GetEnv((void**)&env,JNI_VERSION_1_6) != JNI_OK )
    {
        return -1;
    }

    auto clazz = env->FindClass("com/aly/roger/shell/ProxyApplication");
    JNINativeMethod methods[] = {
            {"fromNative","()I",(void *)fromNative}
    };

    env->RegisterNatives(clazz,methods,1);

    return 0;
}


