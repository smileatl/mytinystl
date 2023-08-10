# 自己实现一个STL

## 特征

- 重写muduo核心组件，去除boost依赖，完全使用C++标准进行重构；
- 底层使用 Epoll + LT 模式的 I/O 复用模型，并且结合非阻塞 I/O 实现主从 Reactor 模型；
- 实现了Channel 模块、Poller 模块、事件循环模块、HTTP 模块、定时器模块、数据库连接池模块。

## 构建

```shell
git clone https://github.com/smileatl/mytinystl.git
cd mytinystl/Test
mkdir build && cmake ..
make 
```



## 文档

- [05.MyTinySTL](./doc/05.MyTinySTL.md)
- [10.template](./doc/10.template.md)
- [20.实现顺序](./doc/20.实现顺序.md)
- [30.空间配置器allocator](./doc/30.空间配置器allocator.md)
- [40.迭代器iterator](./doc/40.迭代器iterator.md)
- [50.仿函数functional](./doc/50.仿函数functional.md)
- [60.容器container](./doc/60.容器container.md)
- [70.算法algorithm](./doc/70.算法algorithm.md)
- [80.适配器adapter](./doc/80.适配器adapter.md)
- [90.测试](./doc/90.测试.md)

## 示例

