#ifndef MYTINYSTL_TYPE_TRAITS_H_
#define MYTINYSTL_TYPE_TRAITS_H_

// 这个头文件用于提取类型信息

// 使用type_traits标准头文件
#include <type_traits>

namespace mystl{

// helper struct
// 这是一个 C++ 的模板结构体，名为 `m_integral_constant`。
// 它有两个模板参数，一个是类型参数 `T`，另一个是值参数 `v`，其中 `T` 是任意类型，`v` 是一个值，类型为 `T`。
// 这个结构体定义了一个静态成员变量 `value`，其值为 `v`，并且这个变量是一个编译期常量，可以在编译时计算出来。
// 这个结构体的作用是用来表示一个编译期常量，类似于 C++11 中的 `std::integral_constant`。
template <class T, T v>
struct m_integral_constant
{
    static constexpr T value=v;
};

// 这是一个 C++ 中的模板定义，它定义了一个模板别名 `m_bool_constant`，用于创建一个布尔类型的模板常量。
// 它接受一个布尔值 `b` 作为模板参数，并将其作为模板参数传递给 `m_integral_constant` 类模板。
// `m_integral_constant` 是一个定义在 `<type_traits>` 头文件中的类模板，用于创建一个编译时常量，它的第一个模板参数是常量的类型，第二个模板参数是常量的值。
// 因此，`m_bool_constant<b>` 将创建一个 `bool` 类型的编译时常量，其值为 `b`。这个模板别名可以用于定义模板函数或模板类中的常量，也可以用于进行类型推断。
template <bool b>
using m_bool_constant=m_integral_constant<bool, b>;

typedef m_bool_constant<true> m_true_type;
typedef m_bool_constant<false> m_false_type;

/* *************************************** */
// type traits

// is_pair

// --- forward declaration begin
// 这段代码是一个 C++ 的模板元编程中的类型特征（type traits）实现，用于判断一个类型是否为 `pair`。
// 其中，`pair` 是一个模板类，有两个模板参数 `T1` 和 `T2`，表示一个键值对。
// 这里的代码只是一个前向声明，没有给出 `pair` 类的具体实现。
template <class T1, class T2>
struct pair;
// --- forward declaration end

template <class T>
struct is_pair:mystl::m_false_type{};

template <class T1, class T2>
struct is_pair<mystl::pair<T1,T2>>:mystl::m_true_type{};


} // namespace mystl

#endif // !MYTINYSTL_TYPE_TRAITS_H