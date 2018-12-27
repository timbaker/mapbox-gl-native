#include <mbgl/util/thread_local.hpp>

#include <mbgl/renderer/backend_scope.hpp>
#include <mbgl/util/logging.hpp>
#include <mbgl/util/run_loop.hpp>

#include <stdexcept>
#include <cassert>

#ifndef WIN32_LEAN_AND_MEAN
#define WIN32_LEAN_AND_MEAN 1
#endif
#include <windows.h>

namespace mbgl {
namespace util {

template <class T>
class ThreadLocal<T>::Impl {
public:
	DWORD dwTlsIndex;
};

template <class T>
ThreadLocal<T>::ThreadLocal() : impl(std::make_unique<Impl>()) {
	if ((impl->dwTlsIndex = TlsAlloc()) == TLS_OUT_OF_INDEXES) {
        throw std::runtime_error("Failed to init local storage key.");
    }
}

template <class T>
ThreadLocal<T>::~ThreadLocal() {
    // ThreadLocal will not take ownership
    // of the pointer it is managing. The pointer
    // needs to be explicitly cleared before we
    // destroy this object.
    assert(!get());

    if (TlsFree(impl->dwTlsIndex) == FALSE) {
        Log::Error(Event::General, "Failed to delete local storage key.");
        assert(false);
    }
}

template <class T>
T* ThreadLocal<T>::get() {
    auto* ret = reinterpret_cast<T*>(TlsGetValue(impl->dwTlsIndex));
    if (!ret) {
        return nullptr;
    }

    return ret;
}

template <class T>
void ThreadLocal<T>::set(T* ptr) {
    if (TlsSetValue(impl->dwTlsIndex, ptr) == FALSE) {
        throw std::runtime_error("Failed to set local storage.");
    }
}

template class ThreadLocal<BackendScope>;
template class ThreadLocal<Scheduler>;
template class ThreadLocal<int>; // For unit tests

} // namespace util
} // namespace mbgl
