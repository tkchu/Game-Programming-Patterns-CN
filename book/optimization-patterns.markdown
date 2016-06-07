^title Optimization Patterns

^title 优化模式

While the rising tide of faster and faster hardware has lifted most software
above worrying about performance, games are one of the few remaining exceptions.
Players always want richer, more realistic and exciting experiences. Screens are
crowded with games vying for a player's attention -- and cash! -- and the game
that pushes the hardware the furthest often wins.

虽然越来越快的硬件解除了大部分软件在性能上的顾虑，对游戏却并非如此。
玩家总是想要更丰富的，更真实的，更激动人心的体验。
屏幕上到处都是争抢玩家注意力——还有金钱——的游戏，能将硬件的功能发挥至极致的游戏往往获胜。

Optimizing for performance is a deep art that touches all aspects of software.
Low-level coders master the myriad idiosyncrasies of hardware architectures.
Meanwhile, algorithms researchers compete to prove mathematically whose
procedure is the most efficient.

优化游戏性能是一门高深的艺术，接触到软件的各个层面。
底层程序员掌握硬件架构的种种特质。同时，算法研究者争先恐后地证明谁的过程是最有效率的。

Here, I touch on a few mid-level patterns that are often used to speed up a
game. [Data Locality](data-locality.html) introduces you to the modern
computer's memory hierarchy and how you can use it to your advantage. The [Dirty
Flag](dirty-flag.html) pattern helps you avoid unnecessary computation while
[Object Pools](object-pool.html) help you avoid unnecessary allocation. [Spatial
Partitioning](spatial-partition.html) speeds up the virtual world and its
inhabitants' arrangement in space.

这里，我描述了几个加速游戏的中间层模式。
[数据局部性](data-locality.html)介绍了计算机的存储层次以及如何使用其以获得优势。
[脏标识](dirty-flag.html)帮你避开不必要的计算。
[对象池](object-pool.html)帮你避开不必要的分配。 
[空间分区](spatial-partition.html)加速了虚拟世界和其中居民的空间布局。

## The Patterns

## 模式

* [Data Locality](data-locality.html)
* [Dirty Flag](dirty-flag.html)
* [Object Pool](object-pool.html)
* [Spatial Partition](spatial-partition.html)


* [数据局部性](data-locality.html)
* [脏标识](dirty-flag.html)
* [对象池](object-pool.html)
* [空间分区](spatial-partition.html)


