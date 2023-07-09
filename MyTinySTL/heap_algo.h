#ifndef MYTINYSTL_HEAP_ALGO_H_
#define MYTINYSTL_HEAP_ALGO_H_

// 这个头文件包含heap的四个算法：push_heap，pop_heap，make_heap
#include "iterator.h"

namespace mystl {

/*************************************/
// push_heap
// 该函数接受两个迭代器，表示一个 heap
// 容器的首尾，并且新元素已经插入到底部容器的最尾端，调整 heap
/*************************************/
/* 这是一个用于堆排序的 `push_heap_aux`
函数的实现。它的作用是将一个新元素插入到堆中，并保持堆的性质不变。这个函数接受四个参数：

- `first`：一个随机访问迭代器，指向堆的起始位置。
- `holeIndex`：一个整数，表示新元素应该插入的位置。
- `topIndex`：一个整数，表示堆顶元素的位置。
- `value`：一个元素，表示要插入的新元素。

这个函数的实现过程如下：

1. 首先计算出新元素的父节点的位置 `parent`，这个位置是 `(holeIndex-1)/2`。
2. 如果当前位置 `holeIndex` 大于堆顶位置
`topIndex`，并且父节点的值小于新元素的值，那么就将父节点的值向下移动到
`holeIndex` 的位置，同时更新 `holeIndex` 和 `parent` 的值。
3. 重复步骤 2，直到当前位置 `holeIndex` 不再大于堆顶位置
`topIndex`，或者父节点的值不再小于新元素的值。
4. 最后将新元素的值放到 `holeIndex` 的位置上，这样就完成了插入操作。

这个函数的实现中使用了 `operator<`
运算符，因此这是一个最大堆（max-heap）。如果要实现最小堆（min-heap），只需要将
`operator<` 改成 `operator>` 即可。 */
template <class RandomIter, class Distance, class T>
void push_heap_aux(RandomIter first,
                   Distance holeIndex,
                   Distance topIndex,
                   T value) {
    auto parent = (holeIndex - 1) / 2;
    while (holeIndex > topIndex && *(first + parent) < value) {
        // 使用operator<，所以heap为max-heap
        *(first + holeIndex) = *(first + parent);
        holeIndex = parent;
        parent = (holeIndex - 1) / 2;
    }
    *(first + holeIndex) = value;
}

template <class RandomIter, class Distance>
void push_heap_d(RandomIter first, RandomIter last, Distance*) {
    mystl::push_heap_aux(first, (last - first) - 1, static_cast<Distance>(0),
                         *(last - 1));
}

template <class RandomIter>
void push_heap(RandomIter first, RandomIter last) {
    // 新元素应该已置于底部容器的最尾端
    mystl::push_heap_d(first, last, distance_type(first));
}

// 重载版本使用函数对象comp代替比较操作
template <class RandomIter, class Distance, class T, class Compred>
void push_heap_aux(RandomIter first,
                   Distance holeIndex,
                   Distance topIndex,
                   T value,
                   Compred comp) {
    auto parent = (holeIndex - 1) / 2;
    while (holeIndex > topIndex && comp(*(first + parent), value)) {
        *(first + holeIndex) = *(first + parent);
        holeIndex = parent;
        parent = (holeIndex - 1) / 2;
    }
    *(first + holeIndex) = value;
}

template <class RandomIter, class Compred, class Distance>
void push_heap_d(RandomIter first, RandomIter last, Distance*, Compred comp) {
    mystl::push_heap_aux(first, (last - first) - 1, static_cast<Distance>(0),
                         *(last - 1), comp);
}

template <class RandomIter, class Compared>
void push_heap(RandomIter first, RandomIter last, Compared comp) {
    mystl::push_heap_d(first, last, distance_type(first), comp);
}

/********************************/
// pop_heap
// 该函数接受两个迭代器，表示 heap 容器的首尾，将 heap
// 的根节点取出放到容器尾部，调整 heap
/*******************************/
/* 这段代码实现了堆排序算法中的 `adjust_heap`
函数。它用于调整堆中的某个元素，使其满足堆的性质。 在这个函数中，参数 `first`
是一个随机访问迭代器，表示堆的起始位置；
参数 `holeIndex`是一个整数，表示需要调整的元素在堆中的下标；
参数 `len`是一个整数，表示堆的大小；
参数 `value` 是一个值类型，表示需要插入到堆中的元素。

该函数的实现过程如下：

1. 首先进行下溯(percolate down)过程：

- 根据堆的性质，需要调整的元素的子结点下标为 `2 * holeIndex + 1` 和 `2*holeIndex
+ 2`，其中左子结点的下标为 `2 * holeIndex + 1`，右子结点的下标为 `2 * holeIndex
+ 2`。
-
如果右子结点存在且比左子结点小，则将需要调整的元素的值更新为右子结点的值，并将其下标更新为右子结点的下标。
- 否则，将需要调整的元素的值更新为左子结点的值，并将其下标更新为左子结点的下标。
- 重复上述步骤，直到需要调整的元素没有子结点或其子结点的值都比它大。

2. 然后执行一次上溯(percolate up)过程：

- 将需要调整的元素的值赋为 `value`。
-
从需要调整的元素的下标开始向上遍历，将其与其父结点比较，如果其父结点的值比它小，则将其父结点的值赋给它。
- 将需要调整的元素的下标更新为其父结点的下标。
- 重复上述步骤，直到需要调整的元素的值不再比其父结点的值小或者到达堆的顶部。
*/
// 用于在堆排序算法中调整堆的结构
// RandomIter first：表示堆的起始位置的迭代器。
// Distance holeIndex：表示需要调整的元素的索引。
// Distance len：表示堆的长度。
// T value：表示需要插入的值。
template <class RandomIter, class T, class Distance>
void adjust_heap(RandomIter first, Distance holeIndex, Distance len, T value) {
    // 先进行下溯(percolate down)过程
    // 将需要调整的元素保存在 topIndex 中
    auto topIndex = holeIndex;
    // 计算右子节点的索引 rchild，并进入一个循环，直到 rchild 超出堆的长度
    auto rchild = 2 * holeIndex + 2;
    while (rchild < len) {
        // 判断右子节点和左子节点的值的大小，选择较小的节点，并将其值赋给当前需要调整的节点
        if (*(first + rchild) < *(first + rchild - 1)) {
            --rchild;
        }
        *(first + holeIndex) = *(first + rchild);
        // 更新当前需要调整的节点的索引为较小节点的索引，同时更新右子节点的索引
        holeIndex = rchild;
        rchild = 2 * (rchild + 1);
    }
    // 循环结束后，右子节点的索引等于堆的长度，说明没有右子节点，
    if (rchild == len) {
        // 将左子节点的值赋给当前需要调整的节点，并更新当前需要调整的节点的索引为左子节点的索引
        *(first + holeIndex) = *(first + (rchild - 1));
        holeIndex = rchild - 1;
    }
    // 再执行一次上溯(percolate up)过程
    // 调用mystl::push_heap_aux函数保存在 topIndex 中的值插入到正确的位置
    mystl::push_heap_aux(first, holeIndex, topIndex, value);
}

template <class RandomIter, class T, class Distance>
void pop_heap_aux(RandomIter first,
                  RandomIter last,
                  RandomIter result,
                  T value,
                  Distance*) {
    // 先将首值调至尾结点，然后调整[first, last-1)使之重新成为一个max-heap
    *result = *first;
    mystl::adjust_heap(first, static_cast<Distance>(0), last - first, value);
}

template <class RandomIter>
void pop_heap(RandomIter first, RandomIter last) {
    mystl::pop_heap_aux(first, last - 1, last - 1, *(last - 1),
                        distance_type(first));
}

// 重载版本使用函数对象comp代替比较操作
template <class RandomIter, class T, class Distance, class Compared>
void adjust_heap(RandomIter first,
                 Distance holeIndex,
                 Distance len,
                 T value,
                 Compared comp) {
    // 先进行下溯过程
    auto topIndex = holeIndex;
    auto rchild = 2 * holeIndex + 2;
    while (rchild < len) {
        if (comp(*(first + rchild), *(first + rchild - 1))) {
            --rchild;
        }
        *(first + holeIndex) = *(first + rchild);
        holeIndex = rchild;
        rchild = 2 * (rchild + 1);
    }
    if (rchild == len) {
        *(first + holeIndex) = *(first + (rchild - 1));
        holeIndex = rchild - 1;
    }
    // 再执行一次上溯过程
    mystl::push_heap_aux(first, holeIndex, topIndex, value, comp);
}

template <class RandomIter, class T, class Distance, class Compared>
void pop_heap_aux(RandomIter first,
                  RandomIter last,
                  RandomIter result,
                  T value,
                  Distance*,
                  Compared comp) {
    *result = *first;  // 先将尾指设置成首值，即尾指为欲求结果
    mystl::adjust_heap(first, static_cast<Distance>(0), last - first, value,
                       comp);
}

template <class RandomIter, class Compared>
void pop_heap(RandomIter first, RandomIter last, Compared comp) {
    mystl::pop_heap_aux(first, last - 1, last - 1, *(last - 1),
                        distance_type(first), comp);
}

/***********************************/
// sort_heap
// 该函数接受两个迭代器，表示 heap 容器的首尾，不断执行 pop_heap
// 操作，直到首尾最多相差1
/**********************************/
/* 是的，这是 `sort_heap`
函数的实现。该函数使用堆排序算法对一个范围内的元素进行排序。在堆排序中，首先将整个范围构建成一个最大堆，然后反复执行
`pop_heap`
操作，将最大元素移动到范围的末尾，并缩小范围，直到范围只剩下一个元素或为空。最后，范围内的元素就按照从小到大的顺序排列。

`sort_heap` 函数的实现中，每次执行 `pop_heap`
操作后，范围的末尾元素已经被放置到正确的位置上，因此可以将范围的末尾指针 `last`
前移一位。最终，当范围内只剩下一个元素时，排序完成。 */
template <class RandomIter>
void sort_heap(RandomIter first, RandomIter last) {
    // 每执行一次pop_heap，最大的元素都被放到尾部，直到容器最多只有一个元素，完成排序
    while(last-first>1){
        mystl::pop_heap(first, last--);
    }
}

// 重载版本使用函数对象comp代替比较操作
template <class RandomIter, class Compared>
void sort_heap(RandomIter first, RandomIter last, Compared comp) {
    while (last - first > 1) {
        mystl::pop_heap(first, last--, comp);
    }
}

/************************************/
// make_heap
// 该函数接受两个迭代器，表示 heap 容器的首尾，把容器内的数据变为一个 heap
/************************************/
// make_heap_aux，用于在堆排序中重排子树
// first和last是一个随机访问迭代器（RandomIter）表示的范围，表示要重排的子树的元素范围
template <class RandomIter, class Distance>
void make_heap_aux(RandomIter first, RandomIter last, Distance*) {
    // 判断子树的元素数量是否小于2，如果是，则直接返回，因为只有一个元素或没有元素的子树已经是一个合法的堆
    if (last - first < 2) {
        return;
    }
    auto len = last - first;
    // 计算子树的长度 len，即 last - first，
    // 并计算出子树的根节点索引
    // holeIndex，即(len-2)/2。这是因为在堆中，最后一个非叶子节点的索引是(len -
    // 2)/2
    auto holeIndex = (len - 2) / 2;
    while (true) {
        // 进入一个循环，不断重排以 holeIndex 为首的子树。
        // 具体的重排操作由 mystl::adjust_heap
        // 函数完成，它会将根节点的值向下调整，以满足堆的性质
        mystl::adjust_heap(first, holeIndex, len, *(first + holeIndex));
        // 判断holeIndex是否为0，如果是，则说明已经重排完所有的子树，函数返回
        if (holeIndex == 0) {
            return;
        }
        holeIndex--;
    }
}

template <class RandomIter>
void make_heap(RandomIter first, RandomIter last) {
    mystl::make_heap_aux(first, last, distance_type(first));
    ;
}

// 重载版本使用函数对象comp代替比较操作
template <class RandomIter, class Distance, class Compared>
void make_heap_aux(RandomIter first,
                   RandomIter last,
                   Distance*,
                   Compared comp) {
    if (last - first < 2) {
        return;
    }
    auto len = last - first;
    auto holeIndex = (len - 2) / 2;
    while (true) {
        // 重排以holeIndex为首的子树
        mystl::adjust_heap(first, holeIndex, len, *(first + holeIndex), comp);
        if (holeIndex == 0) {
            return;
        }
        holeIndex--;
    }
}

template <class RandomIter, class Compared>
void make_heap(RandomIter first, RandomIter last, Compared comp) {
    mystl::make_heap_aux(first, last, distance_type(first), comp);
}

}  // namespace mystl

#endif  // !MYTINYSTL_HEAP_ALGO_H_