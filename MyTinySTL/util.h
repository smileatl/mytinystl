#ifndef MYTINYSTL_UTIL_H_
#define MYTINYSTL_UTIL_H_

// 这个文件包含一些通用工具，包括move，forward，swap等函数，以及pair等
#include <cstddef>
#include "type_traits.h"

namespace mystl {
// move
/* 这段代码是 C++11 中的 std::move()
实现，它是一个模板函数，用于将一个左值转换为右值引用。
它的实现方式是使用了类型推导和引用折叠的特性，其中
std::remove_reference<T>::type 用于移除 T
类型的引用，然后将移除引用后的类型转换为右值引用。

这个函数的作用是将一个左值强制转换为右值引用，并且该操作是 noexcept
的，因此可以在移动语义中使用。
在移动语义中，将资源从一个对象移动到另一个对象，可以避免不必要的内存分配和释放，从而提高程序的性能。使用
std::move() 可以将左值转换为右值引用，以便在移动语义中使用。 */
template <class T>
typename std::remove_reference<T>::type&& move(T&& arg) noexcept {
    return static_cast<typename std::remove_reference<T>::type&&>(arg);
}

// forward
/* 这是一个完美转发的实现，它可以在函数模板中将参数原封不动地转发给另一个函数，并保留参数的值类别（左值或右值）。
在实现中，使用了模板元编程中的 `remove_reference` 来去除参数类型的引用，然后使用
`static_cast` 将参数转换为右值引用。这个实现是 C++11
标准引入的，可以在模板函数中使用。 */
template <class T>
T&& forward(typename std::remove_reference<T>::type& arg) noexcept {
    return static_cast<T&&>(arg);
}

/* 这是一个完美转发（perfect
forwarding）的实现，它可以在函数模板中将参数原封不动地转发给另一个函数，并保留参数的值类别（左值或右值）。
在实现中，使用了模板元编程中的 `remove_reference` 来去除参数类型的引用，然后使用
`static_cast` 将参数转换为右值引用。 同时，使用 `static_assert`
对参数的引用类型进行判断，如果是左值引用，则会产生编译期错误。

这个实现是 C++11
标准引入的，可以在模板函数中使用。它可以避免一些常见的编程错误，例如在函数模板中将参数转发给另一个函数时，如果不使用完美转发，可能会导致参数的值类别被改变，从而影响程序的正确性。
*/
template <class T>
T&& forward(typename std::remove_reference<T>::type&& arg) noexcept {
    static_assert(!std::is_lvalue_reference<T>::value, "bad forward");
    return static_cast<T&&>(arg);
}

// swap
/* 这是一个标准的 `swap` 函数模板，它接受两个参数 `lhs` 和
`rhs`，并交换它们的值。这个函数模板使用了完美转发，因为它将参数 `lhs` 和 `rhs`
原封不动地转发给了 `mystl::move`
函数，保留了它们的值类别（左值或右值）。这样做可以避免参数的值类别被改变，从而影响程序的正确性。

`mystl::move`
是一个移动语义函数，它将一个左值转换为右值引用。这个函数的作用是告诉编译器，我们不再需要这个左值的内容，可以将其移动到另一个对象中，从而避免了不必要的复制操作。在这个函数模板中，我们使用
`mystl::move`
将参数的值移动到临时变量中，然后再将它们移动到另一个参数中，实现了交换的功能。

需要注意的是，这个函数模板只适用于支持移动语义的类型，因为它使用了 `mystl::move`
函数。如果一个类型不支持移动语义，那么这个函数模板将会退化为使用复制操作来交换参数的值。
*/
template <class Tp>
void swap(Tp& lhs, Tp& rhs) {
    auto tmp(mystl::move(lhs));
    lhs = mystl::move(rhs);
    rhs = mystl::move(tmp);
}

/* 这是一个模板函数 `swap_range`，它接受两个迭代器范围 `[first1, last1)` 和
`[first2, ...)`, 并交换它们之间的元素。 它返回迭代器
`first2`，指向交换后的第一个元素。这个函数实现了一种通用的交换算法，可以用于交换两个数组、两个容器或两个
STL 容器的一部分元素。 在实现中，它使用了 `mystl::swap`
函数，这是一个自定义的交换函数，用于交换两个元素的值。 */
template <class ForwardIter1, class ForwardIter2>
ForwardIter2 swap_range(ForwardIter1 first1,
                        ForwardIter1 last1,
                        ForwardIter2 first2) {
    for (; first1 != last1; ++first1, (void)++first2) {
        mystl::swap(*first1, *first2);
    }
    return first2;
}

/* 这是一个函数模板，用于交换两个数组的内容。它接受两个数组的引用作为参数，并使用
`swap_range` 函数将它们的内容交换。 在函数模板中，`Tp` 表示数组元素的类型，`N`
表示数组的大小。这个函数模板可以处理任何类型的数组，只要它们的元素类型相同且大小相等。
*/
template <class Tp, size_t N>
void swap(Tp (&a)[N], Tp (&b)[N]) {
    mystl::swap_range(a, a + N, b);
}

// ---------------------------------------------------------
// pair

// 结构体模板：pair
// 两个模板参数分别表示两个数据的类型
// 用first和second来分别取出第一个数据和第二个数据
template <class Ty1, class Ty2>
struct pair {
    typedef Ty1 first_type;
    typedef Ty2 second_type;

    first_type first;    // 保存第一个数据
    second_type second;  // 保存第二个数据

    // default constructiable
    /* 这是 C++ STL 中 `std::pair` 的默认构造函数的实现。
    它使用了模板默认参数和 SFINAE 技术。
    模板默认参数 `Other1 = Ty1` 和 `Other2 = Ty2` 分别指定了 `pair`
    对象的第一和第二元素的类型，默认情况下它们与 `Ty1` 和 `Ty2` 相同。`typename
    = typename std::enable_if<...>::type` 是一个额外的模板参数，
    它使用了`std::enable_if` 模板，用于在编译期间判断 `Other1` 和 `Other2`
    是否是可默认构造的类型。如果是，那么 `std::enable_if` 的 `type`
    成员将被定义为`void`，从而使得这个构造函数可行。否则，这个构造函数将被禁用，不可用。
    在函数体中，`pair`对象的第一和第二元素将被默认构造。 */
    template <class Other1 = Ty1,
              class Other2 = Ty2,
              typename = typename std::enable_if<
                  std::is_default_constructible<Other1>::value &&
                      std::is_default_constructible<Other2>::value,
                  void>::type>
    constexpr pair() : first(), second() {}

    // implicit constructiable for this type
    /* 这段代码是 C++ 中 `std::pair` 类的构造函数模板的一部分。

    首先，这个构造函数模板有两个模板参数 `U1` 和 `U2`，它们分别表示 `std::pair`
    类型中的第一个元素类型和第二个元素类型。默认情况下，它们的值为 `Ty1` 和
    `Ty2`，也就是 `std::pair` 类型中的模板参数类型。

    然后，这个构造函数模板使用了 `std::enable_if` 和 `std::is_xxx`
    模板来限制模板参数的类型。具体来说，它要求 `U1` 和 `U2`
    类型必须是可复制构造的（即有拷贝构造函数），并且可以隐式转换为 `Ty1` 和
    `Ty2` 类型。如果这些限制都满足，那么这个构造函数模板就会被启用。

    最后，这个构造函数模板实现了 `std::pair` 类型的构造函数，使用传入的两个参数
    `a` 和 `b` 分别初始化 `std::pair` 类型的第一个元素和第二个元素。

    总之，这个构造函数模板的作用是为 `std::pair`
    类型提供一个可以从两个参数隐式构造的构造函数，并且限制了模板参数的类型。 */
    template <class U1 = Ty1,
              class U2 = Ty2,
              typename std::enable_if<
                  std::is_copy_constructible<U1>::value &&
                      std::is_copy_constructible<U2>::value &&
                      std::is_convertible<const U1&, Ty1>::value &&
                      std::is_convertible<const U2&, Ty2>::value,
                  int>::type = 0>
    constexpr pair(const Ty1& a, const Ty2& b) : first(a), second(b) {}

    // explicit constructible for this type
    /* 这是一个 C++ 中的 `pair` 类的构造函数，其中使用了模板和
    `enable_if`。`pair`
    类是一个模板类，表示一对值，可以使用不同的类型来表示这两个值。
    在这个构造函数中，我们可以看到有两个参数 `a` 和
    `b`，分别表示这两个值的初始值。 这个构造函数的作用是将这两个初始值分别赋给
    `first` 和 `second`，这两个变量是 `pair`
    类的成员变量，分别表示这一对值中的第一个值和第二个值。

    在这个构造函数中，我们可以看到有两个模板参数 `U1` 和 `U2`，它们分别表示
    `first` 和 `second` 的类型。默认情况下，它们的类型是 `Ty1` 和 `Ty2`，也就是
    `pair` 类的模板参数类型。如果我们想要使用其他类型来初始化 `first` 和
    `second`，可以通过显式地指定模板参数来实现。

    在模板参数列表的末尾，我们可以看到一个奇怪的 `int` 类型的参数，它的默认值是
    `0`。这个参数的作用是用来启用
    `enable_if`，它表示一个条件，只有当这个条件成立时，才会启用这个构造函数。在这个构造函数中，我们可以看到
    `enable_if` 的条件是一个复杂的逻辑表达式，它使用了 `is_copy_constructible`
    和 `is_convertible`
    这两个类型特征来判断模板参数是否符合要求。如果这个条件成立，那么这个构造函数就会被启用。否则，编译器会尝试寻找其他的构造函数来初始化
    `pair` 类的对象。 */
    template <class U1 = Ty1,
              class U2 = Ty2,
              typename std::enable_if<
                  std::is_copy_constructible<U1>::value &&
                      std::is_copy_constructible<U2>::value &&
                      (!std::is_convertible<const U1&, Ty1>::value ||
                       !std::is_convertible<const U2&, Ty2>::value),
                  int>::type = 0>
    explicit constexpr pair(const Ty1& a, const Ty2& b) : first(a), second(b) {}

    pair(const pair& rhs) = default;
    pair(pair&& rhs) = default;

    // implicit constructiable for other type
    template <
        class Other1,
        class Other2,
        typename std::enable_if<std::is_constructible<Ty1, Other1>::value &&
                                    std::is_constructible<Ty2, Other2>::value &&
                                    std::is_convertible<Other1&&, Ty1>::value &&
                                    std::is_convertible<Other2&&, Ty2>::value,
                                int>::type = 0>
    constexpr pair(Other1&& a, Other2&& b)
        : first(mystl::forward<Other1>(a)), second(mystl::forward<Other2>(b)) {}

    // explicit constructiable for other type
    template <
        class Other1,
        class Other2,
        typename std::enable_if<std::is_constructible<Ty1, Other1>::value &&
                                    std::is_constructible<Ty2, Other2>::value &&
                                    (!std::is_convertible<Other1, Ty1>::value ||
                                     !std::is_convertible<Other2, Ty2>::value),
                                int>::type = 0>
    explicit constexpr pair(Other1&& a, Other2&& b)
        : first(mystl::forward<Other1>(a)), second(mystl::forward<Other2>(b)) {}

    // implicit constructiable for other pair
    template <class Other1,
              class Other2,
              typename std::enable_if<
                  std::is_constructible<Ty1, const Other1&>::value &&
                      std::is_constructible<Ty2, const Other2&>::value &&
                      std::is_convertible<const Other1&, Ty1>::value &&
                      std::is_convertible<const Other2&, Ty2>::value,
                  int>::type = 0>
    constexpr pair(const pair<Other1, Other2>& other)
        : first(other.first), second(other.second) {}

    // explicit constructiable for other pair
    template <class Other1,
              class Other2,
              typename std::enable_if<
                  std::is_constructible<Ty1, const Other1&>::value &&
                      std::is_constructible<Ty2, const Other2&>::value &&
                      (!std::is_convertible<const Other1&, Ty1>::value ||
                       !std::is_convertible<const Other2&, Ty2>::value),
                  int>::type = 0>
    explicit constexpr pair(const pair<Other1, Other2>& other)
        : first(other.first), second(other.second) {}

    // copy assign for this pair
    pair& operator=(const pair& rhs) {
        if (this != &rhs) {
            first = rhs.first;
            second = rhs.second;
        }
        return *this;
    }

    // move assign for this pair
    pair& operator=(pair&& rhs) {
        if (this != &rhs) {
            first = mystl::move(rhs.first);
            second = mystl::move(rhs.second);
        }
    }

    // copy assign for other pair
    template <class Other1, class Other2>
    pair& operator=(const pair<Other1, Other2>& other) {
        first = other.first;
        second = other.second;
        return *this;
    }

    // move assign for other pair
    template <class Other1, class Other2>
    pair& operator=(pair<Other1, Other2>&& other) {
        first = mystl::forward<Other1>(other.first);
        second = mystl::forward<Other2>(other.second);
        return *this;
    }

    ~pair() = default;

    void swap(pair& other) {
        if (this != &other) {
            mystl::swap(first, other.first);
            mystl::swap(second, other.second);
        }
    }
};

// 重载比较操作符
template <class Ty1, class Ty2>
bool operator==(const pair<Ty1, Ty2>& lhs, const pair<Ty1, Ty2>& rhs) {
    return lhs.first == rhs.first && lhs.second == rhs.second;
}

template <class Ty1, class Ty2>
bool operator<(const pair<Ty1, Ty2>& lhs, const pair<Ty1, Ty2>& rhs) {
    return lhs.first < rhs.first ||
           (lhs.first == rhs.first && lhs.second < rhs.second);
}

template <class Ty1, class Ty2>
bool operator!=(const pair<Ty1, Ty2>& lhs, const pair<Ty1, Ty2>& rhs) {
    return !(lhs == rhs);
}

template <class Ty1, class Ty2>
bool operator>(const pair<Ty1, Ty2>& lhs, const pair<Ty1, Ty2>& rhs) {
    return rhs < lhs;
}

template <class Ty1, class Ty2>
bool operator<=(const pair<Ty1, Ty2>& lhs, const pair<Ty1, Ty2>& rhs) {
    return !(rhs < lhs);
}

template <class Ty1, class Ty2>
bool operator>=(const pair<Ty1, Ty2>& lhs, const pair<Ty1, Ty2>& rhs)
{
  return !(lhs < rhs);
}

// 重载mystl的swap
template <class Ty1, class Ty2>
void swap(pair<Ty1, Ty2>& lhs, pair<Ty1, Ty2>& rhs){
    lhs.swap(rhs);
}

// 全局函数，让两个数据成为一个pair
/* 这是一个实现 `make_pair` 函数的示例代码，它使用了完美转发（`std::forward`）来保持参数的值分类（左值或右值）。
这个函数的作用是创建一个 `pair` 对象，其中第一个元素的类型是 `Ty1`，第二个元素的类型是 `Ty2`。
这个函数允许你传入任意类型的参数，并将它们转发给 `pair` 构造函数来创建一个新的 `pair` 对象。在这个实现中，我们使用了 C++11 的模板别名语法 `using` 来简化类型声明。 */
template <class Ty1, class Ty2>
pair<Ty1,Ty2> make_pair(Ty1&& first, Ty2&& second){
    return pair<Ty1, Ty2>(mystl::forward<Ty1>(first), mystl::forward<Ty2>(second));
}




}  // namespace mystl

#endif