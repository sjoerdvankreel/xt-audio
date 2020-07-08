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
using System;

namespace Xt
{
    public sealed class XtException : Exception
    {
        private readonly ulong error;
        internal XtException(ulong error) => this.error = error;

        public ulong GetError() => error;
        public override string ToString() => XtPrint.ErrorToString(GetError());

        public static int GetFault(ulong error) => XtNative.XtErrorGetFault(error);
        public static XtCause GetCause(ulong error) => XtNative.XtErrorGetCause(error);
        public static XtSystem GetSystem(ulong error) => XtNative.XtErrorGetSystem(error);
        public static string GetText(ulong error) => XtNative.StringFromUtf8(XtNative.XtErrorGetText(error));
    }
}