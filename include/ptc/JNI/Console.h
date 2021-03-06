/* DO NOT EDIT THIS FILE - it is machine generated */
#include <jni.h>
/* Header for class ptc_jni_Console */

#ifndef _Included_ptc_jni_Console
#define _Included_ptc_jni_Console
#ifdef __cplusplus
extern "C" {
#endif
/*
 * Class:     ptc_jni_Console
 * Method:    create
 * Signature: ()J
 */
JNIEXPORT jlong JNICALL Java_ptc_jni_Console_create
  (JNIEnv *, jobject);

/*
 * Class:     ptc_jni_Console
 * Method:    destroy
 * Signature: (J)V
 */
JNIEXPORT void JNICALL Java_ptc_jni_Console_destroy
  (JNIEnv *, jobject, jlong);

/*
 * Class:     ptc_jni_Console
 * Method:    configure
 * Signature: (JLjava/lang/String;)V
 */
JNIEXPORT void JNICALL Java_ptc_jni_Console_configure
  (JNIEnv *, jobject, jlong, jstring);

/*
 * Class:     ptc_jni_Console
 * Method:    option
 * Signature: (JLjava/lang/String;)Z
 */
JNIEXPORT jboolean JNICALL Java_ptc_jni_Console_option
  (JNIEnv *, jobject, jlong, jstring);

/*
 * Class:     ptc_jni_Console
 * Method:    modes
 * Signature: (J)[Lptc/Mode;
 */
JNIEXPORT jobjectArray JNICALL Java_ptc_jni_Console_modes
  (JNIEnv *, jobject, jlong);

/*
 * Class:     ptc_jni_Console
 * Method:    open
 * Signature: (JLjava/lang/String;I)V
 */
JNIEXPORT void JNICALL Java_ptc_jni_Console_open__JLjava_lang_String_2I
  (JNIEnv *, jobject, jlong, jstring, jint);

/*
 * Class:     ptc_jni_Console
 * Method:    open
 * Signature: (JLjava/lang/String;Lptc/Format;I)V
 */
JNIEXPORT void JNICALL Java_ptc_jni_Console_open__JLjava_lang_String_2Lptc_Format_2I
  (JNIEnv *, jobject, jlong, jstring, jobject, jint);

/*
 * Class:     ptc_jni_Console
 * Method:    open
 * Signature: (JLjava/lang/String;IILptc/Format;I)V
 */
JNIEXPORT void JNICALL Java_ptc_jni_Console_open__JLjava_lang_String_2IILptc_Format_2I
  (JNIEnv *, jobject, jlong, jstring, jint, jint, jobject, jint);

/*
 * Class:     ptc_jni_Console
 * Method:    open
 * Signature: (JLjava/lang/String;Lptc/Mode;I)V
 */
JNIEXPORT void JNICALL Java_ptc_jni_Console_open__JLjava_lang_String_2Lptc_Mode_2I
  (JNIEnv *, jobject, jlong, jstring, jobject, jint);

/*
 * Class:     ptc_jni_Console
 * Method:    close
 * Signature: (J)V
 */
JNIEXPORT void JNICALL Java_ptc_jni_Console_close
  (JNIEnv *, jobject, jlong);

/*
 * Class:     ptc_jni_Console
 * Method:    flush
 * Signature: (J)V
 */
JNIEXPORT void JNICALL Java_ptc_jni_Console_flush
  (JNIEnv *, jobject, jlong);

/*
 * Class:     ptc_jni_Console
 * Method:    finish
 * Signature: (J)V
 */
JNIEXPORT void JNICALL Java_ptc_jni_Console_finish
  (JNIEnv *, jobject, jlong);

/*
 * Class:     ptc_jni_Console
 * Method:    update
 * Signature: (J)V
 */
JNIEXPORT void JNICALL Java_ptc_jni_Console_update__J
  (JNIEnv *, jobject, jlong);

/*
 * Class:     ptc_jni_Console
 * Method:    update
 * Signature: (JLptc/Area;)V
 */
JNIEXPORT void JNICALL Java_ptc_jni_Console_update__JLptc_Area_2
  (JNIEnv *, jobject, jlong, jobject);

/*
 * Class:     ptc_jni_Console
 * Method:    key
 * Signature: (J)Z
 */
JNIEXPORT jboolean JNICALL Java_ptc_jni_Console_key
  (JNIEnv *, jobject, jlong);

/*
 * Class:     ptc_jni_Console
 * Method:    read
 * Signature: (J)Lptc/Key;
 */
JNIEXPORT jobject JNICALL Java_ptc_jni_Console_read
  (JNIEnv *, jobject, jlong);

/*
 * Class:     ptc_jni_Console
 * Method:    copy
 * Signature: (JLptc/base/Surface;)V
 */
JNIEXPORT void JNICALL Java_ptc_jni_Console_copy__JLptc_base_Surface_2
  (JNIEnv *, jobject, jlong, jobject);

/*
 * Class:     ptc_jni_Console
 * Method:    copy
 * Signature: (JLptc/base/Surface;Lptc/Area;Lptc/Area;)V
 */
JNIEXPORT void JNICALL Java_ptc_jni_Console_copy__JLptc_base_Surface_2Lptc_Area_2Lptc_Area_2
  (JNIEnv *, jobject, jlong, jobject, jobject, jobject);

/*
 * Class:     ptc_jni_Console
 * Method:    lock
 * Signature: (J)Ljava/lang/Object;
 */
JNIEXPORT jobject JNICALL Java_ptc_jni_Console_lock
  (JNIEnv *, jobject, jlong);

/*
 * Class:     ptc_jni_Console
 * Method:    unlock
 * Signature: (J)V
 */
JNIEXPORT void JNICALL Java_ptc_jni_Console_unlock
  (JNIEnv *, jobject, jlong);

/*
 * Class:     ptc_jni_Console
 * Method:    load
 * Signature: (JLjava/lang/Object;IIILptc/Format;Lptc/Palette;)V
 */
JNIEXPORT void JNICALL Java_ptc_jni_Console_load__JLjava_lang_Object_2IIILptc_Format_2Lptc_Palette_2
  (JNIEnv *, jobject, jlong, jobject, jint, jint, jint, jobject, jobject);

/*
 * Class:     ptc_jni_Console
 * Method:    load
 * Signature: (JLjava/lang/Object;IIILptc/Format;Lptc/Palette;Lptc/Area;Lptc/Area;)V
 */
JNIEXPORT void JNICALL Java_ptc_jni_Console_load__JLjava_lang_Object_2IIILptc_Format_2Lptc_Palette_2Lptc_Area_2Lptc_Area_2
  (JNIEnv *, jobject, jlong, jobject, jint, jint, jint, jobject, jobject, jobject, jobject);

/*
 * Class:     ptc_jni_Console
 * Method:    save
 * Signature: (JLjava/lang/Object;IIILptc/Format;Lptc/Palette;)V
 */
JNIEXPORT void JNICALL Java_ptc_jni_Console_save__JLjava_lang_Object_2IIILptc_Format_2Lptc_Palette_2
  (JNIEnv *, jobject, jlong, jobject, jint, jint, jint, jobject, jobject);

/*
 * Class:     ptc_jni_Console
 * Method:    save
 * Signature: (JLjava/lang/Object;IIILptc/Format;Lptc/Palette;Lptc/Area;Lptc/Area;)V
 */
JNIEXPORT void JNICALL Java_ptc_jni_Console_save__JLjava_lang_Object_2IIILptc_Format_2Lptc_Palette_2Lptc_Area_2Lptc_Area_2
  (JNIEnv *, jobject, jlong, jobject, jint, jint, jint, jobject, jobject, jobject, jobject);

/*
 * Class:     ptc_jni_Console
 * Method:    clear
 * Signature: (J)V
 */
JNIEXPORT void JNICALL Java_ptc_jni_Console_clear__J
  (JNIEnv *, jobject, jlong);

/*
 * Class:     ptc_jni_Console
 * Method:    clear
 * Signature: (JLptc/Color;)V
 */
JNIEXPORT void JNICALL Java_ptc_jni_Console_clear__JLptc_Color_2
  (JNIEnv *, jobject, jlong, jobject);

/*
 * Class:     ptc_jni_Console
 * Method:    clear
 * Signature: (JLptc/Color;Lptc/Area;)V
 */
JNIEXPORT void JNICALL Java_ptc_jni_Console_clear__JLptc_Color_2Lptc_Area_2
  (JNIEnv *, jobject, jlong, jobject, jobject);

/*
 * Class:     ptc_jni_Console
 * Method:    palette
 * Signature: (JLptc/Palette;)V
 */
JNIEXPORT void JNICALL Java_ptc_jni_Console_palette__JLptc_Palette_2
  (JNIEnv *, jobject, jlong, jobject);

/*
 * Class:     ptc_jni_Console
 * Method:    palette
 * Signature: (J)Lptc/Palette;
 */
JNIEXPORT jobject JNICALL Java_ptc_jni_Console_palette__J
  (JNIEnv *, jobject, jlong);

/*
 * Class:     ptc_jni_Console
 * Method:    clip
 * Signature: (JLptc/Area;)V
 */
JNIEXPORT void JNICALL Java_ptc_jni_Console_clip__JLptc_Area_2
  (JNIEnv *, jobject, jlong, jobject);

/*
 * Class:     ptc_jni_Console
 * Method:    width
 * Signature: (J)I
 */
JNIEXPORT jint JNICALL Java_ptc_jni_Console_width
  (JNIEnv *, jobject, jlong);

/*
 * Class:     ptc_jni_Console
 * Method:    height
 * Signature: (J)I
 */
JNIEXPORT jint JNICALL Java_ptc_jni_Console_height
  (JNIEnv *, jobject, jlong);

/*
 * Class:     ptc_jni_Console
 * Method:    pages
 * Signature: (J)I
 */
JNIEXPORT jint JNICALL Java_ptc_jni_Console_pages
  (JNIEnv *, jobject, jlong);

/*
 * Class:     ptc_jni_Console
 * Method:    pitch
 * Signature: (J)I
 */
JNIEXPORT jint JNICALL Java_ptc_jni_Console_pitch
  (JNIEnv *, jobject, jlong);

/*
 * Class:     ptc_jni_Console
 * Method:    area
 * Signature: (J)Lptc/Area;
 */
JNIEXPORT jobject JNICALL Java_ptc_jni_Console_area
  (JNIEnv *, jobject, jlong);

/*
 * Class:     ptc_jni_Console
 * Method:    clip
 * Signature: (J)Lptc/Area;
 */
JNIEXPORT jobject JNICALL Java_ptc_jni_Console_clip__J
  (JNIEnv *, jobject, jlong);

/*
 * Class:     ptc_jni_Console
 * Method:    format
 * Signature: (J)Lptc/Format;
 */
JNIEXPORT jobject JNICALL Java_ptc_jni_Console_format
  (JNIEnv *, jobject, jlong);

/*
 * Class:     ptc_jni_Console
 * Method:    name
 * Signature: (J)Ljava/lang/String;
 */
JNIEXPORT jstring JNICALL Java_ptc_jni_Console_name
  (JNIEnv *, jobject, jlong);

/*
 * Class:     ptc_jni_Console
 * Method:    title
 * Signature: (J)Ljava/lang/String;
 */
JNIEXPORT jstring JNICALL Java_ptc_jni_Console_title
  (JNIEnv *, jobject, jlong);

/*
 * Class:     ptc_jni_Console
 * Method:    information
 * Signature: (J)Ljava/lang/String;
 */
JNIEXPORT jstring JNICALL Java_ptc_jni_Console_information
  (JNIEnv *, jobject, jlong);

#ifdef __cplusplus
}
#endif
#endif
