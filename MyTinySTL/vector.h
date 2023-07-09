#ifndef MYTINYSTL_VECTOR_H_
#define MYTINYSTL_VECTOR_H_

// 这个头文件包含一个模板类 vector
// vector：向量

// notes:
//
// 异常保证：
// mystl::vecotr<T>
// 满足基本异常保证，部分函数无异常保证，并对以下函数做强异常安全保证：
//   * emplace
//   * emplace_back
//   * push_back
// 当 std::is_nothrow_move_assignable<T>::value == true
// 时，以下函数也满足强异常保证：
//   * reserve
//   * resize
//   * insert

#include <initializer_list>
#include "algo.h"
#include "exceptdef.h"
#include "iterator.h"
#include "memory.h"
#include "util.h"

namespace mystl {

#ifdef max
#pragma message("#undefing marco max")
#undef max
#endif  // max

#ifdef min
#pragma message("#undefing marco min")
#undef min
#endif  // min

// 模板类
// 模板参数 T 代表类型
template <class T>
class vector {
    static_assert(!std::is_same<bool, T>::value,
                  "vector<bool> is abandoned in mystl");

public:
    // vector 的嵌套型别定义
    typedef mystl::allocator<T> allocator_type;
    typedef mystl::allocator<T> data_allocator;

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

    allocator_type get_allocator() { return data_allocator(); }

private:
    iterator begin_;  // 表示目前使用空间的头部
    iterator end_;    // 表示目前使用空间的尾部
    iterator cap_;    // 表示目前储存空间的尾部

public:
    // 构造、复制、移动、析构函数
    vector() noexcept { try_init(); }

    explicit vector(size_type n) { fill_init(n, value_type()); }

    vector(size_type n, const value_type& value) { fill_init(n, value); }

    template <class Iter,
              typename std::enable_if<mystl::is_input_iterator<Iter>::value,
                                      int>::type = 0>
    vector(Iter first, Iter last) {
        MYSTL_DEBUG(!(last < first));
        range_init(first, last);
    }

    vector(const vector& rhs) { range_init(rhs.begin_, rhs.end_); }

    /* 这是一个移动构造函数，用于将一个右值引用的 `vector`
    对象的资源（即存储空间）移动到当前的对象中。
    在移动构造函数中，我们首先将右值引用的对象的成员变量 `begin_`、`end_` 和
    `cap_`（即指向存储空间的指针）赋值给当前对象的相应成员变量。
    由于移动构造函数的语义是“窃取”右值引用对象的资源，因此在赋值后，我们需要将右值引用对象的成员变量设置为
    `nullptr`，以避免在其析构函数中释放已经移动的资源。

    需要注意的是，移动构造函数应该是 `noexcept`
    的，以便在使用容器类时进行优化。因为 `noexcept`
    保证了在移动构造函数中不会发生异常抛出，这使得编译器可以在某些情况下对代码进行优化，提高程序的性能
  */
    vector(vector&& rhs) noexcept
        : begin_(rhs.begin_), end_(rhs.end_), cap_(rhs.cap_) {
        rhs.begin_ = nullptr;
        rhs.end_ = nullptr;
        rhs.cap_ = nullptr;
    }

    vector(std::initializer_list<value_type> ilist) {
        range_init(ilist.begin(), ilist.end());
    }

    vector& operator=(const vector& rhs);
    vector& operator=(vector&& rhs) noexcept;

    vector& operator=(std::initializer_list<value_type> ilist) {
        vector tmp(ilist.begin(), ilist.end());
        swap(tmp);
        return *this;
    }

    ~vector() {
        destroy_and_recover(begin_, end_, cap_ - begin_);
        begin_ = end_ = cap_ = nullptr;
    }

public:
    // 迭代器相关操作
    iterator begin() noexcept { return begin_; }
    const_iterator begin() const noexcept { return begin_; }
    iterator end() noexcept { return end_; }
    const_iterator end() const noexcept { return end_; }

    reverse_iterator rbegin() noexcept { return reverse_iterator(end()); }
    const_reverse_iterator rbegin() const noexcept {
        return const_reverst_iterator(end());
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
    bool empty() const noexcept { return begin_ == end_; }
    size_type size() const noexcept {
        return static_cast<size_type>(end_ - begin_);
    }
    /* 这段代码是一个 C++ STL 中的函数，用于返回当前容器所能容纳的最大元素数量。
    在这段代码中，max_size() 函数首先将 -1 转换为 size_type 类型，-1
    表示无符号整数的最大值，即 2^32-1 或
    2^64-1，具体取决于编译器和操作系统的位数。 然后将这个值除以元素类型 T
    的大小，得到的结果就是当前容器所能容纳的最大元素数量。
    由于这个值可能非常大，因此需要将其转换为 size_type
    类型，这是一个无符号整数类型，用于表示容器的大小。 最后，函数使用关键字
    noexcept 声明，表示这个函数不会抛出任何异常。 */
    size_type max_size() const noexcept {
        return static_cast<size_type>(-1) / sizeof(T);
    }
    size_type capacity() const noexcept {
        return static_cast<size_type>(cap_ - begin_);
    }
    void reserve(size_type n);
    void shrink_to_fit();

    // 访问元素相关操作
    reference operator[](size_type n) {
        // 在size内访问就没错
        MYSTL_DEBUG(n < size());
        return *(begin_ + n);
    }
    const_reference operator[](size_type n) const {
        MYSTL_DEBUG(n < size());
        return *(begin_ + n);
    }
    reference at(size_type n) {
        // 抛出超出返回异常
        THROW_OUT_OF_RANGE_IF(!(n < size()),
                              "vector<T>::at() subscript out of range");
        return (*this)[n];
    }
    const_reference at(size_type n) const {
        THROW_OUT_OF_RANGE_IF(!(n < size()),
                              "vector<T>::at() subscript out of range");
        return (*this)[n];
    }
    reference front() {
        // 不为空才是正确的
        MYSTL_DEBUG(!empty());
        return *begin_;
    }
    const_reference front() const {
        MYSTL_DEBUG(!empty());
        return *begin_;
    }
    reference back() {
        MYSTL_DEBUG(!empty());
        return *(end_ - 1);
    }
    const_reference back() const {
        MYSTL_DEBUG(!empty());
        return *(end_ - 1);
    }
    pointer data() noexcept { return begin_; }
    const_pointer data() const noexcept { return begin_; }

    // 修改容器相关操作
    // assign

    void assign(size_type n, const value_type& value) { fill_assign(n, value); }

    template <class Iter,
              typename std::enable_if<mystl::is_input_iterator<Iter>::value,
                                      int>::type = 0>
    void assign(Iter first, Iter last) {
        // 需要last大于first
        MYSTL_DEBUG(!(last < first));
        copy_assign(first, last, iterator_category(first));
    }

    void assign(std::initializer_list<value_type> il) {
        copy_assign(il.begin(), il.end(), mystl::forward_iterator_tag{});
    }

    // emplace / emplace_back
    template <class... Args>
    iterator emplace(const_iterator pos, Args&&... args);

    template <class... Args>
    void emplace_back(Args&&... args);

    // push_back / pop_back
    void push_back(const value_type& value);
    void push_back(value_type&& value) { emplace_back(mystl::move(value)); }

    void pop_back();

    // insert
    iterator insert(const_iterator pos, const value_type& value);
    iterator insert(const_iterator pos, value_type&& value) {
        return emplace(pos, mystl::move(value));
    }

    iterator insert(const_iterator pos, size_type n, const value_type& value) {
        MYSTL_DEBUG(pos >= begin() && pos <= end());
        return fill_insert(const_cast<iterator>(pos), n, value);
    }

    template <class Iter,
              typename std::enable_if<mystl::is_input_iterator<Iter>::value,
                                      int>::type = 0>
    void insert(const_iterator pos, Iter first, Iter last) {
        MYSTL_DEBUG(pos >= begin() && pos <= end() && !(last < first));
        copy_insert(const_cast<iterator>(pos), first, last);
    }

    // erase / clear
    iterator erase(const_iterator pos);
    iterator erase(const_iterator first, const_iterator last);
    void clear() { erase(begin(), end()); }

    // resize / reverse
    void resize(size_type new_size) { return resize(new_size, value_type()); }
    void resize(size_type new_size, const value_type& value);

    void reverse() { mystl::reverse(begin(), end()); }

    // swap
    void swap(vector& rhs) noexcept;

private:
    // helper functions
    // initialize / destroy
    void try_init() noexcept;
    void init_space(size_type size, size_type cap);
    void fill_init(size_type n, const value_type& value);

    template <class Iter>
    void range_init(Iter first, Iter last);

    void destroy_and_recover(iterator first, iterator last, size_type n);

    // calculate the growth size
    size_type get_new_cap(size_type add_size);

    // assign
    void fill_assign(size_type n, const value_type& value);

    template <class IIter>
    void copy_assign(IIter first, IIter last, input_iterator_tag);

    template <class FIter>
    void copy_assign(FIter first, FIter last, forward_iterator_tag);

    // reallocate
    template <class... Args>
    void reallocate_emplace(iterator pos, Args&&... args);
    void reallocate_insert(iterator pos, const value_type& value);

    // insert
    iterator fill_insert(iterator pos, size_type n, const value_type& value);
    template <class IIter>
    void copy_insert(iterator pos, IIter first, IIter last);

    // shrink_to_fit
    void reinsert(size_type size);
};

/**************************************************/
// 复制赋值操作符
/* 这是 `vector` 类模板中的赋值运算符重载函数的实现。该函数将一个 `vector`
对象赋值给另一个 `vector` 对象，返回赋值后的对象的引用。

该函数的实现分为三个部分，分别对应于三种情况：

1. 如果右值 `rhs` 的长度大于当前对象的容量，那么创建一个新的 `vector` 对象
`tmp`，并将右值赋值给 `tmp`。最后，通过调用 `swap` 函数交换当前对象和 `tmp`
的内容，从而使当前对象的长度和容量都等于右值的长度。

2. 如果右值 `rhs`
的长度小于或等于当前对象的长度，那么将右值的元素复制到当前对象中，并销毁多余的元素。

3. 如果右值 `rhs`
的长度大于当前对象的长度但小于当前对象的容量，那么将右值的元素复制到当前对象中，并在当前对象的尾部添加新元素，直到当前对象的长度等于右值的长度。

需要注意的是，该函数只适用于 `vector`
类模板，不能用于其他类型的容器。此外，该函数的实现中使用了 `mystl`
命名空间中的函数和类型，这些函数和类型不是标准库中的内容，可能是该类模板的作者自己实现的。
*/
template <class T>
vector<T>& vector<T>::operator=(const vector& rhs) {
    if (this != &rhs) {
        const auto len = rhs.size();
        // 大于当前的容量
        if (len > capacity()) {
            // 创建新的vector对象tmp
            vector tmp(rhs.begin(), rhs.end());
            // swap函数交换当前对象和tmp的内容
            // 使当前对象的长度和容量都等于rhs的长度
            swap(tmp);
        } else if (size() >= len) {
            // rhs小于等于当前对象的长度，复制到当前对象中，并销毁多余的元素
            auto i = mystl::copy(rhs.begin(), rhs.end(), begin());
            data_allocator::destroy(i, end_);
            end_ = begin_ + len;
        } else {
            //  `rhs` 的长度大于当前对象的长度但小于当前对象的容量，
            // 那么将rhs的元素复制到当前对象中，并在当前对象的尾部添加新元素，直到当前对象的长度等于rhs的长度。
            mystl::copy(rhs.begin(), rhs.begin() + size(), begin_);
            // 从当前对象的end_开始，一直到然后到rhs.end()
            mystl::uninitialized_copy(rhs.begin() + size(), rhs.end(), end_);
            cap_ = end_ = begin_ + len;
        }
    }
    return *this;
}

// 移动赋值操作符
// 移动赋值原队形的值会被销毁或置为默认值
template <class T>
vector<T>& vector<T>::operator=(vector&& rhs) noexcept {
    destroy_and_recover(begin_, end_, cap_ - begin_);
    begin_ = rhs.begin_;
    end_ = rhs.end_;
    cap_ = rhs.cap_;
    rhs.begin_ = nullptr;
    rhs.end_ = nullptr;
    rhs.cap_ = nullptr;
    return *this;
}

// 预留空间大小，当原容量小于要求大小时，才会重新分配
template <class T>
void vector<T>::reserve(size_type n) {
    if (capacity() < n) {
        THROW_LENGTH_ERROR_IF(
            n > max_size(),
            "n can not larger than max_size() in vector<T>::reserve<n");
        const auto old_size = size();
        auto tmp = data_allocator::allocate(n);
        mystl::uninitialized_move(begin_, end_, tmp);
        data_allocator::deallocate(begin_, cap_ - begin_);
        begin_ = tmp;
        end_ = tmp + old_size;
        cap_ = begin_ + n;
    }
}

// 放弃多余的容量
template <class T>
void vector<T>::shrink_to_fit() {
    if (end_ < cap_) {
        reinsert(size());
    }
}

// 在pos位置就地构造元素，避免额外的赋值或移动开销
template <class T>
template <class... Args>
typename vector<T>::iterator vector<T>::emplace(const_iterator pos,
                                                Args&&... args) {
    // 检查传入的迭代器是否在 `vector` 的有效范围内
    MYSTL_DEBUG(pos >= begin() && pos <= end());
    // 将传入的迭代器 `pos` 转换为 `iterator` 类型的 `xpos`
    iterator xpos = const_cast<iterator>(pos);
    // 计算出 `xpos` 在 `vector` 中的下标 `n`
    const size_type n = xpos - begin_;
    // 如果 `end_` 没有达到 `cap_` 并且 `xpos` 恰好指向 `end_`
    if (end_ != cap_ && xpos == end_) {
        // 在 `end_` 处构造一个新元素，并将 `end_` 后移一位
        data_allocator::construct(mystl::address_of(*end_),
                                  mystl::forward<Args>(args)...);
        ++end_;
    }
    // 如果 `end_` 没有达到 `cap_`
    else if (end_ != cap_) {
        auto new_end = end_;
        // 将 `end_-1` 处的元素复制到 `end_` 处
        data_allocator::construct(mystl::address_of(*end_), *(end_ - 1));
        ++new_end;
        // 将 `[pos, end_-1]` 区间内的元素向后移动一位，以为新元素腾出位置
        mystl::copy_backward(xpos, end_ - 1, end_);
        // 在 `end_` 处构造一个新元素，
        *xpos = value_type(mystl::forward<Args>(args)...);
        // 并将 `end_` 后移一位
        end_ = new_end;
    } else {
        // 如果 `end_` 已经达到 `cap_`，则调用 `reallocate_emplace`
        // 函数进行重新分配空间，并在 `xpos` 处构造新元素
        reallocate_emplace(xpos, mystl::forward<Args>(args)...);
    }
    // 返回新插入元素的迭代器
    return begin() + n;
}

// 在尾部就地构造元素，避免额外的复制或移动开销
template <class T>
template <class... Args>
void vector<T>::emplace_back(Args&&... args) {
    if (end_ < cap_) {
        // 直接在end_处构造元素
        data_allocator::construct(mystl::address_of(*end_),
                                  mystl::forward<Args>(args)...);
        // 然后end_++
        ++end_;
    } else {
        // 如果 `end_` 已经达到 `cap_`，则调用 `reallocate_emplace`
        // 函数进行重新分配空间，并在 end_ 处构造新元素
        reallocate_emplace(end_, mystl::forward<Args>(args)...);
    }
}

// 在尾部插入元素
template <class T>
void vector<T>::push_back(const value_type& value) {
    if (end_ != cap_) {
        data_allocator::construct(mystl::address_of(*end_), value);
        ++end_;
    } else {
        // 重新分配空间，并作插入
        reallocate_insert(end_, value);
    }
}

// 弹出尾部元素
template <class T>
void vector<T>::pop_back() {
    MYSTL_DEBUG(!empty());
    data_allocator::destroy(end_ - 1);
    --end_;
}

// 在pos处插入元素
template <class T>
typename vector<T>::iterator vector<T>::insert(const_iterator pos,
                                               const value_type& value) {
    MYSTL_DEBUG(pos >= begin() && pos <= end());
    iterator xpos = const_cast<iterator>(pos);
    const size_type n = pos - begin_;
    if (end_ != cap_ && xpos == end_) {
        data_allocator::construct(mystl::address_of(*end_), value);
        ++end_;
    } else if (end_ != cap_) {
        auto new_end = end_;
        data_allocator::construct(mystl::address_of(*end_), *(end_ - 1));
        ++new_end;
        auto value_copy = value;  // 避免元素因以下复制操作而被改变
        mystl::copy_backward(xpos, end_ - 1, end_);
        // 也是在xpos出构造一个新的元素
        *xpos = mystl::move(value_copy);
        end_ = new_end;
    } else {
        reallocate_insert(xpos, value);
    }
    return begin + n;
}

// 删除pos位置上的元素
// `pos` 是一个常量迭代器，指向要删除的元素。
template <class T>
typename vector<T>::iterator vector<T>::erase(const_iterator pos) {
    MYSTL_DEBUG(pos >= begin() && pos <= end());
    // 将参数 `pos` 转换为一个普通迭代器 `xpos`，这是为了方便后面的操作
    iterator xpos = begin_ + (pos - begin());
    // 函数调用 `mystl::move` 算法将 `xpos`
    // 后面的所有元素向前移动一个位置，覆盖掉要删除的元素。这样，要删除的元素就被“删除”了，实际上是被覆盖掉了
    mystl::move(xpos + 1, end_, xpos);
    // `data_allocator::destroy`
    // 函数销毁最后一个元素（即原来的最后一个元素已经被覆盖掉了，现在成为了倒数第二个元素）
    data_allocator::destroy(end_ - 1);
    // `end_` 指针向前移动一个位置，表示容器中的元素数量减少了一个
    --end_;
    // 函数返回一个迭代器
    // `xpos`，指向被删除的元素的下一个位置，这是为了方便后续操作
    return xpos;
}

// 删除[first, last)上的元素
template <class T>
// `first` 和 `last` 分别指定了要删除的范围
typename vector<T>::iterator vector<T>::erase(const_iterator first,
                                              const_iterator last) {
    // 参数检查
    MYSTL_DEBUG(first >= begin() && last <= end() && !(last < first));
    // 计算要删除的元素个数 `n`，即 `first` 在容器中的位置
    const auto n = first - begin();
    // 将 `first` 转换为一个普通迭代器 `r`
    iterator r = begin_ + (first - begin());
    // 使用 `data_allocator::destroy` 函数销毁范围 `[r + (last - first), end_)`
    // 内的元素 （注意，这里使用了 `mystl::move`
    // 函数，将要销毁的元素移动到了容器的末尾）
    data_allocator::destroy(mystl::move(r + (last - first), end_, r), end_);
    // 更新 `end_`，使其指向删除操作后的新末尾位置
    end_ = end_ - (last - first);
    // 并返回指向删除操作后的第一个元素的迭代器 `begin_ + n`
    return begin_ + n;
}

// 重置容器大小
template <class T>
void vector<T>::resize(size_type new_size, const value_type& value) {
    if (new_size < size()) {
        // 少了就删
        erase(begin() + new_size, end());
    } else {
        // 多了就加
        insert(end(), new_size - size(), value);
    }
}

// 与另一个vector交换
template <class T>
void vector<T>::swap(vector<T>& rhs) noexcept {
    if (this != &rhs) {
        mystl::swap(begin_, rhs.begin_);
        mystl::swap(end_, rhs.end_);
        mystl::swap(cap_, rhs.cap_);
    }
}

/**************************************/
// helper function
// try_init函数, 若分配失败则忽略, 不抛出异常
template <class T>
void vector<T>::try_init() noexcept {
    try {
        begin_ = data_allocator::allocate(16);
        end_ = begin_;
        cap_ = begin_ + 16;  // 容量最少为16
    } catch (...) {
        begin_ = nullptr;
        end_ = nullptr;
        cap_ = nullptr;
    }
}

// init_space函数
// 初始化空间函数，用于在创建 vector 对象时分配存储空间
template <class T>
//  `size` 和 `cap`，分别表示要分配的初始元素个数和容量大小
void vector<T>::init_space(size_type size, size_type cap) {
    try {
        // 首先使用 `data_allocator::allocate(cap)` 分配了 `cap`
        // 个元素的存储空间，并将其指针保存在 `begin_` 中
        begin_ = data_allocator::allocate(cap);
        // 将 `end_` 指针指向 `begin_ + size`，表示 `vector` 中已经存储了 `size`
        // 个元素
        end_ = begin_ + size;
        // 将 `cap_` 指针指向 `begin_ + cap`，表示 `vector` 的容量为 `cap`
        cap_ = begin_ + cap;
    } catch (...) {
        // 如果在分配存储空间时发生异常，我们会将 `begin_`、`end_` 和 `cap_`
        // 指针全部置为空指针，并重新抛出异常，以确保 `vector` 对象的创建失败
        begin_ = nullptr;
        end_ = nullptr;
        cap_ = nullptr;
        throw;
    }
}

// fill_init函数
template <class T>
// 在 `vector` 对象中填充指定数量的元素，并对它们进行初始化
// `n` 和 `value`，分别表示要填充的元素数量和要填充的值。
void vector<T>::fill_init(size_type n, const value_type& value) {
    // 计算出初始大小 `init_size`，其值为 `16` 和 `n` 中的较大者，
    const size_type init_size = mystl::max(static_cast<size_type>(16), n);
    // 然后调用 `init_space` 函数来初始化 `vector`
    // 对象的内部存储空间，使其能够容纳至少 `init_size` 个元素
    init_space(n, init_size);
    // 函数调用 `mystl::uninitialized_fill_n` 函数来对 `vector` 对象的前 `n`
    // 个元素进行初始化
    mystl::uninitialized_fill_n(begin_, n, value);
}
/* 这段代码实现了 `vector` 类模板的范围初始化函数
`range_init`，它接受两个迭代器参数 `first` 和
`last`，用于指定要初始化的元素范围。该函数的实现步骤如下：

1. 计算要初始化的元素个数 `len`，通过 `mystl::distance` 函数计算 `first` 和
`last` 之间的距离。
2. 计算需要分配的内存空间大小 `init_size`，为 `len` 和 16 中的较大值。
3. 调用 `init_space` 函数分配内存空间，并设置 `begin_`、`end_` 和
`end_of_storage_` 指针。
4. 使用 `mystl::uninitialized_copy` 函数将范围 `[first, last)`
中的元素复制到新分配的内存空间中，完成初始化。

需要注意的是，该函数是 `vector` 类模板的成员函数，因此在调用时需要使用 `vector`
对象的成员函数语法来调用 */
// range_init函数
template <class T>
template <class Iter>
// 两个迭代器参数 `first` 和 `last`，用于指定要初始化的元素范围
void vector<T>::range_init(Iter first, Iter last) {
    // 计算要初始化的元素个数 `len`，通过 `mystl::distance` 函数计算 `first` 和
    // `last` 之间的距离
    const size_type len = mystl::distance(first, last);
    // 计算需要分配的内存空间大小 `init_size`，为 `len` 和 16 中的较大值。
    const size_type init_size = mystl::max(len, static_cast<size_type>(16));
    // len为容器大小，init_size为容器容量
    init_space(len, init_size);
    // 使用 `mystl::uninitialized_copy` 函数将范围 `[first, last)`
    // 中的元素复制到新分配的内存空间中，完成初始化
    mystl::uninitialized_copy(first, last, begin_);
}

// destroy_and_recover函数
template <class T>
void vector<T>::destroy_and_recover(iterator first,
                                    iterator last,
                                    size_type n) {
    data_allocator::destroy(first, last);
    data_allocator::deallocate(first, n);
}

// get_new_cap函数
// 获取新容量的函数，在插入元素时，根据当前的容量和需要插入的元素个数，计算出一个新的合适的容量大小
template <class T>
typename vector<T>::size_type vector<T>::get_new_cap(size_type add_size) {
    // 1. 首先获取当前容量 `old_size`
    const auto old_size = capacity();
    // 2. 如果当前容量加上要插入的元素个数 `add_size` 大于
    // `max_size()`，则抛出一个 `length_error` 异常
    THROW_LENGTH_ERROR_IF(old_size > max_size() - add_size,
                          "vector<T>'s size too big");
    // 3. 如果当前容量大于 `max_size()` 减去当前容量的一半
    if (old_size > max_size() - old_size / 2) {
        // 至少16，16是为了避免频繁的分配和释放内存
        return old_size + add_size > max_size() - 16 ? old_size + add_size
                                                     : old_size + add_size + 16;
    }
    // 4. 否则，新容量为 `old_size + old_size / 2` 和 `old_size + add_size`
    // 中的较大值。如果当前容量为 `0`，则新容量为 `16` 和 `add_size` 中的较大值
    const size_type new_size =
        old_size == 0
            ? mystl::max(add_size, static_cast<size_type>(16))
            : mystl::max(old_size + old_size / 2, old_size + add_size);
    return new_size;
}

// fill_assign函数
template <class T>
void vector<T>::fill_assign(size_type n, const value_type& value) {
    if (n > capacity()) {
        // 判断需要填充的元素数量是否大于当前容器的容量，如果是，就创建一个新的容量为
        // `n` 的 `vector` 对象，将其与当前对象交换，以扩展容量
        vector tmp(n, value);
        swap(tmp);
    } else if (n > size()) {
        // 否则，如果需要填充的元素数量大于当前容器的大小，但小于等于当前容器的容量，就使用
        // `mystl::fill` 函数将当前容器中的元素全部替换为指定的值
        mystl::fill(begin(), end(), value);
        // 然后使用 `mystl::uninitialized_fill_n` 函数在容器末尾添加新元素
        end_ = mystl::uninitialized_fill_n(end_, n - size(), value);
    } else {
        // 最后，如果需要填充的元素数量小于等于当前容器的大小，就使用 `erase`
        // 函数删除超出需要填充的元素数量的元素，然后使用 `mystl::fill_n`
        // 函数将前 `n` 个元素替换为指定的值。
        // mystl::fill_n(begin_, n, value)返回的是填充完n个的位置
        erase(mystl::fill_n(begin_, n, value), end_);
    }
}

// copy_assign函数
template <class T>
template <class IIter>
void vector<T>::copy_assign(IIter first, IIter last, input_iterator_tag) {
    auto cur = begin_;
    for (; first != last && cur != end_; ++first, ++cur) {
        *cur = *first;
    }
    if (first == last) {
        erase(cur, end);
    } else {
        insert(end_, first, last);
    }
}

// 用 [first, last) 为容器赋值
// 用于将一个迭代器范围内的元素赋值给 `vector` 容器
template <class T>
template <class FIter>
void vector<T>::copy_assign(FIter first, FIter last, forward_iterator_tag) {
    const size_type len = mystl::distance(first, last);
    // 扩展容量
    if (len > capacity()) {
        vector tmp(first, last);
        swap(tmp);
    } else if (size() >= len) {
        // 需要复制的元素数量小于等于当前容器的大小
        // 如果需要复制的元素数量小于等于当前容器的大小，就使用 `mystl::copy`
        // 函数将迭代器范围内的元素复制到当前容器中
        auto new_end = mystl::copy(first, last, begin_);
        // 然后使用 `data_allocator::destroy` 函数销毁多余的元素
        data_allocator::destroy(new_end, end_);
        // 更改为新的end_
        end_ = new_end;
    } else {
        // 如果需要复制的元素数量大于当前容器的大小
        auto mid = first;
        // 前进size()步
        mystl::advance(mid, size());
        // 就使用 `mystl::copy` 函数将前 `size()` 个元素复制到当前容器中
        mystl::copy(first, mid, begin_);
        // 然后使用 `mystl::uninitialized_copy` 函数在容器末尾添加新元素
        auto new_end = mystl::uninitialized_copy(mid, last, end_);
        end_ = new_end;
    }
}

// 重新分配空间并在pos处就地构造元素
template <class T>
template <class... Args>
void vector<T>::reallocate_emplace(iterator pos, Args&&... args) {
    // 调用 `get_new_cap`
    // 函数来计算新的容量大小，这个函数会根据需要插入的元素数量来计算新的容量大小
    const auto new_size = get_new_cap(1);
    // 用新的容量大小来分配一块新的内存空间，这块内存空间的起始地址被赋值给
    // `new_begin`
    auto new_begin = data_allocator::allocate(new_size);
    auto new_end = new_begin;
    try {
        // 使用 `mystl::uninitialized_move` 函数将 `begin_` 到 `pos`
        // 之间的元素移动到新的内存空间中，这个函数会返回一个迭代器，这个迭代器指向新的内存空间中最后一个被移动的元素
        new_end = mystl::uninitialized_move(begin_, pos, new_begin);
        // 使用 `data_allocator::construct` 函数在 `new_end`
        // 指向的位置构造一个新的元素，这个函数会调用元素类型的构造函数来构造这个新的元素
        data_allocator::construct(mystl::address_of(*new_end),
                                  mystl::forward<Args>(args)...);
        // 将 `new_end` 向后移动一个位置，使其指向新插入的元素之后的位置。
        ++new_end;
        // 使用 `mystl::uninitialized_move` 函数将 `pos` 到 `end_`
        // 之间的元素移动到新的内存空间中，这个函数会返回一个迭代器，这个迭代器指向新的内存空间中最后一个被移动的元素之后的位置
        new_end = mystl::uninitialized_move(pos, end_, new_end);
    } catch (...) {
        data_allocator::deallocate(new_begin, new_size);
        throw;
    }
    // 使用 `destroy_and_recover` 函数销毁原来的元素，并释放原来的内存空间
    destroy_and_recover(begin_, end_, cap_ - begin_);
    // 然后将 `begin_`、`end_` 和 `cap_`
    // 分别指向新的内存空间的起始位置、最后一个元素之后的位置和新的内存空间的末尾位置。
    begin_ = new_begin;
    end_ = new_end;
    cap_ = new_begin + new_size;
}

// 重新分配空间并在pos处插入元素
template <class T>
void vector<T>::reallocate_insert(iterator pos, const value_type& value) {
    const auto new_size = get_new_cap(1);
    auto new_begin = data_allocator::allocate(new_size);
    auto new_end = new_begin;
    const value_type& value_copy = value;
    try {
        new_end = mystl::uninitialized_move(begin_, pos, new_begin);
        data_allocator::construct(mystl::address_of(*new_end), value_copy);
        ++new_end;
        new_end = mystl::uninitialized_move(pos, end_, new_end);
    } catch (...) {
        data_allocator::deallocate(new_begin, new_size);
        throw;
    }
    destroy_and_recover(begin_, end_, cap_ - begin_);
    begin_ = new_begin;
    end_ = new_end;
    cap_ = new_begin + new_size;
}

// fill_insert函数
// 在指定位置插入指定数量的元素，每个元素的值都为指定的值
template <class T>
typename vector<T>::iterator vector<T>::fill_insert(iterator pos,
                                                    size_type n,
                                                    const value_type& value) {
    // 如果要插入的元素数量为 0，则直接返回插入位置
    if (n == 0) {
        return pos;
    }
    // 计算出插入位置的下标（即 `pos - begin_`）和要插入的元素值的拷贝
    const size_type xpos = pos - begin_;
    const value_type value_copy = value;  // 避免被覆盖
    if (static_cast<size_type>(cap_ - end_) >= n) {
        // 如果备用空间大于等于增加的空间
        // a. 计算出插入位置后面的元素个数（即 `end_ - pos`）
        const size_type after_elems = end_ - pos;
        auto old_end = end_;
        // b. 如果插入位置后面的元素个数大于等于要插入的元素数量
        if (after_elems > n) {
            // 则先将后面的元素向后移动 `n` 个位置
            mystl::uninitialized_copy(end_ - n, end_, end_);
            end_ += n;
            mystl::move_backward(pos, old_end - n, old_end);
            // 再在插入位置处插入 `n` 个元素
            mystl::uninitialized_fill_n(pos, n, value_copy);
        } else {
            // 如果插入位置后面的元素个数小于要插入的元素数量，
            // 则先在原有空间中插入 `n - after_elems` 个元素，
            end_ =
                mystl::uninitialized_fill_n(end_, n - after_elems, value_copy);
            // 再将插入位置后面的元素移动到新的空间中
            end_ = mystl::unchecked_move(pos, old_end, end_);
            // 最后在插入位置处插入 `after_elems` 个元素
            mystl::uninitialized_fill_n(pos, after_elems, value_copy);
        }
    } else {
        // 如果备用空间不足
        // a. 计算出需要分配的新空间大小（即 `get_new_cap(n)`）。
        const auto new_size = get_new_cap(n);
        // b. 分配新的内存空间，并将原有元素拷贝到新的内存空间中
        auto new_begin = data_allocator::allocate(new_size);
        auto new_end = new_begin;
        try {
            new_end = mystl::uninitialized_move(begin_, pos, new_begin);
            new_end = mystl::uninitialized_fill_n(new_end, n, value);
            new_end = mystl::uninitialized_move(pos, end_, new_end);
        } catch (...) {
            destroy_and_recover(new_begin, new_end, new_size);
            throw;
        }
        // b. 释放原有的内存空间，并将 `begin_`、`end_` 和 `cap_`
        // 指向新的内存空间的起始位置、末尾位置和尾后位置
        data_allocator::deallocate(begin_, cap_ - begin_);
        begin_ = new_begin;
        end_ = new_end;
        cap_ = begin_ + new_size;
    }
    return begin_ + xpos;
}

// / copy_insert 函数
template <class T>
template <class IIter>
void vector<T>::copy_insert(iterator pos, IIter first, IIter last) {
    if (first == last)
        return;
    const auto n = mystl::distance(first, last);
    if ((cap_ - end_) >= n) {  // 如果备用空间大小足够
        const auto after_elems = end_ - pos;
        auto old_end = end_;
        if (after_elems > n) {
            end_ = mystl::uninitialized_copy(end_ - n, end_, end_);
            mystl::move_backward(pos, old_end - n, old_end);
            mystl::uninitialized_copy(first, last, pos);
        } else {
            auto mid = first;
            mystl::advance(mid, after_elems);
            end_ = mystl::uninitialized_copy(mid, last, end_);
            end_ = mystl::uninitialized_move(pos, old_end, end_);
            mystl::uninitialized_copy(first, mid, pos);
        }
    } else {  // 备用空间不足
        const auto new_size = get_new_cap(n);
        auto new_begin = data_allocator::allocate(new_size);
        auto new_end = new_begin;
        try {
            new_end = mystl::uninitialized_move(begin_, pos, new_begin);
            new_end = mystl::uninitialized_copy(first, last, new_end);
            new_end = mystl::uninitialized_move(pos, end_, new_end);
        } catch (...) {
            destroy_and_recover(new_begin, new_end, new_size);
            throw;
        }
        data_allocator::deallocate(begin_, cap_ - begin_);
        begin_ = new_begin;
        end_ = new_end;
        cap_ = begin_ + new_size;
    }
}

// 这段代码实现了 `vector` 类模板的重新插入函数
// `reinsert`。这个函数的作用是重新分配内存空间，将 `vector`
// 中存储的元素拷贝到新的内存空间中，并释放旧的内存空间。

// 具体来说，这个函数首先使用 `data_allocator::allocate`
// 函数分配一块新的内存空间，大小为 `size`。然后，使用
// `mystl::uninitialized_move`
// 函数，将旧的内存空间中的元素拷贝到新的内存空间中。如果在拷贝过程中出现异常，就需要使用
// `data_allocator::deallocate` 函数释放新的内存空间，并将异常继续抛出。

// 如果拷贝操作成功完成，就使用 `data_allocator::deallocate`
// 函数释放旧的内存空间，并将 `begin_` 指向新的内存空间的起始位置，将 `end_`
// 指向新的内存空间中存储的最后一个元素的下一个位置，将 `cap_`
// 指向新的内存空间的末尾位置。

// 总之，这个函数的实现步骤非常清晰，而且非常重要，因为 `vector`
// 类模板的扩容操作和重新插入操作都需要使用到这个函数。 reinsert函数
// 函数的作用是重新分配内存空间，将 `vector`
// 中存储的元素拷贝到新的内存空间中，并释放旧的内存空间
template <class T>
void vector<T>::reinsert(size_type size) {
    // 使用 `data_allocator::allocate` 函数分配一块新的内存空间，大小为 `size`
    auto new_begin = data_allocator::allocate(size);
    try {
        // 然后，使用 `mystl::uninitialized_move`
        // 函数，将旧的内存空间中的元素拷贝到新的内存空间中
        mystl::uninitialized_move(begin_, end_, new_begin);
    } catch (...) {
        // 如果在拷贝过程中出现异常，就需要使用 `data_allocator::deallocate`
        // 函数释放新的内存空间，并将异常继续抛出
        data_allocator::deallocate(new_begin, size);
        throw;
    }
    data_allocator::deallocate(begin_, cap_ - begin_);
    begin_ = new_begin;
    end_ = begin_ + size;
    cap_ = begin_ + size;
}

/*******************************************/
// 重载比较操作符

template <class T>
bool operator==(const vector<T>& lhs, const vector<T>& rhs) {
    return lhs.size() == rhs.size() &&
           mystl::equal(lhs.begin(), lhs.end(), rhs.begin());
}

template <class T>
bool operator<(const vector<T>& lhs, const vector<T>& rhs) {
    return mystl::lexicographical_compare(lhs.begin(), lhs.end(), rhs.begin(),
                                          rhs.end());
}

template <class T>
bool operator!=(const vector<T>& lhs, const vector<T>& rhs) {
    return !(lhs == rhs);
}

template <class T>
bool operator>(const vector<T>& lhs, const vector<T>& rhs) {
    return rhs < lhs;
}

template <class T>
bool operator<=(const vector<T>& lhs, const vector<T>& rhs) {
    return !(rhs < lhs);
}

template <class T>
bool operator>=(const vector<T>& lhs, const vector<T>& rhs) {
    return !(lhs < rhs);
}

// 重载 mystl 的 swap
template <class T>
void swap(vector<T>& lhs, vector<T>& rhs) {
    // 简单的交换
    lhs.swap(rhs);
}

}  // namespace mystl

#endif
