^title Sequencing Patterns

^title 序列模式

Videogames are exciting in large part because they take us somewhere else. For a
few minutes (or, let's be honest with ourselves, much longer), we become
inhabitants of a virtual world. Creating these worlds is one of the supreme
delights of being a game programmer.

电子游戏之所有有趣，很大程度上归功于它们会将我们带到别的地方。
几分钟后（或者，诚实点，可能会更长），我们栖息在一个虚拟的世界。
创造那样一个世界是游戏程序员至上的欢愉。

One aspect that most of these game worlds feature is *time* -- the artificial
world lives and breathes at its own cadence. As world builders, we must invent
time and craft the gears that drive our game's great clock.

大多数游戏世界都有的特性是*时间*——虚构世界以其特定的昼夜生活。
作为世界的架构师，我们必须发明时间，制造推动游戏时间运作的齿轮。

The patterns in this section are tools for doing just that. A [Game
Loop](game-loop.html) is the central axle that the clock spins on. Objects hear
its ticking through [Update Methods](update-method.html). We can hide the
computer's sequential nature behind a facade of snapshots of moments in time
using [Double Buffering](double-buffer.html) so that the world appears to
update simultaneously.

这一部分的模式是建构这些方面的工具。
[游戏循环](game-loop.html)是时钟的中间轴。
对象通过[更新模式](update-method.html)来听时钟的滴答声。
我们可以用[双重缓存](double-buffer.html)存储快照来隐藏计算机的顺序执行，这样看起来世界是同步运行的。

## The Patterns

## 模式

* [Double Buffer](double-buffer.html)
* [Game Loop](game-loop.html)
* [Update Method](update-method.html)


* [双缓冲](double-buffer.html)
* [游戏循环](game-loop.html)
* [更新方法](update-method.html)
