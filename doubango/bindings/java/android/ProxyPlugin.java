/* ----------------------------------------------------------------------------
 * This file was automatically generated by SWIG (http://www.swig.org).
 * Version 1.3.39
 *
 * Do not make changes to this file unless you know what you are doing--modify
 * the SWIG interface file instead.
 * ----------------------------------------------------------------------------- */

package org.doubango.tinyWRAP;

public class ProxyPlugin {
  private long swigCPtr;
  protected boolean swigCMemOwn;

  protected ProxyPlugin(long cPtr, boolean cMemoryOwn) {
    swigCMemOwn = cMemoryOwn;
    swigCPtr = cPtr;
  }

  protected static long getCPtr(ProxyPlugin obj) {
    return (obj == null) ? 0 : obj.swigCPtr;
  }

  protected void finalize() {
    delete();
  }

  public synchronized void delete() {
    if(swigCPtr != 0 && swigCMemOwn) {
      swigCMemOwn = false;
      tinyWRAPJNI.delete_ProxyPlugin(swigCPtr);
    }
    swigCPtr = 0;
  }

  public twrap_proxy_plugin_type_t getType() {
    return twrap_proxy_plugin_type_t.swigToEnum(tinyWRAPJNI.ProxyPlugin_getType(swigCPtr, this));
  }

  public java.math.BigInteger getId() {
    return tinyWRAPJNI.ProxyPlugin_getId(swigCPtr, this);
  }

}
