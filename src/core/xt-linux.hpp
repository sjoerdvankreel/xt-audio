#ifndef _XT_LINUX_HPP
#define _XT_LINUX_HPP
#ifdef __linux__

#include "xt-private.hpp"
#include <pthread.h>

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

 // ---- linux ----

extern const XtService* XtiServiceAlsa;
extern const XtService* XtiServiceJack;
extern const XtService* XtiServicePulse;

void XtlInitJack();
void XtlInitAlsa();
void XtlTerminateJack();
void XtlTerminateAlsa();
XtCause XtlPosixErrorToCause(XtFault fault);

struct XtlMutex {
  pthread_mutex_t m;
  XtlMutex(const XtlMutex&) = delete;
  XtlMutex& operator=(const XtlMutex&) = delete;
  ~XtlMutex() { XT_ASSERT(pthread_mutex_destroy(&m) == 0); }
  XtlMutex(): m() { XT_ASSERT(pthread_mutex_init(&m, nullptr) == 0); }
};

struct XtlCondition {
  pthread_cond_t cv;
  XtlCondition(const XtlCondition&) = delete;
  XtlCondition& operator=(const XtlCondition&) = delete;
  ~XtlCondition() { XT_ASSERT(pthread_cond_destroy(&cv) == 0); }
  XtlCondition(): cv() { XT_ASSERT(pthread_cond_init(&cv, nullptr) == 0); }
};

struct XtlLinuxStream: public XtManagedStream {
  XtlMutex lock;
  XtStreamState state;
  XtlCondition respondCv;
  XtlCondition controlCv;
  XT_IMPLEMENT_STREAM_CONTROL();

  XtlLinuxStream(bool secondary);
  ~XtlLinuxStream();
  virtual void RequestStop();
  bool VerifyStreamCallback(int error, const char* file, int line, const char* func, const char* expr);
};

#endif // __linux__
#endif // _XT_LINUX_HPP
