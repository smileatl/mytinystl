#ifndef MYTINYSTL_CONSTRUCT_H_
#define MYTINYSTL_CONSTRUCT_H_

// 这个头文件包含两个函数
//  construct：负责对象的构造
//  destroy：负责对象的析构

#include <new>

#include "iterator.h"
#include "type_traits.h"

/* 这段代码是用于禁用 Visual Studio 编译器的一个警告，警告代码为
 * 4100，表示未使用的形参。`#ifdef _MSC_VER` 判断是否是 Visual Studio
 * 编译器，`#pragma warning(push)` 将当前警告状态压入堆栈，`#pragma
 * warning(disable:4100)` 禁用警告 4100，最后通过 `#endif`
 * 结束条件编译。这段代码的作用是为了避免在编译时出现过多的警告信息，使得开发者能够更加专注于代码的实现。
 */
#ifdef _MSC_VER
#pragma warning(push)
#pragma warning(disable : 4100)  // unused parameter
#endif                           // _MSC_VER

namespace mystl {

// construct 构造函数

template <class Ty>
void construct(Ty* ptr) {
    ::new ((void*)ptr) Ty();
}

template <class Ty1, class Ty2>
void construct(Ty1* ptr, const Ty2& value) {
    ::new ((void*)ptr) Ty1(value);
}

template <class Ty, class... Args>
void construct(Ty* ptr, Args&&... args) {
    ::new ((void*)ptr) Ty(mystl::forward<Args>(args)...);
}

// destroy将对象析构
template <class Ty>
void destroy_one(Ty*, std::true_type) {}

template <class Ty>
void destroy_one(Ty* pointer, std::false_type) {
    if (pointer != nullptr) {
        pointer->~Ty();
    }
}

template <class ForwardIter>
void destroy_cat(ForwardIter, ForwardIter, std::true_type) {}

/* 这段代码是一个模板函数，名为 `destroy_cat`，接受两个迭代器 `first` 和
`last`，以及一个类型为 `std::false_type` 的参数。它的作用是销毁 `[first, last)`
区间内的对象。

这里的 `std::false_type`
是一个类型标记，用于判断迭代器的类型。如果迭代器是原生指针类型，那么
`std::false_type` 会被传递给函数，否则会被传递 `std::true_type`。

在函数内部，我们使用了一个循环来遍历 `[first, last)` 区间内的所有元素，并调用
`destroy` 函数销毁它们。注意，这里使用了 `&*first`
来获取迭代器指向的对象的地址，这是因为 `destroy`
函数接受的是一个指针类型的参数。 */
template <class ForwardIter>
void destroy_cat(ForwardIter first, ForwardIter last, std::false_type) {
    for (; first != last; ++first) {
        destroy(&*first);
    }
}

template <class Ty>
void destroy(Ty* pointer) {
    // is_trivially_constructible用于检查一个类型是否满足平凡构造的要求。如果一个类型是平凡构造的，那么它的默认构造函数是平凡的，即不需要执行任何操作就能成功构造一个对象。
    // 也不需要任何操作就能析构一个对象
    // 如果一个类型不是平凡构造的，那么它的默认构造函数可能会执行一些操作，例如初始化某些成员变量。
    destroy_one(pointer, std::is_trivially_constructible<Ty>{});
}

template <class ForwardIter>
void destroy(ForwardIter first, ForwardIter last) {
    destroy_cat(first, last,
                std::is_trivially_default_constructible<
                    typename iterator_traits<ForwardIter>::value_type>{});
}
}  // namespace mystl

/* 这是针对 Microsoft Visual C++ 编译器的预处理指令。`#ifdef` 检查是否定义了
 * `_MSC_VER` 宏，如果定义了，则执行 `#pragma
 * warning(pop)`，该指令用于取消之前的 `#pragma
 * warning(push)`，以便恢复先前的警告设置。这是为了确保编译器在不同的代码段中具有相同的警告设置。
 */
#ifdef _MSC_VER
#pragma warning(pop)
#endif  // _MSC_VER

#endif  // !MYTINYSTL_CONSTRUCT_H_