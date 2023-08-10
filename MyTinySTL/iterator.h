#ifndef MYTINYSTL_ITERATOR_H_
#define MYTINYSTL_ITERATOR_H_

// 这个头文件用于迭代器设计，包含了一些模板结构体与全局函数

#include <cstddef>

#include "type_traits.h"

namespace mystl {

// 五种迭代器类型
struct input_iterator_tag {};
struct output_iterator_tag {};
struct forward_iterator_tag : public input_iterator_tag {};
struct bidirectional_iterator_tag : public forward_iterator_tag {};
struct random_access_iterator_tag : public bidirectional_iterator_tag {};

// iterator模板
// 为了避免写代码的时候遗漏，最好继承此类
template <class Category,
          class T,
          class Distance = ptrdiff_t,
          class Pointer = T*,
          class Reference = T&>
struct iterator {
    typedef Category iterator_category;  // 迭代器类型
    typedef T value_type;                // 迭代器所指对象类型
    typedef Pointer pointer;             // 迭代器所指对象的类型指针
    typedef Reference reference;         // 迭代器所指对象的类型引用
    typedef Distance difference_type;    // 两个迭代器之间的距离
};

// iterator traits
// 迭代器特性
// 这是一个 C++ 模板元编程的例子，用于检查一个类型 `T` 是否拥有
// `iterator_category` 这个类型的成员。 具体来说，它定义了一个模板结构体
// `has_iterator_cat`，其中包含了一个静态常量 `value`，表示是否存在
// `iterator_category` 成员。 如果存在，则 `value` 的值为 `true`，否则为
// `false`。

// 这个模板结构体的实现比较巧妙。它定义了一个内部的 `two` 结构体，其中包含了两个
// `char` 类型的成员变量 `a` 和 `b`。 然后，它定义了两个模板函数
// `test`，一个接受任意类型的参数，另一个接受一个指向 `U` 类型的成员
// `iterator_category` 的指针。 第一个函数的返回类型是
// `two`，第二个函数的返回类型是 `char`。 在 `has_iterator_cat`
// 结构体中，通过调用 `sizeof(test<t>(0))` 来判断 `U` 是否有 `iterator_category`
// 成员。 如果有，则第二个 `test` 函数会被调用，返回 `char` 类型，而第一个
// `test` 函数则被忽略。因此，如果 `value` 的值为 `true`，说明 `T` 类型有
// `iterator_category` 成员，否则没有。
template <class T>
struct has_iterator_cat {
private:
    struct two {
        char a;
        char b;
    };
    // 函数重载
    template <class U>
    static two test(...);
    template <class U>
    static char test(typename U::iterator_category* = 0);

public:
    static const bool value = sizeof(test<T>(0)) == sizeof(char);
};

template <class Iterator, bool>
struct iterator_traits_impl {};

template <class Iterator>
struct iterator_traits_impl<Iterator, true> {
    // typename通常用于指示一个依赖类型的名称
    // Iterator::iterator_category是一个依赖类型
    typedef typename Iterator::iterator_category iterator_category;
    typedef typename Iterator::value_type value_type;
    typedef typename Iterator::pointer pointer;
    typedef typename Iterator::reference reference;
    typedef typename Iterator::difference_type difference_type;
};

template <class Iterator, bool>
struct iterator_traits_helper {};

// 这段代码是一个模板元编程技巧，用于检查一个类型是否拥有 `iterator_category`
// 成员。如果一个类型拥有 `iterator_category`
// 成员，那么它就可以被视为一个迭代器。这个技巧通过继承 `iterator_traits_impl`
// 类来实现。

// 这里使用了一个模板参数 `Iterator`，用于指定要检查的类型。
// 另外，还有一个 `bool` 类型的模板参数，用于指定是否检查 `iterator_category`
// 成员。 如果这个参数为 `true`，那么就会检查 `iterator_category` 成员；如果为
// `false`，则不会检查。

// 在这个代码中，如果 `Iterator` 类型拥有 `iterator_category`
// 成员，并且这个成员可以被转换为 `input_iterator_tag` 或
// `output_iterator_tag`，那么就会继承 `iterator_traits_impl<Iterator,
// true>`；否则就不会继承。 这样，就可以通过检查
// `iterator_traits_helper<Iterator, true>` 是否有 `iterator_category`
// 成员来判断 `Iterator` 类型是否为迭代器。
template <class Iterator>
struct iterator_traits_helper<Iterator, true>
    : public iterator_traits_impl<
          Iterator,
          std::is_convertible<typename Iterator::iterator_category,
                              input_iterator_tag>::value ||
              std::is_convertible<typename Iterator::iterator_category,
                                  output_iterator_tag>::value> {};

// 萃取迭代器的特性
template <class Iterator>
struct iterator_traits
    : public iterator_traits_helper<Iterator,
                                    has_iterator_cat<Iterator>::value> {};

// 针对原生指针 T* 生成的 traits 偏特化版本
template <class T>
struct iterator_traits<T*> {
    typedef random_access_iterator_tag iterator_category;
    typedef T value_type;
    typedef T* pointer;
    typedef T& reference;
    typedef ptrdiff_t difference_type;
};

// 针对原生指针 const T* 生成的 traits 偏特化
template <class T>
struct iterator_traits<const T*> {
    typedef random_access_iterator_tag iterator_category;
    typedef T value_type;
    typedef T* pointer;
    typedef T& reference;
    typedef ptrdiff_t difference_type;
};

// 这是一个模板元编程的代码片段，它定义了一个模板类
// `has_iterator_cat_of`，该类用于检查类型 `T` 是否具有名为 `iterator_category`
// 的成员，并且该成员是否可转换为类型 `U`。 其中，`has_iterator_cat`
// 是另一个模板类，用于检查类型 `T` 是否具有名为 `iterator_category` 的成员。

/* 具体来说，这个代码片段的实现思路是：
- 如果类型 `T` 没有 `iterator_category` 成员，则默认该类型的
`has_iterator_cat_of` 值为 `false`。
- 如果类型 `T` 有 `iterator_category` 成员，则检查该成员是否可转换为类型
`U`，如果可以，则该类型的 `has_iterator_cat_of` 值为 `true`，否则为 `false`。
这个代码片段可以用于检查一个类型是否满足某些迭代器的特性，例如是否支持双向迭代器、随机访问迭代器等。
*/
template <class T, class U, bool = has_iterator_cat<iterator_traits<T>>::value>
struct has_iterator_cat_of
    : public m_bool_constant<
          std::is_convertible<typename iterator_traits<T>::iterator_category,
                              U>::value> {};

// 萃取某种迭代器
template <class T, class U>
struct has_iterator_cat_of<T, U, false> : public m_false_type {};

template <class Iter>
struct is_input_iterator
    : public has_iterator_cat_of<Iter, input_iterator_tag> {};

template <class Iter>
struct is_output_iterator
    : public has_iterator_cat_of<Iter, output_iterator_tag> {};

template <class Iter>
struct is_forward_iterator
    : public has_iterator_cat_of<Iter, forward_iterator_tag> {};

template <class Iter>
struct is_bidirectional_iterator
    : public has_iterator_cat_of<Iter, bidirectional_iterator_tag> {};

template <class Iter>
struct is_random_access_iterator
    : public has_iterator_cat_of<Iter, random_access_iterator_tag> {};

/* 这是另一个模板元编程的代码片段，它定义了一个模板类
`is_iterator`，该类用于检查类型 `Iterator` 是否为迭代器类型。
其中，`is_input_iterator` 和 `is_output_iterator` 是另外两个模板类，用于检查类型
`Iterator` 是否为输入迭代器和输出迭代器。

具体来说，这个代码片段的实现思路是：
- 如果类型 `Iterator` 是输入迭代器或输出迭代器，则该类型的 `is_iterator` 值为
`true`。
- 如果类型 `Iterator` 不是输入迭代器或输出迭代器，则该类型的 `is_iterator` 值为
`false`。

这个代码片段可以用于检查一个类型是否为迭代器类型，例如在编写泛型算法时，需要使用迭代器作为参数。
*/
template <class Iterator>
struct is_iterator
    : public m_bool_constant<is_input_iterator<Iterator>::value ||
                             is_output_iterator<Iterator>::value> {};

// 萃取某个迭代器的category
/* 这段代码是一个模板函数，名为 `iterator_category`，它接受一个迭代器类型的参数
`Iterator`，并返回该迭代器所属的迭代器类型类别，即
`iterator_traits<Iterator>::iterator_category`。 其中，`iterator_traits`
是一个模板类，用于获取迭代器类型的信息，例如迭代器所指向的类型、迭代器的类型类别等。
在这个函数中，我们首先使用 `iterator_traits<Iterator>`
获取迭代器类型的信息，然后通过 `typename` 关键字指定
`iterator_traits<Iterator>::iterator_category` 为类型名，并定义一个 `Category`
类型别名。 最后，我们返回 `Category()`，即 `Category`
类型的默认构造函数的返回值，这个返回值就是迭代器所属的迭代器类型类别。 */
template <class Iterator>
typename iterator_traits<Iterator>::iterator_category iterator_category(
    const Iterator&) {
    typedef typename iterator_traits<Iterator>::iterator_category Category;
    return Category();
}

// 萃取某个迭代器的distance_type
/* 这是一个模板元函数，名为 `distance_type`，它接受一个迭代器类型 `Iterator`
的引用作为参数，并返回一个指向该迭代器类型的差值类型的指针。

在函数体内，我们使用了 `iterator_traits` 模板来获取迭代器类型的特征类型，即
`difference_type`。然后，我们使用 `static_cast` 将一个值为 0 的指针转换为指向
`difference_type` 类型的指针，并将其返回。

这个函数的作用是为了在编译期间检查一个迭代器是否支持差值操作。如果迭代器类型不支持差值操作，那么在编译期间会发生错误。这在泛型编程中是非常有用的，因为我们可以在编译期间捕获一些错误，而不是在运行期间才发现这些错误。
*/
template <class Iterator>
typename iterator_traits<Iterator>::difference_type* distance_type(
    const Iterator&) {
    return static_cast<typename iterator_traits<Iterator>::difference_type*>(0);
}

// 萃取某个迭代器的value_type
template <class Iterator>
typename iterator_traits<Iterator>::value_type* value_type(const Iterator&) {
    return static_cast<typename iterator_traits<Iterator>::value_type*>(0);
}

// 以下函数用于计算迭代器间的距离

// distance的input_iterator_tag的版本
/* 这段代码是模板元编程中的一个例子，用于计算两个迭代器之间的距离。它使用了迭代器的类型特性，即
`iterator_traits` 模板，来获取迭代器的类型信息，例如迭代器的 `difference_type`
类型，这是两个迭代器之间的距离类型。

具体来说，这个函数使用了迭代器的 `input_iterator_tag`
类型，这是一种迭代器标签，用于指示迭代器的类别。对于输入迭代器，我们只能通过逐个递增迭代器来遍历整个序列，因此这个函数使用了一个
while
循环来逐个递增迭代器，并计算两个迭代器之间的距离。最后，它返回了这个距离值。

需要注意的是，这个函数只适用于输入迭代器，对于其他类型的迭代器，例如随机访问迭代器，我们可以使用更高效的方法来计算两个迭代器之间的距离。
*/
template <class InputIterator>
typename iterator_traits<InputIterator>::difference_type
distance_dispatch(InputIterator first, InputIterator last, input_iterator_tag) {
    typename iterator_traits<InputIterator>::difference_type n = 0;
    while (first != last) {
        ++first;
        ++n;
    }
    return n;
}

// distance的random_access_iterator_tag版本
template <class RandomIter>
typename iterator_traits<RandomIter>::difference_type distance_dispatch(
    RandomIter first,
    RandomIter last,
    random_access_iterator_tag) {
    return last - first;
}

template <class InputIterator>
typename iterator_traits<InputIterator>::difference_type distance(
    InputIterator first,
    InputIterator last) {
    return distance_dispatch(first, last, iterator_category(first));
}

// 以下函数用于让迭代器前进n个距离

// advance的input_iterator_tag的版本
/* 这是一个模板元编程中的函数模板，用于对迭代器进行前进操作。该函数模板的参数包括：

- `InputIterator& i`：迭代器对象
- `Distance n`：前进的步数
- `input_iterator_tag`：迭代器类别标签，用于区分不同类型的迭代器

该函数模板的实现方式是通过循环将迭代器对象 `i` 前进 `n` 步，直到 `n` 为
0。由于该函数模板是根据迭代器类别标签进行重载的，因此可以处理不同类型的迭代器，例如输入迭代器、随机访问迭代器等。
*/
template <class BidirectionalIterator, class Distance>
void advance_dispatch(BidirectionalIterator& i,
                      Distance n,
                      bidirectional_iterator_tag) {
    if (n >= 0)
        while (n--)
            ++i;
    else
        while (n++)
            --i;
}

// advance 的 random_access_iterator_tag 的版本
template <class RandomIter, class Distance>
void advance_dispatch(RandomIter& i, Distance n, random_access_iterator_tag) {
    i += n;
}

template <class InputIterator, class Distance>
void advance(InputIterator& i, Distance n) {
    advance_dispatch(i, n, iterator_category(i));
}

/* *********************************************** */

// 模板类：reverse_iterator
// 代表反向迭代器，使前进为后退，后退为前进
template <class Iterator>
class reverse_iterator {
private:
    Iterator current;  // 记录对应的正向迭代器
public:
    // 反向迭代器的五种相应型别
    typedef
        typename iterator_traits<Iterator>::iterator_category iterator_category;
    typedef typename iterator_traits<Iterator>::value_type value_type;
    typedef typename iterator_traits<Iterator>::difference_type difference_type;
    typedef typename iterator_traits<Iterator>::pointer pointer;
    typedef typename iterator_traits<Iterator>::reference reference;

    typedef Iterator iterator_type;
    typedef reverse_iterator<Iterator> self;

public:
    // 构造函数
    reverse_iterator() {}
    explicit reverse_iterator(iterator_type i) : current(i) {}
    reverse_iterator(const self& rhs) : current(rhs.current) {}

public:
    // 取出对应的正向迭代器
    iterator_type base() const { return current; }

    // 重载操作符
    reference operator*() const {
        // 实际对应正向迭代器的前一个位置
        auto tmp = current;
        // 先--，在解引用*
        return *--tmp;
    }
    /* 这是一个重载了箭头操作符的 const 成员函数。它返回一个指向 operator*()
    所返回的对象的指针。
    箭头操作符一般用于访问对象的成员，当对一个指针进行箭头操作时，实际上是先对指针进行解引用操作，然后再访问其成员。
    因此，这个函数返回了一个指向 operator*()
    所返回的对象的指针，以便对其成员进行访问。需要注意的是，这个函数是 const
    成员函数，因此不能修改对象的状态。 */
    pointer operator->() const { return &(operator*()); }

    // 前进(++)变为后退(--)
    self& operator++() {
        --current;
        return *this;
    }
    self operator++(int) {
        self tmp = *this;
        --current;
        return tmp;
    }
    // 后退(--)变为前进(++)
    self& operator--() {
        ++current;
        return *this;
    }
    self operator--(int) {
        self tmp = *this;
        ++current;
        return tmp;
    }

    /* 这是一个迭代器的加法运算符重载函数，用于实现迭代器的加法操作。第一个函数实现了迭代器的复合赋值加法运算符（+=），将当前迭代器的指针向后移动
     * n 个位置，并返回一个指向该迭代器的引用。
     */
    self& operator+=(difference_type n) {
        current -= n;
        return *this;
    }
    /* 第二个函数实现了迭代器的加法运算符（+），返回一个新的迭代器对象，该对象的指针指向当前迭代器指针向后移动
    n 个位置后的位置。需要注意的是，这两个函数中的 current
    指针是指向迭代器所指向的元素的指针，而不是指向迭代器本身的指针。
    此外，这两个函数中的 difference_type
    是一个整数类型，表示两个迭代器之间的距离。 */
    self operator+(difference_type n) const { return self(current - n); }
    self& operator-=(difference_type n) {
        current += n;
        return *this;
    }
    self operator-(difference_type n) const { return self(current + n); }

    /* 这是一个迭代器的下标运算符重载函数，用于实现迭代器的下标操作。
    该函数接受一个整数类型的参数 n，表示要访问的元素的偏移量。
    函数实现中，首先使用当前迭代器和 n
    计算出要访问的元素的迭代器，然后使用解引用运算符 *
    获取该元素的引用，并返回该引用。
    需要注意的是，该函数返回的是一个引用，而不是一个值，因此可以使用该函数来修改迭代器所指向的元素的值。
    此外，这个函数中的 difference_type
    是一个整数类型，表示两个迭代器之间的距离。 */
    reference operator[](difference_type n) const { return *(*this + n); }
};

// 重载 operator-
template <class Iterator>
typename reverse_iterator<Iterator>::difference_type operator-(
    const reverse_iterator<Iterator>& lhs,
    const reverse_iterator<Iterator>& rhs) {
    return rhs.base() - lhs.base();
}

// 重载比较操作符
template <class Iterator>
bool operator==(const reverse_iterator<Iterator>& lhs,
                const reverse_iterator<Iterator>& rhs) {
    return lhs.base() == rhs.base();
}

template <class Iterator>
bool operator<(const reverse_iterator<Iterator>& lhs,
               const reverse_iterator<Iterator>& rhs) {
    return rhs.base() < lhs.base();
}

template <class Iterator>
bool operator!=(const reverse_iterator<Iterator>& lhs,
                const reverse_iterator<Iterator>& rhs) {
    return !(lhs == rhs);
}

template <class Iterator>
bool operator>(const reverse_iterator<Iterator>& lhs,
               const reverse_iterator<Iterator>& rhs) {
    return rhs < lhs;
}

template <class Iterator>
bool operator<=(const reverse_iterator<Iterator>& lhs,
                const reverse_iterator<Iterator>& rhs) {
    return !(rhs < lhs);
}

}  // namespace mystl

#endif