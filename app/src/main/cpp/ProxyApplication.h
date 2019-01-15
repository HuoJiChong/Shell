//
// Created by J.C. on 2019/1/1.
//

#ifndef SHELL_PROXYAPPLICATION_H
#define SHELL_PROXYAPPLICATION_H

#include "jni.h"

namespace ProxyApplication{
    void attachBaseContext(JNIEnv* env,jobject obj,jobject context);

    bool registerNativeMethod(JNIEnv* env);
}

#endif //SHELL_PROXYAPPLICATION_H
