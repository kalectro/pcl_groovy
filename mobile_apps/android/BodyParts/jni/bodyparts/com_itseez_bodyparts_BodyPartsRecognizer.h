/* DO NOT EDIT THIS FILE - it is machine generated */
#include <jni.h>
/* Header for class com_itseez_bodyparts_BodyPartsRecognizer */

#ifndef _Included_com_itseez_bodyparts_BodyPartsRecognizer
#define _Included_com_itseez_bodyparts_BodyPartsRecognizer
#ifdef __cplusplus
extern "C" {
#endif
/*
 * Class:     com_itseez_bodyparts_BodyPartsRecognizer
 * Method:    cacheIds
 * Signature: ()V
 */
JNIEXPORT void JNICALL Java_com_itseez_bodyparts_BodyPartsRecognizer_cacheIds
  (JNIEnv *, jclass);

/*
 * Class:     com_itseez_bodyparts_BodyPartsRecognizer
 * Method:    create
 * Signature: ([[B)V
 */
JNIEXPORT void JNICALL Java_com_itseez_bodyparts_BodyPartsRecognizer_create
  (JNIEnv *, jobject, jobjectArray);

/*
 * Class:     com_itseez_bodyparts_BodyPartsRecognizer
 * Method:    free
 * Signature: ()V
 */
JNIEXPORT void JNICALL Java_com_itseez_bodyparts_BodyPartsRecognizer_free
  (JNIEnv *, jobject);

/*
 * Class:     com_itseez_bodyparts_BodyPartsRecognizer
 * Method:    recognize
 * Signature: (Lcom/itseez/bodyparts/Cloud;)[B
 */
JNIEXPORT jbyteArray JNICALL Java_com_itseez_bodyparts_BodyPartsRecognizer_recognize
  (JNIEnv *, jobject, jobject);

#ifdef __cplusplus
}
#endif
#endif
