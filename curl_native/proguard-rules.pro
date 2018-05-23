# Add project specific ProGuard rules here.
# You can control the set of applied configuration files using the
# proguardFiles setting in build.gradle.
#
# For more details, see
#   http://developer.android.com/guide/developing/tools/proguard.html

# If your project uses WebView with JS, uncomment the following
# and specify the fully qualified class name to the JavaScript interface
# class:
#-keepclassmembers class fqcn.of.javascript.interface.for.webview {
#   public *;
#}

# Uncomment this to preserve the line number information for
# debugging stack traces.
#-keepattributes SourceFile,LineNumberTable

# If you keep the line number information, uncomment this to
# hide the original source file name.
#-renamesourcefileattribute SourceFile


-renamesourcefileattribute SourceFile
-dontusemixedcaseclassnames
-keeppackagenames
-dontskipnonpubliclibraryclasses
-dontskipnonpubliclibraryclassmembers
-ignorewarnings
-dontpreverify
-verbose
-keepattributes SourceFile,LineNumberTable
-repackageclasses ''
-keepattributes Exceptions,InnerClasses,Signature,Deprecated,SourceFile,LineNumberTable,*Annotation*,EnclosingMethod
-optimizations !code/simplification/arithmetic,!field/*,!class/merging/*
-dontoptimize

-keep public class * extends android.app.Activity
-keep public class * extends android.app.Application
-keep public class * extends android.app.Service
-keep public class * extends android.content.BroadcastReceiver
-keep public class * extends android.content.ContentProvider

-dontwarn android.support.v4.**,**CompatHoneycomb,**CompatCreatorHoneycombMR2,org.apache.**

-keepclasseswithmembernames class * {
    native <methods>;
}

-keep class com.github.yutianzuo.curl_native.HttpManager{*;}
-keep class com.github.yutianzuo.curl_native.HttpCallback{*;}
-keep class com.github.yutianzuo.curl_native.RequestManager{*;}
-keep class com.github.yutianzuo.curl_native.utils.Misc{*;}