//
// Created by J.C. on 2019/1/1.
//

#include "jni.h"
#include "ProxyApplication.h"

/**
 * 必须要有env 和 obj,供Java调用
 */
int fromNative(JNIEnv* env,jobject obj)
{
    return 1;
}

JNIEXPORT jint JNICALL JNI_OnLoad(JavaVM* vm,void* reser)
{
    JNIEnv* env = nullptr;
    if(vm->GetEnv((void**)&env, JNI_VERSION_1_4 ) != JNI_OK )
    {
        return -1;
    }

    auto clazz = env->FindClass("com/aly/roger/shell/ProxyApplication");
    JNINativeMethod methods[] = {
            {"fromNative","()V",(void *)fromNative}
    };
    env->RegisterNatives(clazz,methods,1);

// 注册代理方法
    ProxyApplication::registerNativeMethod(env);

    return JNI_VERSION_1_4;
}

JNIEXPORT void JNICALL JNI_OnUnload(JavaVM*vm ,void* reser)
{
    JNIEnv * env = nullptr;
    if(vm->GetEnv((void**)&env,JNI_VERSION_1_4) != JNI_OK){
        return;
    }
    auto clazz = env->FindClass("com/aly/roger/shell/ProxyApplication");
    env->UnregisterNatives(clazz);

    return;
}


