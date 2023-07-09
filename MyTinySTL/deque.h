#ifndef MYTINYSTL_DEQUE_H_
#define MYTINYSTL_DEQUE_H_

// 这个头文件包含了一个模板类 deque
// deque: 双端队列

// notes:
//
// 异常保证：
// mystl::deque<T>
// 满足基本异常保证，部分函数无异常保证，并对以下等函数做强异常安全保证：
//   * emplace_front
//   * emplace_back
//   * emplace
//   * push_front
//   * push_back
//   * insert

#include <initializer_list>

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

// deque map 初始化的大小
#ifndef DEQUE_MAP_INIT_SIZE
#define DEQUE_MAP_INIT_SIZE 8
#endif

template <class T>
struct deque_buf_size {
    static constexpr size_t value = sizeof(T) < 256 ? 4096 / sizeof(T) : 16;
};

// deque 的迭代器设计
template <class T, class Ref, class Ptr>
struct deque_iterator : public iterator<random_access_iterator_tag, T> {
    typedef deque_iterator<T, T&, T*> iterator;
    typedef deque_iterator<T, const T&, const T*> const_iterator;
    typedef deque_iterator self;

    typedef T value_type;
    typedef Ptr pointer;
    typedef Ref reference;
    typedef size_t size_type;
    typedef ptrdiff_t difference_type;
    typedef T* value_pointer;
    typedef T** map_pointer;

    static const size_type buffer_size = deque_buf_size<T>::value;

    // 迭代器所含成员数据
    value_pointer cur;    // 指向所在缓冲区的当前元素
    value_pointer first;  // 指向所在缓冲区的头部
    value_pointer last;   // 指向所在缓冲区的尾部
    map_pointer node;     // 缓冲区所在节点

    // 构造、复制、移动函数
    deque_iterator() noexcept
        : cur(nullptr), first(nullptr), last(nullptr), node(nullptr) {}

    deque_iterator(value_pointer v, map_pointer n)
        : cur(v), first(*n), last(*n + buffer_size), node(n) {}

    deque_iterator(const iterator& rhs)
        : cur(rhs.cur), first(rhs.first), last(rhs.last), node(rhs.node) {}
    deque_iterator(iterator&& rhs) noexcept
        : cur(rhs.cur), first(rhs.first), last(rhs.last), node(rhs.node) {
        rhs.cur = nullptr;
        rhs.first = nullptr;
        rhs.last = nullptr;
        rhs.node = nullptr;
    }

    deque_iterator(const const_iterator& rhs)
        : cur(rhs.cur), first(rhs.first), last(rhs.last), node(rhs.node) {}

    self& operator=(const iterator& rhs) {
        if (this != &rhs) {
            cur = rhs.cur;
            first = rhs.first;
            last = rhs.last;
            node = rhs.node;
        }
        return *this;
    }

    // 转到另一个缓冲区
    void set_node(map_pointer new_node) {
        node = new_node;
        first = *new_node;
        last = first + buffer_size;
    }

    // 重载运算符
    reference operator*() const { return *cur; }
    pointer operator->() const { return cur; }

    difference_type operator-(const self& x) const {
        return static_cast<difference_type>(buffer_size) * (node - x.node) +
               (cur - first) - (x.cur - x.first);
    }

    self& operator++() {
        ++cur;
        if (cur == last) {  // 如果到达缓冲区的尾
            set_node(node + 1);
            cur = first;
        }
        return *this;
    }
    self operator++(int) {
        self tmp = *this;
        ++*this;
        return tmp;
    }

    self& operator--() {
        if (cur == first) {  // 如果到达缓冲区的头
            set_node(node - 1);
            cur = last;
        }
        --cur;
        return *this;
    }
    self operator--(int) {
        self tmp = *this;
        --*this;
        return tmp;
    }

    self& operator+=(difference_type n) {
        const auto offset = n + (cur - first);
        if (offset >= 0 && offset < static_cast<difference_type>(
                                        buffer_size)) {  // 仍在当前缓冲区
            cur += n;
        } else {  // 要跳到其他的缓冲区
            const auto node_offset =
                offset > 0 ? offset / static_cast<difference_type>(buffer_size)
                           : -static_cast<difference_type>((-offset - 1) /
                                                           buffer_size) -
                                 1;
            set_node(node + node_offset);
            cur = first + (offset - node_offset * static_cast<difference_type>(
                                                      buffer_size));
        }
        return *this;
    }
    self operator+(difference_type n) const {
        self tmp = *this;
        return tmp += n;
    }
    self& operator-=(difference_type n) { return *this += -n; }
    self operator-(difference_type n) const {
        self tmp = *this;
        return tmp -= n;
    }

    reference operator[](difference_type n) const { return *(*this + n); }

    // 重载比较操作符
    bool operator==(const self& rhs) const { return cur == rhs.cur; }
    bool operator<(const self& rhs) const {
        return node == rhs.node ? (cur < rhs.cur) : (node < rhs.node);
    }
    bool operator!=(const self& rhs) const { return !(*this == rhs); }
    bool operator>(const self& rhs) const { return rhs < *this; }
    bool operator<=(const self& rhs) const { return !(rhs < *this); }
    bool operator>=(const self& rhs) const { return !(*this < rhs); }
};

// 模板类 deque
// 模板参数代表数据类型
template <class T>
class deque {
public:
    // deque 的型别定义
    typedef mystl::allocator<T> allocator_type;
    typedef mystl::allocator<T> data_allocator;
    typedef mystl::allocator<T*> map_allocator;

    typedef typename allocator_type::value_type value_type;
    typedef typename allocator_type::pointer pointer;
    typedef typename allocator_type::const_pointer const_pointer;
    typedef typename allocator_type::reference reference;
    typedef typename allocator_type::const_reference const_reference;
    typedef typename allocator_type::size_type size_type;
    typedef typename allocator_type::difference_type difference_type;
    typedef pointer* map_pointer;  // 指针指针，指向一个缓冲区
    typedef const_pointer* const_map_pointer;

    typedef deque_iterator<T, T&, T*> iterator;
    typedef deque_iterator<T, const T&, const T*> const_iterator;
    typedef mystl::reverse_iterator<iterator> reverse_iterator;
    typedef mystl::reverse_iterator<const_iterator> const_reverse_iterator;

    allocator_type get_allocator() { return allocator_type(); }

    static const size_type buffer_size = deque_buf_size<T>::value;

private:
    // 用以下四个数据来表现一个 deque
    iterator begin_;  // 指向第一个节点
    iterator end_;    // 指向最后一个结点
    map_pointer
        map_;  // 指向一块 map，map 中的每个元素都是一个指针，指向一个缓冲区
    size_type map_size_;  // map 内指针的数目

public:
    // 构造、复制、移动、析构函数

    deque() { fill_init(0, value_type()); }

    explicit deque(size_type n) { fill_init(n, value_type()); }

    deque(size_type n, const value_type& value) { fill_init(n, value); }

    template <class IIter,
              typename std::enable_if<mystl::is_input_iterator<IIter>::value,
                                      int>::type = 0>
    deque(IIter first, IIter last) {
        copy_init(first, last, iterator_category(first));
    }

    deque(std::initializer_list<value_type> ilist) {
        copy_init(ilist.begin(), ilist.end(), mystl::forward_iterator_tag());
    }

    deque(const deque& rhs) {
        copy_init(rhs.begin(), rhs.end(), mystl::forward_iterator_tag());
    }
    deque(deque&& rhs) noexcept
        : begin_(mystl::move(rhs.begin_)),
          end_(mystl::move(rhs.end_)),
          map_(rhs.map_),
          map_size_(rhs.map_size_) {
        rhs.map_ = nullptr;
        rhs.map_size_ = 0;
    }

    deque& operator=(const deque& rhs);
    deque& operator=(deque&& rhs);

    deque& operator=(std::initializer_list<value_type> ilist) {
        deque tmp(ilist);
        swap(tmp);
        return *this;
    }

    ~deque() {
        if (map_ != nullptr) {
            clear();
            data_allocator::deallocate(*begin_.node, buffer_size);
            *begin_.node = nullptr;
            map_allocator::deallocate(map_, map_size_);
            map_ = nullptr;
        }
    }

public:
    // 迭代器相关操作

    iterator begin() noexcept { return begin_; }
    const_iterator begin() const noexcept { return begin_; }
    iterator end() noexcept { return end_; }
    const_iterator end() const noexcept { return end_; }

    reverse_iterator rbegin() noexcept { return reverse_iterator(end()); }
    const_reverse_iterator rbegin() const noexcept {
        return reverse_iterator(end());
    }
    reverse_iterator rend() noexcept { return reverse_iterator(begin()); }
    const_reverse_iterator rend() const noexcept {
        return reverse_iterator(begin());
    }

    const_iterator cbegin() const noexcept { return begin(); }
    const_iterator cend() const noexcept { return end(); }
    const_reverse_iterator crbegin() const noexcept { return rbegin(); }
    const_reverse_iterator crend() const noexcept { return rend(); }

    // 容量相关操作

    bool empty() const noexcept { return begin() == end(); }
    size_type size() const noexcept { return end_ - begin_; }
    size_type max_size() const noexcept { return static_cast<size_type>(-1); }
    void resize(size_type new_size) { resize(new_size, value_type()); }
    void resize(size_type new_size, const value_type& value);
    void shrink_to_fit() noexcept;

    // 访问元素相关操作
    reference operator[](size_type n) {
        MYSTL_DEBUG(n < size());
        return begin_[n];
    }
    const_reference operator[](size_type n) const {
        MYSTL_DEBUG(n < size());
        return begin_[n];
    }

    reference at(size_type n) {
        THROW_OUT_OF_RANGE_IF(!(n < size()),
                              "deque<T>::at() subscript out of range");
        return (*this)[n];
    }
    const_reference at(size_type n) const {
        THROW_OUT_OF_RANGE_IF(!(n < size()),
                              "deque<T>::at() subscript out of range");
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

    // 修改容器相关操作

    // assign

    void assign(size_type n, const value_type& value) { fill_assign(n, value); }

    template <class IIter,
              typename std::enable_if<mystl::is_input_iterator<IIter>::value,
                                      int>::type = 0>
    void assign(IIter first, IIter last) {
        copy_assign(first, last, iterator_category(first));
    }

    void assign(std::initializer_list<value_type> ilist) {
        copy_assign(ilist.begin(), ilist.end(), mystl::forward_iterator_tag{});
    }

    // emplace_front / emplace_back / emplace

    template <class... Args>
    void emplace_front(Args&&... args);
    template <class... Args>
    void emplace_back(Args&&... args);
    template <class... Args>
    iterator emplace(iterator pos, Args&&... args);

    // push_front / push_back

    void push_front(const value_type& value);
    void push_back(const value_type& value);

    void push_front(value_type&& value) { emplace_front(mystl::move(value)); }
    void push_back(value_type&& value) { emplace_back(mystl::move(value)); }

    // pop_back / pop_front

    void pop_front();
    void pop_back();

    // insert

    iterator insert(iterator position, const value_type& value);
    iterator insert(iterator position, value_type&& value);
    void insert(iterator position, size_type n, const value_type& value);
    template <class IIter,
              typename std::enable_if<mystl::is_input_iterator<IIter>::value,
                                      int>::type = 0>
    void insert(iterator position, IIter first, IIter last) {
        insert_dispatch(position, first, last, iterator_category(first));
    }

    // erase /clear

    iterator erase(iterator position);
    iterator erase(iterator first, iterator last);
    void clear();

    // swap

    void swap(deque& rhs) noexcept;

private:
    // helper functions

    // create node / destroy node
    map_pointer create_map(size_type size);
    void create_buffer(map_pointer nstart, map_pointer nfinish);
    void destroy_buffer(map_pointer nstart, map_pointer nfinish);

    // initialize
    void map_init(size_type nelem);
    void fill_init(size_type n, const value_type& value);
    template <class IIter>
    void copy_init(IIter, IIter, input_iterator_tag);
    template <class FIter>
    void copy_init(FIter, FIter, forward_iterator_tag);

    // assign
    void fill_assign(size_type n, const value_type& value);
    template <class IIter>
    void copy_assign(IIter first, IIter last, input_iterator_tag);
    template <class FIter>
    void copy_assign(FIter first, FIter last, forward_iterator_tag);

    // insert
    template <class... Args>
    iterator insert_aux(iterator position, Args&&... args);
    void fill_insert(iterator position, size_type n, const value_type& x);
    template <class FIter>
    void copy_insert(iterator, FIter, FIter, size_type);
    template <class IIter>
    void insert_dispatch(iterator, IIter, IIter, input_iterator_tag);
    template <class FIter>
    void insert_dispatch(iterator, FIter, FIter, forward_iterator_tag);

    // reallocate
    void require_capacity(size_type n, bool front);
    void reallocate_map_at_front(size_type need);
    void reallocate_map_at_back(size_type need);
};

// 复制赋值运算符
template <class T>
deque<T>& deque<T>::operator=(const deque& rhs) {
    // 当赋值的deque不是自身时
    if (this != &rhs) {
        // 判断当前deque的大小，如果大于等于rhs的大小
        const auto len = size();
        if (len >= rhs.size()) {
            // 则用rhs中的元素覆盖当前deque中的元素
            erase(mystl::copy(rhs.begin_, rhs.end_, begin_), end_);
        } else {
            // 如果小于rhs的大小
            iterator mid = rhs.begin() + static_cast<difference_type>(len);
            // 先拷贝rhs开始的len个元素
            mystl::copy(rhs.begin_, mid, begin_);
            // 在拷贝剩下rhs多余长度
            insert(end_, mid, rhs.end_);
        }
    }
    return *this;
}

// 移动赋值运算符
// 制赋值是将一个对象的值复制到另一个对象中，
// 而移动赋值则是将一个对象的资源（比如内存或文件句柄）移动到另一个对象中，同时将原对象置为空。
template <class T>
deque<T>& deque<T>::operator=(deque&& rhs) {
    clear();
    begin_ = mystl::move(rhs.begin_);
    end_ = mystl::move(rhs.end_);
    map_ = rhs.map_;
    map_size_ = rhs.map_size_;
    rhs.map_ = nullptr;
    rhs.map_size_ = 0;
    return *this;
}

// 重置容器大小
template <class T>
void deque<T>::resize(size_type new_size, const value_type& value) {
    const auto len = size();
    if (new_size < len) {
        // 减小容器大小，做erase清楚操作
        erase(begin_ + new_size, end_);
    } else {
        // 扩大容器大小，在end_出新添new_size-len个value
        insert(end_, new_size - len, value);
    }
}

// 减小容器容量
// 作用:将deque中多余的缓冲区释放，使得deque中只留下必要的缓冲区
template <class T>
void deque<T>::shrink_to_fit() noexcept {
    // 至少会留下头部缓冲区
    // 遍历map_指向的数组，释放begin_.node之前的缓冲区
    for (auto cur = map_; cur < begin_.node; ++cur) {
        data_allocator::deallocate(*cur, buffer_size);
        *cur = nullptr;
    }
    // 遍历end_.node之后的缓冲区，释放多余的缓冲区
    for (auto cur = end_.node + 1; cur < map_ + map_size_; ++cur) {
        // 调用data_allocator::deallocate函数来释放内存，并将对应的指针设置为nullptr，以避免重复释放
        data_allocator::deallocate(*cur, buffer_size);
        *cur = nullptr;
    }
}

// 在头部就地构建元素
template <class T>
template <class... Args>
void deque<T>::emplace_front(Args&&... args) {
    if (begin_.cur != begin_.first) {
        // 说明当前begin_节点缓冲区还有可用位置
        // 直接在cur之前构造一个
        data_allocator::construct(begin_.cur - 1,
                                  mystl::forward<Args>(args)...);
        --begin_.cur;
    } else {
        require_capacity(1, true);
        try {
            // 往前移动一个节点，然后狗仔
            --begin_;
            data_allocator::construct(begin_.cur,
                                      mystl::forward<Args>(args)...);
        } catch (...) {
            // 异常了再加回来
            ++begin_;
            throw;
        }
    }
}

// 在尾部就地构建元素
template <class T>
template <class... Args>
void deque<T>::emplace_back(Args&&... args) {
    if (end_.cur != end_.last - 1) {
        data_allocator::construct(end_.cur, mystl::forward<Args>(args)...);
        ++end_.cur;
    } else {
        require_capacity(1, false);
        data_allocator::construct(end_.cur, mystl::forward<Args>(args)...);
        ++end_;
    }
}

// 在 pos 位置就地构建元素
template <class T>
template <class... Args>
typename deque<T>::iterator deque<T>::emplace(iterator pos, Args&&... args) {
    // 如果pos位置刚好在头尾
    if (pos.cur == begin_.cur) {
        emplace_front(mystl::forward<Args>(args)...);
        return begin_;
    } else if (pos.cur == end_.cur) {
        emplace_back(mystl::forward<Args>(args)...);
        return end_ - 1;
    }
    // 在pos位置插入元素函数
    return insert_aux(pos, mystl::forward<Args>(args)...);
}

// 在头部插入元素
template <class T>
void deque<T>::push_front(const value_type& value) {
    if (begin_.cur != begin_.first) {
        // 当前begin_节点缓冲区还有可用位置
        data_allocator::construct(begin_.cur - 1, value);
        --begin_.cur;
    } else {
        require_capacity(1, true);
        try {
            // 节点前移
            --begin_;
            data_allocator::construct(begin_.cur, value);
        } catch (...) {
            ++begin_;
            throw;
        }
    }
}

// 在尾部插入元素
template <class T>
void deque<T>::push_back(const value_type& value) {
    if (end_.cur != end_.last - 1) {
        data_allocator::construct(end_.cur, value);
        ++end_.cur;
    } else {
        require_capacity(1, false);
        data_allocator::construct(end_.cur, value);
        ++end_;
    }
}

// 弹出头部元素
template <class T>
void deque<T>::pop_front() {
    MYSTL_DEBUG(!empty());
    if (begin_.cur != begin_.last - 1) {
        data_allocator::destroy(begin_.cur);
        ++begin_.cur;
    } else {
        // 如果在当前begin_节点所在缓冲区的尾部
        data_allocator::destroy(begin_.cur);
        ++begin_;
        destroy_buffer(begin_.node - 1, begin_.node - 1);
    }
}

// 弹出尾部元素
template <class T>
void deque<T>::pop_back() {
    MYSTL_DEBUG(!empty());
    if (end_.cur != end_.first) {
        --end_.cur;
        data_allocator::destroy(end_.cur);
    } else {
        --end_;
        data_allocator::destroy(end_.cur);
        // 只删除一个元素，缓冲区中开始位置和结束位置可以一样
        destroy_buffer(end_.node + 1, end_.node + 1);
    }
}

// 在 position 处插入元素
template <class T>
typename deque<T>::iterator deque<T>::insert(iterator position,
                                             const value_type& value) {
    if (position.cur == begin_.cur) {
        push_front(value);
        return begin_;
    } else if (position.cur == end_.cur) {
        push_back(value);
        auto tmp = end_;
        --tmp;
        return tmp;
    } else {
        return insert_aux(position, value);
    }
}

template <class T>
typename deque<T>::iterator deque<T>::insert(iterator position,
                                             value_type&& value) {
    if (position.cur == begin_.cur) {
        // 移动引用
        emplace_front(mystl::move(value));
        return begin_;
    } else if (position.cur == end_.cur) {
        emplace_back(mystl::move(value));
        auto tmp = end_;
        --tmp;
        return tmp;
    } else {
        return insert_aux(position, mystl::move(value));
    }
}

// 在 position 位置插入 n 个元素
template <class T>
void deque<T>::insert(iterator position, size_type n, const value_type& value) {
    if (position.cur == begin_.cur) {
        // 刚好指向最头部
        require_capacity(n, true);
        auto new_begin = begin_ - n;
        mystl::uninitialized_fill_n(new_begin, n, value);
        begin_ = new_begin;
    } else if (position.cur == end_.cur) {
        // 刚好指向最尾部
        require_capacity(n, false);
        auto new_end = end_ + n;
        mystl::uninitialized_fill_n(end_, n, value);
        end_ = new_end;
    } else {
        fill_insert(position, n, value);
    }
}

// 删除 position 处的元素
// 要么前移，要么后移，选择移动元素少的
template <class T>
typename deque<T>::iterator deque<T>::erase(iterator position) {
    auto next = position;
    ++next;
    const size_type elems_before = position - begin_;
    if (elems_before < (size() / 2)) {
        // 将 [beging_, position)区间内的元素拷贝到 [next - (position - begin_),
        // next) 就是把position位置的元素删掉，整体后移
        mystl::copy_backward(begin_, position, next);
        pop_front();
    } else {
        // 相当于后移
        mystl::copy(next, end_, position);
        pop_back();
    }
    return begin_ + elems_before;
}

// 删除[first, last)上的元素
template <class T>
typename deque<T>::iterator deque<T>::erase(iterator first, iterator last) {
    // 如果真个deque删除，直接调用clear
    if (first == begin_ && last == end_) {
        clear();
        return end_;
    } else {
        const size_type len = last - first;
        const size_type elems_before = first - begin_;
        if (elems_before < ((size() - len) / 2)) {
            // 如果要删除的元素位于deque的前半部分，
            // 那么就将[first, last)之前的元素向后移动len个位置，
            // 然后调整begin_指针的位置，使其指向新的起始位置
            mystl::copy_backward(begin_, first, last);
            auto new_begin = begin_ + len;
            data_allocator::destroy(begin_.cur, new_begin.cur);
            begin_ = new_begin;
        } else {
            // 要删除的元素位于deque的后半部分，
            // 那么就将[last,end_)之后的元素向前移动len个位置，
            // 然后调整end_指针的位置，使其指向新的结束位置。
            mystl::copy(last, end_, first);
            auto new_end = end_ - len;
            data_allocator::destroy(new_end.cur, end_.cur);
            end_ = new_end;
        }
        // 最后返回的是begin_+elems_before，也就是删除操作之后，第一个未被删除的元素的位置
        return begin_ + elems_before;
    }
}

// 清空 deque
template <class T>
void deque<T>::clear() {
    // clear会保留头部的缓冲区，因为只有一个缓冲区时，需要保留该缓冲区以供后续使用
    // 遍历 deque 容器中除了头部和尾部缓冲区之外的所有缓冲区，并调用 destroy
    // 函数销毁其中的所有元素。
    for (map_pointer cur = begin_.node + 1; cur < end_.node; ++cur) {
        // data_allocator::destroy删除缓冲区
        data_allocator::destroy(*cur, *cur + buffer_size);
    }

    // mystl::destroy删除缓冲区内的元素
    if (begin_.node != end_.node) {  // 有两个以上的缓冲区
        mystl::destroy(begin_.cur, begin_.last);
        mystl::destroy(end_.first, end_.cur);
    } else {
        mystl::destroy(begin_.cur, end_.cur);
    }
    shrink_to_fit();
    // mystl::destroy
    end_ = begin_;
}

// 交换两个 deque
template <class T>
void deque<T>::swap(deque& rhs) noexcept {
    if (this != &rhs) {
        mystl::swap(begin_, rhs.begin_);
        mystl::swap(end_, rhs.end_);
        mystl::swap(map_, rhs.map_);
        mystl::swap(map_size_, rhs.map_size_);
    }
}

/*****************************************************************************************/
// helper function

// 创建一个大小为size的map，map是一个指针数组，每个指针指向一个缓冲区，缓冲区中存储了多个元素
template <class T>
typename deque<T>::map_pointer deque<T>::create_map(size_type size) {
    map_pointer mp = nullptr;
    // allocate函数分配一段连续内存空间，大小为size个指针的大小
    mp = map_allocator::allocate(size);
    for (size_type i = 0; i < size; ++i)
        // 使用for循环将每个指针初始化为nullptr
        *(mp + i) = nullptr;
    return mp;
}

// create_buffer 函数
// 为 deque 分配一段内存空间，并将这段内存空间划分成若干个大小为 buffer_size
// 的块，这些块被称为“缓冲区”，每个缓冲区可以容纳多个元素
template <class T>
// 该函数接受两个参数 nstart 和 nfinish，它们是指向指针的指针，表示 deque
// 的起始和结束位置
void deque<T>::create_buffer(map_pointer nstart, map_pointer nfinish) {
    map_pointer cur;
    try {
        for (cur = nstart; cur <= nfinish; ++cur) {
            // 一个一个分配大小为buffer_size的块，并将大小存在cur这个指向指针的指针里
            // buffer_size是一个之前算出来的值
            *cur = data_allocator::allocate(buffer_size);
        }
    } catch (...) {
        // 否则从后往前释放已经分配的内存
        while (cur != nstart) {
            --cur;
            data_allocator::deallocate(*cur, buffer_size);
            *cur = nullptr;
        }
        throw;
    }
}

// destroy_buffer 函数
template <class T>
void deque<T>::destroy_buffer(map_pointer nstart, map_pointer nfinish) {
    for (map_pointer n = nstart; n <= nfinish; ++n) {
        data_allocator::deallocate(*n, buffer_size);
        *n = nullptr;
    }
}

// map_init 函数
// 用于初始化deque的map和buffer
// 根据需要存储的元素数量nElem，计算需要分配的缓冲区数量nNode，然后分配map和buffer的内存空间，并将它们初始化
template <class T>
void deque<T>::map_init(size_type nElem) {
    const size_type nNode = nElem / buffer_size + 1;  // 需要分配的缓冲区个数
    // map数组的大小map_size_
    map_size_ =
        mystl::max(static_cast<size_type>(DEQUE_MAP_INIT_SIZE), nNode + 2);
    try {
        // 产生map_数组
        map_ = create_map(map_size_);
    } catch (...) {
        map_ = nullptr;
        map_size_ = 0;
        throw;
    }

    // 让 nstart 和 nfinish 都指向 map_ 最中央的区域，方便向头尾扩充
    map_pointer nstart = map_ + (map_size_ - nNode) / 2;
    map_pointer nfinish = nstart + nNode - 1;
    try {
        // 分配缓冲区的内存空间
        create_buffer(nstart, nfinish);
    } catch (...) {
        map_allocator::deallocate(map_, map_size_);
        map_ = nullptr;
        map_size_ = 0;
        throw;
    }
    // begin_节点为nstart所在的节点
    begin_.set_node(nstart);
    // end_节点为end_所在的节点
    end_.set_node(nfinish);
    begin_.cur = begin_.first;
    end_.cur = end_.first + (nElem % buffer_size);
}

// fill_init 函数
template <class T>
void deque<T>::fill_init(size_type n, const value_type& value) {
    map_init(n);
    if (n != 0) {
        for (auto cur = begin_.node; cur < end_.node; ++cur) {
            mystl::uninitialized_fill(*cur, *cur + buffer_size, value);
        }
        mystl::uninitialized_fill(end_.first, end_.cur, value);
    }
}

// copy_init 函数
template <class T>
template <class IIter>
void deque<T>::copy_init(IIter first, IIter last, input_iterator_tag) {
    const size_type n = mystl::distance(first, last);
    map_init(n);
    for (; first != last; ++first)
        emplace_back(*first);
}

template <class T>
template <class FIter>
void deque<T>::copy_init(FIter first, FIter last, forward_iterator_tag) {
    const size_type n = mystl::distance(first, last);
    map_init(n);
    for (auto cur = begin_.node; cur < end_.node; ++cur) {
        auto next = first;
        mystl::advance(next, buffer_size);
        mystl::uninitialized_copy(first, next, *cur);
        first = next;
    }
    mystl::uninitialized_copy(first, last, end_.first);
}

// fill_assign 函数
template <class T>
void deque<T>::fill_assign(size_type n, const value_type& value) {
    if (n > size()) {
        // 现填充几个
        mystl::fill(begin(), end(), value);
        // 再填充剩下的n-size()个value
        insert(end(), n - size(), value);
    } else {
        // 把尾巴几个清除
        erase(begin() + n, end());
        mystl::fill(begin(), end(), value);
    }
}

// copy_assign 函数
template <class T>
template <class IIter>
void deque<T>::copy_assign(IIter first, IIter last, input_iterator_tag) {
    auto first1 = begin();
    auto last1 = end();
    for (; first != last && first1 != last1; ++first, ++first1) {
        *first1 = *first;
    }
    if (first1 != last1) {
        erase(first1, last1);
    } else {
        insert_dispatch(end_, first, last, input_iterator_tag{});
    }
}

template <class T>
template <class FIter>
void deque<T>::copy_assign(FIter first, FIter last, forward_iterator_tag) {
    const size_type len1 = size();
    const size_type len2 = mystl::distance(first, last);
    if (len1 < len2) {
        auto next = first;
        mystl::advance(next, len1);
        mystl::copy(first, next, begin_);
        insert_dispatch(end_, next, last, forward_iterator_tag{});
    } else {
        erase(mystl::copy(first, last, begin_), end_);
    }
}

// insert_aux 函数
// 用于在指定位置插入元素
template <class T>
template <class... Args>
typename deque<T>::iterator deque<T>::insert_aux(iterator position,
                                                 Args&&... args) {
    // 首先计算出目标位置之前的元素个数，即elems_before
    const size_type elems_before = position - begin_;
    // 创建一个value_copy对象，用于保存插入的元素值
    value_type value_copy = value_type(mystl::forward<Args>(args)...);
    if (elems_before < (size() / 2)) {  // 在前半段插入
        // deque的头部插入一个元素，并将插入位置之后的元素向后移动一位
        emplace_front(front());
        auto front1 = begin_;
        ++front1;
        auto front2 = front1;
        ++front2;
        position = begin_ + elems_before;
        auto pos = position;
        ++pos;
        // 因为pos是右开，所以前面++了
        mystl::copy(front2, pos, front1);
    } else {  // 在后半段插入
        // deque的尾部插入一个元素，并将插入位置之前的元素向前移动一位
        emplace_back(back());
        auto back1 = end_;
        --back1;
        auto back2 = back1;
        --back2;
        position = begin_ + elems_before;
        mystl::copy_backward(position, back2, back1);
    }
    *position = mystl::move(value_copy);
    return position;
}

// fill_insert 函数
template <class T>
void deque<T>::fill_insert(iterator position,
                           size_type n,
                           const value_type& value) {
    const size_type elems_before = position - begin_;
    const size_type len = size();
    auto value_copy = value;
    if (elems_before < (len / 2)) {
        require_capacity(n, true);
        // 原来的迭代器可能会失效
        auto old_begin = begin_;
        auto new_begin = begin_ - n;
        position = begin_ + elems_before;
        try {
            if (elems_before >= n) {
                auto begin_n = begin_ + n;
                mystl::uninitialized_copy(begin_, begin_n, new_begin);
                begin_ = new_begin;
                mystl::copy(begin_n, position, old_begin);
                mystl::fill(position - n, position, value_copy);
            } else {
                mystl::uninitialized_fill(
                    mystl::uninitialized_copy(begin_, position, new_begin),
                    begin_, value_copy);
                begin_ = new_begin;
                mystl::fill(old_begin, position, value_copy);
            }
        } catch (...) {
            if (new_begin.node != begin_.node)
                destroy_buffer(new_begin.node, begin_.node - 1);
            throw;
        }
    } else {
        require_capacity(n, false);
        // 原来的迭代器可能会失效
        auto old_end = end_;
        auto new_end = end_ + n;
        const size_type elems_after = len - elems_before;
        position = end_ - elems_after;
        try {
            if (elems_after > n) {
                auto end_n = end_ - n;
                mystl::uninitialized_copy(end_n, end_, end_);
                end_ = new_end;
                mystl::copy_backward(position, end_n, old_end);
                mystl::fill(position, position + n, value_copy);
            } else {
                mystl::uninitialized_fill(end_, position + n, value_copy);
                mystl::uninitialized_copy(position, end_, position + n);
                end_ = new_end;
                mystl::fill(position, old_end, value_copy);
            }
        } catch (...) {
            if (new_end.node != end_.node)
                destroy_buffer(end_.node + 1, new_end.node);
            throw;
        }
    }
}

// copy_insert
template <class T>
template <class FIter>
void deque<T>::copy_insert(iterator position,
                           FIter first,
                           FIter last,
                           size_type n) {
    const size_type elems_before = position - begin_;
    auto len = size();
    if (elems_before < (len / 2)) {
        require_capacity(n, true);
        // 原来的迭代器可能会失效
        auto old_begin = begin_;
        auto new_begin = begin_ - n;
        position = begin_ + elems_before;
        try {
            if (elems_before >= n) {
                auto begin_n = begin_ + n;
                mystl::uninitialized_copy(begin_, begin_n, new_begin);
                begin_ = new_begin;
                mystl::copy(begin_n, position, old_begin);
                mystl::copy(first, last, position - n);
            } else {
                auto mid = first;
                mystl::advance(mid, n - elems_before);
                mystl::uninitialized_copy(
                    first, mid,
                    mystl::uninitialized_copy(begin_, position, new_begin));
                begin_ = new_begin;
                mystl::copy(mid, last, old_begin);
            }
        } catch (...) {
            if (new_begin.node != begin_.node)
                destroy_buffer(new_begin.node, begin_.node - 1);
            throw;
        }
    } else {
        require_capacity(n, false);
        // 原来的迭代器可能会失效
        auto old_end = end_;
        auto new_end = end_ + n;
        const auto elems_after = len - elems_before;
        position = end_ - elems_after;
        try {
            if (elems_after > n) {
                auto end_n = end_ - n;
                mystl::uninitialized_copy(end_n, end_, end_);
                end_ = new_end;
                mystl::copy_backward(position, end_n, old_end);
                mystl::copy(first, last, position);
            } else {
                auto mid = first;
                mystl::advance(mid, elems_after);
                mystl::uninitialized_copy(
                    position, end_, mystl::uninitialized_copy(mid, last, end_));
                end_ = new_end;
                mystl::copy(first, mid, position);
            }
        } catch (...) {
            if (new_end.node != end_.node)
                destroy_buffer(end_.node + 1, new_end.node);
            throw;
        }
    }
}

// insert_dispatch 函数
// 用于在deque中插入元素，使用了迭代器的标签来区分不同类型的迭代器
template <class T>
template <class IIter>
void deque<T>::insert_dispatch(iterator position,
                               IIter first,
                               IIter last,
                               input_iterator_tag) {
    if (last <= first)
        return;
    // 先计算需要插入的元素个数n
    const size_type n = mystl::distance(first, last);
    // 插入位置之前的元素个数elems_before
    const size_type elems_before = position - begin_;
    // 是否需要扩容
    if (elems_before < (size() / 2)) {
        require_capacity(n, true);
    } else {
        require_capacity(n, false);
    }
    position = begin_ + elems_before;
    auto cur = --last;
    // 它从deque的最后一个元素开始，逐个将元素插入到position指向的位置，最终实现了将deque中的元素逆序复制到新的内存空间中的操作
    for (size_type i = 0; i < n; ++i, --cur) {
        insert(position, *cur);
    }
}

template <class T>
template <class FIter>
void deque<T>::insert_dispatch(iterator position,
                               FIter first,
                               FIter last,
                               forward_iterator_tag) {
    if (last <= first)
        return;
    const size_type n = mystl::distance(first, last);
    if (position.cur == begin_.cur) {
        require_capacity(n, true);
        auto new_begin = begin_ - n;
        try {
            mystl::uninitialized_copy(first, last, new_begin);
            begin_ = new_begin;
        } catch (...) {
            if (new_begin.node != begin_.node)
                destroy_buffer(new_begin.node, begin_.node - 1);
            throw;
        }
    } else if (position.cur == end_.cur) {
        require_capacity(n, false);
        auto new_end = end_ + n;
        try {
            mystl::uninitialized_copy(first, last, end_);
            end_ = new_end;
        } catch (...) {
            if (new_end.node != end_.node)
                destroy_buffer(end_.node + 1, new_end.node);
            throw;
        }
    } else {
        copy_insert(position, first, last, n);
    }
}

// require_capacity 函数
// 用于确保deque中有足够的容量来存储元素，至少有n个元素的存储空间
template <class T>
// front参数指定了是在deque的前端进行插入或删除操作，还是在后端进行插入或删除操作
void deque<T>::require_capacity(size_type n, bool front) {
    // 前端进行插入或删除操作，并且当前deque的缓存块不足以容纳n个元素，那么就需要重新分配缓存块
    if (front && (static_cast<size_type>(begin_.cur - begin_.first) < n)) {
        // 判断需要多少个缓存块才能容纳n个元素
        const size_type need_buffer =
            (n - (begin_.cur - begin_.first)) / buffer_size + 1;
        // 需要的缓存块数大于当前deque中已剩余的缓存块数
        if (need_buffer > static_cast<size_type>(begin_.node - map_)) {
            // 则需要重新分配缓存块，并将原有的缓存块移动到新的位置上
            reallocate_map_at_front(need_buffer);
            return;
        }
        // 否则，只需要在原有的缓存块中创建新的缓存块即可
        create_buffer(begin_.node - need_buffer, begin_.node - 1);
    } else if (!front &&
               (static_cast<size_type>(end_.last - end_.cur - 1) < n)) {
        const size_type need_buffer =
            (n - (end_.last - end_.cur - 1)) / buffer_size + 1;
        if (need_buffer >
            static_cast<size_type>((map_ + map_size_) - end_.node - 1)) {
            reallocate_map_at_back(need_buffer);
            return;
        }
        create_buffer(end_.node + 1, end_.node + need_buffer);
    }
}

// reallocate_map_at_front 函数
// 用于在 deque 的前端重新分配内存
template <class T>
void deque<T>::reallocate_map_at_front(size_type need_buffer) {
    // 首先计算出新的 map 大小，即将原来的 map 大小翻倍，或者是原来的 map
    // 大小加上需要的缓存空间和一个初始大小（DEQUE_MAP_INIT_SIZE）的较大值
    const size_type new_map_size = mystl::max(
        map_size_ << 1, map_size_ + need_buffer + DEQUE_MAP_INIT_SIZE);
    // 然后创建一个新的 map，大小为新的 map 大小
    map_pointer new_map = create_map(new_map_size);
    // 计算出原始缓存区的大小和新的缓存区大小
    const size_type old_buffer = end_.node - begin_.node + 1;
    // 新的缓存区大小为原始缓存区大小加上需要的缓存空间
    const size_type new_buffer = old_buffer + need_buffer;

    // 另新的 map 中的指针指向原来的 buffer，并开辟新的 buffer
    // 根据新的缓存区大小和新的 map 大小计算出新的 begin、mid 和 end 指针
    auto begin = new_map + (new_map_size - new_buffer) / 2;
    auto mid = begin + need_buffer;
    auto end = mid + old_buffer;
    // 创建begin到mid-1的缓冲区，用来存新创建的
    create_buffer(begin, mid - 1);
    // 原始缓存区中的元素复制到新的缓存区中
    for (auto begin1 = mid, begin2 = begin_.node; begin1 != end;
         ++begin1, ++begin2)
        *begin1 = *begin2;

    // 更新数据
    // 释放原来的 map 内存
    map_allocator::deallocate(map_, map_size_);
    map_ = new_map;
    map_size_ = new_map_size;
    begin_ = iterator(*mid + (begin_.cur - begin_.first), mid);
    end_ = iterator(*(end - 1) + (end_.cur - end_.first), end - 1);
}

// reallocate_map_at_back 函数
template <class T>
void deque<T>::reallocate_map_at_back(size_type need_buffer) {
    const size_type new_map_size = mystl::max(
        map_size_ << 1, map_size_ + need_buffer + DEQUE_MAP_INIT_SIZE);
    map_pointer new_map = create_map(new_map_size);
    const size_type old_buffer = end_.node - begin_.node + 1;
    const size_type new_buffer = old_buffer + need_buffer;

    // 另新的 map 中的指针指向原来的 buffer，并开辟新的 buffer
    auto begin = new_map + ((new_map_size - new_buffer) / 2);
    auto mid = begin + old_buffer;
    auto end = mid + need_buffer;
    for (auto begin1 = begin, begin2 = begin_.node; begin1 != mid;
         ++begin1, ++begin2)
        *begin1 = *begin2;
    create_buffer(mid, end - 1);

    // 更新数据
    map_allocator::deallocate(map_, map_size_);
    map_ = new_map;
    map_size_ = new_map_size;
    begin_ = iterator(*begin + (begin_.cur - begin_.first), begin);
    end_ = iterator(*(mid - 1) + (end_.cur - end_.first), mid - 1);
}

// 重载比较操作符
template <class T>
bool operator==(const deque<T>& lhs, const deque<T>& rhs) {
    return lhs.size() == rhs.size() &&
           mystl::equal(lhs.begin(), lhs.end(), rhs.begin());
}

template <class T>
bool operator<(const deque<T>& lhs, const deque<T>& rhs) {
    return mystl::lexicographical_compare(lhs.begin(), lhs.end(), rhs.begin(),
                                          rhs.end());
}

template <class T>
bool operator!=(const deque<T>& lhs, const deque<T>& rhs) {
    return !(lhs == rhs);
}

template <class T>
bool operator>(const deque<T>& lhs, const deque<T>& rhs) {
    return rhs < lhs;
}

template <class T>
bool operator<=(const deque<T>& lhs, const deque<T>& rhs) {
    return !(rhs < lhs);
}

template <class T>
bool operator>=(const deque<T>& lhs, const deque<T>& rhs) {
    return !(lhs < rhs);
}

// 重载 mystl 的 swap
template <class T>
void swap(deque<T>& lhs, deque<T>& rhs) {
    lhs.swap(rhs);
}

}  // namespace mystl
#endif  // !MYTINYSTL_DEQUE_H_
