#ifndef CgssWrapper_H
#define CgssWrapper_H
#include <string>
#include <memory>
#include "rbAdapter.h"

namespace cgss {
    class Sprite;
}

template <class T, template <class> class ContainerPtr>
class CgssWrapper {
public:
    CgssWrapper() = default;
    virtual ~CgssWrapper() = default;

    template <class ... Args>
    void init(Args&& ... args) {
        data = ContainerPtr<T>(new T(std::forward<Args>(args)...));
        setup();
    }

    void steal(ContainerPtr<T> container) {
        data = std::move(container);
        setup();
    }

    template <class ... Args>
    static T create(Args&& ... args) {
        return T::create(std::forward<Args>(args)...);
    }

    T* instance() { return data.get(); }
    const T* instance() const { return data.get(); }

    T* operator->() {
        protect();
        return instance();
    }

    const T* operator->() const {
        protect();
        return instance();
    }

    bool operator==(std::nullptr_t nptr) const {
        return data.get() == nullptr;
    }

    bool operator!=(std::nullptr_t nptr) const {
        return data.get() != nullptr;
    }

    template <template <class> class ContPtr>
    bool operator==(const CgssWrapper<T, ContPtr>& wrapper) const {
        return *data == *wrapper.data;
    }

    template <template <class> class ContPtr>
    bool operator!=(const CgssWrapper<T, ContPtr>& wrapper) const {
        return *data != *wrapper.data;
    }

protected:
    virtual void setup() {}

private:
	void protect() const {
		if(data == nullptr) { 
			auto errorMessage = std::string { "Disposed "}; 
			errorMessage += cgss::meta::Log<T>::classname;
			errorMessage += ".";
			rb_raise(rb_eRGSSError, "%s", errorMessage.c_str());
			throw std::runtime_error(errorMessage);
		}
	}

    ContainerPtr<T> data;
};


template <class T>
using CgssInstance = CgssWrapper<T, std::unique_ptr>;

template <class T>
using CgssMultiInstance = CgssWrapper<T, std::shared_ptr>;

#endif