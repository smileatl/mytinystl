#ifndef MYTINYSTL_ALLOCATOR_H_
#define MYTINYSTL_ALLOCATOR_H_

// 这个头文件包含一个模板类allocator，用于管理内存的分配、释放，对象的构造、析构

#include "construct.h"
#include "util.h"

namespace mystl {

// 模板类：allocator
// 模板函数代表数据类型
template <class T>
class allocator {
public:
    typedef T value_type;
    typedef T* pointer;
    typedef const T* const_pointer;
    typedef T& reference;
    typedef const T& const_reference;
    typedef size_t size_type;
    typedef ptrdiff_t difference_type;

public:
    static T* allocate();
    static T* allocate(size_type);

    static void deallocate(T* ptr);
    static void deallocate(T* ptr, size_type n);

    static void construct(T* ptr);
    static void construct(T* prt, const T& value);
    static void construct(T* ptr, T&& value);

    template <class... Args>
    static void construct(T* ptr, Args&&... args);

    static void destroy(T* ptr);
    static void destroy(T* first, T* last);
};

// 这是一个 C++ 中的模板函数 `allocate` 的定义，它是一个成员函数，属于
// `allocator` 类模板。 这个函数的作用是在堆上分配一块内存，用于存储类型 `T`
// 的对象，并返回指向该内存块的指针。 这个函数使用了 C++ 中的内存分配函数
// `operator new` 来分配内存，然后使用 `static_cast`
// 进行类型转换，将分配的内存块转换成类型 `T*` 的指针，最后返回该指针。

// 需要注意的是，这个函数只是分配了内存，但并没有调用 `T`
// 的构造函数来初始化对象。 因此，在使用这个函数分配内存后，还需要手动调用 `T`
// 的构造函数来初始化对象。同时，在使用完对象后，还需要手动调用 `T`
// 的析构函数来释放资源。
template <class T>
T* allocator<T>::allocate() {
    return static_cast<T*>(::operator new(sizeof(T)));
}

template <class T>
T* allocator<T>::allocate(size_type n) {
    if (n == 0) {
        return nullptr;
    }
    return static_cast<T*>(::operator new(n * sizeof(T)));
}

// 这是一个 C++ 中的模板函数，它属于 `allocator` 类的成员函数。
// 这个函数的作用是释放一块内存，它接受两个参数：一个是指向要释放的内存块的指针，另一个是内存块的大小。

// 在函数的实现中，首先判断要释放的指针是否为 `nullptr`，如果是则直接返回，
// 否则调用全局的 `operator delete` 函数释放内存。
// 这个函数是 C++ 中的一个内置函数，用于释放由 `operator new` 分配的内存。
// 在这个函数中，我们没有使用第二个参数 `size`，因为在 C++ 中，`delete`
// 操作符会自动获取要释放内存块的大小。
template <class T>
void allocator<T>::deallocate(T* ptr) {
    if (ptr == nullptr) {
        return;
    }
    ::operator delete(ptr);
}

template <class T>
void allocator<T>::deallocate(T* ptr, size_type /*size*/) {
    if (ptr == nullptr)
        return;
    ::operator delete(ptr);
}

template <class T>
void allocator<T>::construct(T* ptr) {
    mystl::construct(ptr);
}

template <class T>
void allocator<T>::construct(T* ptr, const T& value) {
    mystl::construct(ptr, value);
}

// 这是一个 C++ 模板函数，用于在指定位置构造一个对象。该函数的模板参数是类型
// `T`，表示要构造的对象的类型。函数的参数包括一个指向构造位置的指针
// `ptr`，以及一个右值引用 `value`，表示要构造的对象的值。

// 在函数体内，调用了另一个函数
// `mystl::construct`，该函数用于在指定位置构造一个对象。它的第一个参数是指向构造位置的指针
// `ptr`，第二个参数是右值引用 `value`。 函数体内使用了 `mystl::move`
// 函数，将右值引用 `value` 转换为一个右值，以保证在构造对象时使用移动语义。
// 这段代码是一个自定义的内存分配器中的一个函数，用于在分配的内存上构造对象。
template <class T>
void allocator<T>::construct(T* ptr, T&& value) {
    mystl::construct(ptr, mystl::move(value));
}

template <class T>
template <class... Args>
void allocator<T>::construct(T* ptr, Args&&... args) {
    mystl::construct(ptr, mystl::forward<Args>(args)...);
}

template <class T>
void allocator<T>::destroy(T* ptr) {
    mystl::destroy(ptr);
}

template <class T>
void allocator<T>::destroy(T* first, T* last) {
    mystl::destroy(first, last);
}

}  // namespace mystl

#endif  // !MYTINYSTL_ALLOCATOR_H_