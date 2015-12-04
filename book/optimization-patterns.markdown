^title Optimization Patterns

虽然越来越快的硬件解除了大部分软件在性能上的顾虑，游戏却并非如此。
玩家总是想要更丰富的，更真实的，更激动人心的体验。
屏幕上到处都是争抢玩家注意力——还有金钱——的游戏，将硬件的功能发挥至极致的游戏往往获胜。

优化游戏性能是一门高深的艺术，接触到软件的各个层面。
底层程序员掌握硬件架构的种种特质。同时，算法研究者争先恐后地证明谁的过程是最有效率的。

这里，我描述了几个加速游戏的中间层模式。
[数据局部性](data-locality.html)向你介绍了计算机的存储层次以及如何使用其以获得优势。
[脏标识](dirty-flag.html)帮你避开不必要的计算。
[对象池](object-pool.html)帮你避开不必要的分配。 
[空间分区](spatial-partition.html)加速了虚拟世界和其中居民的空间布局。

## 模式

* [数据局部性](data-locality.html)
* [脏标识](dirty-flag.html)
* [对象池](object-pool.html)
* [空间分区](spatial-partition.html)
