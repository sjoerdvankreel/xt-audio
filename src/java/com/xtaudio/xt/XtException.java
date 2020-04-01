package com.xtaudio.xt;

/* Copyright (C) 2015-2020 Sjoerd van Kreel.
 *
 * This file is part of XT-Audio.
 *
 * XT-Audio is free software: you can redistribute it and/or modify it under the 
 * terms of the GNU Lesser General Public License as published by the Free Software
 * Foundation, either version 3 of the License, or (at your option) any later version.
 *
 * XT-Audio is distributed in the hope that it will be useful, but WITHOUT ANY 
 * WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR
 * A PARTICULAR PURPOSE. See the GNU Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public License
 * along with XT-Audio. If not, see<http://www.gnu.org/licenses/>.
 */
public final class XtException extends RuntimeException {

    private final long error;

    XtException(long error) {
        this.error = error;
    }

    public long getError() {
        return error;
    }

    @Override
    public String toString() {
        return XtPrint.errorToString(getError());
    }

    public static XtCause getCause(long error) {
        return XtCause.class.getEnumConstants()[XtNative.XtErrorGetCause(error)];
    }

    public static XtSystem getSystem(long error) {
        return XtSystem.class.getEnumConstants()[XtNative.XtErrorGetSystem(error) - 1];
    }

    public static int getFault(long error) {
        return XtNative.XtErrorGetFault(error);
    }

    public static String getText(long error) {
        return XtNative.XtErrorGetText(error);
    }
}
