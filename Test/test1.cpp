#include <iostream>

void modifyValue(int& ref) {
    ref = 100;  // 修改绑定对象的值
}

void printValue(const int& ref) {
    std::cout << ref << std::endl;  // 读取绑定对象的值
}

int main() {
    int x = 42;

    modifyValue(x);  // 使用左值引用修改x的值
    printValue(x);   // 使用左值引用读取x的值

    modifyValue(10);  // 错误！无法将右值绑定到左值引用
    printValue(10);   // 正确！可以将右值绑定到常量左值引用

    return 0;
}