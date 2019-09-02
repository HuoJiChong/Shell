//
// Created by J.C. on 2019/1/1.
//

#include <jni.h>
#include <vm/native.h>
#include "ProxyApplication.h"
#include "JniInfo.h"
#include "JAsset.h"
#include "dlfcn.h"

bool makeDexElements(JNIEnv* env,jobject classLoader,const std::vector<jobject >& dexFile_objs);

void ::ProxyApplication::attachBaseContext(JNIEnv* env,jobject obj,jobject context)
{
    //调用父类的方法
    JniInfo::CallNonvirtualVoidMethod(env,obj,"android/content/ContextWrapper","attachBaseContext","(Landroid/content/Context;)V",context);

    if (!JniInfo::init(env,context)){
        LOGE("%s","JniInfo::init() failed");
        return;
    }

    auto assetManager = JAsset::getAssetManagerFromCtx(env,context);

    JAsset::releaseEncryptFileIntoCache(assetManager,"encrypt.dex",0666);

    std::string cachefilePath = JniInfo::cachePath + "/encrypt.dex";
    std::string cachefileOptPath = JniInfo::cachePath + "/encrypt.odex";

    jstring cachefilePath_jstr = env->NewStringUTF(cachefilePath.c_str());
    jstring cachefileOpt_jstr = env->NewStringUTF(cachefileOptPath.c_str());

    /**
     *  1、 Java层  太浅啦
     */
    auto dexFile_obj = JniInfo::CallStaticObjectMethod(
            env, "dalvik/system/DexFile", "loadDex",
            "(Ljava/lang/String;Ljava/lang/String;I)Ldalvik/system/DexFile;",
            cachefilePath_jstr, cachefileOpt_jstr, false
    );

    /**
     * 2、 调用Native层
     */
//    auto libdvm = dlopen("libdvm.so",RTLD_NOW);
//    auto dvm_dalvik_system_DexFile = (JNINativeMethod*)dlsym(libdvm,"dvm_dalvik_system_DexFile");
//    void (*fnOpenDexFileNative)(const u4* args,JValue* pResult) = nullptr;
//    for (auto p = dvm_dalvik_system_DexFile; p->fnPtr != nullptr ; p++) {
//        if (strcmp(p->name,"openDexFileNative") == 0 && strcmp(p->signature ,"(Ljava/lang/String;Ljava/lang/String;I)I") == 0) {
//            fnOpenDexFileNative = (void (*)(const u4* ,JValue*))p->fnPtr;
//            break;
//        }
//    }
//
//    DexOrJar* pDexOrJar = nullptr;
//
//    if (fnOpenDexFileNative != nullptr){
//        auto dvmCreateStringFromCst = (void* (*)(const char* utf8Str))dlsym(libdvm,"_Z23dvmCreateStringFromCstPKc");
//        u4 args[2];
//        args[0] = static_cast<u4>(dvmCreateStringFromCst(cachefileOptPath.c_str()));
//        args[1] = static_cast<u4>(dvmCreateStringFromCst(cachefileOptPath.c_str()));
//        JValue result;
//        fnOpenDexFileNative(args,&result);
//        pDexOrJar = (DexOrJar *) result.l;
//
//    }
//
//    auto dexFile_cla = env->FindClass("dalvik/system/DexFile");
//    auto dexFile_obj = env->AllocObject(dexFile_cla);
//    JniInfo::SetObjectFIELD(env,dexFile_obj,"mCookie","I", static_cast<jint>(reinterpret_cast<uintptr_t>(pDexOrJar)));

    auto classLoader_obj = JniInfo::CallObjectMethod(env,context,"getClassLoader","()Ljava/lang/ClassLoader;");

    std::vector<jobject > dexFile_objs;
    dexFile_objs.push_back(dexFile_obj);
    makeDexElements(env,classLoader_obj,dexFile_objs);

    return;
}

bool makeDexElements(JNIEnv* env,jobject classLoader,const std::vector<jobject >& dexFile_objs)
{
    if(dexFile_objs.empty()) {
        return false;
    }
    env->PushLocalFrame(0x10);

    jobject pathList_obj = JniInfo::GetObjectFIELD(env,classLoader,"pathList","Ldalvik/system/DexPathList;");

    jobjectArray dexElements_obj = (jobjectArray)JniInfo::GetObjectFIELD(env,pathList_obj,"dexElements","[Ldalvik/system/DexPathList$Element;");

    jint dexElementCount = env->GetArrayLength(dexElements_obj);
    jint dexElementNewCount = dexElementCount + dexFile_objs.size();

    jclass element_cla = env->FindClass("dalvik/system/DexPathList$Element");
    jobjectArray new_dexElements_obj = env->NewObjectArray(dexElementNewCount,element_cla, nullptr);

    for (int i = 0; i < dexElementCount; ++i) {
        env->SetObjectArrayElement(new_dexElements_obj,i,env->GetObjectArrayElement(dexElements_obj,i));
    }

    jmethodID element_init_mid = env->GetMethodID(element_cla,"<init>","(Ljava/io/File;ZLjava/io/File;Ldalvik/system/DexFile;)V");

    for (int i = 0,length = dexFile_objs.size(); i < length; ++i) {
        jobject new_dexElement = env->NewObject(element_cla,element_init_mid, nullptr, nullptr,
                                                nullptr,dexFile_objs[i]);

        env->SetObjectArrayElement(new_dexElements_obj,i+dexElementCount,new_dexElement);
    }

    JniInfo::SetObjectFIELD(env, pathList_obj, "dexElements",
                            "[Ldalvik/system/DexPathList$Element;", new_dexElements_obj);

    env->PopLocalFrame(nullptr);
    return true;
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