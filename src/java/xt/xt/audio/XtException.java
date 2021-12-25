package xt.audio;

public final class XtException extends RuntimeException {
    private final long _error;
    public long getError() { return _error; }
    XtException(long error) { _error = error; }
    @Override public String getMessage() { return XtAudio.getErrorInfo(getError()).toString(); }
}