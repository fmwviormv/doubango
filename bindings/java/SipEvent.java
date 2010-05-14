/* ----------------------------------------------------------------------------
 * This file was automatically generated by SWIG (http://www.swig.org).
 * Version 1.3.39
 *
 * Do not make changes to this file unless you know what you are doing--modify
 * the SWIG interface file instead.
 * ----------------------------------------------------------------------------- */

package org.doubango.tinyWRAP;

public class SipEvent {
  private long swigCPtr;
  protected boolean swigCMemOwn;

  protected SipEvent(long cPtr, boolean cMemoryOwn) {
    swigCMemOwn = cMemoryOwn;
    swigCPtr = cPtr;
  }

  protected static long getCPtr(SipEvent obj) {
    return (obj == null) ? 0 : obj.swigCPtr;
  }

  protected void finalize() {
    delete();
  }

  public synchronized void delete() {
    if(swigCPtr != 0 && swigCMemOwn) {
      swigCMemOwn = false;
      tinyWRAPJNI.delete_SipEvent(swigCPtr);
    }
    swigCPtr = 0;
  }

  public SipEvent() {
    this(tinyWRAPJNI.new_SipEvent__SWIG_0(), true);
  }

  public SipEvent(tsip_event_type_t type, short code, String phrase) {
    this(tinyWRAPJNI.new_SipEvent__SWIG_1(type.swigValue(), code, phrase), true);
  }

  public short getCode() {
    return tinyWRAPJNI.SipEvent_getCode(swigCPtr, this);
  }

  public String getPhrase() {
    return tinyWRAPJNI.SipEvent_getPhrase(swigCPtr, this);
  }

  public SipSession getBaseSession() {
    long cPtr = tinyWRAPJNI.SipEvent_getBaseSession(swigCPtr, this);
    return (cPtr == 0) ? null : new SipSession(cPtr, false);
  }

}
