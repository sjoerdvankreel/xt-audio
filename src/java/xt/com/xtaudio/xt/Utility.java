package com.xtaudio.xt;

import com.sun.jna.DefaultTypeMapper;
import com.sun.jna.FromNativeContext;
import com.sun.jna.Library;
import com.sun.jna.Native;
import com.sun.jna.NativeLibrary;
import com.sun.jna.ToNativeContext;
import com.sun.jna.TypeConverter;
import com.xtaudio.xt.CoreEnums.XtCause;
import com.xtaudio.xt.CoreEnums.XtSample;
import com.xtaudio.xt.CoreEnums.XtSetup;
import com.xtaudio.xt.CoreEnums.XtSystem;
import com.xtaudio.xt.CoreStructs.XtErrorInfo;
import java.util.HashMap;
import java.util.Map;

class XtTypeMapper extends DefaultTypeMapper {
    XtTypeMapper() {
        addTypeConverter(XtSetup.class, new EnumConverter<>(XtSetup.class, 0));
        addTypeConverter(XtCause.class, new EnumConverter<>(XtCause.class, 0));
        addTypeConverter(XtSample.class, new EnumConverter<>(XtSample.class, 0));
        addTypeConverter(XtSystem.class, new EnumConverter<>(XtSystem.class, 1));
    }
}

class EnumConverter<E extends Enum<E>> implements TypeConverter {
    final int _base;
    final Class<E> _type;
    EnumConverter(Class<E> type, int base) { _base = base; _type = type; }
    @Override public Class<Integer> nativeType() { return Integer.class; }
    @Override public Object toNative(Object o, ToNativeContext tnc) { return o == null? 0: ((Enum<E>)o).ordinal() + _base; }
    @Override public Object fromNative(Object o, FromNativeContext fnc) { return _type.getEnumConstants()[((int)o) - _base]; }
}

class Utility {
    static final NativeLibrary LIBRARY;
    static {
        System.setProperty("jna.encoding", "UTF-8");
        Map<String, Object> options = new HashMap<>();
        options.put(Library.OPTION_TYPE_MAPPER, new XtTypeMapper());
        LIBRARY = NativeLibrary.getInstance("xt-core", options);
        Native.register(LIBRARY);
    }

    static native String XtPrintErrorInfoToString(XtErrorInfo info);
    static void handleError(long error) { if(error != 0) throw new XtException(error); }
    static <T> T handleError(long error, T result) { if(error != 0) throw new XtException(error); return result; }
}