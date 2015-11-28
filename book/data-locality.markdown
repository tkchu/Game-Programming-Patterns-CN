^title Data Locality
^section Optimization Patterns

数据局部性
优化模式

## Intent

## 意图

*Accelerate memory access by arranging data to take advantage of CPU caching.*

*安排数据充分使用CPU的缓存来加速内存读取。*

## Motivation

## 动机

We've been lied to. They keep showing us charts where CPU speed goes up and up
every year as if Moore's Law isn't just a historical observation but some kind
of divine right. Without lifting a finger, we software folks watch our programs
magically accelerate just by virtue of new hardware.

我们被欺骗流。他们一直向我们展示CPU速度每年递增的图表，就好象摩尔定律不是一个观察历史的结果，而是某种定理。无需吹灰之力，我们的软件凭借着新硬件就可以奇迹般的加速。

Chips *have* been getting faster (though even that's plateauing now), but the
hardware heads failed to mention something. Sure, we can *process* data faster
than ever, but we can't *get* that data faster.

芯片*确实*爱越来越快（就算现在也在变快），但硬件头头没有提到一些事情。是的，我们可以更快的*处理*数据，但我们不能更快的*获得*数据。

<span name="legend"></span>

<img src="images/data-locality-chart.png" alt="A chart showing processor and RAM speed from 1980 to 2010. Processor speed increases quickly, but RAM speed lags behind." />

<aside name="legend">

Processor and RAM speed relative to their respective speeds in 1980. As you can
see, CPUs have grown in leaps and bounds, but RAM access is lagging far behind.

Data for this is from *Computer Architecture: A Quantitative Approach*
by John L. Hennessy, David A. Patterson, Andrea C. Arpaci-Dusseau by way of Tony
Albrecht's "[Pitfalls of Object-Oriented Programming][poop]&rdquo;.

[poop]: http://seven-degrees-of-freedom.blogspot.com/2009/12/pitfalls-of-object-oriented-programming.html

</aside>

For your super-fast CPU to blow through a ream of calculations, it actually has
to get the data out of main memory and into registers. As you can see, RAM hasn't
been keeping up with increasing CPU speeds. Not even close.

为了让你超高速的CPU可以刮起指令风暴，它需要从内存获取数据加载到寄存器。如你所知，RAM没有紧跟CPU的速度增长，差远了。

With today's hardware, it can take *hundreds* of cycles to fetch a byte of data
from <span name="ram">RAM</span>. If most instructions need data, and it takes
hundreds of cycles to get it, how is it that our CPUs aren't sitting idle 99%
of the time waiting for data?

借助现代的硬件，需要*上百个*周期从RAM获得一比特的数据。如果大部分指令需要的数据都需要上百个周期去获取，那么为什么我们的CPU没有在99%的时间中等待数据而空转？

Actually, they *are* stuck waiting on memory an astonishingly large fraction of
time these days, but it's not as bad as it could be. To explain how, let's take
a trip to the Land of Overly Long Analogies...

事实上，停滞等待内存*确实*消耗很长时间，但是没有那么糟糕。为了解释为什么，让我们看一看这一长串类比……

<aside name="ram">

It's called "random access memory" because, unlike disc drives, you can
theoretically access any piece of it as quick as any other. You don't have
to worry about reading things consecutively like you do a disc.

Or, at least, you *didn't*. As we'll see, RAM isn't so random access anymore.

</aside>

### A data warehouse

### 数据仓库

Imagine you're an accountant in a tiny little office. Your job is to request a
box of papers and then do some <span name="accountant">accountant</span>-y stuff
with them -- add up a bunch of numbers or something. You must do this for
specific labeled boxes according to some arcane logic that only makes sense to
other accountants.

想象一下，你是一个小办公室里的会计。你的任务是拿一盒文件，然后做一些会计工作——把数据加起来什么的。你必须通过一堆只对其他会计师有意义的晦涩难懂的逻辑取出特定标记的盒子而工作。

<aside name="accountant">

I probably shouldn't have used a job I know absolutely nothing about in this
analogy.

</aside>

Thanks to a mixture of hard work, natural aptitude, and stimulants, you can
finish an entire box in, say, a minute. There's a little problem, though. All of
those boxes are stored in a warehouse in a separate building. To get a box, you
have to ask the warehouse guy to bring it to you. He goes and gets a forklift
and drives around the aisles until he finds the box you want.

感谢辛勤的工作，天生的悟性，还有兴奋剂，你可以在一分钟内完成一个盒子。但是这里有一个小小的问题。所有这些盒子都存储在一个分离的仓库中。想要拿到一个盒子，你需要让仓库管理员带给你。他开着叉车在传送带周围移动，直到他能找到你要的盒子。

It takes him, seriously, an entire day to do this. Unlike you, he's not getting
employee of the month any time soon. This means that no matter how fast you are,
you only get one box a day. The rest of the time, you just sit there and
question the life decisions that led to this soul-sucking job.

这会消耗他，认真的说，一整天才能完成。不像你，他下个月就不会被雇佣了。这就意味着无论你有多快，你一天只能拿到一个盒子。剩下的时间，你只能坐在那里，质疑这个折磨灵魂的工作。

One day, a group of industrial designers shows up. Their job is to improve the
efficiency of operations -- things like making assembly lines go faster. After
watching you work for a few days, they notice a few things:

一天，一组工业设计师出现了。他们的人物是提高操作的效率——比如让传送带跑得更快。在看着你工作流几天后，他们发现了几件事情：

 *  Pretty often, when you're done with one box, the next box you request is
    right <span name="next">next</span> to it on the same shelf in the
    warehouse.

 *  很经常的，当你处理一个盒子的时候，下一个你需要的盒子就在仓库同样架子上面。

 *  Using a forklift to carry a single box of papers is pretty dumb.

 *  叉车只取一个盒子很笨。

 *  There's actually a little bit of spare room in the corner of your office.

 *  在你的办公室角落里还是有些空余空间的。

<aside name="next">

The technical term for using something near the thing you just used is *locality
of reference*.

</aside>

They come up with a clever fix. Whenever you request a box from the warehouse
guy, he'll grab an entire pallet of them. He gets the box you want and then some
more boxes that are next to it. He doesn't know if you want those (and, given
his work ethic, clearly doesn't care); he simply takes as many as he can fit on
the pallet.

他们想出来一个巧妙的办法。无论何时你问仓库要一个盒子的时候，他都会将一托盘盒子带个你。他给你想要的盒子，以及它周围的盒子。他不知道你是不是想要这些（而且，根据他的工作条件，他根本不在乎）；他只是尽可能的塞满托盘。

He loads the whole pallet and brings it to you. Disregarding concerns for
workplace safety, he drives the forklift right in and drops the pallet in the
corner of your office.

他装满托盘然后带给你。无视工作场地的安全，他直接将叉车开到你的办公室然后将托盘放在你办公室的角落。

When you need a new box, now, the first thing you do is see if it's already on
the pallet in your office. If it is, great! It only takes you a second to grab
it and get back to crunching numbers. If a pallet holds fifty boxes and you
got lucky and *all* of the boxes you need happen to be on it, you can churn
through fifty times more work than you could before.

当你需要一个新盒子，你需要做的第一件事就是看看它是不是在你办公室角落的托盘里。如果在，很好！你只需要几分钟拿起它然后继续计算数据。如果一个托盘中有乌五十个盒子，而你幸运的需要*所有*盒子，你可以以以前五十倍的速度工作。

But if you need a box that's *not* on the pallet, you're back to square one.
Since you can only fit one pallet in your office, your warehouse friend will
have to take that one back and then bring you an entirely new one.

但是如果你需要的盒子*不在*托盘上，你就要一新托盘。由于你的办公室里只能放一个托盘，你的仓库朋友只能将老的拿走，带给你一个全新的。

### A pallet for your CPU

### 你CPU的托盘

Strangely enough, this is similar to how CPUs in modern computers work. In case
it isn't obvious, you play the role of the CPU. Your desk is the CPU's
registers, and the box of papers is the data you can fit in them. The warehouse
is your machine's RAM, and that annoying warehouse guy is the bus that pulls
data from main memory into registers.

奇怪的是，这就是现代CPU运转的方式。如果还是不够明显，你是CPU的角色。你的桌子是CPU的寄存器，一盒文件就是你桌上可以放下的数据。仓库是机器的RAM，那个烦人的仓库管理员是从主存加载数据到寄存器的总线。

If I were writing this chapter thirty years ago, the analogy would stop there.
But as chips got faster and RAM, well, *didn't*, hardware engineers started
looking for solutions. What they came up with was *CPU caching*.

如果我在三十年前写这一章，这个比喻就到此为止了。但是芯片越来越快，而RAM，好吧，“没有跟上”，硬件工程师开始寻找解决方案。他们想到的是*CPU缓存*。

Modern computers have a <span name="caches">little chunk</span> of memory right
inside the chip. The CPU can pull data from this much faster than it can from
main memory. It's small because it has to fit in the chip and because the faster
type of memory it uses (static RAM or "SRAM") is way more expensive.

现代的电脑在芯片内部有一小块存储器。CPU从那上面取数据比从内存取数据快得多。它很小，因为它需要放在芯片上，它很快，因为它使用的（静态RAM，或者SRAM）内存更贵。

<aside name="caches">

Modern hardware has multiple levels of caching, which is what they mean
when you hear "L1", "L2", "L3", etc. Each level is larger but slower than the
previous. For this chapter, we won't worry about the fact that memory is
actually a [hierarchy](http://en.wikipedia.org/wiki/Memory_hierarchy), but it's
important to know.

</aside>

This little chunk of memory is called a *cache* (in particular, the chunk on the
chip is your *L1 cache*), and in my belabored analogy, its part was played by the
pallet of boxes. Whenever your chip needs a byte of data from RAM, it
automatically grabs a whole chunk of contiguous memory -- usually around 64 to
128 bytes -- and puts it in the cache. This dollop of memory is called a *cache
line*.

这一小片内存被称为*缓存*（特别的，芯片上的被称为*L1级缓存*），在我的比喻中，这一块是由托盘扮演的。无论何时你的芯片需要从RAM取一字节的数据，他自动将一整块内存读入——通常是64到128字节——然后将其放入缓存。这些一次性传输的字节被称为*cache line*。

<img src="images/data-locality-cache-line.png" alt="A cache line showing the one byte requested along with the adjacent bytes that also get loaded into the cache." />

If the <span name="pallet">next byte</span> of data you need happens to be in
that chunk, the CPU reads it straight from the cache, which is *much* faster
than hitting RAM. Successfully finding a piece of data in the cache is called a
*cache hit*. If it can't find it in there and has to go to main memory, that's a
*cache miss*.

如果你需要的下一个字节就在这块上，CPU从缓存中直接读取，比从RAM中读取*快得多*。成功从缓存中找到数据被称为“缓存命中”。如果不能从中获得而得去主存里取，这就是一次*缓存不命中*。

<aside name="pallet">

I glossed over (at least) one detail in the analogy. In your office, there was
only room for one pallet, or one cache line. A real cache contains a number of
cache lines. The details about how those work is out of scope here, but search
for "cache associativity" to feed your brain.

</aside>

When a cache miss occurs, the CPU *stalls* -- it can't process the next
instruction because it needs data. It sits there, bored out of its mind for a
few hundred cycles until the fetch completes. Our mission is to avoid that.
Imagine you're trying to optimize some performance-critical piece of game code
and it looks like this:

当缓存不命中时，CPU*空转*——它不能执行下一条指令，因为它没有数据。它坐在那里，无聊的等待几百个周期直到数据被获取了。我们的任务是避免这一点。想象你在优化一块性能攸关的游戏代码，长得像这样：

^code do-nothing

What's the first change you're going to make to that code? Right. Take out that
pointless, expensive function call. That call is equivalent to the performance
cost of a cache miss. Every time you bounce to main memory, it's like you put a
delay in your code.

你会做的第一个变化是什么？对了。将那个无用的，代价巨大的函数调用拿出来。这个调用等价于一次缓存不命中的代价。每一次你跳到内存，你都会延误你的代码。

### Wait, data is performance?

### 等等，数据是性能？

When I started working on this chapter, I spent some time putting together
little game-like programs that would trigger best case and worst case cache
usage. I wanted benchmarks that would thrash the cache so I could see first-hand
how much bloodshed it causes.

当我开始写这一章的时候，我花费了一些时间收集缓存使用最好和最坏的例子。我想要缓存速度的基准，这样我可以得到浪费的第一手消息。

When I got some stuff working, I was surprised. I knew it was a big deal, but
there's nothing quite like seeing it with your own eyes. <span name="ymmv">I
wrote two programs</span> that did the *exact same* computation. The only
difference was how many cache misses they caused. The slow one was *fifty times*
slower than the other.

但我得到了一些工作的例子，我惊到了。我知道这是一个大问题，但眼见为实。我写过两个程序完成*完全相同*的计算。唯一的区别是它们会造成缓存不命中的数量。慢的那个比另一个慢*五十*倍。

<aside name="ymmv">

There are a lot of caveats here. In particular, different computers have different
cache setups, so my machine may be different from yours, and dedicated game
consoles are very different from PCs, which are quite different from mobile
devices.

Your mileage will vary.

</aside>

This was a real eye-opener to me. I'm used to thinking of performance being an
aspect of *code*, not *data*. A byte isn't slow or fast, it's just some static
thing sitting there. But because of caching, *the way you organize data
directly impacts performance*.

这让我大开眼界。我一直从*代码*的角度考虑性能，而不是*数据*。一个字节没有射门快慢，它是静态的。但是因为缓存的存在，*你组织数据的方式直接影响了性能*。

The challenge now is to wrap that up into something that fits into a chapter
here. Optimization for cache usage is a huge topic. I haven't even touched on
*instruction caching*. Remember, code is in memory too and has to be loaded onto
the CPU before it can be executed. Someone more versed on the subject could
write an entire <span name="book">book</span> on it.

现在真正的挑战是将其打包成在一章内可以讲完的东西。优化缓存使用是一个很大的话题。我还没有谈到*指令缓存*呢。记住，代码也在内存上，而且在执行前需要加载到CPU上。有些更熟悉这个主题的人可以就这个问题写一整本书。

<aside name="book">

In fact, someone *did* write a book on it: [*Data-Oriented
Design*](http://www.dataorienteddesign.com/dodmain/), by Richard Fabian.

</aside>

Since you're already reading *this* book right now, though, I have a few basic
techniques that will get you started along the path of thinking about how data
structures impact your performance.

既然你已经在阅读*这本书*了，我有几个基本技术让你开始考虑数据结构是如何影响性能的。

It all boils down to something pretty simple: whenever the chip reads some
memory, it gets a whole cache line. The more you can use stuff in that <span
name="line">cache line, the faster you go</span>. So the goal then is to
*organize your data structures so that the things you're processing are next to
each other in memory*.

这可以归结得很简单：芯片读内存，总是获得一整块cache line，你能从cache line读到越多你要的东西，你跑的就越快。所以目标是*组织数据结构，让你要处理的数据紧紧相邻。*

<aside name="line">

There's a key assumption here, though: one thread. If you are modifying nearby
data on multiple threads, it's faster to have it on *different* cache lines. If
two threads try to tweak data on the same cache line, both cores have to do some
costly synchronization of their caches.

</aside>

In other words, if your code is crunching on `Thing`, then `Another`, then
`Also`, you want them laid out in memory like this:

换言之，如果你的正在处理`Thing`，然后`Another`然后`Also`,，你需要它们这样呆在内存里：

<img src="images/data-locality-things.png" alt="Thing, Another, and Also laid out directly next to each other in order in memory." />

Note, these aren't *pointers* to `Thing`, `Another`, and `Also`. This is the
actual data for them, in place, lined up one after the other. As soon as the CPU
reads in `Thing`, it will start to get `Another` and `Also` too (depending on
how big they are and how big a cache line is). When you start working on them
next, they'll already be cached. Your chip is happy, and you're happy.

注意，这不是`Thing`，`Another`，和`Also`的*指针*。这就是真实的数据，一个接着一个。CPU一读到`Thing`，他也会读取`Another`和`Also`（取决于数据有多大和cache line 有多大）。当你开始下一个的时候，他们已经在缓存上了。你的芯片很高兴，你也很高兴。

## The Pattern

## 模式

Modern CPUs have **caches to speed up memory access**. These can access memory
**adjacent to recently accessed memory much quicker**. Take advantage of that to
improve performance by **increasing data locality** -- keeping data in
**contiguous memory in the order that you process it**.

现代的CPU有*缓存来加速内存读取*。它可以*更快的读取最近访问过的内存毗邻的内存*。通过*提高内存局部性*来提高性能——保证数据以*你处理它们的连续内存上*。

## When to Use It

## 何时使用

Like most optimizations, the first guideline for using the Data Locality pattern is *when you have a
performance problem.* Don't waste time applying this to some infrequently
executed corner of your codebase. Optimizing code that doesn't need it just
makes your life harder since the result is almost always more complex and less
flexible.

就像很多其他的优化方案一样，使用数据局部性的第一条准则是*在你遇到性能问题的时候使用。*不要将其应用在你代码库不经常使用的角落上。优化代码不会让你过得更轻松，因为其结果往往更加复杂，更加缺乏灵活性。

With this pattern specifically, you'll also want to be sure your performance
problems *are caused by cache misses*. If your code is slow for other reasons,
this won't help.

就本模式而言，你还得确认你的性能问题*确实由缓存不命中*引发。如果你的代码是因为其他原因而缓慢，这个模式不会有帮助。

The cheap way to profile is to manually add a bit of instrumentation that checks
how much time has elapsed between two points in the code, hopefully using a
precise timer. To catch poor cache usage, you'll want something a little more
sophisticated. You really want to see how many cache misses are occurring and
where.

简单的检查方案是手动添加一些指令，检查代码中任意两点间消耗的时间，希望能使用精确的计时器。为了找到糟糕的缓存使用，你需要使用更加复杂的东西。你想要知道缓存不命中有多少发生，又是在哪里发生的。

Fortunately, there are <span name="cachegrind">profilers</span> out there that
report this. It's worth spending the time to get one of these working and make
sure you understand the (surprisingly complex) numbers it throws at you before
you do major surgery on your data structures.

幸运的是，这里有工具报告这些。在你在数据结构上做大手术前，花一些时间了解他们是如何工作的，理解它们抛出的一大堆数据（令人惊讶的复杂）是很有意义的。

<aside name="cachegrind">

Unfortunately, most of those tools aren't cheap. If you're on a console dev
team, you probably already have licenses for them.

If not, an excellent free option is
[Cachegrind](http://valgrind.org/docs/manual/cg-manual.html). It runs your
program on top of a simulated CPU and cache hierarchy and then reports all of
the cache interactions.

</aside>

That being said, cache misses *will* affect the performance of your game. While
you shouldn't spend a ton of time pre-emptively optimizing for cache usage, do
think about how cache-friendly your data structures are throughout the design
process.

话虽这么说，缓存不命中*仍会*影响你游戏的性能。虽然你不应该花费大量时间有限优化缓存的使用，但是在设计过程中仍要思考你的数据结构是不是对缓存友好。

## Keep in Mind

## 记住。

One of the hallmarks of software architecture is *abstraction*. A large chunk of
this book is about patterns to decouple pieces of code from each other so that
they can be changed more easily. In object-oriented languages, this almost
always means interfaces.

软件体系结构的特点之一是*抽象*。这本书很多的章节都在谈论如何解耦代码块这样它们可以更容易的进行改变。在面向对象的语言中，这几乎总是指代接口。

In C++, using interfaces implies accessing objects through <span
name="virtual">pointers or references</span>. But going through a pointer means
hopping across memory, which leads to the cache misses this pattern works to
avoid.

在C++中，使用接口意味着通过指针或者引用访问对象。但是通过指针就意味在内存中跳跃，这就带来了这章想要避免的缓存不命中。

<aside name="virtual">

The other half of interfaces is *virtual method calls*. Those require the CPU to
look up an object's vtable and then find the pointer to the actual method to
call there. So, again, you're chasing pointers, which can cause cache misses.

</aside>

In order to please this pattern, you will have to sacrifice some of your
precious abstractions. The more you design your program around data locality,
the more you will have to give up inheritance, interfaces, and the benefits
those tools can provide. There's no silver bullet here, only challenging
trade-offs. That's what makes it fun!

为了讨好这个模式，你需要牺牲一些你宝贵的抽象。你越围绕数据局部性设计程序，你就越放弃继承接口和这些东西带来的好处。没有银弹，只有挑战的交换。这就是乐趣所在！

## Sample Code

## 示例代码

If you really go down the rathole of optimizing for data locality, you'll
discover countless ways to slice and dice your data structures into pieces your
CPU can most easily digest. To get you started, I'll show an example for each of
a few of the most common ways to organize your data. We'll cover them in the
context of some specific part of a game engine, but (as with other patterns),
keep in mind that the general technique can be applied anywhere it fits.

如果你真的要一探数据局部性优化的鼠洞，那么你会发现无数的方法去分割数据结构，将其切为CPU更好处理的小块。为了热热身，我会先从一些最通用的组织数据的方法开始。我们会在游戏引擎的特定部分介绍它们，但是（就像其他章节一样）记住这些通用方法也能在其他部分使用。

### Contiguous arrays

### 连续数组

Let's start with a <a href="game-loop.html" class="pattern">game loop</a> that
processes a bunch of game entities. Those entities are decomposed into different
domains -- AI, physics, and rendering -- using the <a href="component.html"
class="pattern">Component</a> pattern. Here's the `GameEntity` class:

让我们从处理一系列游戏实体的游戏循环开始。这些实体被分解到不同的领域——AI，物理，渲染——使用了组件模式。这里是`GmaeEntity`类。

^code game-entity

Each component has a relatively small amount of state, maybe little more than a
few vectors or a matrix, and then a method to <span name="update">update</span>
it. The details aren't important here, but imagine something roughly along the
lines of:

每一个组件都有相对较少的状态，也许只有几个向量或一个矩阵，然后会有一个方法去更新它。这里的细节无关紧要，但是想象一下，事情大概是这样发展的：

<aside name="update">

As the name implies, these are examples of the <a href="update-method.html"
class="pattern">Update Method</a> pattern. Even `render()` is this pattern, just
by another name.

</aside>

^code components

The game maintains a big array of pointers to all of the entities in the world.
Each spin of the game loop, we need to run the following:

游戏管理游戏世界中一大堆实体的指针数组。每一个游戏循环，我们都要做如下事情：

 1. Update the AI components for all of the entities.

 1. 为每个实体更新他们的AI组件。

 2. Update the physics components for them.

 2. 为每个实体更新他们的物理组件。

 3. Render them using their render components.

 3. 为每个实体更新他们的渲染组件。

Lots of game engines implement that like so:

很多游戏引擎以这种方式实现：

^code game-loop

Before you ever heard of a CPU cache, this looked totally innocuous. But by now,
you've got an inkling that something isn't right here. This code isn't just
thrashing the cache, it's taking it around back and beating it to a pulp. Watch
what it's doing:

在你听说CPU缓存之前，这些看上去完全无害。但是现在，你得看到这里有隐藏着的不对之处。这个带不只是在颠簸缓存，它是四处乱晃然后猛烈的敲击着。看看它做了什么：

 1. The array of game entities is storing *pointers* to them, so for each element in the
    array, we have to traverse that pointer. That's a cache miss.

 1. 游戏实体的数组存储的是他们的*指针*，所有每一个数组中的游戏实体，我们得使用指针。缓存不命中。

 2. Then the game entity has a pointer to the component. Another cache miss.

 2. 然后游戏实体有组件的指针，又一次缓存不命中。

 3. Then we update the component.

 3. 然后我们更新组件。

 4. Now we go back to step one for *every component of every entity in the
    game*.

 4. 再然后我们退回第一布，为*每一个游戏中的实体*做这件事。

The scary part is that we have no idea how these objects are laid out in memory.
We're completely at the mercy of the memory manager. As entities get allocated
and freed over time, the heap is likely to become increasingly randomly
organized.

令人害怕的是，我们组织到这些对象是如何在内存中布局的。我们完全由内存管理器摆布。随着实体的分配和释放，堆会组织更加乱。

<span name="lines"></span>

<img src="images/data-locality-pointer-chasing.png" alt="A tangled mess of objects strewn randomly through memory with pointers wiring them all together." />

<aside name="lines">

Every frame, the game loop has to follow all of those arrows to get to the data
it cares about.

</aside>

If our goal was to take a whirlwind tour around the game's address space like
some "256MB of RAM in Four Nights!" cheap vacation package, this would be a
fantastic deal. But our goal is to run the game quickly, and <span
name="chase">traipsing</span> all over main memory is *not* the way to do that.
Remember that `sleepFor500Cycles()` function? Well this code is effectively
calling that *all the time*.

如果我们的目标是在游戏地址空间中像“四晚尽享256MB”的廉价假期包一样刮起旋风，这也许是一个很好的决定。但是我们的目标是让游戏跑得尽可能快，而在主存各种遍历*不是*一个好办法。记得`sleepFor500Cycles()`函数吗？那个函数*总是*可以认为是更有效率的。

<aside name="chase">

The term for wasting a bunch of time traversing pointers is "pointer chasing",
which it turns out is nowhere near as fun as it sounds.

</aside>

Let's do something better. Our first observation is that the only reason we
follow a pointer to get to the game entity is so we can immediately follow
*another* pointer to get to a component. `GameEntity` itself has no interesting
state and no useful methods. The *components* are what the game loop cares
about.

让我们做的更好。我们的第一个发现是我们跟着指针去寻找一个游戏实体的唯一原因是我们可以立刻跟着*另一个*指针去获得一个组件。`GameEntity`本身没有有意义的状态和有用的方法。*组件*才是游戏循环需要的。

Instead of a giant constellation of game entities and components scattered
across the inky darkness of address space, we're going to get back down to
Earth. We'll have a big array for each type of component: a flat array of AI
components, another for physics, and another for rendering.

众多实体和组件不能像星星散落在黑暗天空布置，我们得脚踏实地。我们将每种组件存入一个巨大的数组：一个数组给AI组件，一个给物理，另一个给渲染。

Like this:

就像这样：

<span name="long-name"></span>

^code component-arrays

<aside name="long-name">

My least favorite part about using components is how long the word "component"
is.

使用自己过程中我最不喜欢的就是组件拼起来很长。

</aside>

Let me stress that these are arrays of *components* and not *pointers to
components*. The data is all there, one byte after the other. The game loop can
then walk these directly:

让我强调一点，这些都是*组件*的数组，而不是*指向组件的指针*数据都在那里，一个接着一个。游戏循环现在可以直接遍历它们了。

<span name="arrow"></span>

^code game-loop-arrays

<aside name="arrow">

One hint that we're doing better here is how few `->` operators there are in the
new code. If you want to improve data locality, look for indirection operators
you can get rid of.

</aside>

We've ditched all of that pointer chasing. Instead of skipping around in memory,
we're doing a straight crawl through three contiguous arrays.

我们消除了所有的指针跳转。不是在内存中跳来跳去，我们直接在三个数组中做直线的遍历。

<img src="images/data-locality-component-arrays.png" alt="An array for each of three different kinds of components. Each array neatly packs its components together." />

This pumps a solid stream of bytes right into the hungry maw of the CPU. In my
testing, this change made the update loop *fifty times* faster than the previous
version.

这将一股字节流直接泵到了CPU饥饿的肚子里。在我的测试中，这个改写后的更新循环是之前性能的*50*倍。

Interestingly, we haven't lost much encapsulation here. Sure, the game loop is
updating the components directly instead of going through the game entities, but
it was doing that before to ensure they were processed in the right order. Even
so, each component itself is still nicely encapsulated. It owns its own data and
methods. We simply changed the way it's used.

有趣的是，我们并没有在这里损失很多封装。是的游戏玄幻直接更新的游戏组件而没有通过游戏实体，但它在此之前确保了它们以正确的方式运行。即使如此，每个组件内部还是很好的封装了。这取决于它们自己的数据和方法。我们只是改变了它使用的方法。

This doesn't mean we need to get rid of `GameEntity` either. We can leave it as it
is with pointers to its components. They'll just point into those
arrays. This is still useful for other parts of the game where you want to pass
around a conceptual "game entity" and everything that goes with it. The
important part is that the performance-critical game loop sidesteps that and
goes straight to the data.

这也不意味着我们摆脱了`GameEntity`。它拥有它组件指针这一状态仍然得以保持。它的组件指针现在只是指到了这个数组之中。对游戏的其他部分，如果你还是想传递一个“游戏实体”，一切照旧。重要的是性能攸关的游戏循环部分回避了直接获取数据这一点。

### Packed data

### 打包数据

Say we're doing a particle system. Following the advice of the previous section,
we've got all of our particles in a nice big contiguous array. Let's wrap it in
a little <span name="pool">manager class</span> too:

假设我们在做一个粒子系统。根据上一节的建议，我们将所有的粒子放在一个巨大的连续数组中。让我们用一个管理类包住它。

<aside name="pool">

The `ParticleSystem` class is an example of an <a href="object-pool.html"
class="pattern">Object Pool</a> custom built for a single type of object.

</aside>

^code particle-system

A rudimentary update method for the system just looks like this:

系统中的基本更新方法看起来是这样的：

^code update-particle-system

But it turns out that we don't actually need to process *all* of the particles
all the time. The particle system has a fixed-size pool of objects, but they
aren't usually all actively twinkling across the screen. The easy answer is
something like this:

但结果是我们不需要同时更新*所有的*粒子。粒子系统具有固定大小的对象池，但是他们通常不是同时活跃在屏幕上。最简单的解决方案是这样的：

^code particles-is-active

We give `Particle` a flag to track whether its in use or not. In the update
loop, we <span name="branch">check</span> that for each particle. That loads the
flag into the cache along with all of that particle's other data. If the
particle *isn't* active, then we skip over it to the next one. The rest
of the particle's data that we loaded into the cache is a waste.

我们给`Particle`一个标志位来追踪其是否在使用状态。在更新循环时，我们检查每个粒子的这一位。这会将粒子其他部分的时间也加载到缓存中。如果粒子*没有*在使用，那么我们跳过它去检查下一个。我们加载到内存中粒子的其他数据都是浪费。

The fewer active particles there are, the more we're skipping across memory. The
more we do that, the more cache misses there are between actually doing useful
work updating active particles. If the array is large and has *lots* of inactive
particles in it, we're back to thrashing the cache again.

活跃的粒子越少，我们要在内存中跳过的部分就越多。我们越这样做，在两次实际有用的更新活跃粒子之间的缓存不命中就越多。如果数组很大又有*很多*不活跃的粒子，我们又在颠簸缓存了。

Having objects in a contiguous array doesn't solve much if the objects we're
actually processing aren't contiguous in it. If it's littered with inactive
objects we have to dance around, we're right back to the original problem.

如果对象不是连续处理的，使用一个连续的数组实际上不能解决太多问题。如果有太多不活跃的对象需要跳过，我们又回到了原来的问题。

<aside name="branch">

Savvy low-level coders can see another problem here. Doing an `if` check for
every particle can cause a *branch misprediction* and a *pipeline stall*. In
modern CPUs, a single "instruction" actually takes several clock cycles. To keep
the CPU busy, instructions are *pipelined* so that the subsequent instructions
start processing before the first one finishes.

To do that, the CPU has to guess which instructions it will be executing next.
In straight line code, that's easy, but with control flow, it's harder. While
it's executing the instructions for that `if`, does it guess that the particle
is active and start executing the code for the `update()` call, or does it guess
that it isn't?

To answer that, the chip does *branch prediction* -- it sees which branches your
code previously took and guesses that it will do that again. But when the loop
is constantly toggling between particles that are and aren't active, that
prediction fails.

When it does, the CPU has to ditch the instructions it had started speculatively
processing (a *pipeline flush*) and start over. The performance impact of this
varies widely by machine, but this is why you sometimes see developers avoid
flow control in hot code.

</aside>

Given the title of this section, you can probably guess the answer. Instead of
*checking* the active flag, we'll *sort* by it. We'll keep all of the active
particles in the front of the list. If we know all of those particles are
active, we don't have to check the flag at all.

监狱本节的标题，你大概可以猜出答案是什么了。我们不*监测*活跃与否的标签，我们*排序*它。我们将所有活跃的粒子放在列表的前头。如果我们知道了这些粒子都是活跃的，我们就不必再检查这些标识位了。

We can also easily keep track of how many active particles there are. With this,
our update loop turns into this thing of beauty:

我们还可以很容易的追踪有多少活跃的粒子。通过这个，我们的更新循环变成了这种美丽的东西：

^code update-particles

Now we aren't skipping over *any* data. Every byte that gets sucked into the
cache is a piece of an active particle that we actually need to process.

现在我们没有跳过*任何*数据。加载入缓存的每一字节都是我们需要处理的粒子的一部分。

Of course, I'm not saying you should quicksort the entire collection of
particles every frame. That would more than eliminate the gains here. What we
want to do is *keep* the array sorted.

当然，我并没有说你要每帧都要对整个数组做快排。这将抵消这里的收益。我们想要的是*保持*数组排好序。

Assuming the array is already sorted -- and it is at first when all particles
are inactive -- the only time it can become *un*sorted is when a particle has
been activated or deactivated. We can handle those two cases pretty easily. When
a particle gets activated, we move it up to the end of the active particles by
swapping it with the first *in*active one:

假设数组已经排好序了——开始时当所有的粒子都不活跃——它变成未排序的时候即是粒子被激活或者被关闭时。我们可以很轻易的处理着两种情况。当一个粒子激活时，我们将它与第一个*不*活跃粒子交互位置，这样将其移动到激活粒子序列的尾端。

^code activate-particle

To deactivate a particle, we just do the opposite:

为了关闭粒子，我们只需做相反的事情：

^code deactivate-particle

Lots of programmers (myself included) have developed allergies to moving things
around in memory. Schlepping a bunch of bytes around *feels* heavyweight
compared to assigning a pointer. But when you add in the cost of
*traversing* that pointer, it turns out that our intuition is sometimes wrong.
In <span name="profile">some cases</span>, it's cheaper to push things around in
memory if it helps you keep the cache full.

很多程序员（包括我在内）已经对于在内存中移动数据过敏了。将一堆数据移来移去*感觉*比发送一个指针要消耗大得多。但是如果你加上了*解析*指针的代价，这证明有时候我们的估算是错误的。在有些情况下，如果能够保持缓存命中，将一些数据移动消耗更小。

<aside name="profile">

This is your friendly reminder to *profile* when making these kinds of
decisions.

</aside>

There's a neat consequence of keeping the particles *sorted* by their active
state -- we don't need to store an active flag in each particle at all. It can be
inferred by its position in the array and the `numActive_` counter. This makes
our particle objects smaller, which means we can pack more in our cache lines,
and that makes them even faster.

将粒子根据激活状态保持为*排序*——我们就不需要给内阁粒子都添加激活标志位了。这可以有它在数组中的位置和`numActive_`计数器推断而得。这让我们的粒子对象更加小，这就意味着我们在cache lines中能够打包更多的数据，这让他们跑得更快。

It's not all rosy, though. As you can see from the API, we've lost a bit of
object orientation here. The `Particle` class no longer controls its own active
state. You can't call some `activate()` method on it since it doesn't know
its index. Instead, any code that wants to activate particles needs access to
the particle *system*.

但是这并不都是鲜花。你可以从API看出，我们失去了一定的面向对象。`Particle`类不再控制其激活状态了。你不能在他上面调用`activate()`因为他不知道自己的索引。相反，任何想要激活粒子的代码都需要接触到粒子*系统*。

In this case, I'm OK with `ParticleSystem` and `Particle` being tightly tied
like this. I think of them as a single *concept* spread across two physical
*classes*. It just means accepting the idea that particles are *only* meaningful
in the context of some particle system. Also, in this case it's likely to be the
particle system that will be spawning and killing particles anyway.

在这个例子中，我对于`ParticleSystem`和`Particle`像这样牢牢的绑在一起没有问题。这只是我将太慢思维两个跨物理*类*的概念。这只是意味着要接受粒子只在*特定*的粒子系统中有意义。同样，在这种情况下，很可能是粒子系统在复制和销毁粒子。

### Hot/cold splitting

### 热/冷 分割

OK, this is the last example of a simple technique for making your cache
happier. Say we've got an AI component for some game entity. It has some state
in it -- the animation it's currently playing, a goal position it's heading
towards, energy level, etc. -- stuff it checks and tweaks every single frame.
Something like:

这里是最后一种雀跃你缓存的技术例子来。假设我们为某些游戏实体有一个AI控件。他其中包括一些状态——现在正在播放的动画，正在前往的方向，能量等级，等等——这些东西每帧都会发生变化。就像这样：

^code ai-component

But it also has some state for rarer eventualities. It stores some data
describing what loot it drops when it has an unfortunate encounter with the
noisy end of a shotgun. That drop data is only used once in the entity's
lifetime, right at its bitter end:

但它也有一些罕见事件的状态。他存储了一些数据描述它在遭遇到一阵猎枪的嘈杂后会掉落什么战利品。掉落数据在实体的整个生命周期智慧使用一次，就在它结束的前一霎那：

^code loot-drop

Assuming we followed the earlier patterns, when we update these AI components,
we walk through a nice packed, contiguous array of data. But that data includes
all of the loot drop information. That makes each component bigger, which
reduces the number of components we can fit in a cache line. We get more cache misses
because the total memory we walk over is larger. The loot data gets pulled into
the cache for every component in every frame, even though we aren't even touching
it.

假设我们遵循了前面的章节，当我们更新AI组件时，我们穿过了一序列打包好的连续数组。但是那个数据包含 了所有掉落物的信息。这让每个组件都变得更大了，这就减少了我们能够加载到cache line中的组件个数。每一帧每一个组件都会将战利品数据加载到内存中去，即使我们根本不会去使用它。

The solution for this is called "hot/cold splitting". The idea is to break our
data structure into two separate pieces. The first holds the "hot" data, the
state we need to touch every frame. The other piece is the "cold" data,
everything else that gets used less frequently.

这里的解决方案被称为“热/冷分割”。这个电子来源于将我们的数据结构划分为两个分离的部分。第一部分保存“热”数据，那些我们每帧都要调用的数据。剩下的片段被称为“冷”数据，任何在那里的数据都是使用的更少。

The hot piece is the *main* AI component. It's the one we need to use the most,
so we don't want to chase a pointer to find it. The cold component can be off to
the side, but we still need to get to it, so we give the hot component a pointer
to it, like so:

热的部分是AI组件的*主体*。那是我们需要使用最多的部分，所以我们不希望解析一个指针去找到它。冷冷组件可以被归到一边去，但是我们还是需要获得之，因此我们在热组件中包含一个指向它的指针，就像这样：

^code hot-cold

Now when we're walking the AI components every frame, the only data that gets
loaded into the cache is stuff we are actually processing (with the <span
name="parallel">exception</span> of that one little pointer to the cold data).

现在我们每一帧都要遍历AI组件，加载到内存的数据只包含我们需要的数据（以及那个指向冷数据的指针）。

<aside name="parallel">

We could conceivably ditch the pointer too by having parallel arrays for the hot
and cold components. Then we can find the cold AI data for a component since
both pieces will be at the same index in their respective arrays.

</aside>

You can see how this starts to get fuzzy, though. In my example here, it's
pretty obvious which data should be hot and cold, but in a real game it's
rarely so clear-cut.
What if you have fields that are used when an entity is in a certain mode but
not in others? What if entities use a certain chunk of data only when they're in
certain parts of the level?

你可以看到事情这里是怎么变得模棱两可的。在我的例子中，那些是冷数据，哪些是热数据是很明确的，但是在真实的游戏中一般很少会这么明显的分割出来。如果你有一部分数据，实体在一种状态下会经常使用，另一种状态则不会，那该怎么办？如果实体只在特定等级的时候使用特定的一块数据，又该怎么办？

Doing this kind of optimization is somewhere between a black art and a rathole.
It's easy to get sucked in and spend endless time pushing data around to see
what speed difference it makes. It will take practice to get a handle on where
to spend your effort.

做这种优化有时就是在黑色艺术和老鼠洞之间走钢丝。很容易陷入其中然后消耗无尽的时间把数据挪来挪去看看性能如何。需要实践来掌握在哪里付出努力。

## Design Decisions

## 设计决策

This pattern is really about a mindset -- it's getting you to think about your
data's arrangement in memory as a key part of your game's performance story. The
actual concrete design space is wide open. You can let <span name="dod">data
locality</span> affect your whole architecture, or maybe it's just a localized
pattern you apply to a few core data structures.

这章真的有关于思维定势——思考你数据的组织模式是你游戏性能的关键部分。实际上具体的设计空间是开放的。你可以让数据局部性影响你的整个架构，或者只在局部几个核心的数据结构上使用这个模式。

The biggest questions you'll need to answer are when and where you apply this
pattern, but here are a couple of others that may come up.

你需要回答的最大问题是何时何地需要你使用这个模式，但是这里还有其他几个问题需要回答。

<aside name="dod">

Noel Llopis' [famous article](http://gamesfromwithin.com/data-oriented-design)
that got a lot more people thinking about designing games around cache usage
calls this "data-oriented design".

</aside>

### How do you handle polymorphism?

### 你如何处理多态？

Up to this point, we've avoided subclassing and virtual methods. We have assumed we
have nice packed arrays of *homogenous* objects. That way, we know they're all
the exact same size. But polymorphism and dynamic dispatch are useful tools
too. How do we reconcile this?

到了现在，我们回避了子类和虚机制。我们假设我们有打包好的同样的对象。这种情况下，我们知道他们都有同样的大小。但是多态和动态调用也是有用的工具。我们如何调和呢？

 *  **Don't:**

 *  *别这么干*

    The <span name="type">simplest</span> answer is to avoid subclassing,
    or at least avoid it in places where you're optimizing for cache usage.
    Software engineer culture is drifting away from heavy use of inheritance
    anyway.

    最简单的解决方案是避免子类，至少在你做内存优化的部分避免使用。无论如何，软件工程师文化已经和大量使用继承渐行渐远了。

    <aside name="type">

    One way to keep much of the flexibility of polymorphism without using
    subclassing is through the <a href="type-object.html" class="pattern">Type
    Object</a> pattern.

    </aside>

     *  *It's safe and easy.* You know exactly what class you're dealing with,
        and all objects are obviously the same size.

     *  *简洁安全。*你知道你在处理什么类，所有的对象都明显是同样的体积。

     *  *It's faster.* Dynamic dispatch means looking up the method in the
        vtable and then traversing that pointer to get to the actual code. While
        the cost of this varies widely across different hardware, there is <span
        name="cpp">*some*</span> cost to dynamic dispatch.

     *  *更快*动态调用意味着在跳转表中寻找方法，然后跟着指针寻找特定的代码。这种消耗在不同的硬件区别很大，但是动态调用总会消耗*一些*代价。

    <aside name="cpp">

    As usual, the only absolute is that there are no absolutes. In most cases, a
    C++ compiler will require an indirection for a virtual method call. But in
    *some* cases, the compiler may be able to do *devirtualization* and
    statically call the right method if it knows what concrete type the receiver
    is. Devirtualization is more common in just-in-time compilers for languages
    like Java and JavaScript.

    </aside>

     *  *It's inflexible.* Of course, the reason we use dynamic dispatch is
        because it gives us a powerful way to vary behavior between objects. If
        you want different entities in your game to have their own rendering
        styles or their own special moves and attacks, virtual methods are a
        proven way to model that. Having to instead stuff all of that code into
        a single non-virtual method that does something like a big `switch` gets
        messy quickly.

     *  *不灵活*当然，我们使用动态调用的原因就是它给了我们强大的方式在不同对象间展示不同的行为。如果你的游戏想要不同的实体使用相同的渲染方式或者独特的移动和攻击，虚方法被证明是处理它的好方法。把他换成一个包含巨大的`switch`的非虚方法会超级慢。

 *  **Use separate arrays for each type:**

 *  *为每种类型使用分离的数组：*

    We use polymorphism so that we can invoke behavior on an object whose type
    we don't know. In other words, we have a mixed bag of stuff, and we want each
    object in there to do its own thing when we tell it to go.

    我们使用多态，这样即使我们不知道对象的类型，我们也能引入行为。换言之，我们有了一堆混合的东西，当我们通知时，我们想要每个对象去做自己的事情。

    But that raises the question of why mix the bag to begin with? Instead,
    why not maintain separate, homogenous collections for each type?

    但是这提出来为什么开始的时候要把它们混在一起呢？取而代之，为什么不为每种类型保持一个单独的集合呢？

     *  *It keeps objects tightly packed.* Since each array only contains
        objects of one class, there's no padding or other weirdness.

     *  *对象被紧密的排列着。*每一个数组只包含同类的对象，这里没有其他填充或者古怪之处。

     *  *You can statically dispatch.* Once you've got objects partitioned by
        type, you don't need polymorphism at all any more. You can use
        regular, non-virtual method calls.

     *  *静态调度*一旦你获得了对象的类型，你不必在所有时候使用多态。你可以使用通常的，非虚方法调用。

     *  *You have to keep track of a bunch of collections.* If you have a lot of
        different object types, the overhead and complexity of maintaining
        separate arrays for each can be a chore.

     *  *你的追踪每一个集合。*如果你有很多不同的类型，这种过度的，复杂的管理每一种类型分别的类型可是件苦差事。

     *  *You have to be aware of every type*. Since you have to maintain
        separate collections for each type, you can't be decoupled from the
        *set* of classes. Part of the magic of polymorphism is that it's
        *open-ended* -- code that works with an interface can be completely
        decoupled from the potentially large set of types that implement that
        interface.

     *  *你得明了每种类型。*由于你的为每种类型管理分离的集合，你无法解耦*类型集合*。多态的魔力之一在于它是*开放的*——与一个接口交互的代码可以与实现此接口的众多类型解耦。

 *  **Use a collection of pointers:**

 *  *使用指针的集合：*

    If you weren't worried about caching, this is the natural solution. Just
    have an array of pointers to some base class or interface type. You get all the
    polymorphism you could want, and objects can be whatever size they want.

    如果你不太考虑缓存，这是自然的解法。只要一个指针数组保存基本的类或者接口类型。你获得了所有你想要的多态，以及他们想多大多大的对象。

     *  *It's flexible.* The code that consumes the collection can work with
        objects of any type as long as it supports the interface you care about.
        It's completely open-ended.

     *  *灵活*。这样构建集合的代码可以与任何支持接口的类工作。完全的开放式。

     *  *It's less cache-friendly.* Of course, the whole reason we're discussing
        other options here is because this means cache-unfriendly pointer
        indirection. But, remember, if this code isn't performance-critical,
        that's probably OK.

     *  *对缓存不友好*。当然，我们讨论在这里讨论其他选项的原因就是指针的缓存不友好跳转。但是，记住，如果代码不是性能攸关的，这很有可能是行得通的。

### How are game entities defined?

### 游戏实体是如何定义的？

If you use this pattern in tandem with the <a href="component.html"
class="pattern">Component</a> pattern, you'll have nice contiguous arrays for
all of the components that make up your game entities. The game loop will be
iterating over those directly, so the object for the game entity itself is less
important, but it's still useful in other parts of the codebase where you want
to work with a single conceptual "entity".

如果与组件模式串联使用这个模式，你会获得组成你游戏实体的组件的多个数组。游戏循环会在那里直接遍历他们，所以有些实体本身就不是那么重要了，但是在其他你想要以一个统一“实体”交互的代码库部分，还是很有用的。

The question then is how should it be represented? How does it keep track of its
components?

这里的问题是它该如何被表示》如何追踪这些组件？

 *  **If game entities are classes with pointers to their components:**

 *  *如果游戏实体是拥有他们组件指针的类：*

    This is what our first example looked like. It's sort of the vanilla OOP
    solution. You've got a class for `GameEntity`, and it has pointers to the
    components it owns. Since they're just pointers, it's agnostic about where
    and how those components are organized in memory.

    这是我们第一个例子的样子。这是完全的OOP解决方案。你获得了`GameEntity`类，以及指向它拥有的组件的指针。由于他们只是指针，那就不知道这些组件是如何在内存中组织的了。

     *  *You can store components in contiguous arrays.* Since the game entity
        doesn't care where its components are, you can organize them in a nice
        packed array to optimize iterating over them.

     *  *你可以将实体存储到连续数组中。*既然游戏实体不在乎他的组件在哪里，你可以将它们都组织到一个个好好打包的数组中来管理他们的遍历。

     *  *Given an entity, you can easily get to its components.* They're just a
        pointer indirection away.

     *  *拿到一个实体，你可以轻易的获得他的组件。*他们就在一次指针跳转后的位置。

     *  *Moving components in memory is hard.* When components get enabled or
        disabled, you may want to move them around in the array to keep the
        active ones up front and contiguous. If you move a component while the
        entity has a raw pointer to it, though, that pointer gets broken if you
        aren't careful. You'll have to make sure to update the entity's pointer
        at the same time.

     *  *在内存中移动组件很难。*当组件启用或者关闭时，你可能想要在数组中移动他们来保证启用的部件在前面连续。如果你在实体有一指针指向其时直接移动它，一不小心指针就会损毁。你得保证同时更新组件的指针。

 *  **If game entities are classes with IDs for their components:**

 *  *如果游戏实体拥有他们组件的ID：*

    The challenge with raw pointers to components is that it makes it harder to
    move them around in memory. You can address that by using something more
    abstract: an ID or index that can be used to *look up* a component.

    直接使用指针的挑战在于在内存中移动它是困难的。你可以使用更加直接的方案解决之：使用ID或者索引来*查找*组件

    The actual semantics of the ID and lookup process are up to you. It could be
    as simple as storing a unique ID in each component and walking the array, or
    more complex like a hash table that maps IDs to their current index in the
    component array.

    ID的实际查找过程是由你决定的，他可能很简单，只需要为每一个实体保存一个独特的ID然后遍历数组，或者更加复杂地使用哈希表就爱那个ID映射到到他们组件的现有位置。

     *  *It's more complex.* Your ID system doesn't have to be rocket science,
        but it's still more work than a basic pointer. You'll have to implement
        and debug it, and there will be memory overhead for bookkeeping.

     *  *更复杂*你的ID系统不必是高科技，但是他韩式需要比指针多做一些事情。你得实现它然后排出漏洞，然后这里消耗内存开销。

     *  *It's slower*. It's hard to beat traversing a raw pointer. There may be
        some searching or hashing involved to get from an entity to one
        of its components.

     *  *更慢*。很难比直接使用指针更快。这里也许有一些搜索或者哈希来帮助一个实体找到它的组件。

     *  *You'll need access to the component "manager".* The basic idea is that
        you have some abstract ID that identifies a component. You can use it to
        get a reference to the actual component object. But to do that, you need
        to hand that ID to something that can actually find the component. That
        will be the class that wraps your raw contiguous array of component
        objects.

     *  *你需要接触组件“管理器”*基本的主意是你拥有一个抽象的ID标识一个组件。你可以使用它来获得对应组件对象的引用。但是为了做到这一点，你需要处理ID找到组件的部分。这也许是一个包裹着你整个连续组件数组的对象。

        With raw pointers, if you have a game entity, you can find its
        components. With this, you <span name="singleton">need</span> the game
        entity *and the component registry too*.

        通过裸指针，如果你有一个游戏实体，你可以直接找到组件，而通过这种方式你需要接触游戏实体和*组件注册器*。

        <aside name="singleton">

        You may be thinking, "I'll just make it a singleton! Problem solved!"
        Well, sort of. You might want to check out <a href="singleton.html">the
        chapter</a> on those first.

        </aside>

 *  **If the game entity is *itself* just an ID:**

 *  *如果游戏实体*本身*就是一个ID：*

    This is a newer style that some game engines use. Once you've moved all of
    your entity's behavior and state out of the main class and into components,
    what's left? It turns out, not much. The only thing an entity does is bind a
    set of components together. It exists just to say *this* AI component and
    *this* physics component and *this* render component define one living
    entity in the world.

    这是一种某些游戏引擎使用的新方式。一旦你将实体的行为和状态移出主类放入组件，还剩什么呢？事实上，没什么了。实体干的唯一事情就是将组件链接在一起。他的存在只是为了说明*这个*AI组件和*这个*物理组件还有*这个*渲染组件定义了一个存在于游戏世界的实体。

    That's important because components interact. The render component needs to
    know where the entity is, which may be a property of the physics component.
    The AI component wants to move the entity, so it needs to apply a force to
    the physics component. Each component needs a way to get the other sibling
    components of the entity it's a part of.

    这一点很重要因为组件要相互交互。渲染组件需要知道实体在哪里，这也许是物理组件的属性。AI组件想要移动实体，因此他需要对物理组件施加力。每一个组件都需要一种方式获得同在一个实体中的其他组件。

    Some smart people realized all you need for that is an ID. Instead of the
    entity knowing its components, the components know their entity. Each
    component knows the ID of the entity that owns it. When the AI component
    needs the physics component for its entity, it simply asks for the physics
    component with the same entity ID that it holds.

    有些聪明人意识到你需要的唯一东西就是ID。不是实体知道组件，而是组件知道实体。每一个组件知道拥有它的实体的ID。当AI组件需要它所属实体的物理组件时，它只需要找到那个拥有同样ID的物理组件。

    Your entity *classes* disappear entirely, replaced by a glorified wrapper
    around a number.

    你的实体*类*整个消失，取而代之的是一个围绕数字的华而不实的包装。

     *  *Entities are tiny.* When you want to pass around a reference to a game
        entity, it's just a single value.

     *  *实体很小。*当你想要传递游戏实体的引用是，只需一个简单的值。

     *  *Entities are empty.* Of course, the downside of moving everything out
        of entities is that you *have* to move everything out of entities. You
        no longer have a place to put non-component-specific state or behavior.
        This style doubles down on the <a href="component.html"
    class="pattern">Component</a> pattern.

     *  *实体是空的。*担任，将所有东西移出实体的代价是你*必须*将所有东西移出来。你不能再拥有组件独有的状态和行为，这种行为双管齐下的解决了组件模式。

     *  *You don't have to manage their lifetime.* Since entities are just dumb
        value types, they don't need to be explicitly allocated and freed. An
        entity implicitly "dies" when all of its components are destroyed.

     *  *你不必在管理他们的生命周期。*由于实体只是内置值的类型，他们不需要被明确的分配和释放。当它所有的组件都被释放时，一个对象就含蓄的“死”了。

     *  *Looking up a component for an entity may be slow.* This is the same
        problem as the previous answer, but in the opposite direction. To find a
        component for some entity, you have to map an ID to an object. That
        process may be costly.

     *  *查找实体的某一组件也许会很慢。*这和前一方案有相同的问题，但是是在相反的方向。为了给某一个实体找组件，你需要给对象分一个ID。这一过程也许消耗很大。

        This time, though, it *is* performance-critical. Components often
        interact with their siblings during update, so you will need to find
        components frequently. One solution is to make the "ID" of an entity
        the index of the component in its array.

        但是，这一次，这*是*性能攸关的。在更新时，组件经常与他的兄弟组件交互，因此你需要经常的查找组件。我们的解法是让实体的“ID”作为他组件在数组中的索引。

        If every entity has the same set of components, then your component
        arrays are completely parallel. The component in slot three of the AI
        component array will be for the same entity that the physics component
        in slot three of *its* array is associated with.

        如果每一个实体都拥有相同集合的组件，那么你的组件数组就是完全同步的。组件数组三号位的AI组件与在*物理组件*数组三号位的组件相关联。

        Keep in mind, though, that this *forces* you to keep those arrays in
        parallel. That's hard if you want to start sorting or packing them by
        different criteria. You may have some entities with disabled physics and
        others that are invisible. There's no way to sort the physics and render
        component arrays optimally for both cases if they have to stay in sync
        with each other.

        但是，记住，这*强迫*你保持这些数组同步。如果你想要用不同的方式排序或者打包他们就会变得很难。你也许需要一些没有物理组件或者隐形的实体。他们得保证与其他同步，这样没有办法独自排序物理和渲染组件数组。

## See Also

## 参见

 *  Much of this chapter revolves around the <a href="component.html"
    class="pattern">Component</a> pattern, and that pattern is definitely one of the
    most common data structures that gets optimized for cache usage. In fact,
    using the Component pattern makes this optimization easier. Since entities
    are updated one "domain" (AI, physics, etc.) at a time, splitting them out
    into components lets you slice a bunch of entities into the right
    pieces to be cache-friendly.

    这一章大部分围绕着组件模式。，这种模式绝对是一个最常见的数据结构被为缓存优化的例子。事实上，使用组件模式让这种优化变容易了。由于实体是一个一个“域”（AI，物理，等等）更新的，将它们划出去变成组件让你更容易将它们保存为缓存友好的合适大小。

    But that doesn't mean you can *only* use this pattern with components! Any
    time you have performance-critical code that touches a lot of data, it's
    important to think about locality.


    但是这不意味着你*只能*为组件使用这种模式！任何时候你有需要接触很多数据的性能攸关的代码，考虑局部性就是很重要的。

 *  Tony Albrecht's <a href="http://research.scee.net/files/presentations/gcapaustralia09/Pitfalls_of_Object_Oriented_Programming_GCAP_09.pdf"
    class="pdf">"Pitfalls of Object-Oriented Programming"</a> is probably the
    most widely-read introduction to designing your game's data structures for
    cache-friendliness. It made a lot more people (including me!) aware of how
    big of a deal this is for performance.

 *  Tony Albrecht的《Pitfalls of Object-Oriented Programming》也许是最被广泛阅读的指南来为内存友好设计你游戏的指南录。它让很多人（包括我！）明白了这点对于性能是多么重要。

 *  Around the same time, Noel Llopis wrote a [very influential blog
    post](http://gamesfromwithin.com/data-oriented-design) on the same topic.

 *  几乎同时，Noel Llopis写了关于同一话题写了一篇非常有影响力的博客。

 *  This pattern almost invariably takes advantage of a contiguous array of
    homogenous objects. Over time, you'll very likely be adding and removing
    objects from that array. The <a href="object-pool.html"
    class="pattern">Object Pool</a> pattern is about exactly that.

 *  这一章几乎完全得益于同类对象的连续存储数组。随着时间的推移，你也许需要向那个数组增加或删除对象。对象池模式正是关于这一点。

 *  The [Artemis](http://gamadu.com/artemis/) game engine is one of the first
    and better-known frameworks that uses simple IDs for game entities.

 *  游戏引擎Artemis是第一个也是最著名的为游戏实体使用简单ID的框架。
