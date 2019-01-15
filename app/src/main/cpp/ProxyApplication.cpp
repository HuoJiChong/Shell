//
// Created by J.C. on 2019/1/1.
//

#include "ProxyApplication.h"


void ::ProxyApplication::attachBaseContext(JNIEnv* env,jobject obj,jobject context)
{

}

bool ::ProxyApplication::registerNativeMethod(JNIEnv* env)
{
    auto clazz = env->FindClass("com/aly/roger/shell/ProxyApplication");
    JNINativeMethod methods[] = {
            { "attachBaseContext","(Landroid/content/Context;)V",(void *)attachBaseContext }
    };

    env->RegisterNatives(clazz,methods,1);
    return true;
}