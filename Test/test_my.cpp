/*
禁用 Microsoft Visual Studio 编译器的安全检查机制而设置的。

`_MSC_VER` 是 Microsoft Visual Studio 编译器的预定义宏，用于判断代码是否在Visual
Studio 编译器下编译。`#define`
指令用于定义一个宏，这里定义了`_SCL_SECURE_NO_WARNINGS` 宏，用于禁用
STL（标准模板库）中的安全检查机制。

在 Visual Studio 编译器下，STL 中的某些函数会被视为不安全的，比如
`strcpy`、`strcat`等，因为它们可能会导致缓冲区溢出。为了避免这种情况的发生，编译器会在编译时进行安全检查，如果发现可能会导致缓冲区溢出的代码，就会产生编译错误。通过定义`_SCL_SECURE_NO_WARNINGS`
宏，可以禁用这种安全检查机制，从而让编译通过。 */
#ifdef _MSC_VER
#define _SCL_SECURE_NO_WARNINGS
#endif

/* 这段代码是用于检查内存泄漏的，它的作用是在使用 Visual Studio
编译器和调试模式时，定义了 `_CRTDBG_MAP_ALLOC` 宏，然后包含了 `stdlib.h`
和`crtdbg.h` 头文件。最后，通过 `_CrtDumpMemoryLeaks()` 函数来检查内存泄漏。

在程序结束时，如果发现有内存泄漏，会在输出窗口中显示相关信息，以帮助开发者找出内存泄漏的原因。这段代码只在调试模式下起作用，在发布模式下不会产生任何影响。
*/
#if defined(_MSC_VER) && defined(_DEBUG)
#define _CRTDBG_MAP_ALLOC
#include <crtdbg.h>
#include <stdlib.h>
#endif  // check memory leaks

#include "algorithm_test.h"
#include "deque_test.h"
#include "queue_test.h"
#include "string_test.h"
#include "vector_test.h"
#include "stack_test.h"
#include "list_test.h"
#include "set_test.h"
#include "map_test.h"
#include "unordered_map_test.h"
#include "unordered_set_test.h"
#include "algorithm_performance_test.h"

int main() {
    using namespace mystl::test;

    /* `std::cout.sync_with_stdio(false)` 是一个 C++
     * 的标准库函数，它的作用是取消 stdio 流与 iostream
     * 流之间的同步，从而提高程序的输入输出效率。具体来说，取消同步后，iostream流就可以像stdio流一样使用缓冲区，从而避免频繁的系统调用，提高程序的运行效率。不过，需要注意的是，一旦使用了
     * `std::cout.sync_with_stdio(false)`，就不能再使用 stdio
     * 库中的函数进行输入输出，否则会导致未定义行为。 */
    // std::cout.sync_with_stdio(false);

    RUN_ALL_TESTS();
    algorithm_performance_test::algorithm_performance_test();
    vector_test::vector_test();
    string_test::string_test();
    deque_test::deque_test();
    queue_test::queue_test();
    queue_test::priority_test();
    stack_test::stack_test();
    list_test::list_test();
    set_test::set_test();
    set_test::multiset_test();
    map_test::map_test();  
    map_test::multimap_test();
    unordered_map_test::unordered_map_test();
    unordered_map_test::unordered_multimap_test();
    unordered_set_test::unordered_set_test();
    unordered_set_test::unordered_multiset_test();

// 使用 _CrtDumpMemoryLeaks()
// 函数可以在程序退出时检查是否有内存泄漏。这个函数只有在程序以调试模式（Debug）编译时才会有效
#if defined(_MSC_VER) && defined(_DEBUG)
    _CrtDumpMemoryLeaks();
#endif  // check memory leaks
}

