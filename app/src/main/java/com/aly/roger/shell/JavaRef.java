package com.aly.roger.shell;

import java.lang.reflect.Field;
import java.lang.reflect.InvocationTargetException;
import java.lang.reflect.Method;
import java.lang.reflect.Modifier;

import dalvik.system.DexClassLoader;

/**
 * Created by J.C. on 2018/12/30.
 */

public class JavaRef {

    public static Method getMethod(String class_name,String method_name,Class[] pareType){
        try {
            Class obj_class = Class.forName(class_name);
            Method method = obj_class.getDeclaredMethod(method_name,pareType);
            method.setAccessible(true);
            return method;
        } catch (ClassNotFoundException e) {
            e.printStackTrace();
        } catch (NoSuchMethodException e) {
            e.printStackTrace();
        }
        return  null;
    }

    public static Method getMethod(Class clazz, String method_name,Class[] pareType){
        try {
            Method method = clazz.getDeclaredMethod(method_name,pareType);
            method.setAccessible(true);
            return method;
        } catch (NoSuchMethodException e) {
            e.printStackTrace();
        }
        return  null;
    }

    public static Method getMethodByFirstName(Class clazz,String method_name){
        Method[] methods = clazz.getMethods();
        for (Method method : methods){
            if (method.getName().equals(method_name)){
                method.setAccessible(true);
                return method;
            }
        }

        return null;
    }


    public static Object invokeMethod(String class_name,String method_name,Class[] pareType,Object obj,Object[] pareValues){
        try {
            Method method = getMethod(class_name,method_name,pareType);
            method.setAccessible(true);
            return method.invoke(obj,pareValues);
        } catch (Exception e) {
            e.printStackTrace();
        }
        return null;
    }

    public static Object invokeMethod(Class clazz,String method_name,Class[] pareType,Object obj,Object[] pareValues){
        try {
            Method method = getMethod(clazz,method_name,pareType);
            method.setAccessible(true);
            return method.invoke(obj,pareValues);
        } catch (Exception e) {
            e.printStackTrace();
        }
        return null;
    }

    public static Object invokeStaticMethod(String class_name,String method_name,Class[] pareTyple,Object[] pareValues){
        return invokeMethod(class_name,method_name,pareTyple,null,pareValues);
    }

    public static Object invokeStaticMethod(Class clazz,String method_name,Class[] pareTyple,Object[] pareValues){
        return invokeMethod(clazz,method_name,pareTyple,null,pareValues);
    }

    public static Object getFieldObject(String class_name,String filedName,Object obj){
        try {
            Class obj_class = Class.forName(class_name);
            Field field = obj_class.getDeclaredField(filedName);
            field.setAccessible(true);
            return field.get(obj);
        } catch (Exception e) {
            e.printStackTrace();
        }
        return null;
    }

    public static Object getFieldObject(Class clazz,String filedName,Object obj){
        try {
            Field field = clazz.getDeclaredField(filedName);
            field.setAccessible(true);
            return field.get(obj);
        }  catch (Exception e) {
            e.printStackTrace();
        }
        return null;
    }

    public static Object getStaticFieldObject(String class_name,String fieldName){
        return getFieldObject(class_name,fieldName,null);
    }

    public static Object getStaticFieldObject(Class clazz,String fieldName){
        return getFieldObject(clazz,fieldName,null);
    }

    public static Field getField(Class clazz , String name) throws NoSuchFieldException {
        Field f = clazz.getField(name);
        f.setAccessible(true);
        return f;
    }

    public static void setFieldObject(String class_name,String filedName,Object obj,Object fieldValue ){
        try {
            Class clazz = Class.forName(class_name);
            Field field = clazz.getDeclaredField(filedName);
            field.setAccessible(true);
            field.set(obj,fieldValue);

        } catch (Exception e) {
            e.printStackTrace();
        }
    }

    public static void setFieldObject(Class clazz,String filedName,Object obj,Object fieldValue ){
        try {
            Field field = clazz.getDeclaredField(filedName);
            field.setAccessible(true);
            field.set(obj,fieldValue);

        } catch (NoSuchFieldException e) {
            e.printStackTrace();
        } catch (IllegalAccessException e) {
            e.printStackTrace();
        }
    }

    public static void setStaticFieldObject(String class_name,String filedName,Object fieldValue ){
        setFieldObject(class_name,filedName,null,fieldValue);
    }

    public static void setStaticFieldObject(Class clazz,String filedName,Object fieldValue ){
        setFieldObject(clazz,filedName,null,fieldValue);
    }

}
