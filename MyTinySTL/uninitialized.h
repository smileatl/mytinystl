#ifndef MYTINYSTL_UNINITIALIZED_H_
#define MYTINYSTL_UNINITIALIZED_H_

// 这个头文件用于对未初始化空间构造元素

#include "algobase.h"
#include "construct.h"
#include "iterator.h"
#include "type_traits.h"
#include "util.h"

namespace mystl {
/*******************************/
// uninitialized_copy
// 把 [first, last) 上的内容复制到以 result 为起始处的空间，返回复制结束的位置
/********************************/
template <class InputIter, class ForwardIter>
ForwardIter unchecked_uninit_copy(InputIter first,
                                  InputIter last,
                                  ForwardIter result,
                                  std::true_type) {
    return mystl::copy(first, last, result);
}

/* 这是一个 C++
模板函数，用于将一个迭代器范围内的元素拷贝到另一个迭代器范围内，并在拷贝过程中进行构造操作。
其中，`InputIter` 是输入迭代器类型，`ForwardIter` 是前向迭代器类型。
函数的第一个参数 `first` 和第二个参数 `last`
分别表示输入迭代器范围的起始和终止位置，第三个参数 `result`
表示前向迭代器范围的起始位置。 函数使用了一个 `std::false_type`
参数来指示是否需要进行 "uninitialized"
操作，这个参数在函数内部没有被使用到，可能是为了兼容其他代码而添加的。

函数内部使用了一个 `try-catch` 块来处理构造过程中可能抛出的异常。
在正常情况下，函数会依次将输入迭代器范围内的元素拷贝到前向迭代器范围内，并调用
`mystl::construct` 函数进行构造操作。 如果在构造过程中出现异常，函数会立即进入
`catch` 块，然后从后往前遍历已经构造好的元素，并调用 `mystl::destroy`
函数进行析构操作。最后，函数返回前向迭代器范围的终止位置。 */
template <class InputIter, class ForwardIter>
ForwardIter unchecked_uninit_copy(InputIter first,
                                  InputIter last,
                                  ForwardIter result,
                                  std::false_type) {
    auto cur = result;
    try {
        for (; first != last; ++first, ++cur) {
            mystl::construct(&*cur, *first);
        }
    } catch (...) {
        for (; result != cur; --cur) {
            mystl::destroy(&*cur);
        }
    }
    return cur;
}

template <class InputIter, class ForwardIter>
ForwardIter uninitialized_copy(InputIter first,
                               InputIter last,
                               ForwardIter result) {
    return mystl::unchecked_uninit_copy(
        first, last, result,
        std::is_trivially_copy_assignable<
            typename iterator_traits<ForwardIter>::value_type>{});
}

/**********************************/
// uninitialized_copy_n
// 把 [first, first + n) 上的内容复制到以 result
// 为起始处的空间，返回复制结束的位置
/**********************************/
template <class InputIter, class Size, class ForwardIter>
ForwardIter unchecked_uninit_copy_n(InputIter first,
                                    Size n,
                                    ForwardIter result,
                                    std::true_type) {
    // 返回输出迭代器
    return mystl::copy_n(first, n, result).second;
}

/* 这段代码实现了一个未初始化的拷贝函数
`unchecked_uninit_copy_n`，它将输入迭代器 `first` 开始的 `n`
个元素拷贝到输出迭代器 `result` 开始的位置，返回拷贝结束后输出迭代器的位置。

具体来说，这个函数使用了两个模板参数 `InputIter` 和
`Size`，分别表示输入迭代器和拷贝元素的数量。还有一个模板参数
`ForwardIter`，表示输出迭代器的类型。这个函数还使用了一个 SFINAE
技巧，即通过一个 `std::false_type` 类型作为函数参数来避免函数模板的重载冲突。

在函数体内，首先定义了一个迭代器 `cur`，用于记录当前输出位置。然后使用一个
`try-catch`
块来进行拷贝操作，如果在拷贝过程中发生异常，则需要对已经拷贝成功的元素进行析构操作。最后返回输出迭代器的位置。

需要注意的是，这个函数并没有对输出迭代器进行初始化，所以使用时需要确保输出迭代器指向的内存空间已经被分配。另外，这个函数使用了
`mystl::construct` 和 `mystl::destroy`
函数来构造和析构元素，这两个函数的作用类似于 `std::construct` 和 `std::destroy`
函数，用于在指定的内存位置上构造和析构对象。 */
template <class InputIter, class Size, class ForwardIter>
ForwardIter unchecked_uninit_copy_n(InputIter first,
                                    Size n,
                                    ForwardIter result,
                                    std::false_type) {
    auto cur = result;
    try {
        for (; n > 0; --n, ++cur, ++first) {
            mystl::construct(&*cur, *first);
        }
    } catch (...) {
        for (; result != cur; --cur) {
            mystl::destroy(&*cur);
        }
    }
    return cur;
}

template <class InputIter, class Size, class ForwardIter>
ForwardIter uninitialized_copy_n(InputIter first, Size n, ForwardIter result) {
    return mystl::unchecked_uninit_copy_n(
        first, n, result,
        std::is_trivially_copy_assignable<
            typename iterator_traits<InputIter>::value_type>{});
}

/**************************************/
// uninitialized_fill
// 在 [first, last) 区间内填充元素值
/**************************************/
template <class ForwardIter, class T>
void unchecked_uninit_fill(ForwardIter first,
                           ForwardIter last,
                           const T& value,
                           std::true_type) {
    mystl::fill(first, last, value);
}

template <class ForwardIter, class T>
void unchecked_uninit_fill(ForwardIter first,
                           ForwardIter last,
                           const T& value,
                           std::false_type) {
    auto cur = first;
    try {
        for (; cur != last; ++cur) {
            mystl::construct(&*cur, value);
        }
    } catch (...) {
        for (; first != cur; ++first) {
            mystl::destroy(&*first);
        }
    }
}

template <class ForwardIter, class T>
void uninitialized_fill(ForwardIter first, ForwardIter last, const T& value) {
    mystl::unchecked_uninit_fill(
        first, last, value,
        std::is_trivially_copy_assignable<
            typename iterator_traits<ForwardIter>::value_type>{});
}

/*******************************/
// uninitialized_fill_n
// 从 first 位置开始，填充 n 个元素值，返回填充结束的位置
/*******************************/
template <class ForwardIter, class Size, class T>
ForwardIter unchecked_uninit_fill_n(ForwardIter first,
                                    Size n,
                                    const T& value,
                                    std::true_type) {
    return mystl::fill_n(first, n, value);
}

/* 这段代码是一个函数模板的实现，其功能是在未初始化的内存区域中填充指定数量的值。具体来说，它接受三个参数：`first`
表示要填充的内存区域的起始位置，`n` 表示要填充的元素个数，`value`
表示要填充的值。

该函数使用了一个名为 `mystl::construct`
的函数模板来在未初始化的内存区域中构造对象。如果构造过程中抛出了异常，函数会调用
`mystl::destroy` 函数来销毁已经构造的对象，并将异常继续抛出。

需要注意的是，该函数模板的第三个参数是一个 `std::false_type`
类型的值，这是一个用于函数重载的技巧。在该模板的另一个重载版本中，第三个参数是一个
`std::true_type` 类型的值，用于处理内置类型的填充。 */
template <class ForwardIter, class Size, class T>
ForwardIter unchecked_uninit_fill_n(ForwardIter first,
                                    Size n,
                                    const T& value,
                                    std::false_type) {
    auto cur = first;
    try {
        for (; n > 0; --n, ++cur) {
            mystl::construct(&*cur, value);
        }
    } catch (...) {
        for (; first != cur; ++first)
            mystl::destroy(&*first);
    }
    return cur;
}

/* 这段代码是一个 C++ 标准库中的函数模板 `uninitialized_fill_n`
的实现。它的作用是在指定的迭代器范围内创建并初始化 `n`
个对象，并返回最后一个被初始化的对象的迭代器。

它接受三个参数：`first` 表示要初始化的第一个对象的迭代器，`n`
表示要初始化的对象数量，`value` 表示要用来初始化对象的值。

这个实现中调用了 `mystl::unchecked_uninit_fill_n`
函数，它的作用是在指定的迭代器范围内创建并初始化 `n`
个对象，并返回最后一个被初始化的对象的迭代器。这个函数还接受一个额外的参数，用于指示对象是否可以使用
`memcpy` 函数进行快速复制初始化。

`std::is_trivially_copy_assignable` 是一个类型特征，用于判断一个类型是否可以通过
`memcpy` 进行快速复制。如果一个类型是可快速复制的，那么就可以使用 `memcpy`
函数进行快速复制初始化，从而提高初始化的效率。 */
template <class ForwardIter, class Size, class T>
ForwardIter uninitialized_fill_n(ForwardIter first, Size n, const T& value) {
    return mystl::unchecked_uninit_fill_n(
        first, n, value,
        std::is_trivially_copy_assignable<
            typename iterator_traits<ForwardIter>::value_type>{});
}

/*******************************************/
// uninitialized_move
// 把[first, last)上的内容移动到以 result 为起始处的空间，返回移动结束的位置
/*******************************************/
template <class InputIter, class ForwardIter>
ForwardIter unchecked_uninit_move(InputIter first,
                                  InputIter last,
                                  ForwardIter result,
                                  std::true_type) {
    return mystl::move(first, last, result);
}

template <class InputIter, class ForwardIter>
ForwardIter unchecked_uninit_move(InputIter first,
                                  InputIter last,
                                  ForwardIter result,
                                  std::false_type) {
    ForwardIter cur = result;
    try {
        for (; first != last; ++first, ++cur) {
            mystl::construct(&*cur, mystl::move(*first));
        }
    } catch (...) {
        mystl::destroy(result, cur);
    }
    return cur;
}

template <class InputIter, class ForwardIter>
ForwardIter uninitialized_move(InputIter first,
                               InputIter last,
                               ForwardIter result) {
    return mystl::unchecked_uninit_move(
        first, last, result,
        std::is_trivially_move_assignable<
            typename iterator_traits<InputIter>::value_type>{});
}

/******************************************/
// uninitialized_move_n
// 把[first, first + n)上的内容移动到以 result
// 为起始处的空间，返回移动结束的位置
/******************************************/
template <class InputIter, class Size, class ForwardIter>
ForwardIter unchecked_uninit_move_n(InputIter first,
                                    Size n,
                                    ForwardIter result,
                                    std::true_type) {
    return mystl::move(first, first + n, result);
}

template <class InputIter, class Size, class ForwardIter>
ForwardIter unchecked_uninit_move_n(InputIter first,
                                    Size n,
                                    ForwardIter result,
                                    std::false_type) {
    auto cur = result;
    try {
        for (; n > 0; --n, ++first, ++cur) {
            mystl::construct(&*cur, mystl::move(*first));
        }
    } catch (...) {
        for (; result != cur; ++result)
            mystl::destroy(&*result);
        throw;  // throw关键字用于抛出异常
    }
    return cur;
}

/* 这是一个函数模板 `uninitialized_move_n`
的实现，它用于将一个范围内的元素移动到另一个范围内，同时在目标范围内构造这些元素的副本。该模板函数的参数分别为输入迭代器
`first`，元素数量 `n`，和输出迭代器 `result`。该函数返回输出迭代器 `result`。

该函数调用了另一个名为 `mystl::unchecked_uninit_move_n`
的函数，该函数的实现细节不在此处展开。
该函数的第四个参数是一个布尔值，用于指示是否可以使用 "trivially assignable/move
assignable" 的方式来进行元素的移动和构造。这里使用了
`std::is_trivially_assignable_move_assignable`
模板来判断是否可以使用这种方式。如果可以，就使用 `std::move`
来进行移动操作；否则，就使用 "copy and destroy" 的方式来进行元素的移动和构造。
*/
template <class InputIter, class Size, class ForwardIter>
ForwardIter uninitialized_move_n(InputIter first, Size n, ForwardIter result) {
    return mystl::unchecked_uninit_move_n(
        first, n, result,
        std::is_trivially_move_assignable<
            typename iterator_traits<InputIter>::value_type>{});
}
}  // namespace mystl

#endif