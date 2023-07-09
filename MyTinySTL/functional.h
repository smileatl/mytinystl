#ifndef MYTINYSTL_FUNCTIONAL_H_
#define MYTINYSTL_FUNCTIONAL_H_

// 这个头文件包含了mystl函数对象和哈希安徽念书
#include <cstddef>

namespace mystl {

// 定义一元函数的参数型别和返回值型别
template <class Arg, class Result>
struct unarg_function {
    typedef Arg argument_type;
    typedef Result result_type;
};

// 定义二元函数的参数型别和返回值型别
template <class Arg1, class Arg2, class Result>
struct binary_function {
    typedef Arg1 first_argument_type;
    typedef Arg2 second_argument_type;
    typedef Result result_type;
};

// 函数对象：加法
template <class T>
struct plus : public binary_function<T, T, T> {
    T operator()(const T& x, const T& y) const { return x + y; }
};

// 函数对象：减法
template <class T>
struct minus : public binary_function<T, T, T> {
    T operator()(const T& x, const T& y) const { return x - y; }
};

// 函数对象：乘法
template <class T>
struct multiplies : public binary_function<T, T, T> {
    T operator()(const T& x, const T& y) const { return x * y; }
};

// 函数对象：除法
template <class T>
struct divides : public binary_function<T, T, T> {
    T operator()(const T& x, const T& y) const { return x / y; }
};

// 函数对象：取模
template <class T>
struct modulus : public binary_function<T, T, T> {
    T operator()(const T& x, const T& y) const { return x % y; }
};

// 函数对象：取负
template <class T>
struct nagate : public unarg_function<T, T> {
    T operator()(const T& x) const { return -x; }
};

// 加法的证同元素
/* 这是一个函数模板，接受一个类型为 `plus<T>` 的参数，并返回一个类型为 `T`
的值。 它的作用是返回一个加法运算的单位元素，即对于任何
`x`，`identity_element(plus<T>()) + x` 的结果都等于 `x`。
在这个函数模板中，我们假设加法的单位元素是 `T(0)`，即对于任何类型
`T`，加法的单位元素都是 `0`。 */
template <class T>
T identity_element(plus<T>) {
    return T(0);
}

// 乘法的证同元素
template <class T>
T identity_element(multiplies<T>) {
    return T(1);
}

// 函数对象：等于
template <class T>
struct equal_to : public binary_function<T, T, bool> {
    bool operator()(const T& x, const T& y) const { return x == y; }
};

// 函数对象：不等于
template <class T>
struct not_equal_to : public binary_function<T, T, bool> {
    bool operator()(const T& x, const T& y) const { return x != y; }
};

// 函数对象：大于
template <class T>
struct greater : public binary_function<T, T, bool> {
    bool operator()(const T& x, const T& y) const { return x > y; }
};

// 函数对象：小于
template <class T>
struct less : public binary_function<T, T, bool> {
    bool operator()(const T& x, const T& y) const { return x < y; }
};

// 函数对象：大于等于
template <class T>
struct greater_equal : public binary_function<T, T, bool> {
    bool operator()(const T& x, const T& y) const { return x >= y; }
};

// 函数对象：小于等于
template <class T>
struct less_equal : public binary_function<T, T, bool> {
    bool operator()(const T& x, const T& y) const { return x <= y; }
};

// 函数对象：逻辑与
template <class T>
struct logical_and : public binary_function<T, T, bool> {
    bool operator()(const T& x, const T& y) const { return x && y; }
};

// 函数对象：逻辑或
template <class T>
struct logical_or : public binary_function<T, T, bool> {
    bool operator()(const T& x, const T& y) const { return x || y; }
};

// 函数对象：逻辑非
template <class T>
struct logical_not : public unarg_function<T, bool> {
    bool operator()(const T& x) const { return !x; }
};

// 证同函数：不会改变元素，返回本身
template <class T>
struct identity : public unarg_function<T, bool> {
    const T& operator()(const T& x) const { return x; }
};

// 选择函数：接受一个pair，返回第一个元素
template <class Pair>
struct selectfirst : public unarg_function<Pair, typename Pair::first_type> {
    const typename Pair::first_type& operator()(const Pair& x) const {
        return x.first;
    }
};

// 选择函数：接受一个 pair，返回第二个元素
template <class Pair>
struct selectsecond : public unarg_function<Pair, typename Pair::second_type> {
    const typename Pair::second_type& operator()(const Pair& x) const {
        return x.second;
    }
};

// 投射函数：返回第一参数
template <class Arg1, class Arg2>
struct projectfirst : public binary_function<Arg1, Arg2, Arg1> {
    Arg1 operator()(const Arg1& x, const Arg2&) const { return x; }
};

// 投射函数：返回第二参数
template <class Arg1, class Arg2>
struct projectsecond : public binary_function<Arg1, Arg2, Arg1> {
    Arg2 operator()(const Arg1&, const Arg2& y) const { return y; }
};

/* ************************************ */
// 哈希函数对象

// 对于大部分类型，hash function什么都不做
template <class Key>
struct hash {};

// 针对指针的偏特化版本
/* 这是一个 C++ 的哈希函数模板，用于将指针类型的值转换为哈希值。
该模板是一个结构体，其中定义了一个名为 `operator()`
的函数，该函数接受一个指向类型 `T` 的指针，并返回一个 `size_t` 类型的哈希值。

该哈希函数的实现非常简单，它只是将指针的地址转换为一个 `size_t`
类型的整数，并将其作为哈希值返回。由于指针地址是唯一的，因此这个哈希函数可以用来对指针类型的值进行哈希，以便在哈希表等数据结构中进行查找和比较操作。

需要注意的是，这个哈希函数只适用于指针类型的值，如果你想对其他类型的值进行哈希，需要定义一个不同的哈希函数。同时，由于指针类型的值在不同的平台上可能具有不同的大小和表示方式，因此这个哈希函数也可能在不同的平台上表现不同。
*/
template <class T>
struct hash<T*> {
    size_t operator()(T* p) const noexcept {
        return reinterpret_cast<size_t>(p);
    }
};

// 对于整型类型，只是返回原值
/* 这是一个 C++ 宏定义，用于生成一个模板特化结构体
`hash`，用于将特定类型的值转换为哈希值。 `Type`
是要特化的类型，这个宏定义中使用了模板元编程技术，使得这个特化结构体可以用于任何类型。在这个特化结构体中，重载了一个圆括号运算符
`()`，它接受一个 `Type` 类型的参数 `val`，并将其转换为 `size_t`
类型的哈希值，最后返回这个哈希值。 这个运算符是一个 `const noexcept`
函数，表示它是一个常量函数，不会抛出异常。 */
#define MYSTL_TRIVIAL_HASH_FCN(Type)                 \
    template <>                                      \
    struct hash<Type> {                              \
        size_t operator()(Type val) const noexcept { \
            return static_cast<size_t>(val);         \
        }                                            \
    };

MYSTL_TRIVIAL_HASH_FCN(bool)

MYSTL_TRIVIAL_HASH_FCN(char)

MYSTL_TRIVIAL_HASH_FCN(signed char)

MYSTL_TRIVIAL_HASH_FCN(unsigned char)

MYSTL_TRIVIAL_HASH_FCN(wchar_t)

MYSTL_TRIVIAL_HASH_FCN(char16_t)

MYSTL_TRIVIAL_HASH_FCN(char32_t)

MYSTL_TRIVIAL_HASH_FCN(short)

MYSTL_TRIVIAL_HASH_FCN(unsigned short)

MYSTL_TRIVIAL_HASH_FCN(int)

MYSTL_TRIVIAL_HASH_FCN(unsigned int)

MYSTL_TRIVIAL_HASH_FCN(long)

MYSTL_TRIVIAL_HASH_FCN(unsigned long)

MYSTL_TRIVIAL_HASH_FCN(long long)

MYSTL_TRIVIAL_HASH_FCN(unsigned long long)

#undef MYSTL_TRIVIAL_HASH_FCN

// 对于浮点数，逐位哈希，对浮点数的每一位都进行哈希操作
// 哈希是一种将任意长度的消息压缩到某一固定长度的消息摘要的函数。
inline size_t bitwise_hash(const unsigned char* first, size_t count) {
#if (_MSC_VER && _WIN64) || ((__GNUC__ || __clang__) && __SIZEOF_POINTER__ == 8)
    const size_t fnv_offset = 14695981039346656037ull;
    const size_t fnv_prime = 1099511628211ull;
#else
    const size_t fnv_offset = 2166136261u;
    const size_t fnv_prime = 16777619u;
#endif
    size_t result = fnv_offset;
    for (size_t i = 0; i < count; ++i) {
        // 对每个字节进行异或操作，然后乘以一个质数
        result ^= (size_t)first[i];
        result *= fnv_prime;
    }
    return result;
}

template <>
struct hash<float> {
    size_t operator()(const float& val) {
        return val == 0.0f
                   ? 0
                   : bitwise_hash((const unsigned char*)&val, sizeof(float));
    }
};

template <>
struct hash<double> {
    size_t operator()(const double& val) {
        return val == 0.0f
                   ? 0
                   : bitwise_hash((const unsigned char*)&val, sizeof(double));
    }
};

template <>
struct hash<long double> {
    size_t operator()(const long double& val) {
        return val == 0.0f ? 0
                           : bitwise_hash((const unsigned char*)&val,
                                          sizeof(long double));
    }
};

}  // namespace mystl

#endif  // !MYTINYSTL_FUNCTIONAL_H_