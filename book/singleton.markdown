^title Singleton
^section Design Patterns Revisited

^title 单例
^section Design Patterns Revisited

This chapter is an anomaly. Every other chapter in this book shows
you how to use a design pattern. This chapter shows you how *not* to use
one.

这个章节不同寻常。
这本书其他章节展示如何使用一个设计模式。
这个章节向你展示如何*不*使用一个设计模式。

Despite noble intentions, the <a class="gof-pattern"
href="http://c2.com/cgi/wiki?SingletonPattern">Singleton</a> pattern described
by the Gang of Four usually does more harm than good. They stress that the
pattern should be used sparingly, but that message was often lost in translation
to the <span name="instance">game industry</span>.

不管它高贵的意图，GoF描述的<a class="gof-pattern" href="http://c2.com/cgi/wiki?SingletonPattern">单例模式</a>带来的坏处通常多于好处。
他们强调这个模式应该谨慎使用，但是这个消息经常消失在<span name="instance">游戏业界</span>的传播途中。

Like any pattern, using Singleton where it doesn't belong is about as helpful as
treating a bullet wound with a splint. Since it's so overused, most of this
chapter will be about *avoiding* singletons, but first, let's go over the
pattern itself.

就像其他模式一样，在不合适的地方使用单例模式就好像用夹板处理子弹伤口。
由于它是如此的滥用，这章的大部分都在讲如何*回避*单例模式，
但首先，让我们浏览一遍模式本身。

<aside name="instance">

When much of the industry moved to object-oriented programming from C, one
problem they ran into was "how do I get an instance?" They had some method they
wanted to call but didn't have an instance of the object that provides that
method in hand. Singletons (in other words, making it global) were an easy way
out.

当业界从C迁移到面向对象的语言，他们遇到的一个问题是“如何获取实例？”
他们有他们要调用的方法，但是没有对象的实例听方法。
单例（换言之，全局化）是一条简单的路。

</aside>

## The Singleton Pattern

## 单例模式

*Design Patterns* summarizes Singleton like this:

*设计模式*像这样总结单例模式：

> Ensure a class has one instance, and provide a global point of access to it.

> 保证类只有一个实例，并提供全局接入点来获得它。

We'll split that at "and" and consider each half separately.

我们从“并”分割这个句子，分别考虑每一部分。

### Restricting a class to one instance

### 限制类只有一个实例

There are times when a class cannot perform correctly if there is more than one
instance of it. The common case is when the class interacts with an external
system that maintains its own global state.

有时候，如果有超过一个实例，类不能正确的运行。
通常情况是类与保存全局状态的外部系统互动。

Consider a class that wraps an underlying file system API. Because file
operations can take a while to complete, our class performs operations
asynchronously. This means multiple operations can be running concurrently, so
they must be coordinated with each other. If we start one call to create a file
and another one to delete that same file, our wrapper needs to be aware of both
to make sure they don't interfere with each other.

考虑包裹了潜在文件系统的API。
因为文件操作需要一段时间完成，类异步完成操作。
这就意味着多个操作可以同时运行，所以它们必须相互协调。
如果一个调用创建文件，另一个删除同一文件，包装器需要同时考虑两个来保证它们没有相互妨碍。

To do this, a call into our wrapper needs to have access to every previous
operation. If users could freely create instances of our class, one instance
would have no way of knowing about operations that other instances started.
Enter the singleton. It provides a way for a class to ensure at compile time
that there is only a single instance of the class.

为了做到这点，对我们包装器的调用需要接触之前的每个操作。
如果用户可以自由的创建类的实例，实例无法知道另一实例之前的操作。
进入单例模式。它提供了一种方式构建类，在编译时保证类只有单一实例。

### Providing a global point of access

### 提供一个全局接入点

Several different systems in the game will use our file system wrapper: logging,
content loading, game state saving, etc. If those systems can't create their own
instances of our file system wrapper, how can they get ahold of one?

游戏中的不同系统都会使用文件系统包装类：日志，内容加载，游戏状态保存，等等。
如果这些系统不能创建它们自己的文件系统包装实例，它们如何获取呢？

Singleton provides a solution to this too. In addition to creating the single
instance, it also provides a globally available method to get it. This way,
anyone anywhere can get their paws on our blessed instance. All together, the
classic implementation looks like this:

单例为这个也提供了解决方案。
创建单一实例以外，它也提供了一种全局获得它的方法。
这种范式，无论何处何人都可以得到祝福的实例。
全部加起来，类经典的实现如下：

^code 1

The static `instance_` member holds an instance of the class, and the private
constructor ensures that it is the *only* one. The public static `instance()`
method grants access to the instance from anywhere in the codebase. It is also
responsible for instantiating the singleton instance lazily the first time
someone asks for it.

静态的`instance_`成员保存了一个类的实例，
私有的构造器保证了它是*唯一*的。
公开的静态方法`instance()`让代码的任何地方都能获取实例。
它同样负责在第一个有人请求的时候才惰性实例化单例。

A modern take looks like this:

一个现代的实现看起来是这样的：

^code local-static

C++11 <span name="thread">mandates</span> that the initializer for a local
static variable is only run once, even in the presence of concurrency. So,
assuming you've got a modern C++ compiler, this code is thread-safe where the
first example is not.

C++11<span name="thread">保证了</span>本地静态变量只会运行一次，
哪怕是同时发生的情况下。
因此，假设你有一个现代C++编译器，这个代码是线程安全的。而第一个例子不是。

<aside name="thread">

Of course, the thread-safety of your singleton class itself is an entirely
different question! This just ensures that its *initialization* is.

淡然，你单例类本身的线程安全是完全不同的问题！这只保证了它的*初始化*没问题。

</aside>

## Why We Use It

## 为什么我们使用它

It seems we have a winner. Our file system wrapper is available wherever we need
it without the tedium of passing it around everywhere. The class itself cleverly
ensures we won't make a mess of things by instantiating a couple of instances.
It's got some other nice features too:

这看起来已经决出了优胜者。
文件系统包装类在我们需要的地方到处可用，而无需沉重的将它到处传递。
类本身明确保证不会实例化多个实例。它还有很多其他的好性质：

*   **It doesn't create the instance if no one uses it.** Saving memory and CPU
    cycles is always good. Since the singleton is initialized only when it's
    first accessed, it won't be instantiated at all if the game never asks for
    it.

*   **如果没人用，就不必创建实例。**节约内存和CPU循环总是好的。由于单例只在第一次被使用时实例化，如果游戏永远不请求，那么它不会被实例化。

*   **It's initialized at runtime.** A common alternative to Singleton is a
    class with static member variables. I like simple solutions, so I use static
    classes instead of singletons when possible, but there's one limitation
    static members have: automatic initialization. The compiler initializes
    statics before `main()` is called. This means they can't use information
    known only once the program is up and running (for example, configuration
    loaded from a file). It also means they can't reliably depend on each other
    -- the compiler does not guarantee the order in which statics are
    initialized relative to each other.

*   **它在运行时实例化。**通常的单例模式是有着静态成员变量的类。我喜欢简单的解决方案，因此我尽可能使用静态类而不是单例，但是静态成员有一个限制：自动初始化。编译器在`main()`调用前初始化静态变量。这就意味着不能使用在程序加载并运行的信息（举个例子，从文件加载的配置）。这也意味着它们不能可靠地依赖彼此——编译器没有保证以什么样的顺序初始化静态变量。

    Lazy initialization solves both of those problems. The singleton will be
    initialized as late as possible, so by that time any information it needs
    should be available. As long as they don't have circular dependencies, one
    singleton can even refer to another when initializing itself.

    惰性初始化解决了以上两个问题。单例会尽可能晚的初始化，所以那时它需要的所有信息都应该可用了。只要它们没有环状依赖，一个单例在初始化它自己的时候甚至可以引用另一个单例。

*   **You can subclass the singleton.** This is a powerful but often overlooked
    capability. Let's say we need our file system wrapper to be cross-platform.
    To make this work, we want it to be an abstract interface for a file system
    with subclasses that implement the interface for each platform. Here is the
    base class:

*   **可将单例作为子类。**这是个很有用但通常过度了的能力。假设我们需要文件系统包装类跨平台。为了使之工作，我们需要它成为文件系统抽象出来的接口，有子类为每个平台实现接口。这里是基类：

    ^code 2

    Then we define derived classes for a couple of platforms:

    然后我们为一堆平台定义子类。

    ^code derived-file-systems

    Next, we turn `FileSystem` into a singleton:

    下一步，我们把`FileSystem`变成一个单例：

    ^code 3

    The clever part is how the instance is created:

    灵巧之处在于实例是如何被创建的。

    ^code 4

    With a simple compiler switch, we bind our file system wrapper to the
    appropriate concrete type. Our entire codebase can access the file system
    using `FileSystem::instance()` without being coupled to any
    platform-specific code. That coupling is instead encapsulated within the
    implementation file for the `FileSystem` class itself.

    通过一个简单的编译器switch，我们把文件系统包装类绑定到合适的具体类型上。整个代码库都可以使用`FileSystem::instance()`接触到文件系统，而无需和任何平台特定的代码耦合。耦合发生在为特定平台写的`FileSystem`类实现文件中。

This takes us about as far as most of us go when it comes to solving a problem
like this. We've got a file system wrapper. It works reliably. It's available
globally so every place that needs it can get to it. It's time to check in the
code and celebrate with a tasty beverage.

这满足了我们解决问题时大多数需要的程度。
我们有一个文件系统包装类。
它工作可靠，它全局有效，只要需要就能获得。
是时候签入代码然后用美味饮料庆祝了。

## Why We Regret Using It

## 为什么我们不要使用它

In the short term, the Singleton pattern is relatively benign. Like many design
choices, we pay the cost in the long term. Once we've cast a few unnecessary
singletons into cold hard code, here's the trouble we've bought ourselves:

短期来看，单例模式是相对良性的。
就像其他设计选择一样，我们需要从长期考虑。
一旦我们将一些不必要的单例写进代码，就会给自己带来的麻烦：

### It's a global variable

### 它是一个全局变量

When games were still written by a couple of guys in a garage, pushing the
hardware was more important than ivory-tower software engineering principles.
Old-school C and assembly coders used globals and statics without any trouble
and shipped good games. As games got bigger and more complex, architecture and
maintainability started to become the bottleneck. We struggled to ship games not
because of hardware limitations, but because of *productivity* limitations.

当游戏还是由几个家伙在车库中完成时，榨干硬件比象牙塔里的软件工程原则更重要
老式的C和汇编程序员能毫无问题的使用全局变量和静态变量，并发布好游戏。
随着游戏变得越来越大，越来越复杂，建构和管理开始变成瓶颈，
我们发布游戏的挣扎，不仅因为硬件限制，而且因为*生产力*限制。

So we moved to languages like C++ and started applying some of the hard-earned
wisdom of our software engineer forebears. One lesson we learned is that global
variables are bad for a variety of reasons:

所以我们迁移到了像C++这样的语言，开始将一些从软件工程师前辈那里学到的智慧应用于实际。我们学到的一课就是有很多原因导致全局变量不好：

*   **They make it harder to reason about code.** Say we're tracking down a bug
    in a function someone else wrote. If that function doesn't touch any global
    state, we can wrap our heads around it just by understanding the body of the
    <span name="pure">function</span> and the arguments being passed to it.

*   **理解代码更加困难。**假设我们在查找其他人写的函数中的漏洞。如果函数没有碰到任何全局状态，我们可以让脑子围着它转，只需搞懂<span name="pure">函数</span>和传给函数的变量。

    <aside name="pure">

    Computer scientists call functions that don't access or modify global state
    "pure" functions. Pure functions are easier to reason about, easier for the
    compiler to optimize, and let you do neat things like memoization where you
    cache and reuse the results from previous calls to the function.

    计算机科学家称不接触或修改全局状态的函数为“纯”函数。纯函数易于理解，易于编译器优化，让你完成优雅的任务，比如记住缓存的情况并继续上次函数调用。

    While there are challenges to using purity exclusively, the benefits are
    enticing enough that computer scientists have created languages like Haskell
    that *only* allow pure functions.

    完全使用纯函数是有难度的，但其好处足以引诱科学家创造像Haskell这样*只*允许纯函数的语言。

    </aside>

    Now, imagine right in the middle of that function is a call to
    `SomeClass::getSomeGlobalData()`. To figure out what's going on, we have
    to hunt through the entire codebase to see what touches that global data.
    You don't really hate global state until you've had to `grep` a million
    lines of code at three in the morning trying to find the one errant call
    that's setting a static variable to the wrong value.

    现在考虑在函数中间是个对`SomeClass::getSomeGlobalData()`的调用。为了查明发生了什么，得追踪整个代码库来看看什么修改了全局数据。你真的不需要讨厌全局变量，直到你在凌晨三点使用`grep`搜索数百万行代码，搞清楚哪一个错误的调用将一个静态变量设为了错误的值。

*   **They encourage coupling.** The new coder on your team isn't familiar with
    your game's beautifully maintainable loosely coupled architecture, but he's
    just been given his first task: make boulders play sounds when they crash
    onto the ground. You and I know we don't want the physics code to be coupled
    to *audio* of all things, but he's just trying to get his task done.
    Unfortunately for us, the instance of our `AudioPlayer` is globally visible.
    So, one little `#include` later, and our new guy has compromised a carefully
    constructed architecture.

*   **激励耦合。**团队的新程序员也许不熟悉游戏的完美管理，松散耦合的架构，但还是刚刚获得了第一个任务：在岩石撞击地面时播放一段声音。你我都知道这不需要将物理和*音频*代码耦合，但是他只想着把他的任务完成。不幸的是，我们的`AudioPlayer`是全局可见的。所以之后一个小小的`#include`，我们的新队员就连累 了整个精心设计的架构。

    Without a global instance of the audio player, even if he *did* `#include`
    the header, he still wouldn't be able to do anything with it. That
    difficulty sends a clear message to him that those two modules should not
    know about each other and that he needs to find another way to solve his
    problem. *By controlling access to instances, you control coupling.*

    不用音频播放器的全局实例，哪怕它*确实*`#include`头文件，它还是不能做任何事情。这种难度给它发送了一个明确的信号，这两个模块不应该知道对方，at需要找另外的方式来解决问题。*通过控制对实例的访问，你控制了耦合。*

*   **They aren't concurrency-friendly.** The days of games running on a simple
    single-core CPU are pretty much over. Code today must at the very least
    *work* in a multi-threaded way even if it doesn't take full advantage of
    concurrency. When we make something global, we've created a chunk of memory
    that every thread can see and poke at, whether or not they know what other
    threads are doing to it. That path leads to deadlocks, race conditions, and
    other hell-to-fix thread-synchronization bugs.

*   **非并行友好。**那些一个简单的单核CPU上运行游戏的日子已经远去。现代的代码至少应考虑使用多线程的方式*工作*，哪怕它完全不需要并行的优势。当我们将某些东西转为全局时，我们创建了一块每个线程都能看到并接触到的内存，不管它们知道不知道其他线程正在使用那块内存。这种方式带来了死锁，竞争状态，以及其他很难解决的线程同步问题。

Issues like these are enough to scare us away from declaring a global variable,
and thus the Singleton pattern too, but that still doesn't tell us how we
*should* design the game. How do you architect a game without global state?

像这样的问题足够吓阻我们声明全局变量了，
同理单例模式也是一样，但是那还没有告诉我们*应该*如何设计游戏。
怎样构建游戏而无需全局变量？

There are some extensive answers to that question (most of this book in many
ways *is* an answer to just that), but they aren't apparent or easy to come by.
In the meantime, we have to get games out the door. The Singleton pattern looks
like a panacea. It's in a book on object-oriented design patterns, so it *must*
be architecturally sound, right? And it lets us design software the way we have
been doing for years.

有很广泛的对这个问题的回答（这本书的大部分都*是*关于这一点的），
但是它们既不明显也不能轻易获取。
与此同时，我们得发布游戏。
单例模式看起来是万能药。
这是在一本关于面向对象设计模式的书中，因此它*必须*有施工的声音，对吧？
这让我们以做了多年的方式设计软件。

Unfortunately, it's more placebo than cure. If you scan the list of problems
that globals cause, you'll notice that the Singleton pattern doesn't solve any
of them. That's because a singleton *is* global state -- it's just encapsulated in a
class.

不幸的是，这不是解药，这是安慰剂。
如果你浏览全局变量造成的问题列表，你会注意到单例模式解决不了任何一个。
这是因为单例*是*全局状态——它只是被封装在一个类中。

### It solves two problems even when you just have one

### 它能在你只有一个问题的时候解决两个

The word "and" in the Gang of Four's description of Singleton is a bit strange.
Is this pattern a solution to one problem or two? What if we have only one of
those? Ensuring a single instance is useful, but who says we want to let
*everyone* poke at it? Likewise, global access is convenient, but that's true
even for a class that allows multiple instances.

在GoF的对单例的描述中，“并”这个词有点奇怪。
这个模式是一个问题还是两个问题的解决方案
？如果我们只有其中一个问题呢？
保证单一的实例是有用的，但是谁告诉我们要让*每个人*都能接触到它？
同样，全局接触很方便，但是一个类不允许多个实例是对的么？

The latter of those two problems, convenient access, is almost always why we
turn to the Singleton pattern. Consider a logging class. Most modules in the
game can benefit from being able to log diagnostic information. However, passing
an instance of our `Log` class to every single function clutters the method
signature and distracts from the intent of the code.

两问题中的后者，方便地获取，是我们使用单例模式几乎全部的原因。
考虑日志类。大部分模块都能从记录诊断性日志中获益。
但是，如果将`Log`类的实例传给每个需要这个方法的函数，那就混杂了产生的数据，模糊了代码的意图。

The obvious fix is to make our `Log` class a singleton. Every function can then
go straight to the class itself to get an instance. But when we do that, we
inadvertently acquire a strange little restriction. All of a sudden, we can no
longer create more than one logger.

明显的解决方案是让`Log`类成为单例。
每个可以直接接触类的函数获得一个实例。
但当我们这样做时，我们无意地制造了一个奇怪的小约束。
突然之间，我们不再能创建超过一个日志记录者了。

At first, this isn't a problem. We're writing only a single log file, so we only
need one instance anyway. Then, deep in the development cycle, we run into
trouble. Everyone on the team has been using the logger for their own
diagnostics, and the log file has become a massive dumping ground. Programmers
have to wade through pages of text just to find the one entry they care about.

最开始的时候，这不是一个问题。
我们写一个单独的日志文件，所以只需要一个实例。
然后，在开发循环的深处，我们遇到了麻烦。
每个团队的成员都使用日志记录各自的诊断信息，日志文件大量倾泻在地上。
程序员为了找到关心的一个入口点，需要翻过很多页代码。

We'd like to fix this by partitioning the logging into multiple files. To do
this, we'll have separate loggers for different game <span
name="worse">domains</span>: online, UI, audio, gameplay. But we can't. Not only
does our `Log` class no longer allow us to create multiple instances, that
design limitation is entrenched in every single call site that uses it:

我们想将日志分散到多个文件中来解决这点。
为了做到这点，我们为游戏不同的<span name="worse">领域</span>创造分散的日志记录者：
在线部分，UI，声音，游戏玩法。
但是我们不能，不仅仅因为`Log`类不再允许我们创建多个实例，
设计限制保证每个使用它的都是单一调用：

    Log::instance().write("Some event.");

In order to make our `Log` class support multiple instantiation (like it
originally did), we'll have to fix both the class itself and every line of code
that mentions it. Our convenient access isn't so convenient anymore.

为了让`Log`类支持多个实例（就像它原来的那样），
我们需要修改类和提及它的每一行代码。
我们的方便获取就不再那么方便了。

<aside name="worse">

It could be even worse than this. Imagine your `Log` class is in a library being
shared across several *games*. Now, to change the design, you'll have to
coordinate the change across several groups of people, most of whom have neither
the time nor the motivation to fix it.

这可能更糟。想象一下你的`Log`类是在多个*游戏*间共享的类。
现在，为了改变设计，需要在多组人之间协调改变，
他们中的大多数既没有时间，也没有动机修复它。

</aside>

### Lazy initialization takes control away from you

### 惰性初始化从你那里剥夺了控制权

In the desktop PC world of virtual memory and soft performance requirements,
lazy initialization is a smart trick. Games are a different animal. Initializing
a system can take time: allocating memory, loading resources, etc. If
initializing the audio system takes a few hundred milliseconds, we need to
control when that's going to happen. If we let it lazy-initialize itself the
first time a sound plays, that could be in the middle of an action-packed part
of the game, causing visibly dropped frames and stuttering gameplay.

拥有虚拟内存和软性性能需求的桌面电脑世界中，惰性初始化是一个小技巧。
游戏则是另一中生物。初始化系统需要消耗时间：分配内存，加载资源，等等。
如果初始化音频系统消耗了几百个毫秒，我们需要控制它发生的时间。
如果在第一次声音播放时惰性初始化它自己，这可能是在动作部分的中间，导致可见的掉帧和断续的游戏体验。

Likewise, games generally need to closely control how memory is laid out in the
heap to avoid <span name="fragment">fragmentation</span>. If our audio
system allocates a chunk of heap when it initializes, we want to know *when*
that initialization is going to happen, so that we can control *where* in the
heap that memory will live.

同样，游戏通常需要严格控制在堆上分配的内存来避免<span name="fragment">碎片</span>。
如果音频系统在初始化时分配到了堆上，我们需要知道*何时*初始化发生了，
这样我们可以控制内存会待在堆的*哪里*。

<aside name="fragment">

See <a class="pattern" href="object-pool.html">Object Pool</a> for a detailed
explanation of memory fragmentation.

<a class="pattern" href="object-pool.html">对象池</a>中有内存碎片的扩展细节。

</aside>

Because of these two problems, most games I've seen don't rely on lazy
initialization. Instead, they implement the Singleton pattern like this:

因为这两个原因，我见到的大多数游戏都不依赖惰性初始化。
相反，它们像这样实现单例模式：

^code 5

That solves the lazy initialization problem, but at the expense of discarding
several singleton features that *do* make it better than a raw global variable.
With a static instance, we can no longer use polymorphism, and the class must be
constructible at static initialization time. Nor can we free the memory that the
instance is using when not needed.

这解决了惰性初始化问题，但是损失了几个单例*确实*比原生的全局变量优良的特性。
通过静态实例，我们不能使用多态，类也必须是在静态初始化时就可以构建的。
我们也不能在不需要的时候释放实例使用的的内存。

Instead of creating a singleton, what we really have here is a simple static
class. That isn't necessarily a bad thing, but if a static class is all you
need, <span name="static">why not</span> get rid of the `instance()` method
entirely and use static functions instead? Calling `Foo::bar()` is simpler than
`Foo::instance().bar()`, and also makes it clear that you really are dealing
with static memory.

不是创建一个单例，而是真正有一个简单的静态类。
这不是坏事，但是如果静态类是你需要的，
<span name="static">为什么不</span>完全摆脱`instance()`方法，
直接使用静态函数呢？调用`Foo::bar()`比`Foo::instance().bar()`更简单，
也更明确地表明你在处理静态内存。

<aside name="static">

The usual argument for choosing singletons over static classes is that if you
decide to change the static class into a non-static one later, you'll need to
fix every call site. In theory, you don't have to do that with singletons
because you could be passing the instance around and calling it like a normal
instance method.

使用单例而不是静态类的论点是，
如果你后来决定将静态类改为非静态的，你需要修复每一个调用点。
理论上，用单例就不必那么做，以为你可以将实例传来传去，像正常实例方法一样使用。

In practice, I've never seen it work that way. Everyone just does
`Foo::instance().bar()` in one line. If we changed Foo to not be a
singleton, we'd still have to touch every call site. Given that, I'd rather have
a simpler class and a simpler syntax to call into it.

实践中，我从没看到这样的情况。
每个人都在一行写下`Foo::instance().bar()`。
如果我们将Foo改成单例，我们还是得修改每一个调用点。
鉴于此，我更喜欢简单的类和简单的调用语法。

</aside>

## What We Can Do Instead

## 我们可以做什么来替代

If I've accomplished my goal so far, you'll think twice before you pull
Singleton out of your toolbox the next time you have a problem. But you still
have a problem that needs solving. What tool *should* you pull out? Depending on
what you're trying to do, I have a few options for you to consider, but first...

如果我现在达到了目标，你就会在下次遇到问题使用单例模式之前三思而后行。
但是你还是有问题需要解决。
你*应该*使用什么工具呢？
取决于你在试图做什么，我有一些你可以考虑的选项，但是首先……

### See if you need the class at all

### 看看你是不是真正的需要类

Many of the singleton classes I see in games are "managers" -- those nebulous
classes that exist just to babysit other objects. I've seen codebases where it
seems like *every* class has a manager: Monster, MonsterManager, Particle,
ParticleManager, Sound, SoundManager, ManagerManager. Sometimes, for variety,
they'll throw a "System" or "Engine" in there, but it's still the same idea.

我在游戏中看到的很多单例类都是“管理器”——那些类存在的意义就是照顾其他对象。
我看到代码库中似乎*所有*类都有管理器：
怪物，怪物管理器，粒子，粒子管理器，声音，声音管理器，管理管理器的管理器。
更多情况下，它们有时会在那里放“系统”或“引擎”，但是思路还是一样的。

While caretaker classes are sometimes useful, often they just reflect
unfamiliarity with OOP. Consider these two contrived classes:

小心处理类有时是有用的，
通常它们只是反映对OOP的不熟悉。思考这两个特制的类：

^code 8

Maybe this example is a bit dumb, but I've seen plenty of code that reveals a
design just like this after you scrape away the crusty details. If you look at
this code, it's natural to think that `BulletManager` should be a singleton. After
all, anything that has a `Bullet` will need the manager too, and how many
instances of `BulletManager` do you need?

也许这个例子有些蠢，但是我见过很多代码，在扫去细节后是一样的设计。
如果你看看这个代码，`BulletManager`很自然应是一个单例。
无论如何，任何有`Bullet`的都需要管理，而你需要多少`BulletManager`实例呢？

The answer here is *zero*, actually. Here's how we solve the "singleton" problem
for our manager class:

事实上，这里的答案是*零*。
这是我们如何为管理类解决“单例”问题：

^code 9

There we go. No manager, no problem. Poorly designed singletons are often
"helpers" that add functionality to another class. If you can, just move all of
that behavior into the class it helps. After all, OOP is about letting objects
take care of themselves.

好了。没有管理员，没问题。
糟糕设计的单例通常“帮助”另一个类增加代码。
如果你可以的话，把所有的行为都移到类中。
无论如何，OOP是关于让对象照顾好他自己。

Outside of managers, though, there are other problems where we'd reach to
Singleton for a solution. For each of those problems, there are some alternative
solutions to consider.

但是在管理器之外，还有其他问题我们需要寻求单例解决。
对于每种问题，都有一些后续方案来考虑。

### To limit a class to a single instance

### 将一个类限制到一个单一的实例

This is one half of what the Singleton pattern gives you. As in our file system
example, it can be critical to ensure there's only a single instance of a class.
However, that doesn't necessarily mean we also want to provide *public*,
*global* access to that instance. We may want to restrict access to certain
areas of the code or even make it <span name="wrapper">private</span> to a
single class. In those cases, providing a public global point of access weakens
the architecture.

这是单例模式给你的一半东西。
就像在文件系统的例子中那样，保证一个类只有一个实例是很重要的。
但是，这不意味着我们需要*公开*提供对那个实例的*全局*访问。
我们想要减少对某部分代码的公开部分，
甚至让它在一个类中是<span name="wrapper">私有的</span>。
在这些情况下，提供一个全局接触点消弱了架构。

<aside name="wrapper">

For example, we may be wrapping our file system wrapper inside *another* layer
of abstraction.

举个例子，我们也许想把我们的文件系统包在*另一*层抽象中。

</aside>

We want a way to ensure single instantiation *without* providing global access.
There are a couple of ways to accomplish this. Here's one:

我们想要一种方式保证只有一个实例而*无需*提供全局接触。
有好几种方法能行。这是其中之一：

<span name="assert"></span>

<span name="assert"></span>

^code 6

This class allows anyone to construct it, but it will assert and fail if you try to
construct more than one instance. As long as the right code creates the instance
first, then we've ensured no other code can either get at that instance or
create their own. The class ensures the single instantiation requirement it
cares about, but it doesn't dictate how the class should be used.

这个类允许任何人构建它，如果你试图构建超过一个实例，它会断言并失败。
只要正确的代码首先创建了实例，那么就得保证没有其他代码可以接触实例或者创建它们自己的实例。这个类保证了它关注的单一实例需求，但是它没有指定类该如何被使用。

<aside name="assert">

An *assertion* function is a way of embedding a contract into your code.
When `assert()` is called, it evaluates the expression passed to it. If it
evaluates to `true`, then it does nothing and lets the game continue. If it
evaluates to `false`, it immediately halts the game at that point. In a
debug build, it will usually bring up the debugger or at least print out the
file and line number where the assertion failed.

*断言*函数是一种向你的代码中添加限制的方法。
当`assert()`被调用时，它计算传入的表达式。
如果结果为`true`，那么什么都不做，游戏继续。
如果结果为`false`，它立刻停止游戏。
在debug build时，这通常会启动漏洞处理器，或至少打印断言失败的文件和行号。

An `assert()` means, "I assert that this should always be true. If it's not,
that's a bug and I want to stop *now* so you can fix it." This lets you
define contracts between regions of code. If a function asserts that one of
its arguments is not `NULL`, that says, "The contract between me and the
caller is that I will not be passed `NULL`."

`assert()`表示，
“我断言这个总该是真的。如果不是，那就是漏洞，我想*立刻*停止来处理它。”
这样你在代码区域之间电议约束。
如果函数断言它的一个参数不能为`NULL`，那就是说，“我和调用者之间的协议是，我不会被传入`NULL`。”

Assertions help us track down bugs as soon as the game does something
unexpected, not later when that error finally manifests as something
visibly wrong to the user. They are fences in your codebase, corralling bugs
so that they can't escape from the code that created them.

断言帮助我们在游戏做出预期以外的事时立刻追踪漏洞，
而不是在错误最终显现在某些用户可见的事物上。
它们是代码中的栅栏，围住漏洞，这样漏洞就不能从制造它的代码边逃开。

</aside>

The downside with this implementation is that the check to prevent multiple
instantiation is only done at *runtime*. The Singleton pattern, in contrast,
guarantees a single instance at compile time by the very nature of the class's
structure.

这个实现的缺点是只在*运行时*检查并防止多个实例化。
单例模式，正相反，通过类的自然结构，在编译时就确定单一实例。

### To provide convenient access to an instance

### 为了给一个实例提供方便的获取方法

Convenient access is the main reason we reach for singletons. They make it easy
to get our hands on an object we need to use in a lot of different places. That
ease comes at a cost, though -- it becomes equally easy to get our hands on the
object in places where we *don't* want it being used.

方便的获取是我们使用单例的一个主要原因。
这让我们在不同地方获取需要的对象更加容易。
这种轻松是需要代价的——在我们*不想*要对象的地方，也能很容易地使用。

The general rule is that we want variables to be as narrowly scoped as possible
while still getting the job done. The smaller the scope an object has, the fewer
places we need to keep in our head while we're working with it. Before we take
the shotgun approach of a singleton object with *global* scope, let's consider
other ways our codebase can get access to an object:

通用原则是在能完成工作的同时，将变量写得尽可能局部。
对象影响的范围越小，我们在处理它的时候就需要把越少东西放在脑子里。
在我们拿起有*全局*范围影响的单例对象猎枪时，
让我们考虑考虑代码中其他获取对象的方式：

 *  **Pass it in.** The <span name="di">simplest</span> solution, and often the
    best, is to simply pass the object you need as an argument to the functions
    that need it. It's worth considering before we discard it as too cumbersome.

 *  **传进来。**<span name="di">最简单的</span>解决办法，通常也是最好的，把你需要的对象简单的作为参数传给需要它的函数。在用其他更加繁杂的方法前，考虑一下这个解决方案。

    <aside name="di">

    Some use the term "dependency injection" to refer to this. Instead of code
    reaching *out* and finding its dependencies by calling into something
    global, the dependencies are pushed *in* to the code that needs it through
    parameters. Others reserve "dependency injection" for more complex ways of
    providing dependencies to code.

    有些人使用术语“依赖注入”来指代它。不是代码*出来*调用某些全局量来确认它的依赖，而是依赖通过参数被*传入*到需要它的代码中去。其他人将“依赖注入”保留给为代码提供依赖的更复杂的方式。

    </aside>

    Consider a function for rendering objects. In order to render, it needs
    access to an object that represents the graphics device and maintains the
    render state. It's very common to simply pass that in to all of the rendering
    functions, usually as a parameter named something like `context`.

    考虑渲染对象的函数。为了渲染，它需要接触一个代表图形设备的对象，管理渲染状态。将其传给所有渲染函数是很自然的，通常是用一个参数，名字像`context`之类的。

    On the other hand, some objects don't belong in the signature of a method.
    For example, a function that handles AI may need to also write to a <span
    name="aop">log file</span>, but logging isn't its core concern. It would be
    strange to see `Log` show up in its argument list, so for cases like that
    we'll want to consider other options.

    另一方面，游戏对象不属于方法的签名。举个例子，处理AI的函数可能也需要写<span name="aop">日志文件</span>，但是日志不是它的核心关注点。看到`Log`出现在它的参数列表中是很奇怪的事情，由于像这样的情况，我们需要考虑其他的选项。

    <aside name="aop">

    The term for things like logging that appear scattered throughout a codebase
    is "cross-cutting concern". Handling cross-cutting concerns gracefully is
    a continuing architectural challenge, especially in statically typed
    languages.

    像日志这样撒布在代码库各处的是“cross-cutting concern”。小心的处理cross-cutting concern是一个持久的架构挑战，特别是在静态类型语言中。

    [Aspect-oriented
    programming](http://en.wikipedia.org/wiki/Aspect-oriented_programming) was
    designed to address these concerns.

    [Aspect-oriented
    programming](http://en.wikipedia.org/wiki/Aspect-oriented_programming)被设计处理应对它们。

    </aside>

 *  **Get it from the base class.** Many game architectures have shallow but
    wide inheritance hierarchies, often only one level deep. For example, you
    may have a base `GameObject` class with derived classes for each enemy or
    object in the game. With architectures like this, a large portion of the
    game code will live in these "leaf" derived classes. This means that all
    these classes already have access to the same thing: their `GameObject` base
    class. We can use that to our advantage:

 *  **从基类中获得。**很多游戏架构有浅层但是广泛的继承层次，通常只有一层深。举个例子，你也许有`GameObject`基类，每个游戏中的敌人或者对象都继承它。使用这样的架构，很大一部分游戏代码会存在这些“子”推导类中。这就意味着这些类已经有了对同样事物的相同获取方法：它们的`GameObject`基类。我们可以利用这点：

    <span name="gameobject"></span>

    ^code 10

    This ensures nothing outside of `GameObject` has access to its `Log` object,
    but every derived entity does using `getLog()`. This pattern of letting
    derived objects implement themselves in terms of protected methods provided
    to them is covered in the <a class="pattern"
    href="subclass-sandbox.html">Subclass Sandbox</a> chapter.

    这保证任何`GameObject`外部的东西都不能接触它的`Log`对象，但是每个推导的实体确实能使用`getLog()`。这种提供保护方法，让推导对象实现它们的模式，涵盖在<a class="pattern"
    href="subclass-sandbox.html">子类沙箱</a> 这章中。

    <aside name="gameobject">

    This raises the question, "how does `GameObject` get the `Log` instance?" A
    simple solution is to have the base class simply create and own a static
    instance.

    这提出了问题，“`GameObject`是怎样获得`Log`实例的？”一个简单的方案是让基类创建并拥有静态实例。

    If you don't want the base class to take such an active role, you can
    provide an initialization function to pass it in or use the <a
    class="pattern" href="service-locator.html">Service Locator</a> pattern to
    find it.

    如果你不想要基类做这样的角色，你可以土工初始化方法传入，或使用<a     class="pattern" href="service-locator.html">服务定位器</a>模式找到它。

    </aside>

 *  **Get it from something already global.** The goal of removing *all* global
    state is admirable, but rarely practical. Most codebases will still have a
    couple of globally available objects, such as a single `Game` or `World`
    object representing the entire game state.

 *  **从已经是全局的东西中获取。**移除*所有*全局状态的目标令人钦佩，但是并不实际。大多数代码库还有一些全局可用对象，比如一个代表了整个游戏状态的`Game`或`World`对象。

    We can reduce the number of global classes by piggybacking on existing ones
    like that. Instead of making singletons out of `Log`, `FileSystem`, and
    `AudioPlayer`, do this:

    我们可以让现有的全局对象捎带需要的东西，来减少全局变量类的数目。不让`Log`，`FileSystem`和`AudioPlayer`变成单例，而这样做：

    ^code 11

    With this, only `Game` is globally available. Functions can get to the
    other systems <span name="demeter">through</span> it:

    这样，只有`Game`是全局可用的。需要到其他系统的函数得<span name="demeter">经过</span>它：

    ^code 12

    <aside name="demeter">

    Purists will claim this violates the Law of Demeter. I claim that's still
    better than a giant pile of singletons.

    纯粹主义者会声称这违反了Demeter法则。我则声称这比一大坨单例要好。

    </aside>

    If, later, the architecture is changed to support multiple `Game` instances
    (perhaps for streaming or testing purposes), `Log`, `FileSystem`, and
    `AudioPlayer` are all unaffected -- they won't even know the difference. The
    downside with this, of course, is that more code ends up coupled to `Game`
    itself. If a class just needs to play sound, our example still requires it
    to know about the world in order to get to the audio player.

    如果，稍后，架构被改为支持多个`Game`实例（可能是为了流或者测试目的），`Log`，`FileSystem`，和`AudioPlayer`都不会被影响到——它们甚至不知道有什么区别。缺陷是，当然，更多的代码耦合到了`Game`中。如果一个类只是需要播放声音，为了获得音频播放器，我们的例子仍然要它知道游戏世界。

    We solve this with a hybrid solution. Code that already knows about `Game`
    can simply access `AudioPlayer` directly from it. For code that doesn't, we
    provide access to `AudioPlayer` using one of the other options described
    here.

    我们通过混合方案解决这点。知道`Game`的代码可以直接从那里获得`AudioPlayer`。而不知道的代码，我们用上面描述的其他选项来提供`AudioPlayer`。

 *  **Get it from a Service Locator.** So far, we're assuming the global class
    is some regular concrete class like `Game`. Another option is to define a
    class whose sole reason for being is to give global access to objects. This
    common pattern is called a <a class="pattern"
    href="service-locator.html">Service Locator</a> and gets its own chapter.

 *  **从服务定位器中获得。**目前为止，我们假设全局类是通常具体的类比如`Game`。另一种选项是定义一个类，存在的唯一目标就是为对象提供全局访问。这种常见的模式被称为<a class="pattern" href="service-locator.html">服务定位器</a>模式，有它单独的章节。

## What's Left for Singleton

## 单例中还剩下什么

The question remains, where *should* we use the real Singleton pattern?
Honestly, I've never used the full Gang of Four implementation in a game. To
ensure single instantiation, I usually simply use a static class. If that
doesn't work, I'll use a static flag to check at runtime that only one instance
of the class is constructed.

剩下的问题，何处我们*应该*使用真实的单例模式？
说实话，我从来没有在游戏中使用全部的GoF模式。
为了保证单一实例化，我通常简单的使用静态类。
如果这无效，我使用静态标识在运行时检测是不是只有一个实例被创建了。

There are a couple of other chapters in this book that can also help here. The
<a class="pattern" href="subclass-sandbox.html">Subclass Sandbox</a> pattern
gives instances of a class access to some shared state without making it
globally available. The <a class="pattern" href="service-locator.html">Service
Locator</a> pattern *does* make an object globally available, but it gives you more
flexibility with how that object is configured.


<a class="pattern" href="subclass-sandbox.html">子类沙箱</a>模式通过分享状态，给实例以类访问权限而无需让其全局可用。
<a class="pattern" href="service-locator.html">服务定位器</a>模式*确实*让一个对象全局可用，但它给了你如何设置对象的更多自由度。
