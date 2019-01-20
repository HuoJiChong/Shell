package com.aly.roger.shell;

import android.app.Application;
import android.app.Instrumentation;
import android.content.Context;
import android.content.pm.ApplicationInfo;
import android.view.View;
import android.widget.Button;

import java.util.ArrayList;

/**
 * Created by J.C. on 2018/12/30.
 */

public class ProxyApplication extends Application {

    static {
        System.loadLibrary("HShell");
    }
    
    private static final String ActivityThreadName = "android.app.ActivityThread";
    private static final String LoadedApkName = "android.app.LoadedApk";

    @Override
    public void onCreate() {
        super.onCreate();
        Object currentActivityThread = JavaRef.invokeStaticMethod(ActivityThreadName,"currentActivityThread",new Class[]{},new Object[]{});
        Object mBoundApplication = JavaRef.getFieldObject(ActivityThreadName,"mBoundApplication",currentActivityThread);
        Object loadedApkInfo = JavaRef.getFieldObject(ActivityThreadName+"$AppBindData","info",mBoundApplication);

//      先将LoadedApkInfo的mApplication 设置为空，
        JavaRef.setFieldObject(LoadedApkName,"mApplication",loadedApkInfo,null);
//      重新设置Application的info
        String srcAppName = "com.aly.roger.shell.MyApplication";
        ApplicationInfo appInfo_in_loadedApk = (ApplicationInfo) JavaRef.getFieldObject(LoadedApkName,"mApplicationInfo",loadedApkInfo);
        appInfo_in_loadedApk.className = srcAppName;

        Application oldApplication = (Application) JavaRef.getFieldObject(ActivityThreadName,"mInitialApplication",currentActivityThread);
        ArrayList<Application> mAllApplication = (ArrayList<Application>) JavaRef.getFieldObject(ActivityThreadName,"mAllApplications",currentActivityThread);
        mAllApplication.remove(oldApplication);

        Application realApp = (Application) JavaRef.invokeMethod(LoadedApkName,"makeApplication",new Class[]{boolean.class, Instrumentation.class},loadedApkInfo,new Object[]{false,null});
        realApp.onCreate();

        JavaRef.setFieldObject(ActivityThreadName,"mInitialApplication",currentActivityThread,realApp);
        return;
    }

//    @Override
//    protected void attachBaseContext(Context base) {
//        super.attachBaseContext(base);
//
//        int a = fromNative();
//        Log.e("fromNative",a+"");
//
//        File cache = getDir("shell",MODE_PRIVATE);
//        String srcDex = cache + "/encrypt.dex";
//        File dexFile = FileManager.releaseAssetsFile(this,"encrypt.dex",srcDex,null);
//        DexClassLoader cl = new DexClassLoader(srcDex,getDir("shell_oat",MODE_PRIVATE).getAbsolutePath(),
//                getApplicationInfo().nativeLibraryDir,getClassLoader());

//        Object currentActivityThread = JavaRef.invokeStaticMethod(ActivityThreadName,"currentActivityThread", new Class[]{},new Object[]{});
//
//        HashMap mPackages = (HashMap)JavaRef.getFieldObject(ActivityThreadName,"mPackages",currentActivityThread);
//
//        WeakReference wr = (WeakReference) mPackages.get(getPackageName());
//        JavaRef.setFieldObject("android.app.LoadedApk","mClassLoader",wr.get(),cl);
//
//        return ;
//    }


    @Override
    protected native void attachBaseContext(Context base);
}
