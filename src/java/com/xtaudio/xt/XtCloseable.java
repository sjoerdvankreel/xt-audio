package com.xtaudio.xt;

public interface XtCloseable extends AutoCloseable {

    @Override
    public void close();
}