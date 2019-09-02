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
//    extern std::string packageName;
//    extern std::string apkPath;
//    extern std::string libPath;
//    extern std::string filePath;
//    extern std::string cachePath;
//
//    extern int sdk_int;
//
//    extern JavaVM* vm;
//    extern bool isDalyik;
//    extern bool isArt;

    std::string packageName;
    std::string apkPath;
    std::string libPath;
    std::string filePath;
    std::string cachePath;

    int sdk_int;

    JavaVM* vm;
    bool isDalyik;
    bool isArt;
}

bool JniInfo::init(JNIEnv *env, jobject context_obj)
{
    jstring pkgName_jstr = (jstring)CallObjectMethod(env,context_obj,"getPackageName","()Ljava/lang/String;");
    packageName = jstrToCstr(env,pkgName_jstr);
    env->DeleteLocalRef(pkgName_jstr);

    jstring apkPath_jstr = (jstring)CallObjectMethod(env,context_obj,"getPackageResourcePath","()Ljava/lang/String;");
    apkPath = jstrToCstr(env,apkPath_jstr);
    env->DeleteLocalRef(apkPath_jstr);

    jobject appInfo_jstr = (jstring)CallObjectMethod(env,context_obj,"getApplicationInfo","()Landroid/content/pm/ApplicationInfo;");
    jstring nativeLibraryDir_jstr = (jstring)GetObjectFIELD(env,appInfo_jstr,"nativeLibraryDir","Ljava/lang/String;");
    libPath = jstrToCstr(env,nativeLibraryDir_jstr);
    env->DeleteLocalRef(appInfo_jstr);
    env->DeleteLocalRef(nativeLibraryDir_jstr);

//    get file data path
    jobject filesDir_obj = CallObjectMethod(env,context_obj,"getFilesDir","()Ljava/io/File;");
    jstring filesDir_jstr = (jstring)CallObjectMethod(env,filesDir_obj,"getAbsolutePath","()Ljava/lang/String;");
    filePath = jstrToCstr(env,filesDir_jstr);
    env->DeleteLocalRef(filesDir_obj);
    env->DeleteLocalRef(filesDir_jstr);

//    setup cachePath (and mkdir if not exist)
    cachePath = filePath.substr(0,filePath.find_last_of('/')) + "/cache";
    mkdir(cachePath.c_str(),0700);

    FLOGD("============== Global Jni Information Begin ===============");
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

std::string JniInfo::jstrToCstr(JNIEnv* env,jstring jstr){
    std::string cstr = env->GetStringUTFChars(jstr,NULL);
    env->ReleaseStringUTFChars(jstr,cstr.c_str());
    return cstr;
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

bool JniInfo::attatch(JavaVM *jvm, JNIEnv *env)
{
    return jvm->AttachCurrentThread(&env,NULL) != 0;
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

#define JNIINFO_CALL_TYPE_METHOD(_jtype, _jname)                            \
                                                                            \
    _jtype Call##_jname##Method(JNIEnv* env,jobject obj,const char* methodName, const char* methodSig, ...)       \
    {                                                                       \
        _jtype result;                                                      \
        va_list args;                                                       \
        va_start(args, methodSig);                                          \
        result = Call##_jname##MethodV( env,obj, methodName,methodSig,args);\
        va_end(args);                                                       \
        return result;                                                      \
    }

#define JNIINFO_CALL_TYPE_METHODV(_jtype, _jname)                           \
                                                              				\
    _jtype Call##_jname##MethodV(JNIEnv* env,jobject obj, const char* methodName, const char* methodSig,           \
        va_list args)                                                       \
    {                                                                       \
        jclass clazz = env->GetObjectClass(obj);                            \
        jmethodID methodID = env->GetMethodID(clazz,methodName,methodSig);  \
        return env->Call##_jname##MethodV( obj, methodID, args);;           \
    }

#define JNIINFO_CALL_TYPE_METHODA(_jtype, _jname)                           \
                                                              				\
    _jtype Call##_jname##MethodA(JNIEnv* env,jobject obj,  const char* methodName, const char* methodSig,           \
        jvalue* args)                                                       \
    { 																		\
    	jclass clazz = env->GetObjectClass(obj);							\
    	jmethodID methodID = env->GetMethodID(clazz,methodName,methodSig);	\
    	return env->Call##_jname##MethodA( obj, methodID, args); 			\
    }

#define JNIINFO_CALL_TYPE(_jtype, _jname)                                           \
    JNIINFO_CALL_TYPE_METHOD(_jtype, _jname)                                        \
    JNIINFO_CALL_TYPE_METHODV(_jtype, _jname)                                       \
    JNIINFO_CALL_TYPE_METHODA(_jtype, _jname)

    JNIINFO_CALL_TYPE(jobject, Object)
    JNIINFO_CALL_TYPE(jboolean, Boolean)
    JNIINFO_CALL_TYPE(jbyte, Byte)
    JNIINFO_CALL_TYPE(jchar, Char)
    JNIINFO_CALL_TYPE(jshort, Short)
    JNIINFO_CALL_TYPE(jint, Int)
    JNIINFO_CALL_TYPE(jlong, Long)
    JNIINFO_CALL_TYPE(jfloat, Float)
    JNIINFO_CALL_TYPE(jdouble, Double)

    void CallVoidMethod(JNIEnv* env,jobject obj,   const char* methodName, const char* methodSig, ...)
    {
        va_list args;
        va_start(args, methodSig);
        CallVoidMethodV(env,obj, methodName,methodSig, args);
        va_end(args);
    }

    void CallVoidMethodV(JNIEnv* env,jobject obj,   const char* methodName, const char* methodSig, va_list args)
    {
        jclass clazz = env->GetObjectClass(obj);
        jmethodID methodID = env->GetMethodID(clazz,methodName,methodSig);
        env->CallVoidMethodV(obj,methodID,args);
    }

    void CallVoidMethodA(JNIEnv* env,jobject obj,   const char* methodName, const char* methodSig, jvalue* args)
    {
        jclass clazz = env->GetObjectClass(obj);
        jmethodID methodID = env->GetMethodID(clazz,methodName,methodSig);
        env->CallVoidMethodA( obj, methodID, args);
    }

#define JNIINFO_CALLNONVIRTUAL_TYPE_METHOD(_jtype, _jname)                  \
                                                                            \
    _jtype CallNonvirtual##_jname##Method(JNIEnv* env,jobject obj,const char* classSig,  const char* methodName, const char* methodSig, ...)  \
    {                                                                       \
        _jtype result;                                                      \
        va_list args;                                                       \
        va_start(args, methodSig);                                          \
        result = CallNonvirtual##_jname##MethodV(env, obj,classSig, methodName,methodSig,args);  \
        va_end(args);                                                       \
        return result;                                                     \
    }

#define JNIINFO_CALLNONVIRTUAL_TYPE_METHODV(_jtype, _jname)                 \
                                                              				\
    _jtype CallNonvirtual##_jname##MethodV(JNIEnv* env,jobject obj,const char* classSig,  const char* methodName, const char* methodSig, va_list args)                                                       \
    {                                                                       \
        jclass clazz = env->FindClass(classSig);                            \
        jmethodID methodID = env->GetMethodID(clazz,methodName,methodSig);  \
        return env->CallNonvirtual##_jname##MethodV(obj,clazz, methodID, args);     \
    }

#define JNIINFO_CALLNONVIRTUAL_TYPE_METHODA(_jtype, _jname)                           \
                                                              				\
    _jtype CallNonvirtual##_jname##MethodA(JNIEnv* env,jobject obj,const char* classSig,   const char* methodName, const char* methodSig,jvalue* args)                                                       \
    { 																		\
    	jclass clazz = env->FindClass(classSig);							\
    	jmethodID methodID = env->GetMethodID(clazz,methodName,methodSig);	\
    	return env->CallNonvirtual##_jname##MethodA( obj, clazz,methodID, args); 			\
    }

#define JNIINFO_CALLNONVIRTUAL_TYPE(_jtype, _jname)                                           \
    JNIINFO_CALLNONVIRTUAL_TYPE_METHOD(_jtype, _jname)                                        \
    JNIINFO_CALLNONVIRTUAL_TYPE_METHODV(_jtype, _jname)                                       \
    JNIINFO_CALLNONVIRTUAL_TYPE_METHODA(_jtype, _jname)

    JNIINFO_CALLNONVIRTUAL_TYPE(jobject, Object)
    JNIINFO_CALLNONVIRTUAL_TYPE(jboolean, Boolean)
    JNIINFO_CALLNONVIRTUAL_TYPE(jbyte, Byte)
    JNIINFO_CALLNONVIRTUAL_TYPE(jchar, Char)
    JNIINFO_CALLNONVIRTUAL_TYPE(jshort, Short)
    JNIINFO_CALLNONVIRTUAL_TYPE(jint, Int)
    JNIINFO_CALLNONVIRTUAL_TYPE(jlong, Long)
    JNIINFO_CALLNONVIRTUAL_TYPE(jfloat, Float)
    JNIINFO_CALLNONVIRTUAL_TYPE(jdouble, Double)

    /**
     *
     */
    void CallNonvirtualVoidMethod(JNIEnv* env,jobject obj,const char* classSig,   const char* methodName, const char* methodSig, ...)
    {
        va_list args;
        va_start(args, methodSig);
        CallNonvirtualVoidMethodV(env,obj,classSig, methodName,methodSig, args);
        va_end(args);
    }

    void CallNonvirtualVoidMethodV(JNIEnv* env,jobject obj, const char* classSig,  const char* methodName, const char* methodSig, va_list args)
    {
        jclass clazz = env->FindClass(classSig);
        jmethodID methodID = env->GetMethodID(clazz,methodName,methodSig);
        env->CallNonvirtualVoidMethodV(obj,clazz,methodID,args);
    }

    void CallNonvirtualVoidMethodA(JNIEnv* env,jobject obj, const char* classSig,   const char* methodName, const char* methodSig, jvalue* args)
    {
        jclass clazz = env->FindClass(classSig);
        jmethodID methodID = env->GetMethodID(clazz,methodName,methodSig);
        env->CallNonvirtualVoidMethodA(obj,clazz,methodID,args);
    }

#define JNIINFO_CALL_STATIC_TYPE_METHOD(_jtype, _jname)                  \
    _jtype CallStatic##_jname##Method(JNIEnv* env,jclass clazz,          \
                    const char* methodName, const char* methodSig, ...)  \
    {                                                                       \
        _jtype result;                                                      \
        va_list args;                                                       \
        va_start(args, methodSig);                                          \
        result = CallStatic##_jname##MethodV(env, clazz, methodName,methodSig,args);  \
        va_end(args);                                                       \
        return result;                                                     \
    }\
    _jtype CallStatic##_jname##Method(JNIEnv* env,const char* clazzName,const char* methodName,const char * methodSig, ...)\
    {   \
        _jtype result;                                                      \
        va_list args;                                                       \
        va_start(args, methodSig);                                          \
        result = CallStatic##_jname##MethodV(env, clazzName, methodName,methodSig,args);  \
        va_end(args);                                                       \
        return result;      \
    }

#define JNIINFO_CALL_STATIC_TYPE_METHODV(_jtype, _jname)                    \
                                                              				\
    _jtype CallStatic##_jname##MethodV(JNIEnv* env,jclass clazz, const char* methodName, const char* methodSig,           \
        va_list args)                                                       \
    {                                                                       \
        jmethodID methodID = env->GetStaticMethodID(clazz,methodName,methodSig);  \
        return env->CallStatic##_jname##MethodV(clazz, methodID, args);     \
    }\
    _jtype CallStatic##_jname##MethodV(JNIEnv* env,const char* clazzName,const char* methodName,const char * methodSig, va_list args)\
    {   \
        jclass clazz = env->FindClass(clazzName);                           \
        jmethodID methodID = env->GetStaticMethodID(clazz,methodName,methodSig);  \
        return env->CallStatic##_jname##MethodV(clazz, methodID, args);     \
    }

#define JNIINFO_CALL_STATIC_TYPE_METHODA(_jtype, _jname)                    \
                                                              				\
    _jtype CallStatic##_jname##MethodA(JNIEnv* env,jclass clazz,  const char* methodName, const char* methodSig,jvalue* args) \
    { 																		\
    	jmethodID methodID = env->GetStaticMethodID(clazz,methodName,methodSig);	\
    	return env->CallStatic##_jname##MethodA( clazz,methodID, args);     \
    }\
    _jtype CallStatic##_jname##MethodA(JNIEnv* env,const char* clazzName,const char* methodName,const char * methodSig, jvalue* args)\
    {\
        jclass clazz = env->FindClass(clazzName);                           \
        jmethodID methodID = env->GetStaticMethodID(clazz,methodName,methodSig);  \
        return env->CallStatic##_jname##MethodA(clazz, methodID, args);     \
    }

#define JNIINFO_CALL_STATIC_TYPE(_jtype, _jname)                                           \
    JNIINFO_CALL_STATIC_TYPE_METHOD(_jtype, _jname)                                        \
    JNIINFO_CALL_STATIC_TYPE_METHODV(_jtype, _jname)                                       \
    JNIINFO_CALL_STATIC_TYPE_METHODA(_jtype, _jname)

    JNIINFO_CALL_STATIC_TYPE(jobject, Object)
    JNIINFO_CALL_STATIC_TYPE(jboolean, Boolean)
    JNIINFO_CALL_STATIC_TYPE(jbyte, Byte)
    JNIINFO_CALL_STATIC_TYPE(jchar, Char)
    JNIINFO_CALL_STATIC_TYPE(jshort, Short)
    JNIINFO_CALL_STATIC_TYPE(jint, Int)
    JNIINFO_CALL_STATIC_TYPE(jlong, Long)
    JNIINFO_CALL_STATIC_TYPE(jfloat, Float)
    JNIINFO_CALL_STATIC_TYPE(jdouble, Double)


    void CallStaticVoidMethodV(JNIEnv* env,jclass clazz,   const char* methodName, const char* methodSig, va_list args)
    {
        jmethodID methodID = env->GetStaticMethodID(clazz,methodName,methodSig);
        env->CallStaticVoidMethodV(clazz,methodID,args);
    }

    void CallStaticVoidMethodV(JNIEnv* env,const char*  clazzName,   const char* methodName, const char* methodSig, va_list args)
    {
        jclass clazz = env->FindClass(clazzName);
        jmethodID methodID = env->GetStaticMethodID(clazz,methodName,methodSig);
        env->CallStaticVoidMethodV(clazz,methodID,args);
    }
    void CallStaticVoidMethod(JNIEnv* env,jclass clazz,   const char* methodName, const char* methodSig, ...)
    {
        va_list args;
        va_start(args, methodSig);
        CallStaticVoidMethodV(env,clazz, methodName,methodSig, args);
        va_end(args);
    }

    void CallStaticVoidMethod(JNIEnv* env,const char* clazzName,   const char* methodName, const char* methodSig, ...)
    {
        va_list args;
        va_start(args, methodSig);
        CallStaticVoidMethodV(env,clazzName, methodName,methodSig, args);
        va_end(args);
    }

    void CallStaticVoidMethodA(JNIEnv* env,jclass clazz,   const char* methodName, const char* methodSig, jvalue* args)
    {
        jmethodID methodID = env->GetStaticMethodID(clazz,methodName,methodSig);
        env->CallStaticVoidMethodA(clazz,methodID,args);
    }

    void CallStaticVoidMethodA(JNIEnv* env,const char* clazzName,   const char* methodName, const char* methodSig, jvalue* args)
    {
        jclass clazz = env->FindClass(clazzName);
        jmethodID methodID = env->GetStaticMethodID(clazz,methodName,methodSig);
        env->CallStaticVoidMethodA(clazz,methodID,args);
    }


#define JniInfo_GetFIELD(_jtype,_jname)                     \
    _jtype Get##_jname##FIELD(JNIEnv* env,jobject obj,const char* fname, const char* fsig)         \
    {                                                                                           \
        jclass clazz = env->GetObjectClass(obj);                                                \
        jfieldID fid = env->GetFieldID(clazz,fname,fsig);                                       \
        return (env->Get##_jname##Field(obj, fid));                                             \
    } \
    _jtype GetStatic##_jname##FIELD(JNIEnv* env,jobject obj, const char* clazzSig,const char* fname, const char* fsig)    \
    {                                                                                   \
        jclass clazz = env->GetObjectClass(obj);                                        \
        jfieldID fid = env->GetStaticFieldID(clazz,fname,fsig);                              \
        return env->GetStatic##_jname##Field(clazz,fid);                                \
    }

    JniInfo_GetFIELD(jobject,Object);
    JniInfo_GetFIELD(jboolean,Boolean);
    JniInfo_GetFIELD(jbyte ,Byte);
    JniInfo_GetFIELD(jchar ,Char);
    JniInfo_GetFIELD(jshort ,Short);
    JniInfo_GetFIELD(jint ,Int);
    JniInfo_GetFIELD(jlong ,Long);
    JniInfo_GetFIELD(jfloat ,Float);
    JniInfo_GetFIELD(jdouble ,Double);

//    设置对象的属性
#define JniInfo_SetFIELD(_jtype,_jname)                     \
    void Set##_jname##FIELD(JNIEnv* env,jobject obj, const char* fname, const char* fsig,_jtype value)  \
    {                                                                           \
        jclass clazz = env->GetObjectClass(obj);                                \
        jfieldID fid = env->GetFieldID(clazz,fname,fsig);                       \
        env->Set##_jname##Field(obj,fid,value);                                 \
    } \
    void SetStatic##_jname##FIELD(JNIEnv* env,jobject obj, const char* clazzSig,const char* fname, const char* fsig,_jtype value)   \
    {                                                                            \
        jclass clazz = env->GetObjectClass(obj);                                 \
        jfieldID  fid = env->GetStaticFieldID(clazz,fname,fsig);                \
        env->SetStatic##_jname##Field(clazz,fid,value);                              \
    }

    JniInfo_SetFIELD(jobject ,Object);
    JniInfo_SetFIELD(jboolean,Boolean);
    JniInfo_SetFIELD(jbyte ,Byte);
    JniInfo_SetFIELD(jchar ,Char);
    JniInfo_SetFIELD(jshort ,Short);
    JniInfo_SetFIELD(jint ,Int);
    JniInfo_SetFIELD(jlong ,Long);
    JniInfo_SetFIELD(jfloat ,Float);
    JniInfo_SetFIELD(jdouble ,Double);

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

