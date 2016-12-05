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

#include <functional>
#include <memory>
#include <vector>

namespace std {

template<typename>
class FunctionTraits;

template<typename>
class FunctionResult;

template<typename R, typename C, typename... P>
class FunctionTraits<R (C::*)(P...)> {
public:
    typedef R ResultType;
};

template<typename T>
class FunctionResult {
public:
    FunctionResult() : m_value(0) { }

    template<typename C, typename F, typename... P> inline bool call(C* thisObject, F function, P&&... arguments)
    {
        T value = (thisObject->*function)(arguments...);
        return (value) ? !!(m_value = value) : false;
    }

    T result() const { return m_value; }

private:
    T m_value;
};

template<>
class FunctionResult<void> {
public:
    template<typename C, typename F, typename... P> inline bool call(C* thisObject, F function, P&&... arguments)
    {
        (thisObject->*function)(arguments...);
        return false;
    }

    void result() const { }
};

template<typename C, typename F, typename... P> inline typename FunctionTraits<F>::ResultType propagate(std::vector<C>& items, F function, P&&... arguments)
{
    FunctionResult<FunctionTraits<F>::ResultType> result;
    for (auto& item : items)
        if (result.call(item, function, arguments...))
            break;
    return result.result();
}

template<typename C, typename F, typename... P> inline typename FunctionTraits<F>::ResultType propagate(std::vector<std::shared_ptr<C>>& items, F function, P&&... arguments)
{
    FunctionResult<FunctionTraits<F>::ResultType> result;
    for (auto& item : items)
        if (result.call(item.get(), function, arguments...))
            break;
    return result.result();
}

template<typename C, typename F, typename... P> inline typename FunctionTraits<F>::ResultType propagate(std::vector<std::unique_ptr<C>>& items, F function, P&&... arguments)
{
    FunctionResult<FunctionTraits<F>::ResultType> result;
    for (auto& item : items)
        if (result.call(item.get(), function, arguments...))
            break;
    return result.result();
}

// Safer version of C++11 std::function implementation.
template<class>
class safe_function;

template<class R, class... Args>
class safe_function<R(Args...)> : public std::function<R(Args...)> {
public:
    safe_function() { reset(); }
    safe_function(std::nullptr_t) { reset(); }
    safe_function(const std::function<R(Args...)>& other) : std::function<R(Args...)>(other) { if (!static_cast<bool>(*this)) reset(); }
    safe_function(std::function<R(Args...)>&& other) : std::function<R(Args...)>(other) { if (!static_cast<bool>(*this)) reset(); }
    template<class F>
    safe_function(F f) { reset(f); }

    void reset() { std::function<R(Args...)>(std::move([] (Args...) -> R { return R(); })).swap(*this); }
    template<class F>
    void reset(F f) { std::function<R(Args...)>(std::move(f)).swap(*this); }
};

}
