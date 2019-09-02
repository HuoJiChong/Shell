//
// Created by J.C. on 2019/1/16.
//

#include "JAsset.h"
#include "JniInfo.h"
#include <sys/mman.h>
#include <cstring>
#include <string>
#include <sys/stat.h>
#include <errno.h>

namespace JAsset{

    AAssetManager * getAssetManagerFromCtx(JNIEnv *env, jobject jobj){
        auto assetManager_obj = JniInfo::CallObjectMethod(env,jobj,"getAssets","()Landroid/content/res/AssetManager;");
        auto assetManager = AAssetManager_fromJava(env,assetManager_obj);
        env->DeleteLocalRef(assetManager_obj);
        return assetManager;
    }

    bool releaseEncryptFileIntoMem(AAssetManager* mgr,char* filename, const u1 *& pMem,u4 &len){
        AAsset * pFile = AAssetManager_open(mgr,filename,AASSET_MODE_BUFFER);
        if (pFile != NULL){
            len = AAsset_getLength(pFile);

            // 有疑问  后面的参数
            void * m = mmap(0,len,PROT_READ | PROT_WRITE,MAP_PRIVATE,-1,0);
            if (m != MAP_FAILED){
                pMem = (u1*)m;
                char tmp[1024];
                int iRead;
                u1* wPoint = (u1*)pMem;
                auto restLen = len;

                while ((iRead = AAsset_read(pFile,tmp,len > 1024 ? 1024 : len)) !=  EOF){
                    memcpy(wPoint,tmp,iRead);
                    wPoint += iRead;
                    restLen -= iRead;
                }
            }

            AAsset_close(pFile);
            return  m != MAP_FAILED;
        }

        return false;
    }

    bool releaseEncryptFileIntoCache(AAssetManager* mgr,const char* filename,mode_t  mode){
        AAsset * pFile = AAssetManager_open(mgr,filename,AASSET_MODE_BUFFER);
        if (pFile != NULL){
            std::string relPath = JniInfo::cachePath + "/" + filename;
            auto fp = fopen(relPath.c_str(),"w+");

            if (fp != NULL){
                int fd = fileno(fp);
//                修改权限
                fchmod(fd,mode);
                auto fileLen = AAsset_getLength(pFile);
                char buf[1024];
                int iRead;
                LOGI("%s"," 开始读取文件 ");
                while ((iRead = AAsset_read(pFile,buf,1024)) > 0 ){
                    fwrite(buf,1,iRead,fp);
                    LOGI("读取到的文件大小： %d",iRead);
                }
                fclose(fp);
            }else{
                FLOGE("unable to write file %s %s", relPath.c_str() , strerror(errno));
            }

            AAsset_close(pFile);
            return  fp != MAP_FAILED;
        }

        return false;
    }
}


