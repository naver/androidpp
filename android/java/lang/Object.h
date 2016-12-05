/*
 * Copyright (C) 2016 Naver Corp. All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 *  * Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 *  * Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS ``AS IS'' AND ANY
 * EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR
 * PURPOSE ARE DISCLAIMED.  IN NO EVENT SHALL THE COPYRIGHT OWNER OR
 * CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL,
 * EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO,
 * PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR
 * PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY
 * OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
 * OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

#pragma once

#include <java/lang.h>

#include <condition_variable>
#include <mutex>
#include <thread>

namespace Android {

class Lockable {
public:
    void acquireLock()
    {
        m_threadMutex.lock();
    }

    void releaseLock()
    {
        m_threadMutex.unlock();
    }

    void notifyAll()
    {
        m_threadWaiter.notify_all();
    }

    void wait()
    {
        std::unique_lock<std::mutex> lock(m_threadMutex, std::adopt_lock);
        m_threadWaiter.wait(lock);
        lock.release();
    }

    void threadExiting()
    {
        notifyAll();
    }

    Lockable() { }
    virtual ~Lockable() = default;

private:
    std::mutex m_threadMutex;
    std::condition_variable m_threadWaiter;
};

class Lock {
public:
    Lock(Lockable& lockable)
        : m_lockable(lockable)
        , m_stepOut(false)
    {
    }
    Lock(Lockable* lockable)
        : m_lockable(*lockable)
        , m_stepOut(false)
    {
    }
    ~Lock()
    {
        if (!m_stepOut)
            m_lockable.releaseLock();
    }

    operator bool() const
    {
        if (m_stepOut)
            return false;
        m_lockable.acquireLock();
        return true;
    }
    Lock& operator++()
    {
        m_lockable.releaseLock();
        m_stepOut = true;
        return *this;
    }

private:
    Lockable& m_lockable;
    bool m_stepOut;
};

} // namespace Android

namespace java {
namespace lang {

using Object = Android::Lockable;

} // namespace lang
} // namespace java

#define synchronized(lockable) \
    for (Android::Lock l(lockable); l; ++l)

using Object = java::lang::Object;
