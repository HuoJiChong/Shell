//
// Created by J.C. on 2019/1/15.
//

#include <sys/stat.h>
#include "JniInfo.h"

#define PROP_VALUE_MAX 1024

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

bool JniInfo::init(JNIEnv *env, jobject context_obj)
{
    jstring pkgName_jstr = (jstring)CallObjectMethod(env,context_obj,"getPackageName","()Ljava/lang/String;");
    packageName = jstrToCstr(env,pkgName_jstr);
    env->DeleteLocalRef(pkgName_jstr);

    jstring apkPath_jstr = (jstring)CallObjectMethod(
            env,context_obj,"getPackageResourcePath","()Ljava/lang/String");
    apkPath = jstrToCstr(env,apkPath_jstr);
    env->DeleteLocalRef(apkPath_jstr);

    jstring appInfo_jstr = (jstring)CallObjectMethod(
            env,context_obj,"getApplicationInfo","()Landroid/content/pm/ApplicationInfo;");
    jstring nativeLibraryDir_jstr = (jstring)GetObjectFIELD(
            env,context_obj,"nativeLibraryDir","Ljava/lang/String;");
    libPath = jstrToCstr(env,nativeLibraryDir_jstr);
    env->DeleteLocalRef(appInfo_jstr);
    env->DeleteLocalRef(nativeLibraryDir_jstr);

//    get file data path
    jobject filesDir_obj = CallObjectMethod(
            env,context_obj,"getFilesDir","()Ljava/io/File;");
    jstring filesDir_jstr = (jstring)CallObjectMethod(env,filesDir_obj,"getAbsolutePath","()Ljava/lang/String;");
    filePath = jstrToCstr(env,filesDir_jstr);
    env->DeleteLocalRef(filesDir_obj);
    env->DeleteLocalRef(filesDir_jstr);

//    setup cachePath (and mkdir if not exist)
    cachePath = filePath.substr(0,filePath.find('/')) + "/.cache";
    mkdir(cachePath.c_str(),0700);

    FLOGD("============== Global Jni Information End ===============");
    FLOGD("packageName %s",packageName.c_str());
    FLOGD("apkPath %s",apkPath.c_str());
    FLOGD("libPath %s",libPath.c_str());
    FLOGD("filePath %s",filePath.c_str());
    FLOGD("cachePath %s",cachePath.c_str());
    FLOGD("sdk_is %s",sdk_int);
    if (isArt){
        FLOGD("device is running in art mode");
    }else{
        FLOGD("device is running in dalvik mode");
    }

    if (is64Bit()){
        FLOGD("process is running in 64 mode");
    }else{
        FLOGD("process is running in 32 mode");
    }

    FLOGD("================ Global Jni Information End ===================");
    return true;

}

bool JniInfo::isArtMode()
{
    char value1[PROP_VALUE_MAX];
    char value2[PROP_VALUE_MAX];

}

int JniInfo::getSdkVersion()
{
    char value[PROP_VALUE_MAX];
    if(__system_property_get("ro.build.version.sdk",value) <= 16){
        return -1;
    }
    int rel = 0;
    sscanf(value,"%d",&rel);
    return rel;
}

JniInfo::RuntimeVersion JniInfo::getRuntimeVersion()
{
#if defined(__arm__)
    return ARM;
#elif defined(__aarch64__)
    return ARM64;
#elif defined(__i386__)
    return X86;
#elif defined(__x86_64__)
    return X64;
#endif
    return UNKNOWN;
}

bool JniInfo::attatch(JavaVM *vm, JNIEnv *evn)
{

}

bool JniInfo::is64Bit()
{
    switch (getRuntimeVersion())
    {
        case X64:
        case ARM64:
        case MIPS64:
            return true;
        default:
            return false;
    }
}

namespace JniInfo{
#define JniInfo_CALLTYPE(_jtype,_jname)                     \
    _jtype Call##_jname##Method(JNIEnv* env,jobject obj,    \
        const char* name, const char* sig,...);             {  \
                                                                        \
        }                                                                               \
    _jtype Call##_jname##MethodV(JNIEnv* env,jobject obj,    \
        const char* name, const char* sig,va_list args){    \
                                                                    \
        }    \
    _jtype Call##_jname##MethodA(JNIEnv* env,jobject obj,    \
        const char* name, const char* sig,jvalue* args){        \
                                                                \
        }

#define JniInfo_CALL_NONVIRT_TYPE(_ctype,_jname,_retfail) \
    _ctype JniInfo::CallNonvirtual##_jname##Method(JNIEnv* env,jobject obj ,const char* classSig,const char* name,const char* sig,...) \
{      \
    va_list args;                       \
    va_start(args,sig);                 \
    VarArgs vargs(args);                \
    va_end(args);                       \
    return CallNonvirtual##_jname##MethodV(env,obj,classSig,name,sig,vargs);  \
} \
_ctype JniInfo::CallNonvirtual##_jname##MethodV(JNIEnv* env,jobject obj ,const char* classSig,const char* name,const char* sig,...) \
{\
}

}

//expended jni bridge


//#define CALL_VIRTUAL(_ctype,_jname,_retfail) \
//    _ctype JniInfo::Call##_jname##Method(JNIEnv* env,jobject obj,const char* name,const char* sig,...)  \
//{   \
//    va_list args;                       \
//    va_start(args,sig);                 \
//    VarArgs vargs(args);                \
//    va_end(args);                       \
//    return Call##_jname##Method(env,obj,name,sign,vargs);  \
//}


jobject JniInfo::getApplicatoinContext(JNIEnv *env)
{

}

