//
// Created by J.C. on 2019/1/15.
//

#ifndef SHELL_JNIINFO_H
#define SHELL_JNIINFO_H

#include <jni.h>
#include <string>
#include "AndroidDef/vm/oo/Object.h"
#include <vector>
#include <sys/system_properties.h>

class VarArgs;

union JniValueType{
    jarray  a;
    jboolean b;
    jclass c;
    jfieldID f;
    jint i;
    jmethodID m;
    const void* p;
    jint r;//release mode.
    jstring s;
    jthrowable t;
    const char* u;
    JavaVM* v;
    jobjectRefType w;
    jsize z;
    jbyte B;
    jchar C;
    jdouble D;
    JNIEnv* E;
    jfloat F;
    jint I;
    jlong J;
    jobject L;
    jshort S;
    const void* V;
    jboolean Z;
    const VarArgs* va;
};

struct VarArgs{
public:
    VarArgs(va_list var):type_(kTypeVaList),cnt_(0){
        va_copy(vargs_,var);
    }

    VarArgs(const jvalue* vals):type_(kTypePtr),cnt_(0),ptr_(0){
    }

    ~VarArgs(){
        if(type_ == kTypeVaList){
            va_end(vargs_);
        }
    }

    VarArgs(VarArgs&& other){
        cnt_ = other.cnt_;
        type_ = other.type_;
        if(other.type_ == kTypeVaList){
            va_copy(vargs_,other.vargs_);
        }else{
            ptr_ = other.ptr_;
        }
    }

    va_list& vaargs(){
        assert(type_ == kTypeVaList && "Type not match");
        return vargs_;
    }

    const jvalue* vaptrs(){
            assert(type_ == kTypePtr && "type not match ptrs");
            return ptr_;
    };

    VarArgs Clone(){
        if(type_ == kTypeVaList){
            return VarArgs(cnt_, (static_cast<VarArgs*>(this))->vargs_);
        }else{
            return VarArgs(cnt_,ptr_);
        }
    }


    JniValueType GetValue(char fmt){
        JniValueType o;
        if (type_ == kTypeVaList){
            switch (fmt){
                case 'Z':o.Z = static_cast<jboolean>(va_arg(vargs_,jint)); break;
                case 'B':o.B = static_cast<jbyte >(va_arg(vargs_,jint)); break;
                case 'C':o.C = static_cast<jchar >(va_arg(vargs_,jint)); break;
                case 'S':o.S = static_cast<jshort >(va_arg(vargs_,jint)); break;
                case 'I':o.I = va_arg(vargs_,jint); break;
                case 'J':o.J = va_arg(vargs_,jlong); break;
                case 'F':o.F = static_cast<jfloat >(va_arg(vargs_,jdouble)); break;
                case 'D':o.D = va_arg(vargs_,jdouble); break;
                case 'L':o.L = va_arg(vargs_,jobject); break;
                default:assert(false && "Unknown format");break;
            }
        }else{
            jvalue v = ptr_[cnt_];
            cnt_++;
            switch (fmt){
                case 'Z':o.Z = v.z; break;
                case 'B':o.B = v.b; break;
                case 'C':o.C = v.c; break;
                case 'S':o.S = v.s; break;
                case 'I':o.I = v.i; break;
                case 'J':o.J = v.j; break;
                case 'F':o.F = v.f; break;
                case 'D':o.D = v.d; break;
                case 'L':o.L = v.l; break;
                default:assert(false && "UnKnown format");break;
            }
        }

        return o;
    }


private:
    VarArgs(uint32_t cnt,va_list var):type_(kTypeVaList),cnt_(cnt){
        va_copy(vargs_,var);
    }

    VarArgs(uint32_t cnt,const jvalue* vals):type_(kTypePtr),cnt_(cnt),ptr_(vals){}

    enum  VarArgsType{
        kTypeVaList,
        kTypePtr
    };

    VarArgsType type_;
    uint32_t cnt_;
    union {
        va_list vargs_;
        const jvalue* ptr_;
    };
};


struct AutoJniEnvRelease{
    AutoJniEnvRelease(JNIEnv* env){
        assert(env!= nullptr);
        m_env = env;
        m_env->PushLocalFrame(0x10);
    }
    ~AutoJniEnvRelease(){
        m_env->PopLocalFrame(nullptr);
    }
    JNIEnv* m_env;
};

struct AutoJniRefRelease{
    AutoJniRefRelease(JNIEnv* env,jobject obj){
        assert(env != nullptr);
        assert(obj != nullptr);
        m_env = env;
        m_obj = obj;
    }
    ~AutoJniRefRelease(){
        m_env->DeleteLocalRef(m_obj);
    }

    JNIEnv* m_env;
    jobject m_obj;
};


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

    enum RuntimeVersion{
        X86,
        X64,
        ARM,
        ARM64,
        MIPS,
        MIPS64,
        UNKNOWN
    };

    RuntimeVersion getRuntimeVersion();

    bool is64Bit();

    bool attatch(JavaVM* vm,JNIEnv* evn = nullptr);

    bool init (JNIEnv* env,jobject context_obj);

    jobject getApplicatoinContext(JNIEnv* env = nullptr);

    bool isArtMode();
    int getSdkVersion();
    std::string jstrToCstr(JNIEnv* env,jstring jstr);
    int getTracePid();

//成员方法
#define JNIINFO_CALL_TYPE_METHOD(_jtype, _jname)                                    \
                                                              \
    _jtype Call##_jname##Method(JNIEnv* env,jobject obj,const char* methodName, const char* methodSig, ...) ;

#define JNIINFO_CALL_TYPE_METHODV(_jtype, _jname)                                   \
                                                              \
    _jtype Call##_jname##MethodV(JNIEnv* env,jobject obj, const char* methodName, const char* methodSig,           \
        va_list args)  ;
#define JNIINFO_CALL_TYPE_METHODA(_jtype, _jname)                                   \
                                                              \
    _jtype Call##_jname##MethodA(JNIEnv* env,jobject obj, const char* methodName, const char* methodSig,           \
        jvalue* args);


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

    void CallVoidMethod(JNIEnv* env,jobject obj,   const char* methodName, const char* methodSig, ...);

    void CallVoidMethodV(JNIEnv* env,jobject obj,   const char* methodName, const char* methodSig, va_list args);

    void CallVoidMethodA(JNIEnv* env,jobject obj,   const char* methodName, const char* methodSig, jvalue* args);

// 非虚方法
#define JNIINFO_CALLNONVIRTUAL_TYPEMETHOD(_jtype, _jname)                                                                         \
                                                                                                                            \
    _jtype CallNonvirtual##_jname##Method(JNIEnv* env,jobject obj,const char* classSig,const char* methodName, const char* methodSig, ...) ;

#define JNIINFO_CALLNONVIRTUAL_TYPEMETHODV(_jtype, _jname)                                                                         \
                                                                                                                            \
    _jtype CallNonvirtual##_jname##MethodV(JNIEnv* env,jobject obj,const char* classSig, const char* methodName, const char* methodSig, va_list args)  ;

#define JNIINFO_CALLNONVIRTUAL_TYPEMETHODA(_jtype, _jname)                                                                         \
                                                                                                                            \
    _jtype CallNonvirtual##_jname##MethodA(JNIEnv* env,jobject obj,const char* classSig, const char* methodName, const char* methodSig, jvalue* args);

#define JNIINFO_CALLNONVIRTUAL_TYPE(_jtype, _jname)                                           \
    JNIINFO_CALLNONVIRTUAL_TYPEMETHOD(_jtype, _jname)                                        \
    JNIINFO_CALLNONVIRTUAL_TYPEMETHODV(_jtype, _jname)                                       \
    JNIINFO_CALLNONVIRTUAL_TYPEMETHODA(_jtype, _jname)

    JNIINFO_CALLNONVIRTUAL_TYPE(jobject, Object)
    JNIINFO_CALLNONVIRTUAL_TYPE(jboolean, Boolean)
    JNIINFO_CALLNONVIRTUAL_TYPE(jbyte, Byte)
    JNIINFO_CALLNONVIRTUAL_TYPE(jchar, Char)
    JNIINFO_CALLNONVIRTUAL_TYPE(jshort, Short)
    JNIINFO_CALLNONVIRTUAL_TYPE(jint, Int)
    JNIINFO_CALLNONVIRTUAL_TYPE(jlong, Long)
    JNIINFO_CALLNONVIRTUAL_TYPE(jfloat, Float)
    JNIINFO_CALLNONVIRTUAL_TYPE(jdouble, Double)

    void CallNonvirtualVoidMethod(JNIEnv* env,jobject obj, const char* classSig,  const char* methodName, const char* methodSig, ...);

    void CallNonvirtualVoidMethodV(JNIEnv* env,jobject obj, const char* classSig,  const char* methodName, const char* methodSig, va_list args);

    void CallNonvirtualVoidMethodA(JNIEnv* env,jobject obj, const char* classSig,  const char* methodName, const char* methodSig, jvalue* args);


// 静态方法
#define JNIINFO_CALL_STATIC_TYPE(_jtype, _jname)                                    \
                                                                                    \
    _jtype CallStatic##_jname##Method(JNIEnv* env,jclass clazz,const char* methodName, const char* methodSig, ...) ;            \
    _jtype CallStatic##_jname##Method(JNIEnv* env,const char* clazzName,const char* methodName,const char * methodSig, ...);    \
    _jtype CallStatic##_jname##MethodV(JNIEnv* env,jclass clazz, const char* methodName, const char* methodSig, va_list args)  ; \
    _jtype CallStatic##_jname##MethodV(JNIEnv* env,const char* clazzName,const char* methodName,const char * methodSig, va_list args);   \
    _jtype CallStatic##_jname##MethodA(JNIEnv* env,jclass clazz, const char* methodName, const char* methodSig, jvalue* args); \
    _jtype CallStatic##_jname##MethodA(JNIEnv* env,const char* clazzName,const char* methodName,const char * methodSig, jvalue* args);

    JNIINFO_CALL_STATIC_TYPE(jobject,Object);
    JNIINFO_CALL_STATIC_TYPE(jboolean,Boolean);
    JNIINFO_CALL_STATIC_TYPE(jbyte ,Byte);
    JNIINFO_CALL_STATIC_TYPE(jchar ,Char);
    JNIINFO_CALL_STATIC_TYPE(jshort ,Short);
    JNIINFO_CALL_STATIC_TYPE(jint ,Int);
    JNIINFO_CALL_STATIC_TYPE(jlong ,Long);
    JNIINFO_CALL_STATIC_TYPE(jfloat ,Float);
    JNIINFO_CALL_STATIC_TYPE(jdouble ,Double);


    void CallStaticVoidMethodV(JNIEnv* env,jclass clazz,   const char* methodName, const char* methodSig, va_list args);

    void CallStaticVoidMethodV(JNIEnv* env,const char*  clazzName,   const char* methodName, const char* methodSig, va_list args);

    void CallStaticVoidMethod(JNIEnv* env,jclass clazz,   const char* methodName, const char* methodSig, ...);

    void CallStaticVoidMethod(JNIEnv* env,const char* clazzName,   const char* methodName, const char* methodSig, ...);

    void CallStaticVoidMethodA(JNIEnv* env,jclass clazz,   const char* methodName, const char* methodSig, jvalue* args);

    void CallStaticVoidMethodA(JNIEnv* env,const char* clazzName,   const char* methodName, const char* methodSig, jvalue* args);


//    获得对象的属性
#define JniInfo_GetFIELD(_jtype,_jname)                     \
    _jtype Get##_jname##FIELD(JNIEnv* env,jobject obj,    \
        const char* name, const char* sig);        \
    _jtype GetStatic##_jname##FIELD(JNIEnv* env,jobject obj,    \
        const char* clazzSig,const char* name, const char* sig);

    JniInfo_GetFIELD(jobject,Object);
    JniInfo_GetFIELD(jboolean,Boolean);
    JniInfo_GetFIELD(jbyte ,Byte);
    JniInfo_GetFIELD(jchar ,Char);
    JniInfo_GetFIELD(jshort ,Short);
    JniInfo_GetFIELD(jint ,Int);
    JniInfo_GetFIELD(jlong ,Long);
    JniInfo_GetFIELD(jfloat ,Float);
    JniInfo_GetFIELD(jdouble ,Double);
    JniInfo_GetFIELD(jobjectArray ,ObjectArray);

    /**
     * 在函数中定义了返回值，却没有返回会崩溃，Fatal signal 4 (SIGILL)
     */
//    设置对象的属性
#define JniInfo_SetFIELD(_jtype,_jname)                     \
    void Set##_jname##FIELD(JNIEnv* env,jobject obj,    \
        const char* name, const char* sig,_jtype value);        \
    void SetStatic##_jname##FIELD(JNIEnv* env,jobject obj,    \
        const char* clazzSig,const char* name, const char* sig,_jtype value);

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

#endif //SHELL_JNIINFO_H
