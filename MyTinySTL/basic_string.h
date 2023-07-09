#ifndef MYTINYSTL_BASIC_STRING_H_
#define MYTINYSTL_BASIC_STRING_H_

// 这个头文件包含一个模板类 basic_string
// 用于表示字符串类型

#include "exceptdef.h"
#include "functional.h"
#include "iterator.h"
#include "memory.h"

namespace mystl {

// char_traits
template <class CharType>
struct char_traits {
    typedef CharType char_type;
    static size_t length(const char_type* str) {
        size_t len = 0;
        // 直到遇到字符串的结尾符 \0
        for (; *str != char_type(0); ++str) {
            ++len;
        }
        return len;
    }

    static int compare(const char_type* s1, const char_type* s2, size_t n) {
        for (; n != 0; --n, ++s1, ++s2) {
            if (*s1 < *s2) {
                return -1;
            }
            if (*s2 < *s1) {
                return 1;
            }
        }
        return 0;
    }

    static char_type* copy(char_type* dst, const char_type* src, size_t n) {
        // 目标字符串和源字符串不可重叠，避免出错
        MYSTL_DEBUG(src + n <= dst || dst + n <= src);
        char_type* r = dst;
        for (; n != 0; --n, ++dst, ++src) {
            *dst = *src;
        }
        return r;
    }

    static char_type* move(char_type* dst, const char_type* src, size_t n) {
        char_type* r = dst;
        // dst在src之前直接复制
        if (dst < src) {
            for (; n != 0; --n, ++dst, ++src) {
                *dst = *src;
            }
        }
        // src在dst之前，从后往前复制，防止地址交叉
        else if (src < dst) {
            dst += n;
            src += n;
            for (; n != 0; --n) {
                *--dst = *--src;
            }
        }
        return r;
    }

    // 在dst指向的字符串内存空间上填充count个ch
    static char_type* fill(char_type* dst, char_type ch, size_t count) {
        char_type* r = dst;
        for (; count > 0; --count, ++dst) {
            *dst = ch;
        }
        return r;
    }
};

// Partialized. char_traits<char> 部分化
// 这里就是把上一个结构体用Linux系统函数重现了一下，只不过特定针对char类型
// 用于定义字符类型char的一些基本操作。
// 它提供了一些静态成员函数，可以用于比较、复制、查找、长度计算等操作。
template <>
struct char_traits<char> {
    typedef char char_type;

    static size_t length(const char_type* str) noexcept {
        return std::strlen(str);
    }

    // s2所指的内存内容前n个字节拷贝到s1所指的内存地址上
    static int compare(const char_type* s1,
                       const char_type* s2,
                       size_t n) noexcept {
        return std::memcmp(s1, s2, n);
    }

    static char_type* copy(char_type* dst,
                           const char_type* src,
                           size_t n) noexcept {
        // 目标字符串和源字符串不可重叠
        MYSTL_DEBUG(src + n <= dst || dst + n <= src);
        return static_cast<char_type*>(std::memcpy(dst, src, n));
    }

    // move和copy不一样的地方在于内存区域可以重叠
    static char_type* move(char_type* dst,
                           const char_type* src,
                           size_t n) noexcept {
        return static_cast<char_type*>(std::memmove(dst, src, n));
    }

    static char_type* fill(char_type* dst,
                           char_type ch,
                           size_t count) noexcept {
        return static_cast<char_type*>(std::memset(dst, ch, count));
    }
};

// Partialized. char_traits<wchar_t>
// wchar是C++中的一种宽字符类型，大小比标准char类型大
template <>
struct char_traits<wchar_t> {
    typedef wchar_t char_type;

    static size_t length(const char_type* str) noexcept {
        return std::wcslen(str);
    }

    static int compare(const char_type* s1,
                       const char_type* s2,
                       size_t n) noexcept {
        return std::wmemcmp(s1, s2, n);
    }

    static char_type* copy(char_type* dst,
                           const char_type* src,
                           size_t n) noexcept {
        MYSTL_DEBUG(src + n <= dst || dst + n <= src);
        return static_cast<char_type*>(std::wmemcpy(dst, src, n));
    }

    static char_type* move(char_type* dst,
                           const char_type* src,
                           size_t n) noexcept {
        return static_cast<char_type*>(std::wmemmove(dst, src, n));
    }

    static char_type* fill(char_type* dst,
                           char_type ch,
                           size_t count) noexcept {
        return static_cast<char_type*>(std::wmemset(dst, ch, count));
    }
};

// Partialized. char_traits<char16_t>
// 普通的char默认是8位（1字节）
template <>
struct char_traits<char16_t> {
    typedef char16_t char_type;

    static size_t length(const char_type* str) noexcept {
        size_t len = 0;
        for (; *str != char_type(0); ++str)
            ++len;
        return len;
    }

    static int compare(const char_type* s1,
                       const char_type* s2,
                       size_t n) noexcept {
        for (; n != 0; --n, ++s1, ++s2) {
            if (*s1 < *s2)
                return -1;
            if (*s2 < *s1)
                return 1;
        }
        return 0;
    }

    static char_type* copy(char_type* dst,
                           const char_type* src,
                           size_t n) noexcept {
        MYSTL_DEBUG(src + n <= dst || dst + n <= src);
        char_type* r = dst;
        for (; n != 0; --n, ++dst, ++src)
            *dst = *src;
        return r;
    }

    static char_type* move(char_type* dst,
                           const char_type* src,
                           size_t n) noexcept {
        char_type* r = dst;
        if (dst < src) {
            for (; n != 0; --n, ++dst, ++src)
                *dst = *src;
        } else if (src < dst) {
            dst += n;
            src += n;
            for (; n != 0; --n)
                *--dst = *--src;
        }
        return r;
    }

    static char_type* fill(char_type* dst,
                           char_type ch,
                           size_t count) noexcept {
        char_type* r = dst;
        for (; count > 0; --count, ++dst)
            *dst = ch;
        return r;
    }
};

// Partialized. char_traits<char32_t>
template <>
struct char_traits<char32_t> {
    typedef char32_t char_type;

    static size_t length(const char_type* str) noexcept {
        size_t len = 0;
        for (; *str != char_type(0); ++str)
            ++len;
        return len;
    }

    static int compare(const char_type* s1,
                       const char_type* s2,
                       size_t n) noexcept {
        for (; n != 0; --n, ++s1, ++s2) {
            if (*s1 < *s2)
                return -1;
            if (*s2 < *s1)
                return 1;
        }
        return 0;
    }

    static char_type* copy(char_type* dst,
                           const char_type* src,
                           size_t n) noexcept {
        MYSTL_DEBUG(src + n <= dst || dst + n <= src);
        char_type* r = dst;
        for (; n != 0; --n, ++dst, ++src)
            *dst = *src;
        return r;
    }

    static char_type* move(char_type* dst,
                           const char_type* src,
                           size_t n) noexcept {
        char_type* r = dst;
        if (dst < src) {
            for (; n != 0; --n, ++dst, ++src)
                *dst = *src;
        } else if (src < dst) {
            dst += n;
            src += n;
            for (; n != 0; --n)
                *--dst = *--src;
        }
        return r;
    }

    static char_type* fill(char_type* dst,
                           char_type ch,
                           size_t count) noexcept {
        char_type* r = dst;
        for (; count > 0; --count, ++dst)
            *dst = ch;
        return r;
    }
};

// 初始化 basic_string 尝试分配的最小 buffer 大小，可能被忽略
#define STRING_INIT_SIZE 32

// 模板类 basic_string
// 参数一代表字符类型，参数二代表萃取字符类型的方式，缺省使用 mystl::char_traits
template <class CharType, class CharTraits = mystl::char_traits<CharType>>
class basic_string {
public:
    typedef CharTraits traits_type;
    typedef CharTraits char_traits;

    typedef mystl::allocator<CharType> allocator_type;
    typedef mystl::allocator<CharType> data_allocator;

    typedef typename allocator_type::value_type value_type;
    typedef typename allocator_type::pointer pointer;
    typedef typename allocator_type::const_pointer const_pointer;
    typedef typename allocator_type::reference reference;
    typedef typename allocator_type::const_reference const_reference;
    typedef typename allocator_type::size_type size_type;
    typedef typename allocator_type::difference_type difference_type;

    typedef value_type* iterator;
    typedef const value_type* const_iterator;
    typedef mystl::reverse_iterator<iterator> reverse_iterator;
    typedef mystl::reverse_iterator<const_iterator> const_reverse_iterator;

    allocator_type get_allocator() { return allocator_type(); }

    // static_assert表达式为false，会产生后面的错误信息
    static_assert(std::is_pod<CharType>::value,
                  "Character type of basic_string must be a POD");
    static_assert(
        std::is_same<CharType, typename traits_type::char_type>::value,
        "CharType must be same as traits_type::char_type");

public:
    // 末尾位置的值，例:
    // if (str.find('a') != string::npos) { /* do something */ }
    /* 这行代码定义了一个 `static` 的 `constexpr` 常量 `npos`，其值为
     * `static_cast<size_type>(-1)`。在 C++ 标准库中，`npos`
     * 通常用于表示字符串或容器中不存在某个元素或子串的情况，相当于一个“无效索引”。`static`
     * 关键字用于将 `npos`
     * 声明为一个静态变量，使得它在整个程序运行期间只有一份拷贝，而不是每次使用时都创建一个新的对象。`constexpr`
     * 关键字则表示 `npos`
     * 是一个编译期常量，可以用于编译时计算和初始化。`static_cast<size_type>(-1)`
     * 的作用是将 `-1` 转换为 `size_type` 类型，其中 `size_type`
     * 是一个无符号整数类型，通常用于表示容器或字符串的大小。由于 `-1`
     * 是一个有符号整数，因此需要使用 `static_cast`
     * 进行类型转换，避免出现符号扩展的问题。 */
    static constexpr size_type npos = static_cast<size_type>(-1);

private:
    iterator buffer_;  // 储存字符串的其实位置
    size_type size_;   // 大小
    size_type cap_;    // 容量

public:
    // 构造、复制、移动、析构函数
    basic_string() noexcept { try_init(); }

    basic_string(size_type n, value_type ch)
        : buffer_(nullptr), size_(0), cap_(0) {
        fill_init(n, ch);
    }

    basic_string(const basic_string& other, size_type pos)
        : buffer_(nullptr), size_(0), cap_(0) {
        init_from(other.buffer_, pos, other.size_ - pos);
    }

    basic_string(const basic_string& other, size_type pos, size_type count)
        : buffer_(nullptr), size_(0), cap_(0) {
        init_from(other.buffer_, pos, count);
    }

    basic_string(const_pointer str) : buffer_(nullptr), size_(0), cap_(0) {
        init_from(str, 0, char_traits::length(str));
    }

    basic_string(const_pointer str, size_type count)
        : buffer_(nullptr), size_(0), cap_(0) {
        init_from(str, 0, count);
    }

    template <class Iter,
              typename std::enable_if<mystl::is_input_iterator<Iter>::value,
                                      int>::type = 0>
    basic_string(Iter first, Iter last) {
        copy_init(first, last, iterator_category(first));
    }

    basic_string(const basic_string& rhs)
        : buffer_(nullptr), size_(0), cap_(0) {
        init_from(rhs.buffer_, 0, rhs.size_);
    }

    basic_string(basic_string&& rhs) noexcept
        : buffer_(rhs.buffer_), size_(rhs.size_), cap_(rhs.cap_) {
        rhs.buffer_ = nullptr;
        rhs.size_ = 0;
        rhs.cap_ = 0;
    }

    basic_string& operator=(const basic_string& rhs);
    basic_string& operator=(basic_string&& rhs) noexcept;

    basic_string& operator=(const_pointer str);
    basic_string& operator=(value_type ch);

    ~basic_string() { destroy_buffer(); }

public:
    // 迭代器相关操作
    iterator begin() noexcept { return buffer_; }
    const_iterator begin() const noexcept { return buffer_; }
    iterator end() noexcept { return buffer_ + size_; }
    const_iterator end() const noexcept { return buffer_ + size_; }

    reverse_iterator rbegin() noexcept { return reverse_iterator(end()); }
    const_reverse_iterator rbegin() const noexcept {
        return const_reverse_iterator(end());
    }
    reverse_iterator rend() noexcept { return reverse_iterator(begin()); }
    const_reverse_iterator rend() const noexcept {
        return const_reverse_iterator(begin());
    }

    const_iterator cbegin() const noexcept { return begin(); }
    const_iterator cend() const noexcept { return end(); }
    const_reverse_iterator crbegin() const noexcept { return rbegin(); }
    const_reverse_iterator crend() const noexcept { return rend(); }

    // 容量相关操作
    bool empty() const noexcept { return size_ == 0; }

    size_type size() const noexcept { return size_; }
    size_type length() const noexcept { return size_; }
    size_type capacity() const noexcept { return cap_; }
    // -1无符号整数的最大值，也就是容器可以容纳的最大元素数量
    size_type max_size() const noexcept { return static_cast<size_type>(-1); }

    void reserve(size_type n);
    void shrink_to_fit();

    // 访问元素相关操作
    // 访问元素相关操作
    reference operator[](size_type n) {
        MYSTL_DEBUG(n <= size_);
        if (n == size_)
            *(buffer_ + n) = value_type();
        return *(buffer_ + n);
    }
    const_reference operator[](size_type n) const {
        MYSTL_DEBUG(n <= size_);
        if (n == size_)
            *(buffer_ + n) = value_type();
        return *(buffer_ + n);
    }

    reference at(size_type n) {
        THROW_OUT_OF_RANGE_IF(n >= size_,
                              "basic_string<Char, Traits>::at()"
                              "subscript out of range");
        // 如果没有抛出异常，返回指定位置的字符的引用
        return (*this)[n];
    }
    const_reference at(size_type n) const {
        THROW_OUT_OF_RANGE_IF(n >= size_,
                              "basic_string<Char, Traits>::at()"
                              "subscript out of range");
        return (*this)[n];
    }

    reference front() {
        MYSTL_DEBUG(!empty());
        return *begin();
    }
    const_reference front() const {
        MYSTL_DEBUG(!empty());
        return *begin();
    }

    reference back() {
        MYSTL_DEBUG(!empty());
        return *(end() - 1);
    }
    const_reference back() const {
        MYSTL_DEBUG(!empty());
        return *(end() - 1);
    }

    const_pointer data() const noexcept { return to_raw_pointer(); }
    const_pointer c_str() const noexcept { return to_raw_pointer(); }

    // 添加删除相关操作
    // insert
    iterator insert(const_iterator pos, value_type ch);
    iterator insert(const_iterator pos, size_type count, value_type ch);

    template <class Iter>
    iterator insert(const_iterator pos, Iter first, Iter last);

    // push_back / pop_back
    void push_back(value_type ch) { append(1, ch); }
    void pop_back() {
        MYSTL_DEBUG(!empty());
        --size_;
    }

    // append
    basic_string& append(size_type count, value_type ch);

    basic_string& append(const basic_string& str) {
        return append(str, 0, str.size_);
    }
    basic_string& append(const basic_string& str, size_type pos) {
        return append(str, pos, str.size_ - pos);
    }
    basic_string& append(const basic_string& str,
                         size_type pos,
                         size_type count);

    basic_string& append(const_pointer s) {
        return append(s, char_traits::length(s));
    }
    basic_string& append(const_pointer s, size_type count);

    template <class Iter,
              typename std::enable_if<mystl::is_input_iterator<Iter>::value,
                                      int>::type = 0>
    basic_string& append(Iter first, Iter last) {
        return append_range(first, last);
    }

    // erase /clear
    iterator erase(const_iterator pos);
    iterator erase(const_iterator first, const_iterator last);

    // resize
    void resize(size_type count) { resize(count, value_type()); }
    void resize(size_type count, value_type ch);

    void clear() noexcept { size_ = 0; }

    // basic_string 相关操作

    // compare
    int compare(const basic_string& other) const;
    int compare(size_type pos1,
                size_type count1,
                const basic_string& other) const;
    int compare(size_type pos1,
                size_type count1,
                const basic_string& other,
                size_type pos2,
                size_type count2 = npos) const;
    int compare(const_pointer s) const;
    int compare(size_type pos1, size_type count1, const_pointer s) const;
    int compare(size_type pos1,
                size_type count1,
                const_pointer s,
                size_type count2) const;

    // substr
    basic_string substr(size_type index, size_type count = npos) {
        count = mystl::min(count, size_ - index);
        return basic_string(buffer_ + index, buffer_ + index + count);
    }

    // replace
    basic_string& replace(size_type pos,
                          size_type count,
                          const basic_string& str) {
        THROW_OUT_OF_RANGE_IF(
            pos > size_,
            "basic_string<Char, Traits>::replace's pos out of range");
        return replace_cstr(buffer_ + pos, count, str.buffer_, str.size_);
    }
    basic_string& replace(const_iterator first,
                          const_iterator last,
                          const basic_string& str) {
        MYSTL_DEBUG(begin() <= first && last <= end() && first <= last);
        return replace_cstr(first, static_cast<size_type>(last - first),
                            str.buffer_, str.size_);
    }

    basic_string& replace(size_type pos, size_type count, const_pointer str) {
        THROW_OUT_OF_RANGE_IF(
            pos > size_,
            "basic_string<Char, Traits>::replace's pos out of range");
        return replace_cstr(buffer_ + pos, count, str,
                            char_traits::length(str));
    }
    basic_string& replace(const_iterator first,
                          const_iterator last,
                          const_pointer str) {
        MYSTL_DEBUG(begin() <= first && last <= end() && first <= last);
        return replace_cstr(first, static_cast<size_type>(last - first), str,
                            char_traits::length(str));
    }

    basic_string& replace(size_type pos,
                          size_type count,
                          const_pointer str,
                          size_type count2) {
        THROW_OUT_OF_RANGE_IF(
            pos > size_,
            "basic_string<Char, Traits>::replace's pos out of range");
        return replace_cstr(buffer_ + pos, count, str, count2);
    }
    basic_string& replace(const_iterator first,
                          const_iterator last,
                          const_pointer str,
                          size_type count) {
        MYSTL_DEBUG(begin() <= first && last <= end() && first <= last);
        return replace_cstr(first, static_cast<size_type>(last - first), str,
                            count);
    }

    basic_string& replace(size_type pos,
                          size_type count,
                          size_type count2,
                          value_type ch) {
        THROW_OUT_OF_RANGE_IF(
            pos > size_,
            "basic_string<Char, Traits>::replace's pos out of range");
        return replace_fill(buffer_ + pos, count, count2, ch);
    }
    basic_string& replace(const_iterator first,
                          const_iterator last,
                          size_type count,
                          value_type ch) {
        MYSTL_DEBUG(begin() <= first && last <= end() && first <= last);
        return replace_fill(first, static_cast<size_type>(last - first), count,
                            ch);
    }

    basic_string& replace(size_type pos1,
                          size_type count1,
                          const basic_string& str,
                          size_type pos2,
                          size_type count2 = npos) {
        THROW_OUT_OF_RANGE_IF(
            pos1 > size_ || pos2 > str.size_,
            "basic_string<Char, Traits>::replace's pos out of range");
        return replace_cstr(buffer_ + pos1, count1, str.buffer_ + pos2, count2);
    }

    template <class Iter,
              typename std::enable_if<mystl::is_input_iterator<Iter>::value,
                                      int>::type = 0>
    basic_string& replace(const_iterator first,
                          const_iterator last,
                          Iter first2,
                          Iter last2) {
        MYSTL_DEBUG(begin() <= first && last <= end() && first <= last);
        return replace_copy(first, last, first2, last2);
    }

    // reverse
    void reverse() noexcept;

    // swap
    void swap(basic_string& rhs) noexcept;

    // 查找相关操作

    // find
    size_type find(value_type ch, size_type pos = 0) const noexcept;
    size_type find(const_pointer str, size_type pos = 0) const noexcept;
    size_type find(const_pointer str,
                   size_type pos,
                   size_type count) const noexcept;
    size_type find(const basic_string& str, size_type pos = 0) const noexcept;

    // rfind
    size_type rfind(value_type ch, size_type pos = npos) const noexcept;
    size_type rfind(const_pointer str, size_type pos = npos) const noexcept;
    size_type rfind(const_pointer str,
                    size_type pos,
                    size_type count) const noexcept;
    size_type rfind(const basic_string& str,
                    size_type pos = npos) const noexcept;

    // find_first_of
    size_type find_first_of(value_type ch, size_type pos = 0) const noexcept;
    size_type find_first_of(const_pointer s, size_type pos = 0) const noexcept;
    size_type find_first_of(const_pointer s,
                            size_type pos,
                            size_type count) const noexcept;
    size_type find_first_of(const basic_string& str,
                            size_type pos = 0) const noexcept;

    // find_first_not_of
    size_type find_first_not_of(value_type ch,
                                size_type pos = 0) const noexcept;
    size_type find_first_not_of(const_pointer s,
                                size_type pos = 0) const noexcept;
    size_type find_first_not_of(const_pointer s,
                                size_type pos,
                                size_type count) const noexcept;
    size_type find_first_not_of(const basic_string& str,
                                size_type pos = 0) const noexcept;

    // find_last_of
    size_type find_last_of(value_type ch, size_type pos = 0) const noexcept;
    size_type find_last_of(const_pointer s, size_type pos = 0) const noexcept;
    size_type find_last_of(const_pointer s,
                           size_type pos,
                           size_type count) const noexcept;
    size_type find_last_of(const basic_string& str,
                           size_type pos = 0) const noexcept;

    // find_last_not_of
    size_type find_last_not_of(value_type ch, size_type pos = 0) const noexcept;
    size_type find_last_not_of(const_pointer s,
                               size_type pos = 0) const noexcept;
    size_type find_last_not_of(const_pointer s,
                               size_type pos,
                               size_type count) const noexcept;
    size_type find_last_not_of(const basic_string& str,
                               size_type pos = 0) const noexcept;

    // count
    size_type count(value_type ch, size_type pos = 0) const noexcept;

public:
    // 重载operator+=
    basic_string& operator+=(const basic_string& str) { return append(str); }
    basic_string& operator+=(value_type ch) { return append(1, ch); }
    basic_string& operator+=(const_pointer str) {
        return append(str, str + char_traits::length(str));
    }

    // 重载oprator >> /  operator <<
    // 输入流重载运算符`>>`，用于将输入流中的数据读取到`basic_string`类型的对象中
    // `friend`关键字表示这个函数是`basic_string`类的友元函数，可以访问`basic_string`类的私有成员
    friend std::istream& operator>>(std::istream& is, basic_string& str) {
        // buf用于存储输入流中的数据
        value_type* buf = new value_type[4096];
        // 用输入流对象 `is` 的 `>>` 运算符将数据读取到`buf` 中
        is >> buf;
        // 根据 `buf` 创建一个临时的 `basic_string` 对象 `tmp`
        basic_string tmp(buf);
        // 将`tmp` 对象移动到传入的 `basic_string` 对象 `str`
        str = std::move(tmp);
        // 释放buf内存空间
        delete[] buf;
        // 返回输入流对象is
        return is;
    }

    // 重载输出运算符，将一个 `basic_string` 对象输出到 `std::ostream` 流中
    // 返回这个流对象。这个重载函数的作用是方便我们在输出 `basic_string`
    // 对象时使用 `<<` 运算符，而不需要手动遍历每一个字符并输出
    friend std::ostream& operator<<(std::ostream& os, const basic_string& str) {
        // 遍历 `basic_string` 对象的每一个字符，并将其逐个输出到流中
        for (size_type i = 0; i < str.size_; ++i) {
            os << *(str.buffer_ + i);
        }
        return os;
    }

private:
    // helper functions

    // init/destroy
    void try_init() noexcept;
    void fill_init(size_type n, value_type ch);

    template <class Iter>
    void copy_init(Iter first, Iter last, mystl::input_iterator_tag);
    template <class Iter>
    void copy_init(Iter first, Iter last, mystl::forward_iterator_tag);

    void init_from(const_pointer src, size_type pos, size_type n);

    void destroy_buffer();

    // get raw pointer
    const_pointer to_raw_pointer() const;

    // shrink_to_fit
    void reinsert(size_type size);

    // append
    // append
    template <class Iter>
    basic_string& append_range(Iter first, Iter last);

    // compare
    int compare_cstr(const_pointer s1,
                     size_type n1,
                     const_pointer s2,
                     size_type n2) const;

    // replace
    basic_string& replace_cstr(const_iterator first,
                               size_type count1,
                               const_pointer str,
                               size_type count2);
    basic_string& replace_fill(const_iterator first,
                               size_type count1,
                               size_type count2,
                               value_type ch);
    template <class Iter>
    basic_string& replace_copy(const_iterator first,
                               const_iterator last,
                               Iter first2,
                               Iter last2);

    // reallocate
    void reallocate(size_type need);
    iterator reallocate_and_fill(iterator pos, size_type n, value_type ch);
    iterator reallocate_and_copy(iterator pos,
                                 const_iterator first,
                                 const_iterator last);
};

/* **************************************** */
// 复制复制操作符
template <class CharType, class CharTraits>
basic_string<CharType, CharTraits>&
basic_string<CharType, CharTraits>::operator=(const basic_string& rhs) {
    if (this != &rhs) {
        basic_string tmp(rhs);
        swap(tmp);
    }
    return *this;
}

template <class CharType, class CharTraits>
basic_string<CharType, CharTraits>&
basic_string<CharType, CharTraits>::operator=(basic_string&& rhs) noexcept {
    destroy_buffer();
    buffer_ = rhs.buffer_;
    size_ = rhs.size_;
    cap_ = rhs.cap_;
    rhs.buffer_ = nullptr;
    rhs.size_ = 0;
    rhs.cap_ = 0;
    return *this;
}

// 用一个字符串赋值
template <class CharType, class CharTraits>
basic_string<CharType, CharTraits>&
basic_string<CharType, CharTraits>::operator=(const_pointer str) {
    const size_type len = char_traits::length(str);
    if (cap_ < len) {
        auto new_buffer = data_allocator::allocate(len + 1);
        data_allocator::deallocate(buffer_);
        buffer_ = new_buffer;
        cap_ = len + 1;
    }
    char_traits::copy(buffer_, str, len);
    size_ = len;
    return *this;
}

// 用一个字符赋值
// 重载了赋值运算符，用于将一个字符赋值给一个字符串对象。具体来说，它接受一个
// `value_type` 类型的参数
// `ch`，也就是字符串中字符的类型，然后将这个字符赋值给当前字符串对象
template <class CharType, class CharTraits>
basic_string<CharType, CharTraits>&
basic_string<CharType, CharTraits>::operator=(value_type ch) {
    if (cap_ < 1) {
        // 如果当前字符串对象的容量不足以存储这个字符，它会重新分配一块大小为 2
        // 的内存空间，并将原来的内存空间释放掉
        auto new_buffer = data_allocator::allocate(2);
        data_allocator::deallocate(buffer_);
        buffer_ = new_buffer;
        cap_ = 2;
    }
    *buffer_ = ch;
    size_ = 1;
    // 返回一个指向当前字符串对象的引用
    return *this;
}

// 预留储存空间
template <class CharType, class CharTraits>
void basic_string<CharType, CharTraits>::reserve(size_type n) {
    if (cap_ < n) {
        THROW_LENGTH_ERROR_IF(n > max_size(),
                              "n can not larger than max_size() in "
                              "basic_string<Char,Traits>::reserve(n)");
        auto new_buffer = data_allocator::allocate(n);
        char_traits::move(new_buffer, buffer_, size_);
        buffer_ = new_buffer;
        cap_ = n;
    }
}

// 减少不用的空间
template <class CharType, class CharTraits>
void basic_string<CharType, CharTraits>::shrink_to_fit() {
    if (size_ != cap_) {
        reinsert(size_);
    }
}

// 在 pos 处插入一个元素
template <class CharType, class CharTraits>
typename basic_string<CharType, CharTraits>::iterator
basic_string<CharType, CharTraits>::insert(const_iterator pos, value_type ch) {
    iterator r = const_cast<iterator>(pos);
    if (size_ == cap_) {
        // 重新分配地址空间并填充
        return reallocate_and_fill(r, 1, ch);
    }
    // 把r处开始的字符移到r+1处
    char_traits::move(r + 1, r, end() - r);
    ++size_;
    *r = ch;
    return r;
}

// 在 pos 处插入 n 个元素
template <class CharType, class CharTraits>
typename basic_string<CharType, CharTraits>::iterator
basic_string<CharType, CharTraits>::insert(const_iterator pos,
                                           size_type count,
                                           value_type ch) {
    iterator r = const_cast<iterator>(pos);
    if (count == 0) {
        return r;
    }
    if (cap_ - size_ < count) {
        return reallocate_and_fill(r, count, ch);
    }
    if (pos == end()) {
        char_traits::fill(end(), ch, count);
        size_ += count;
        return r;
    }
    // 把r处的数据移到r+count处
    char_traits::move(r + count, r, count);
    // 在r处填充count个ch
    char_traits::fill(r, ch, count);
    // 大小+count
    size_ += count;
    return r;
}

// 在 pos 处插入 [first, last) 内的元素
template <class CharType, class CharTraits>
template <class Iter>
typename basic_string<CharType, CharTraits>::iterator
basic_string<CharType, CharTraits>::insert(const_iterator pos,
                                           Iter first,
                                           Iter last) {
    iterator r = const_cast<iterator>(pos);
    const size_type len = mystl::distance(first, last);
    if (len == 0) {
        return r;
    }
    if (cap_ - size_ < len) {
        // 如果剩余空间不够，就要重新分配空间
        return reallocate_and_copy(r, first, last);
    }
    if (pos == end()) {
        // 直接从end_处开始
        mystl::uninitialized_copy(first, last, end());
        size_ += len;
        return r;
    }
    // 不然就移动，再插入
    char_traits::move(r + len, r, len);
    mystl::uninitialized_copy(first, last, r);
    size_ += len;
    return r;
}

// 在末尾添加 count 个 ch
template <class CharType, class CharTraits>
basic_string<CharType, CharTraits>& basic_string<CharType, CharTraits>::append(
    size_type count,
    value_type ch) {
    THROW_LENGTH_ERROR_IF(size_ > max_size() - count,
                          "basic_string<Char, Tratis>'s size too big");
    if (cap_ - size_ < count) {
        // 大小不够，重新分配地址空间
        reallocate(count);
    }
    // 从buffer_+size_处开始填充count个ch
    char_traits::fill(buffer_ + size_, ch, count);
    size_ += count;
    return *this;
}

// 在末尾添加 [str[pos] str[pos+count]) 一段
template <class CharType, class CharTraits>
basic_string<CharType, CharTraits>& basic_string<CharType, CharTraits>::append(
    const basic_string& str,
    size_type pos,
    size_type count) {
    THROW_LENGTH_ERROR_IF(size_ > max_size() - count,
                          "basic_string<Char, Tratis>'s size too big");
    if (count == 0) {
        return *this;
    }
    if (count == 0)
        return *this;
    if (cap_ - size_ < count) {
        reallocate(count);
    }
    // 将str.buffer_ + pos开始的count个字符复制到buffer_+size_
    char_traits::copy(buffer_ + size_, str.buffer_ + pos, count);
    size_ += count;
    return *this;
}

// 在末尾添加 [s, s+count) 一段
template <class CharType, class CharTraits>
basic_string<CharType, CharTraits>& basic_string<CharType, CharTraits>::append(
    const_pointer s,
    size_type count) {
    THROW_LENGTH_ERROR_IF(size_ > max_size() - count,
                          "basic_string<Char, Tratis>'s size too big");
    // 容量不足，重新分配
    if (cap_ - size_ < count) {
        reallocate(count);
    }
    // copy到末尾
    char_traits::copy(buffer_ + size_, s, count);
    size_ += count;
    return *this;
}

// 删除 pos 处的元素
template <class CharType, class CharTraits>
typename basic_string<CharType, CharTraits>::iterator
basic_string<CharType, CharTraits>::erase(const_iterator pos) {
    MYSTL_DEBUG(pos != end());
    iterator r = const_cast<iterator>(pos);
    // 把pos+1处的end() - pos - 1个字符移到r
    char_traits::move(r, pos + 1, end() - pos - 1);
    --size_;
    return r;
}

// 删除 [first, last) 的元素
template <class CharType, class CharTraits>
typename basic_string<CharType, CharTraits>::iterator
basic_string<CharType, CharTraits>::erase(const_iterator first,
                                          const_iterator last) {
    if (first == begin() && last == end()) {
        // 直接令size_=0
        clear();
        return end();
    }
    const size_type n = end() - last;
    iterator r = const_cast<iterator>(first);
    // 直接把last开始的n个字符移到first处s
    char_traits::move(r, last, n);
    size_ -= (last - first);
    return r;
}

// 重置容器大小
template <class CharType, class CharTraits>
void basic_string<CharType, CharTraits>::resize(size_type count,
                                                value_type ch) {
    if (count < size_) {
        // 小需要清除多的空间
        erase(buffer_ + count, buffer_ + size_);
    } else {
        // 大需要填充
        append(count - size_, ch);
    }
}

// 比较两个 basic_string，小于返回 -1，大于返回 1，等于返回 0
template <class CharType, class CharTraits>
int basic_string<CharType, CharTraits>::compare(
    const basic_string& other) const {
    return compare_cstr(buffer_, size_, other.buffer_, other.size_);
}

// 从 pos1 下标开始的 count1 个字符跟另一个 basic_string 比较
template <class CharType, class CharTraits>
int basic_string<CharType, CharTraits>::compare(
    size_type pos1,
    size_type count1,
    const basic_string& other) const {
    // size_-pos1可能小于count1，也就是不足count1个字符
    auto n1 = mystl::min(count1, size_ - pos1);
    return compare_cstr(buffer_ + pos1, n1, other.buffer_, other.size_);
}

// 从 pos1 下标开始的 count1 个字符跟另一个 basic_string 下标 pos2 开始的 count2
// 个字符比较
template <class CharType, class CharTraits>
int basic_string<CharType, CharTraits>::compare(size_type pos1,
                                                size_type count1,
                                                const basic_string& other,
                                                size_type pos2,
                                                size_type count2) const {
    auto n1 = mystl::min(count1, size_ - pos1);
    auto n2 = mystl::min(count2, other.size_ - pos2);
    return compare_cstr(buffer_, n1, other.buffer_, n2);
}

// 跟一个字符串比较
template <class CharType, class CharTraits>
int basic_string<CharType, CharTraits>::compare(const_pointer s) const {
    auto n2 = char_traits::length(s);
    return compare_cstr(buffer_, size_, s, n2);
}

// 从下标 pos1 开始的 count1 个字符跟另一个字符串比较
template <class CharType, class CharTraits>
int basic_string<CharType, CharTraits>::compare(size_type pos1,
                                                size_type count1,
                                                const_pointer s) const {
    auto n1 = mystl::min(count1, size_ - pos1);
    auto n2 = char_traits::length(s);
    return compare_cstr(buffer_, n1, s, n2);
}

// 从下标 pos1 开始的 count1 个字符跟另一个字符串的前 count2 个字符比较
template <class CharType, class CharTraits>
int basic_string<CharType, CharTraits>::compare(size_type pos1,
                                                size_type count1,
                                                const_pointer s,
                                                size_type count2) const {
    auto n1 = mystl::min(count1, size_ - pos1);
    return compare_cstr(buffer_, n1, s, count2);
}

// 反转 basic_string
template <class CharType, class CharTraits>
void basic_string<CharType, CharTraits>::reverse() noexcept {
    for (auto i = begin(), j = end(); i < j;) {
        mystl::iter_swap(i++, j--);
    }
}

// 交换连个basic_string
template <class CharType, class CharTraits>
void basic_string<CharType, CharTraits>::swap(basic_string& rhs) noexcept {
    if (this != &rhs) {
        mystl::swap(buffer_, rhs.buffer_);
        mystl::swap(size_, rhs.size_);
        mystl::swap(cap_, rhs.cap_);
    }
}

// 从下标 pos 开始查找字符为 ch 的元素，若找到返回其下标，否则返回 npos
template <class CharType, class CharTraits>
typename basic_string<CharType, CharTraits>::size_type
basic_string<CharType, CharTraits>::find(value_type ch,
                                         size_type pos) const noexcept {
    for (auto i = pos; i < size_; ++i) {
        if (*(buffer_ + 1) == ch) {
            return i;
        }
    }
    return npos;
}

// 从下标 pos 开始查找字符串 str，若找到返回起始位置的下标，否则返回 npos
template <class CharType, class CharTraits>
typename basic_string<CharType, CharTraits>::size_type
basic_string<CharType, CharTraits>::find(const_pointer str,
                                         size_type pos) const noexcept {
    const auto len = char_traits::length(str);
    if (len == 0)
        return pos;
    if (size_ - pos < len)
        return npos;
    const auto left = size_ - len;
    for (auto i = pos; i <= left; ++i) {
        if (*(buffer_ + i) == *str) {
            size_type j = 1;
            for (; j < len; ++j) {
                if (*(buffer_ + i + j) != *(str + j))
                    break;
            }
            if (j == len)
                return i;
        }
    }
    return npos;
}

// 从下标 pos 开始查找字符串 str 的前 count
// 个字符，若找到返回起始位置的下标，否则返回 npos
template <class CharType, class CharTraits>
typename basic_string<CharType, CharTraits>::size_type
basic_string<CharType, CharTraits>::find(const_pointer str,
                                         size_type pos,
                                         size_type count) const noexcept {
    if (count == 0) {
        return pos;
    }
    if (size_ - pos < count) {
        // 从pos开始的总字符数小于count一定找不到
        return npos;
    }
    const auto left = size_ - count;
    // 只要遍历到size_-count，因为最后预留count个
    for (auto i = pos; i <= left; ++i) {
        if (*(buffer_ + i) == *str) {
            size_type j = 1;
            for (; j < count; ++j) {
                if (*(buffer_ + i + j) != *(str + j)) {
                    break;
                }
            }
            // 完整匹配上，return i
            if (j == count) {
                return i;
            }
        }
    }
    return npos;
}

// 从下标 pos 开始查找字符串 str，若找到返回起始位置的下标，否则返回 npos
template <class CharType, class CharTraits>
typename basic_string<CharType, CharTraits>::size_type
basic_string<CharType, CharTraits>::find(const basic_string& str,
                                         size_type pos) const noexcept {
    // 现在的count变成了str的大小
    const size_type count = str.size_;
    if (count == 0)
        return pos;
    if (size_ - pos < count)
        return npos;
    const auto left = size_ - count;
    for (auto i = pos; i <= left; ++i) {
        if (*(buffer_ + i) == str.front()) {
            size_type j = 1;
            for (; j < count; ++j) {
                if (*(buffer_ + i + j) != str[j])
                    break;
            }
            if (j == count)
                return i;
        }
    }
    return npos;
}

// 从下标 pos 开始反向查找值为 ch 的元素，与 find 类似
template <class CharType, class CharTraits>
typename basic_string<CharType, CharTraits>::size_type
basic_string<CharType, CharTraits>::rfind(value_type ch,
                                          size_type pos) const noexcept {
    if (pos >= size_) {
        // 太大了置为尾巴
        pos = size_ - 1;
    }
    for (auto i = pos; i != 0; --i) {
        if (*(buffer_ + 1) == ch) {
            // 反向查找，找到了就返回
            return i;
        }
    }
    // 验证第1个是否相等
    return front() == ch ? 0 : npos;
}

// 从下标 pos 开始反向查找字符串 str，与 find 类似
template <class CharType, class CharTraits>
typename basic_string<CharType, CharTraits>::size_type
basic_string<CharType, CharTraits>::rfind(const_pointer str,
                                          size_type pos) const noexcept {
    if (pos >= size_)
        pos = size_ - 1;
    const size_type len = char_traits::length(str);
    switch (len) {
        case 0:
            // 字符串长度为0，直接返回pos
            return pos;
        case 1: {
            // 1个字符，返回i，同上一个函数
            for (auto i = pos; i != 0; --i) {
                if (*(buffer_ + i) == *str)
                    return i;
            }
            return front() == *str ? 0 : npos;
        }
        default: {  // len >= 2
            // 多个字符，逐一反向查找匹配
            for (auto i = pos; i >= len - 1; --i) {
                if (*(buffer_ + i) == *(str + len - 1)) {
                    size_type j = 1;
                    for (; j < len; ++j) {
                        if (*(buffer_ + i - j) != *(str + len - j - 1))
                            break;
                    }
                    if (j == len)
                        return i - len + 1;
                }
            }
            break;
        }
    }
    return npos;
}

// 从下标 pos 开始反向查找字符串 str 前 count 个字符，与 find 类似
template <class CharType, class CharTraits>
typename basic_string<CharType, CharTraits>::size_type
basic_string<CharType, CharTraits>::rfind(const_pointer str,
                                          size_type pos,
                                          size_type count) const noexcept {
    if (count == 0)
        return pos;
    if (pos >= size_)
        pos = size_ - 1;
    // 因为反向查找，pos本身相对于beign要至少为count
    if (pos < count - 1)
        return npos;
    for (auto i = pos; i >= count - 1; --i) {
        if (*(buffer_ + i) == *(str + count - 1)) {
            size_type j = 1;
            for (; j < count; ++j) {
                if (*(buffer_ + i - j) != *(str + count - j - 1))
                    break;
            }
            if (j == count)
                return i - count + 1;
        }
    }
    return npos;
}

// 从下标 pos 开始反向查找字符串 str，与 find 类似
template <class CharType, class CharTraits>
typename basic_string<CharType, CharTraits>::size_type
basic_string<CharType, CharTraits>::rfind(const basic_string& str,
                                          size_type pos) const noexcept {
    const size_type count = str.size_;
    if (pos >= size_)
        pos = size_ - 1;
    if (count == 0)
        return pos;
    if (pos < count - 1)
        return npos;
    for (auto i = pos; i >= count - 1; --i) {
        if (*(buffer_ + i) == str[count - 1]) {
            size_type j = 1;
            for (; j < count; ++j) {
                if (*(buffer_ + i - j) != str[count - j - 1])
                    break;
            }
            if (j == count)
                return i - count + 1;
        }
    }
    return npos;
}

// 从下标 pos 开始查找 ch 出现的第一个位置
template <class CharType, class CharTraits>
typename basic_string<CharType, CharTraits>::size_type
basic_string<CharType, CharTraits>::find_first_of(value_type ch, size_type pos)
    const noexcept {
    for (auto i = pos; i < size_; ++i) {
        if (*(buffer_ + i) == ch)
            return i;
    }
    return npos;
}

// 从下标 pos 开始查找字符串 s 其中的一个字符出现的第一个位置
template <class CharType, class CharTraits>
typename basic_string<CharType, CharTraits>::size_type
basic_string<CharType, CharTraits>::find_first_of(
    const_pointer s,
    size_type pos) const noexcept {
    const size_type len = char_traits::length(s);
    // 每个pos指向的字符都去匹配s中所有字符
    for (auto i = pos; i < size_; ++i) {
        value_type ch = *(buffer_ + i);
        for (size_type j = 0; j < len; ++j) {
            if (ch == *(s + j))
                return i;
        }
    }
    return npos;
}

// 从下标 pos 开始查找字符串 s 前count个字符中的一个字符出现的第1个位置
// 就是把上一个函数的len用count自己指定了
template <class CharType, class CharTraits>
typename basic_string<CharType, CharTraits>::size_type
basic_string<CharType, CharTraits>::find_first_of(
    const_pointer s,
    size_type pos,
    size_type count) const noexcept {
    for (auto i = pos; i < size_; ++i) {
        value_type ch = *(buffer_ + i);
        for (size_type j = 0; j < count; ++j) {
            if (ch == *(s + j))
                return i;
        }
    }
    return npos;
}

// 从下标 pos 开始查找字符串 str 其中一个字符出现的第一个位置
// 前面函数的s是const_pointer，这里的str是basic_string
template <class CharType, class CharTraits>
typename basic_string<CharType, CharTraits>::size_type
basic_string<CharType, CharTraits>::find_first_of(
    const basic_string& str,
    size_type pos) const noexcept {
    for (auto i = pos; i < size_; ++i) {
        value_type ch = *(buffer_ + i);
        for (size_type j = 0; j < str.size_; ++j) {
            if (ch == str[j])
                return i;
        }
    }
    return npos;
}

// 从下标 pos 开始查找与 ch 不相等的第一个位置
template <class CharType, class CharTraits>
typename basic_string<CharType, CharTraits>::size_type
basic_string<CharType, CharTraits>::find_first_not_of(
    value_type ch,
    size_type pos) const noexcept {
    for (auto i = pos; i < size_; ++i) {
        if (*(buffer_ + i) != ch)
            return i;
    }
    return npos;
}

// 从下标 pos 开始查找与字符串 s 其中一个字符不相等的第一个位置
template <class CharType, class CharTraits>
typename basic_string<CharType, CharTraits>::size_type
basic_string<CharType, CharTraits>::find_first_not_of(
    const_pointer s,
    size_type pos) const noexcept {
    const size_type len = char_traits::length(s);
    for (auto i = pos; i < size_; ++i) {
        value_type ch = *(buffer_ + i);
        for (size_type j = 0; j < len; ++j) {
            if (ch != *(s + j))
                return i;
        }
    }
    return npos;
}

// 从下标 pos 开始查找与字符串 s 前 count 个字符中不相等的第一个位置
template <class CharType, class CharTraits>
typename basic_string<CharType, CharTraits>::size_type
basic_string<CharType, CharTraits>::find_first_not_of(
    const_pointer s,
    size_type pos,
    size_type count) const noexcept {
    for (auto i = pos; i < size_; i++) {
        value_type ch = *(buffer_ + i);
        for (size_type j = 0; j < count; ++j) {
            if (ch != *(s + j)) {
                return i;
            }
        }
    }
    return npos;
}

// 从下标 pos 开始查找与字符串 str 的字符中不相等的第一个位置
template <class CharType, class CharTraits>
typename basic_string<CharType, CharTraits>::size_type
basic_string<CharType, CharTraits>::find_first_not_of(
    const basic_string& str,
    size_type pos) const noexcept {
    for (auto i = pos; i < size_; ++i) {
        value_type ch = *(buffer_ + i);
        for (size_type j = 0; j < str.size_; ++j) {
            if (ch != str[j])
                return i;
        }
    }
    return npos;
}

// 从下标 pos 开始查找与 ch 相等的最后一个位置
// i从最后反着来就行
template <class CharType, class CharTraits>
typename basic_string<CharType, CharTraits>::size_type
basic_string<CharType, CharTraits>::find_last_of(value_type ch,
                                                 size_type pos) const noexcept {
    for (auto i = size_ - 1; i >= pos; --i) {
        if (*(buffer_ + i) == ch)
            return i;
    }
    return npos;
}

// 从下标 pos 开始查找与字符串 s 其中一个字符相等的最后一个位置
template <class CharType, class CharTraits>
typename basic_string<CharType, CharTraits>::size_type
basic_string<CharType, CharTraits>::find_last_of(const_pointer s,
                                                 size_type pos) const noexcept {
    const size_type len = char_traits::length(s);
    for (auto i = size_ - 1; i >= pos; --i) {
        value_type ch = *(buffer_ + i);
        for (size_type j = 0; j < len; ++j) {
            if (ch == *(s + j))
                return i;
        }
    }
    return npos;
}

// 从下标 pos 开始查找与字符串 s 前 count 个字符中相等的最后一个位置
template <class CharType, class CharTraits>
typename basic_string<CharType, CharTraits>::size_type
basic_string<CharType, CharTraits>::find_last_of(
    const_pointer s,
    size_type pos,
    size_type count) const noexcept {
    for (auto i = size_ - 1; i >= pos; --i) {
        value_type ch = *(buffer_ + i);
        for (size_type j = 0; j < count; ++j) {
            if (ch == *(s + j))
                return i;
        }
    }
    return npos;
}

// 从下标 pos 开始查找与字符串 str 字符中相等的最后一个位置
template <class CharType, class CharTraits>
typename basic_string<CharType, CharTraits>::size_type
basic_string<CharType, CharTraits>::find_last_of(const basic_string& str,
                                                 size_type pos) const noexcept {
    for (auto i = size_ - 1; i >= pos; --i) {
        value_type ch = *(buffer_ + i);
        for (size_type j = 0; j < str.size_; ++j) {
            if (ch == str[j])
                return i;
        }
    }
    return npos;
}

// 从下标 pos 开始查找与 ch 字符不相等的最后一个位置
template <class CharType, class CharTraits>
typename basic_string<CharType, CharTraits>::size_type
basic_string<CharType, CharTraits>::find_last_not_of(
    value_type ch,
    size_type pos) const noexcept {
    for (auto i = size_ - 1; i >= pos; --i) {
        if (*(buffer_ + i) != ch)
            return i;
    }
    return npos;
}

// 从下标 pos 开始查找与字符串 s 的字符中不相等的最后一个位置
template <class CharType, class CharTraits>
typename basic_string<CharType, CharTraits>::size_type
basic_string<CharType, CharTraits>::find_last_not_of(
    const_pointer s,
    size_type pos) const noexcept {
    const size_type len = char_traits::length(s);
    for (auto i = size_ - 1; i >= pos; --i) {
        value_type ch = *(buffer_ + i);
        for (size_type j = 0; j < len; ++j) {
            if (ch != *(s + j))
                return i;
        }
    }
    return npos;
}

// 从下标 pos 开始查找与字符串 s 前 count 个字符中不相等的最后一个位置
template <class CharType, class CharTraits>
typename basic_string<CharType, CharTraits>::size_type
basic_string<CharType, CharTraits>::find_last_not_of(
    const_pointer s,
    size_type pos,
    size_type count) const noexcept {
    for (auto i = size_ - 1; i >= pos; --i) {
        value_type ch = *(buffer_ + i);
        for (size_type j = 0; j < count; ++j) {
            if (ch != *(s + j))
                return i;
        }
    }
    return npos;
}

// 从下标 pos 开始查找与字符串 str 字符中不相等的最后一个位置
template <class CharType, class CharTraits>
typename basic_string<CharType, CharTraits>::size_type
basic_string<CharType, CharTraits>::find_last_not_of(
    const basic_string& str,
    size_type pos) const noexcept {
    for (auto i = size_ - 1; i >= pos; --i) {
        value_type ch = *(buffer_ + i);
        for (size_type j = 0; j < str.size_; ++j) {
            if (ch != str[j])
                return i;
        }
    }
    return npos;
}

// 返回从下标 pos 开始字符为 ch 的元素出现的次数
template <class CharType, class CharTraits>
typename basic_string<CharType, CharTraits>::size_type
basic_string<CharType, CharTraits>::count(value_type ch,
                                          size_type pos) const noexcept {
    size_type n = 0;
    for (auto i = pos; i < size_; ++i) {
        if (*(buffer_ + i) == ch)
            ++n;
    }
    return n;
}

/* ***************************************** */
// helper function

// 尝试初始化一段 buffer，若分配失败则忽略，不会抛出异常
template <class CharType, class CharTraits>
void basic_string<CharType, CharTraits>::try_init() noexcept {
    try {
        buffer_ =
            data_allocator::allocate(static_cast<size_type>(STRING_INIT_SIZE));
        size_ = 0;
        cap_ = 0;
    } catch (...) {
        buffer_ = nullptr;
        size_ = 0;
        cap_ = 0;
        // no throw
    }
}

// fill_init函数
template <class CharType, class CharTraits>
void basic_string<CharType, CharTraits>::fill_init(size_type n, value_type ch) {
    const auto init_size =
        mystl::max(static_cast<size_type>(STRING_INIT_SIZE), n + 1);
    buffer_ = data_allocator::allocate(init_size);
    char_traits::fill(buffer_, ch, n);
    size_ = n;
    cap_ = init_size;
}

// copy_init 函数
// 拷贝构造
template <class CharType, class CharTraits>
template <class Iter>
void basic_string<CharType, CharTraits>::copy_init(Iter first,
                                                   Iter last,
                                                   mystl::input_iterator_tag) {
    size_type n = mystl::distance(first, last);
    // 决定string容器的容量
    const auto init_size =
        mystl::max(static_cast<size_type>(STRING_INIT_SIZE), n + 1);
    try {
        // 分配内存空间
        buffer_ = data_allocator::allocate(init_size);
        size_ = n;
        cap_ = init_size;
    } catch (...) {
        // 抛出异常
        buffer_ = nullptr;
        size_ = 0;
        cap_ = 0;
        throw;
    }
    // 逐一填充数据
    for (; n > 0; --n, ++first)
        append(*first);
}

template <class CharType, class CharTraits>
template <class Iter>
void basic_string<CharType, CharTraits>::copy_init(
    Iter first,
    Iter last,
    mystl::forward_iterator_tag) {
    const size_type n = mystl::distance(first, last);
    const auto init_size =
        mystl::max(static_cast<size_type>(STRING_INIT_SIZE), n + 1);
    try {
        buffer_ = data_allocator::allocate(init_size);
        size_ = n;
        cap_ = init_size;
        // uninitialized_copy将[first, last)拷贝到buffer_
        mystl::uninitialized_copy(first, last, buffer_);
    } catch (...) {
        buffer_ = nullptr;
        size_ = 0;
        cap_ = 0;
        throw;
    }
}

// init_from函数
// 从字符串src的pos位置复制count个字符到buffer_
template <class CharType, class CharTraits>
void basic_string<CharType, CharTraits>::init_from(const_pointer src,
                                                   size_type pos,
                                                   size_type count) {
    const auto init_size =
        mystl::max(static_cast<size_type>(STRING_INIT_SIZE), count + 1);
    buffer_ = data_allocator::allocate(init_size);
    char_traits::copy(buffer_, src + pos, count);
    size_ = count;
    cap_ = init_size;
}

// destroy_buffer 函数
template <class CharType, class CharTraits>
void basic_string<CharType, CharTraits>::destroy_buffer() {
    if (buffer_ != nullptr) {
        // 执行的是delete buffer_的操作
        data_allocator::deallocate(buffer_, cap_);
        buffer_ = nullptr;
        size_ = 0;
        cap_ = 0;
    }
}

// to_raw_pointer 函数
// 用于将当前字符串对象转换为一个C风格的字符串指针
template <class CharType, class CharTraits>
typename basic_string<CharType, CharTraits>::const_pointer
basic_string<CharType, CharTraits>::to_raw_pointer() const {
    // 在当前字符串的末尾添加一个空字符，以便将其转换为 C 风格的字符串
    *(buffer_ + size_) = value_type();
    // 返回指向当前字符串首字符的指针
    return buffer_;
}

// reinsert函数
// 重新分配内存空间，以容纳更多的字符，同时保留原有的字符内容
template <class CharType, class CharTraits>
void basic_string<CharType, CharTraits>::reinsert(size_type size) {
    auto new_buffer = data_allocator::allocate(size);
    try {
        // buffer_开始的size个复制到new_buffer
        char_traits::move(new_buffer, buffer_, size);
    } catch (...) {
        data_allocator::deallocate(new_buffer);
    }
    // new_buffer再重新赋给buffer_
    buffer_ = new_buffer;
    size_=size;
    cap_=size;
}

// append_range，末尾追加一段 [first, last) 内的字符
template <class CharType, class CharTraits>
template <class Iter>
basic_string<CharType, CharTraits>&
basic_string<CharType, CharTraits>::append_range(Iter first, Iter last) {
    const size_type n = mystl::distance(first, last);
    THROW_LENGTH_ERROR_IF(size_ > max_size() - n,
                          "basic_string<Char, Tratis>'s size too big");
    if (cap_ - size_ < n) {
        reallocate(n);
    }
    // 把 [first, first + n)区间上的元素拷贝到 [buffer_ + size_, buffer_ + size_
    // + n)上
    mystl::uninitialized_copy_n(first, n, buffer_ + size_);
    size_ += n;
    return *this;
}

// 比较两个字符串是否相等
template <class CharType, class CharTraits>
int basic_string<CharType, CharTraits>::compare_cstr(const_pointer s1,
                                                     size_type n1,
                                                     const_pointer s2,
                                                     size_type n2) const {
    auto rlen = mystl::min(n1, n2);
    // 调用字符 traits 类的 compare 函数比较两个字符串的前 rlen 个字符是否相等
    auto res = char_traits::compare(s1, s2, rlen);
    // 如果不相等，直接返回比较结果
    if (res != 0)
        return res;
    // 如果前rlen个字符相等，就比较长度
    if (n1 < n2)
        return -1;
    if (n1 > n2)
        return 1;
    return 0;
}

// 把 first 开始的 count1 个字符替换成 str 开始的 count2 个字符
template <class CharType, class CharTraits>
basic_string<CharType, CharTraits>&
basic_string<CharType, CharTraits>::replace_cstr(const_iterator first,
                                                 size_type count1,
                                                 const_pointer str,
                                                 size_type count2) {
    if (static_cast<size_type>(cend() - first) < count1) {
        // 字符串从first开始到cend()，不足count1个字符
        count1 = cend() - first;
    }
    if (count1 < count2) {
        // add为需要添加的字符数
        const size_type add = count2 - count1;
        THROW_LENGTH_ERROR_IF(size_ > max_size() - add,
                              "basic_string<Char, Traits>'s size too big");
        if (size_ > cap_ - add) {
            // 剩余空间不足，需要重新分配add
            reallocate(add);
        }
        pointer r = const_cast<pointer>(first);
        // [first+count1,end())移到r+count2开始，空出count2个位置
        char_traits::move(r + count2, first + count1, end() - (first + count1));
        // 拷贝count2个
        char_traits::copy(r, str, count2);
        size_ += add;
    } else {
        pointer r = const_cast<pointer>(first);
        // [first+count1,end())移到r+count2开始，空出count2个位置
        char_traits::move(r + count2, first + count1, end() - (first + count1));
        char_traits::copy(r, str, count2);
        // size_减少了count1-count2个
        size_ -= count1 - count2;
    }
    return *this;
}

// 把 first 开始的 count1 个字符替换成 count2 个 ch 字符
// 把 first 开始的 count1 个字符替换成 count2 个 ch 字符
template <class CharType, class CharTraits>
basic_string<CharType, CharTraits>&
basic_string<CharType, CharTraits>::
replace_fill(const_iterator first, size_type count1, size_type count2, value_type ch)
{
  if (static_cast<size_type>(cend() - first) < count1)
  {
    count1 = cend() - first;
  }
  if (count1 < count2)
  {
    const size_type add = count2 - count1;
    THROW_LENGTH_ERROR_IF(size_ > max_size() - add,
                          "basic_string<Char, Traits>'s size too big");
    if (size_ > cap_ - add)
    {
      reallocate(add);
    }
    pointer r = const_cast<pointer>(first);
    char_traits::move(r + count2, first + count1, end() - (first + count1));
    // 只是把上一个函数的copy改成了fill
    char_traits::fill(r, ch, count2);
    size_ += add;
  }
  else
  {
    pointer r = const_cast<pointer>(first);
    char_traits::move(r + count2, first + count1, end() - (first + count1));
    char_traits::fill(r, ch, count2);
    size_ -= (count1 - count2);
  }
  return *this;
}

// 把 [first, last) 的字符替换成 [first2, last2)
template <class CharType, class CharTraits>
template <class Iter>
basic_string<CharType, CharTraits>&
basic_string<CharType, CharTraits>::replace_copy(const_iterator first,
                                                 const_iterator last,
                                                 Iter first2,
                                                 Iter last2) {
    size_type len1 = last - first;
    size_type len2 = last2 - first2;
    if (len1 < len2) {
        const size_type add = len2 - len2;
        THROW_LENGTH_ERROR_IF(size_ > max_size() - add,
                              "basic_string<Char, Traits>'s size too big");
        if (size_ > cap_ - add) {
            reallocate(add);
        }
        pointer r = const_cast<pointer>(first);
        char_traits::move(r + len2, first + len1, end() - (first + len1));
        char_traits::copy(r, first2, len2);
        size_ += add;
    } else {
        pointer r = const_cast<pointer>(first);
        char_traits::move(r + len2, first + len1, end() - (first + len1));
        char_traits::copy(r, first2, len2);
        size_ -= (len1 - len2);
    }
    return *this;
}

// reallocate 函数
// 重新分配一块容量较大的内存空间
template <class CharType, class CharTraits>
void basic_string<CharType, CharTraits>::reallocate(size_type need) {
    // 新容量为cap_+need, cap_+cap_/2中的较大值
    const auto new_cap = mystl::max(cap_ + need, cap_ + (cap_ >> 1));
    auto new_buffer = data_allocator::allocate(new_cap);
    // 当前的buffer_拷贝到new_buffer
    char_traits::move(new_buffer, buffer_, size_);
    // delete 旧的buffer_
    data_allocator::deallocate(buffer_);
    buffer_ = new_buffer;
    cap_ = new_cap;
}

// reallocate_and_fill 函数
template <class CharType, class CharTraits>
typename basic_string<CharType, CharTraits>::iterator
basic_string<CharType, CharTraits>::reallocate_and_fill(iterator pos,
                                                        size_type n,
                                                        value_type ch) {
    // r为pos相对buffer_的长度
    const auto r = pos - buffer_;
    const auto old_cap = cap_;
    // 设置新的容量
    const auto new_cap = mystl::max(old_cap + n, old_cap + (old_cap >> 1));
    auto new_buffer = data_allocator::allocate(new_cap);
    // 先buffer_中的r个数
    auto e1 = char_traits::move(new_buffer, buffer_, r) + r;
    // 再自己填充的n个ch字符
    auto e2 = char_traits::fill(e1, ch, n) + n;
    // 最后剩下的size_-r个
    char_traits::move(e2, buffer_ + r, size_ - r);
    // delete掉原来的buffer_
    data_allocator::deallocate(buffer_, old_cap);
    buffer_ = new_buffer;
    size_ += n;
    cap_ = new_cap;
    // 返回pos的位置
    return buffer_ + r;
}

// reallocate_and_copy 函数
template <class CharType, class CharTraits>
typename basic_string<CharType, CharTraits>::iterator
basic_string<CharType, CharTraits>::reallocate_and_copy(iterator pos,
                                                        const_iterator first,
                                                        const_iterator last) {
    const auto r = pos - buffer_;
    const auto old_cap = cap_;
    const size_type n = mystl::distance(first, last);
    const auto new_cap = mystl::max(old_cap + n, old_cap + (old_cap >> 1));
    auto new_buffer = data_allocator::allocate(new_cap);
    auto e1 = char_traits::move(new_buffer, buffer_, r) + r;
    auto e2 = mystl::uninitialized_copy_n(first, n, e1) + n;
    char_traits::move(e2, buffer_ + r, size_ - r);
    data_allocator::deallocate(buffer_, old_cap);
    buffer_ = new_buffer;
    size_ += n;
    cap_ = new_cap;
    return buffer_ + r;
}

/* *************************************** */
// 重载全局操作符

// 重载operator+
template <class CharType, class CharTraits>
basic_string<CharType, CharTraits> operator+(
    const basic_string<CharType, CharTraits>& lhs,
    const basic_string<CharType, CharTraits>& rhs) {
    basic_string<CharType, CharTraits> tmp(lhs);
    tmp.append(rhs);
    return tmp;
}

template <class CharType, class CharTraits>
basic_string<CharType, CharTraits> operator+(
    const CharType* lhs,
    const basic_string<CharType, CharTraits>& rhs) {
    basic_string<CharType, CharTraits> tmp(lhs);
    tmp.append(rhs);
    return tmp;
}

// 1个字符ch + rhs
template <class CharType, class CharTraits>
basic_string<CharType, CharTraits> operator+(
    CharType ch,
    const basic_string<CharType, CharTraits>& rhs) {
    basic_string<CharType, CharTraits> tmp(1, ch);
    tmp.append(rhs);
    return tmp;
}

template <class CharType, class CharTraits>
basic_string<CharType, CharTraits> operator+(
    const basic_string<CharType, CharTraits>& lhs,
    const CharType* rhs) {
    basic_string<CharType, CharTraits> tmp(lhs);
    tmp.append(rhs);
    return tmp;
}

template <class CharType, class CharTraits>
basic_string<CharType, CharTraits> operator+(
    const basic_string<CharType, CharTraits>& lhs,
    CharType ch) {
    basic_string<CharType, CharTraits> tmp(lhs);
    tmp.append(1, ch);
    return tmp;
}

template <class CharType, class CharTraits>
basic_string<CharType, CharTraits> operator+(
    basic_string<CharType, CharTraits>&& lhs,
    const basic_string<CharType, CharTraits>& rhs) {
    basic_string<CharType, CharTraits> tmp(mystl::move(lhs));
    tmp.append(rhs);
    return tmp;
}

template <class CharType, class CharTraits>
basic_string<CharType, CharTraits> operator+(
    const basic_string<CharType, CharTraits>& lhs,
    basic_string<CharType, CharTraits>&& rhs) {
    basic_string<CharType, CharTraits> tmp(mystl::move(rhs));
    // 在tmp.begin()处，插入[lhs.begin(), lhs.end())
    tmp.insert(tmp.begin(), lhs.begin(), lhs.end());
    return tmp;
}

template <class CharType, class CharTraits>
basic_string<CharType, CharTraits> operator+(
    basic_string<CharType, CharTraits>&& lhs,
    basic_string<CharType, CharTraits>&& rhs) {
    basic_string<CharType, CharTraits> tmp(mystl::move(lhs));
    tmp.append(rhs);
    return tmp;
}

// 这几个函数的区别在于CharType，basic_string，const，*，&，&&
template <class CharType, class CharTraits>
basic_string<CharType, CharTraits> operator+(
    const CharType* lhs,
    basic_string<CharType, CharTraits>&& rhs) {
    basic_string<CharType, CharTraits> tmp(mystl::move(rhs));
    tmp.insert(tmp.begin(), lhs, lhs + char_traits<CharType>::length(lhs));
    return tmp;
}

template <class CharType, class CharTraits>
basic_string<CharType, CharTraits> operator+(
    CharType ch,
    basic_string<CharType, CharTraits>&& rhs) {
    basic_string<CharType, CharTraits> tmp(mystl::move(rhs));
    tmp.insert(tmp.begin(), ch);
    return tmp;
}

template <class CharType, class CharTraits>
basic_string<CharType, CharTraits> operator+(
    basic_string<CharType, CharTraits>&& lhs,
    const CharType* rhs) {
    basic_string<CharType, CharTraits> tmp(mystl::move(lhs));
    tmp.append(rhs);
    return tmp;
}

template <class CharType, class CharTraits>
basic_string<CharType, CharTraits> operator+(
    basic_string<CharType, CharTraits>&& lhs,
    CharType ch) {
    basic_string<CharType, CharTraits> tmp(mystl::move(lhs));
    tmp.append(1, ch);
    return tmp;
}

// 重载比较操作符
template <class CharType, class CharTraits>
bool operator==(const basic_string<CharType, CharTraits>& lhs,
                const basic_string<CharType, CharTraits>& rhs) {
    return lhs.size() == rhs.size() && lhs.compare(rhs) == 0;
}

template <class CharType, class CharTraits>
bool operator!=(const basic_string<CharType, CharTraits>& lhs,
                const basic_string<CharType, CharTraits>& rhs) {
    return lhs.size() != rhs.size() || lhs.compare(rhs) != 0;
}

template <class CharType, class CharTraits>
bool operator<(const basic_string<CharType, CharTraits>& lhs,
               const basic_string<CharType, CharTraits>& rhs) {
    return lhs.compare(rhs) < 0;
}

template <class CharType, class CharTraits>
bool operator<=(const basic_string<CharType, CharTraits>& lhs,
                const basic_string<CharType, CharTraits>& rhs) {
    return lhs.compare(rhs) <= 0;
}

template <class CharType, class CharTraits>
bool operator>(const basic_string<CharType, CharTraits>& lhs,
               const basic_string<CharType, CharTraits>& rhs) {
    return lhs.compare(rhs) > 0;
}

template <class CharType, class CharTraits>
bool operator>=(const basic_string<CharType, CharTraits>& lhs,
                const basic_string<CharType, CharTraits>& rhs) {
    return lhs.compare(rhs) >= 0;
}

// 重载 mystl 的 swap
template <class CharType, class CharTraits>
void swap(basic_string<CharType, CharTraits>& lhs,
          basic_string<CharType, CharTraits>& rhs) noexcept {
    lhs.swap(rhs);
}

// 特化mystl::hash
template <class CharType, class CharTraits>
struct hash<basic_string<CharType, CharTraits>> {
    size_t operator()(const basic_string<CharType, CharTraits>& str) {
        return bitwise_hash((const unsigned char*)str.c_str(),
                            str.size() * sizeof(CharType));
    }
};

}  // namespace mystl

#endif  // !MYTINYSTL_BASIC_STRING_H_