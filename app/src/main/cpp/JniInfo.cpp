//
// Created by J.C. on 2019/1/15.
//

#include "JniInfo.h"

bool isNumber(char a){
    if (a>='0' && a<='9'){
        return true;
    }
    return false;
}

bool isNumber(char a[]){
    for (int i = 0; a[i]; ++i) {
        if (!isNumber(a[i])){
            return false;
        }
    }
    return true;
}



namespace JniInfo{
    extern std::string packageName;
    extern std::string apkPath;
    extern std::string libPath;
    extern std::string filePath;
    extern std::string cachePath;

    extern int sdk_int;

    extern JavaVM* vm;
    extern bool isDalyik;
    extern bool isArt;
}

JniInfo::RuntimeVersion JniInfo::getRuntimeVersion()
{

}

bool JniInfo::attatch(JavaVM *vm, JNIEnv *evn)
{

}

bool JniInfo::is64Bit()
{

}

bool JniInfo::init(JNIEnv *env, jobject context_obj)
{

}

jobject JniInfo::getApplicatoinContext(JNIEnv *env)
{

}

