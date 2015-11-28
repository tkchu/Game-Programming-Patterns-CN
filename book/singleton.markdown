^title Singleton
^section Design Patterns Revisited

This chapter is an anomaly. Every other chapter in this book shows
you how to use a design pattern. This chapter shows you how *not* to use
one.

这个章节不同寻常。这本书的其他章节展示如何使用一个设计模式。这个章节向你展示如何*不*使用一个设计模式。

Despite noble intentions, the <a class="gof-pattern"
href="http://c2.com/cgi/wiki?SingletonPattern">Singleton</a> pattern described
by the Gang of Four usually does more harm than good. They stress that the
pattern should be used sparingly, but that message was often lost in translation
to the <span name="instance">game industry</span>.

不管他高贵的意图，GoF描述的单例模式通常带来的坏处多于好处。他们强调这个模式应该谨慎使用，但是这个消息在传到游戏工业界时经常就消失了。

Like any pattern, using Singleton where it doesn't belong is about as helpful as
treating a bullet wound with a splint. Since it's so overused, most of this
chapter will be about *avoiding* singletons, but first, let's go over the
pattern itself.

就像其他模式一样，在不合适的地方使用单例模式就好像用夹板处理子弹伤口。由于它是如此的滥用，这章的大部分都在讲如何*回避*单例模式，但首先，我们浏览一遍模式本身。

<aside name="instance">

When much of the industry moved to object-oriented programming from C, one
problem they ran into was "how do I get an instance?" They had some method they
wanted to call but didn't have an instance of the object that provides that
method in hand. Singletons (in other words, making it global) were an easy way
out.

</aside>

## The Singleton Pattern

## 单例模式

*Design Patterns* summarizes Singleton like this:

*设计模式*像这样总结单例模式：

> Ensure a class has one instance, and provide a global point of access to it.

> 保证一个类只有一个实例，并提供一个全局接入点来获得它。

We'll split that at "and" and consider each half separately.

我们从“并”分割这个句子，分别考虑每一部分。

### Restricting a class to one instance

### 限制一个类只有一个实例

There are times when a class cannot perform correctly if there is more than one
instance of it. The common case is when the class interacts with an external
system that maintains its own global state.

有时候，如果有超过一个实例，类不能正确的运行。通常情况是类与保持了它自己的全局状态的外部系统互动。

Consider a class that wraps an underlying file system API. Because file
operations can take a while to complete, our class performs operations
asynchronously. This means multiple operations can be running concurrently, so
they must be coordinated with each other. If we start one call to create a file
and another one to delete that same file, our wrapper needs to be aware of both
to make sure they don't interfere with each other.

考虑一个包裹了潜在文件系统的API。因为文件操作需要一段时间完成，我们的类异步完成操作。这就意味着多个操作可以同时运行，所以他们必须相互协调。乳沟从吗一个调用创建文件，另一个删除同一文件，我们的包装器需要同时考虑这两个来保证他们没有相互妨碍。

To do this, a call into our wrapper needs to have access to every previous
operation. If users could freely create instances of our class, one instance
would have no way of knowing about operations that other instances started.
Enter the singleton. It provides a way for a class to ensure at compile time
that there is only a single instance of the class.

为了做到这一点，一个对包装器的调用需要接触之前的每一个操作。如果用户可以自由的创建类的实例，一个实例无法知道另一个实例开始的操作。进入单例模式。它提供了一种方式构建类，在编译时保证类只有单一实例。

### Providing a global point of access

### 提供一个全局接入点

Several different systems in the game will use our file system wrapper: logging,
content loading, game state saving, etc. If those systems can't create their own
instances of our file system wrapper, how can they get ahold of one?

游戏中的不同系统都会使用我们的文件系统包装类：日志，内容加载，游戏状态保存，等等。如果这些系统不能创建他们自己的文件系统包装实例，他们如何获取一个呢？

Singleton provides a solution to this too. In addition to creating the single
instance, it also provides a globally available method to get it. This way,
anyone anywhere can get their paws on our blessed instance. All together, the
classic implementation looks like this:

单例模式为这个也提供了一个解决方案。处理创建单一实例，它也提供了一种全局可用方法来获得它。这种范式，无论何处何人都可以抓取我们受祝福的实例。所有的加起来，类独栋经典实现如下：

^code 1

The static `instance_` member holds an instance of the class, and the private
constructor ensures that it is the *only* one. The public static `instance()`
method grants access to the instance from anywhere in the codebase. It is also
responsible for instantiating the singleton instance lazily the first time
someone asks for it.

静态的`instance_`成员保持了一个类的实例，私有的构造器保证了*只有*一个。公开的静态方法`instance()`让代码库的任何地方都能获取实例。他同样负责在第一个有人请求的时候才懒惰地实例化单例实例。

A modern take looks like this:

一个现代的实现看起来是这样的：

^code local-static

C++11 <span name="thread">mandates</span> that the initializer for a local
static variable is only run once, even in the presence of concurrency. So,
assuming you've got a modern C++ compiler, this code is thread-safe where the
first example is not.

C++11保证了本地静态变量只会运行一次，哪怕是同时发生的情况下。因此，假设你有一个现代C++编译器，这个代码是线程安全的而第一个例子不是。

<aside name="thread">

Of course, the thread-safety of your singleton class itself is an entirely
different question! This just ensures that its *initialization* is.

</aside>

## Why We Use It

## 为什么我们使用它

It seems we have a winner. Our file system wrapper is available wherever we need
it without the tedium of passing it around everywhere. The class itself cleverly
ensures we won't make a mess of things by instantiating a couple of instances.
It's got some other nice features too:

这看起来已经决出了优胜者。我买单文件系统包装类在我们需要的地方到处可用，而无需沉闷的将它到处传递。类本身明确保证我们我们不会实例化多个实例。它还有很多其他的好性质：

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

*   **它在运行时实例化。**一个通常的单例模式选项是一个有着静态成员变量的类。我喜欢简单的解决方案，因此我尽可能使用静态类而不是单例，但是静态成员有一个限制：自动初始化。编译器在`main()`调用前初始化静态变量。这就意味着他们不能使用在程序加载并运行的信息（举个例子，从文件加载的配置）。这也意味着他们不能可靠地依赖彼此——编译器没有保证以什么样的顺序初始化静态变量。

    Lazy initialization solves both of those problems. The singleton will be
    initialized as late as possible, so by that time any information it needs
    should be available. As long as they don't have circular dependencies, one
    singleton can even refer to another when initializing itself.

    惰性初始化解决了以上两个问题。单例会尽可能晚的初始化，所以那时所有他需要的信息都应该可用了。只要他们没有环状依赖，一个单例在初始化它自己的时候甚至可以引用另一个单例。

*   **You can subclass the singleton.** This is a powerful but often overlooked
    capability. Let's say we need our file system wrapper to be cross-platform.
    To make this work, we want it to be an abstract interface for a file system
    with subclasses that implement the interface for each platform. Here is the
    base class:

*   **你可以将单例作为子类。**这是一个很有用但通常过分了的能力。假设我们需要我们的文件系统包装类跨平台。为了让这个工作，我们需要它作为每个文件系统抽象出来的接口，有子类为每个平台实现接口。这就是基类：

    ^code 2

    Then we define derived classes for a couple of platforms:

    然后我们为一堆平台定义推导类。

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

    通过一个简单的编译器的switch，我们把我们的的文件系统包装类绑定到合适的具体类型上。我买单整个代码库都可以使用`FileSystem::instance()`接触到文件系统，而无需和任何平台特定的代码耦合。耦合发生在为特定平台写的`FileSystem`类实现文件中。

This takes us about as far as most of us go when it comes to solving a problem
like this. We've got a file system wrapper. It works reliably. It's available
globally so every place that needs it can get to it. It's time to check in the
code and celebrate with a tasty beverage.

这带我们完成了解决这样一个问题我们中的大多数需要靠的的程度。我们有一个文件系统包装类。它可靠的工作。它全局有效这样只要需要就能获得。是时候签入代码然后用美味饮料庆祝了。

## Why We Regret Using It

## 为什么我们不要使用它

In the short term, the Singleton pattern is relatively benign. Like many design
choices, we pay the cost in the long term. Once we've cast a few unnecessary
singletons into cold hard code, here's the trouble we've bought ourselves:

短期来看，单例模式是相对良性的。就像其他设计选择一样，我们需要从长期考虑。一旦我们将一些不必要的单例写进代码，这里有我们给自己带来的麻烦：

### It's a global variable

### 它是一个全局变量

When games were still written by a couple of guys in a garage, pushing the
hardware was more important than ivory-tower software engineering principles.
Old-school C and assembly coders used globals and statics without any trouble
and shipped good games. As games got bigger and more complex, architecture and
maintainability started to become the bottleneck. We struggled to ship games not
because of hardware limitations, but because of *productivity* limitations.

当游戏还是几个家伙在车库中完成的东西时，榨干硬件比象牙塔里的软件工程原则更加重要。老式的C和汇编程序员毫无问题的使用全局变量和静态变量，发布好游戏。随着游戏变得越来越大，越来越复杂，建构和管理开始变成瓶颈，我们挣扎着发布游戏不但因为硬件限制，而且因为*生产力*限制。

So we moved to languages like C++ and started applying some of the hard-earned
wisdom of our software engineer forebears. One lesson we learned is that global
variables are bad for a variety of reasons:

所以我们迁移到了像C++这样的语言，开始将一些从我吗软件工程师祖先那里学到的智慧应用于实际。我们学到的一课就是全局变量因为多种原因而不好：

*   **They make it harder to reason about code.** Say we're tracking down a bug
    in a function someone else wrote. If that function doesn't touch any global
    state, we can wrap our heads around it just by understanding the body of the
    <span name="pure">function</span> and the arguments being passed to it.

*   **他们让理解代码更加困难。**假设我们追踪一个其他人写的函数中的漏洞。如果函数没有碰到任何全局状态，我们可以让给脑子围着它转只需搞懂函数的主体和传给它的变量。

    <aside name="pure">

    Computer scientists call functions that don't access or modify global state
    "pure" functions. Pure functions are easier to reason about, easier for the
    compiler to optimize, and let you do neat things like memoization where you
    cache and reuse the results from previous calls to the function.

    While there are challenges to using purity exclusively, the benefits are
    enticing enough that computer scientists have created languages like Haskell
    that *only* allow pure functions.

    </aside>

    Now, imagine right in the middle of that function is a call to
    `SomeClass::getSomeGlobalData()`. To figure out what's going on, we have
    to hunt through the entire codebase to see what touches that global data.
    You don't really hate global state until you've had to `grep` a million
    lines of code at three in the morning trying to find the one errant call
    that's setting a static variable to the wrong value.

    现在考虑在函数的中间部分是一个对`SomeClass::getSomeGlobalData()`的调用。为了查明发生了什么，我们的追踪整个代码库来看看什么修改了全局数据。你真的不需要讨厌全局变量，在凌晨三点使用`grep`搜索数百万行代码来搞清楚哪一个错误滴啊用讲一个静态变量设为了错误的值。

*   **They encourage coupling.** The new coder on your team isn't familiar with
    your game's beautifully maintainable loosely coupled architecture, but he's
    just been given his first task: make boulders play sounds when they crash
    onto the ground. You and I know we don't want the physics code to be coupled
    to *audio* of all things, but he's just trying to get his task done.
    Unfortunately for us, the instance of our `AudioPlayer` is globally visible.
    So, one little `#include` later, and our new guy has compromised a carefully
    constructed architecture.

*   **他们激励耦合。**你团队的新程序员也许不熟悉你游戏的美妙管理松散耦合的架构，但还是他刚刚获得了他的第一个人物：在岩石撞击地面时播放一段声音。你我都知道这不需要将物理待会和*音频*代码耦合，但是他只想着把他的人物完成。对我们不幸的是，我们的`AudioPlayer`是全局可见的。所以之后一个小小的`#include`，我们的新队员就连累 了整个精心设计的架构。

    Without a global instance of the audio player, even if he *did* `#include`
    the header, he still wouldn't be able to do anything with it. That
    difficulty sends a clear message to him that those two modules should not
    know about each other and that he needs to find another way to solve his
    problem. *By controlling access to instances, you control coupling.*

    不是用音频播放器的全局实例，哪怕他*确实*`#include`头文件，他还是不能做任何事情。这种难度给他发送了一个明确的信号，这两个模块不应该知道对方，他需要找另外一个方式来解决这个问题。*通过对实例访问权限的控制，你控制了耦合。*

*   **They aren't concurrency-friendly.** The days of games running on a simple
    single-core CPU are pretty much over. Code today must at the very least
    *work* in a multi-threaded way even if it doesn't take full advantage of
    concurrency. When we make something global, we've created a chunk of memory
    that every thread can see and poke at, whether or not they know what other
    threads are doing to it. That path leads to deadlocks, race conditions, and
    other hell-to-fix thread-synchronization bugs.

*   **他们不是并行友好的。**那些游戏在一个简单的单核CPU上运行的日子已经远去。现代的代码至少应考虑使用多线程的方式*工作*，哪怕他完全不需要并行的优势。当我们将某些东西转为全局时，我们创建了一块内存每一个线程都能看到并接触到，不管他们知道不知道其他线程正在使用它。这种方式带来了死锁，竞争状态，以及很难解决的线程同步问题。

Issues like these are enough to scare us away from declaring a global variable,
and thus the Singleton pattern too, but that still doesn't tell us how we
*should* design the game. How do you architect a game without global state?

像这样的问题足够吓阻我们声明全局变量了，同理单例模式也是一样，但是那还没有告诉我们*应该*如何设计游戏。你怎样不使用全局变量来构建游戏？

There are some extensive answers to that question (most of this book in many
ways *is* an answer to just that), but they aren't apparent or easy to come by.
In the meantime, we have to get games out the door. The Singleton pattern looks
like a panacea. It's in a book on object-oriented design patterns, so it *must*
be architecturally sound, right? And it lets us design software the way we have
been doing for years.

有很广泛的对这个问题的回答（这本书的大部分都*是*关于这一点的），但是他们还没有回答不容易也不明显获取。与此同时，我们得发布游戏。单例模式看起来是万能药。这是在一本关于面向对象设计模式的书中，因此他*必须*建构声音，对吧？这让我们以我们做了多年的方式设计软件。

Unfortunately, it's more placebo than cure. If you scan the list of problems
that globals cause, you'll notice that the Singleton pattern doesn't solve any
of them. That's because a singleton *is* global state -- it's just encapsulated in a
class.

不幸的是，这不是药剂，这是安慰剂。如果你浏览全局变量造成的问题列表，你会注意到单例模式解决不了任何一个。这是因为单例*是*全局状态——它只是被封装在一个类中。

### It solves two problems even when you just have one

### 他能在你只有一个问题的时候解决两个

The word "and" in the Gang of Four's description of Singleton is a bit strange.
Is this pattern a solution to one problem or two? What if we have only one of
those? Ensuring a single instance is useful, but who says we want to let
*everyone* poke at it? Likewise, global access is convenient, but that's true
even for a class that allows multiple instances.

在GoF的对单例的描述中，“并”这个词有一点奇怪。这个模式是一个问题还是两个问题的解决方案？如果我们只有其中一个问题呢？保证单一的实例是有用的，但是谁告诉我们让*每个人*都能接触到它？同样，全局接触是很方便的，但是对于一个类不允许多个实例是对的么？

The latter of those two problems, convenient access, is almost always why we
turn to the Singleton pattern. Consider a logging class. Most modules in the
game can benefit from being able to log diagnostic information. However, passing
an instance of our `Log` class to every single function clutters the method
signature and distracts from the intent of the code.

两问题中的后者，方便地获取，是几乎我们为什么要使用单例模式的原因。考虑一个日志类。大部分模块都能从记录诊断性日志中获益。但是，将一个`Log`类的实例传给每一个需要这个方法的函数混杂了产生的数据，模糊了代码的意图。

The obvious fix is to make our `Log` class a singleton. Every function can then
go straight to the class itself to get an instance. But when we do that, we
inadvertently acquire a strange little restriction. All of a sudden, we can no
longer create more than one logger.

明显的解决方案是让我们的`Log`类成为单例。每一个函数可以直接接触类然后获得一个实例。但是当我们做那个的时候，我们无意地制造了一个小小的奇怪约束。突然之间，我们就不能创建超过一个日志记录者了。

At first, this isn't a problem. We're writing only a single log file, so we only
need one instance anyway. Then, deep in the development cycle, we run into
trouble. Everyone on the team has been using the logger for their own
diagnostics, and the log file has become a massive dumping ground. Programmers
have to wade through pages of text just to find the one entry they care about.

最开始的时候，这不是一个问题。我们写一个单独的日志文件，所以我们只需要一个实例。然后，在开发循环的深处，我们遇到了麻烦。每一个团队的成员都使用日志记录他的自己的诊断信息，日志文件也大量倾泻在地上。程序需要翻过很多页文字只是为了找到他们关心的一个入口。

We'd like to fix this by partitioning the logging into multiple files. To do
this, we'll have separate loggers for different game <span
name="worse">domains</span>: online, UI, audio, gameplay. But we can't. Not only
does our `Log` class no longer allow us to create multiple instances, that
design limitation is entrenched in every single call site that uses it:

我们想将日志分散道多个文件中解决这一点。为了做到这个，我们为不同的游戏领域创造分散的日志记录者：在线部分，UI，声音，游戏玩法。但是我们不能，不仅仅因为`Log`累不再允许我们创建多个实例，在每次站点使用的单一调用中有设计限制：

    Log::instance().write("Some event.");

In order to make our `Log` class support multiple instantiation (like it
originally did), we'll have to fix both the class itself and every line of code
that mentions it. Our convenient access isn't so convenient anymore.

为了让我们的`Log`类支持多个实例（就像他原来的那样），我们需要修改类本身和提及他的每一行代码。我们的方便获取就不再那么方便了。

<aside name="worse">

It could be even worse than this. Imagine your `Log` class is in a library being
shared across several *games*. Now, to change the design, you'll have to
coordinate the change across several groups of people, most of whom have neither
the time nor the motivation to fix it.

</aside>

### Lazy initialization takes control away from you

### 惰性初始化从你那里剥夺队列控制权

In the desktop PC world of virtual memory and soft performance requirements,
lazy initialization is a smart trick. Games are a different animal. Initializing
a system can take time: allocating memory, loading resources, etc. If
initializing the audio system takes a few hundred milliseconds, we need to
control when that's going to happen. If we let it lazy-initialize itself the
first time a sound plays, that could be in the middle of an action-packed part
of the game, causing visibly dropped frames and stuttering gameplay.

拥有虚拟内存和软性的性能需求的桌面电脑世界中，惰性初始化是一个小技巧。游戏是另一个生物。初始化系统需要消耗时间：分配内存，加载资源，等等。如果初始化音频系统消耗了几百个毫秒，我们需要控制发生的东西。如果我们在第一次声音播放时惰性初始化它自己，这可能是在动作部分的中间，但是可见的掉帧和结巴的游戏体验。

Likewise, games generally need to closely control how memory is laid out in the
heap to avoid <span name="fragment">fragmentation</span>. If our audio
system allocates a chunk of heap when it initializes, we want to know *when*
that initialization is going to happen, so that we can control *where* in the
heap that memory will live.

同样，游戏通常需要严格控制在堆上分配的内存来避免碎片。如果我买单音频系统在初始化时分配到了堆上的一，我们需要知道“何时”初始化发生了，所以我们可以控制内存会待在堆的“哪里”。

<aside name="fragment">

See <a class="pattern" href="object-pool.html">Object Pool</a> for a detailed
explanation of memory fragmentation.

</aside>

Because of these two problems, most games I've seen don't rely on lazy
initialization. Instead, they implement the Singleton pattern like this:

因为这两个原因，我见到的大多数游戏都不依赖惰性初始化。相反，他们像这样实现单例模式：

^code 5

That solves the lazy initialization problem, but at the expense of discarding
several singleton features that *do* make it better than a raw global variable.
With a static instance, we can no longer use polymorphism, and the class must be
constructible at static initialization time. Nor can we free the memory that the
instance is using when not needed.

者解决了惰性初始化问题，但是损失了几个单例比原生的全局变量优良的特性。通过一个静态实例，我们不能使用多态了，类也必须是在静态初始化时间就可以构建的。我们也不能在不需要的时候释放实例使用的的内存。

Instead of creating a singleton, what we really have here is a simple static
class. That isn't necessarily a bad thing, but if a static class is all you
need, <span name="static">why not</span> get rid of the `instance()` method
entirely and use static functions instead? Calling `Foo::bar()` is simpler than
`Foo::instance().bar()`, and also makes it clear that you really are dealing
with static memory.

不是创建一个单例，我们这里真正有的是一个简单的静态类。这不是一个坏事情，但是乳沟一个静态类是你需啊哟的，为什么不完全摆脱`instance()`方法直接使用静态函数呢？调用`Foo::bar()`比`Foo::instance().bar()`更简单，也更明确表明你在处理静态内存。

<aside name="static">

The usual argument for choosing singletons over static classes is that if you
decide to change the static class into a non-static one later, you'll need to
fix every call site. In theory, you don't have to do that with singletons
because you could be passing the instance around and calling it like a normal
instance method.

In practice, I've never seen it work that way. Everyone just does
`Foo::instance().bar()` in one line. If we changed Foo to not be a
singleton, we'd still have to touch every call site. Given that, I'd rather have
a simpler class and a simpler syntax to call into it.

</aside>

## What We Can Do Instead

## 我们可以做什么

If I've accomplished my goal so far, you'll think twice before you pull
Singleton out of your toolbox the next time you have a problem. But you still
have a problem that needs solving. What tool *should* you pull out? Depending on
what you're trying to do, I have a few options for you to consider, but first...

如果我现在完成队列我的目标，你就会在下一个遇到问题使用单例模式之前三思而后行。但是你还是有问题需要解决。什么工具你*应该*使用呢？取决于你在试图做什么，我有你可以考虑的一些选项，但是首先……

### See if you need the class at all

### 看看你是不是真正的需要类

Many of the singleton classes I see in games are "managers" -- those nebulous
classes that exist just to babysit other objects. I've seen codebases where it
seems like *every* class has a manager: Monster, MonsterManager, Particle,
ParticleManager, Sound, SoundManager, ManagerManager. Sometimes, for variety,
they'll throw a "System" or "Engine" in there, but it's still the same idea.

我在游戏中看到的很多单例类都是“管理器”——那些模糊的类存在的意义就是照顾其他对象。我看到代码库中好像*所有*类都有一个管理器：怪物，怪物管理器，粒子，粒子管理器，声音，声音管理器，管理管理器的管理器。优势，更多情况下，他们会在那里放一个“系统”或“引擎”但是还是一样的思路。

While caretaker classes are sometimes useful, often they just reflect
unfamiliarity with OOP. Consider these two contrived classes:

当小心处理类有时是有用的，通常他们只是反映对OOP的不熟悉。思考这两个人为的类：

^code 8

Maybe this example is a bit dumb, but I've seen plenty of code that reveals a
design just like this after you scrape away the crusty details. If you look at
this code, it's natural to think that `BulletManager` should be a singleton. After
all, anything that has a `Bullet` will need the manager too, and how many
instances of `BulletManager` do you need?

也许这个例子有些蠢，但是我看到了很多代码在扫去那些细节后都是一样的设计。乳沟你看看这个代码，`BulletManager`很自然应该是一个单例。无论如何，任何有`Bullet`的都需要管理，而你需要多少`BulletManager`实例呢？

The answer here is *zero*, actually. Here's how we solve the "singleton" problem
for our manager class:

这里的回答是*零*，事实上。这是我们如何为我们的管理类解决你的“单例”问题：

^code 9

There we go. No manager, no problem. Poorly designed singletons are often
"helpers" that add functionality to another class. If you can, just move all of
that behavior into the class it helps. After all, OOP is about letting objects
take care of themselves.

好了。没有管理员，没问题。糟糕设计的单例通常“帮助”另一个类增加代码。如果你可以的话，把所有的行为都移到类中。无论如何，OOP是关于然对象照顾好他自己。

Outside of managers, though, there are other problems where we'd reach to
Singleton for a solution. For each of those problems, there are some alternative
solutions to consider.

但是在管理器之外，还有其他问题我们需要寻求单例解决。对于每一种问题，都有一些后续方案来考虑。

### To limit a class to a single instance

### 将一个类限制到一个单一的实例

This is one half of what the Singleton pattern gives you. As in our file system
example, it can be critical to ensure there's only a single instance of a class.
However, that doesn't necessarily mean we also want to provide *public*,
*global* access to that instance. We may want to restrict access to certain
areas of the code or even make it <span name="wrapper">private</span> to a
single class. In those cases, providing a public global point of access weakens
the architecture.

这是单例模式给你的一半东西。就像在我们文件系统的例子中那样，保证一个类只有一个实例是很重要的。但是，这不意味着我们需要*公开*提供，对那个实例的*全局*访问。我们想要减少对代码某部分的接触设置让他对一个类是私有的。在这些情况下，提供一个全局接触点消弱了架构。

<aside name="wrapper">

For example, we may be wrapping our file system wrapper inside *another* layer
of abstraction.

</aside>

We want a way to ensure single instantiation *without* providing global access.
There are a couple of ways to accomplish this. Here's one:

我们想要一种方式保证一个实例而*无需*提供全局接触。这里有好几种方法完成。这是其中之一：

<span name="assert"></span>

^code 6

This class allows anyone to construct it, but it will assert and fail if you try to
construct more than one instance. As long as the right code creates the instance
first, then we've ensured no other code can either get at that instance or
create their own. The class ensures the single instantiation requirement it
cares about, but it doesn't dictate how the class should be used.

这个类运行任何人构建它，乳沟你试图构建多余一个实例，它会断言并失败。只要争取的代码首先创建了实例，那么我们就得保证没有其他代码可以接触实例或者创建他们之间的。这个类保证了他关注的单一实例需求，但是没有指定类该如何被使用。

<aside name="assert">

An *assertion* function is a way of embedding a contract into your code.
When `assert()` is called, it evaluates the expression passed to it. If it
evaluates to `true`, then it does nothing and lets the game continue. If it
evaluates to `false`, it immediately halts the game at that point. In a
debug build, it will usually bring up the debugger or at least print out the
file and line number where the assertion failed.

An `assert()` means, "I assert that this should always be true. If it's not,
that's a bug and I want to stop *now* so you can fix it." This lets you
define contracts between regions of code. If a function asserts that one of
its arguments is not `NULL`, that says, "The contract between me and the
caller is that I will not be passed `NULL`."

Assertions help us track down bugs as soon as the game does something
unexpected, not later when that error finally manifests as something
visibly wrong to the user. They are fences in your codebase, corralling bugs
so that they can't escape from the code that created them.

</aside>

The downside with this implementation is that the check to prevent multiple
instantiation is only done at *runtime*. The Singleton pattern, in contrast,
guarantees a single instance at compile time by the very nature of the class's
structure.

这个事项的缺点是检查并防止多个实例化只在*运行时*进行。单例模式，相反，抱着在编译时就确定单一实例是类的自然结构。

### To provide convenient access to an instance

### 为了给一个实例提供方便的获取方法

Convenient access is the main reason we reach for singletons. They make it easy
to get our hands on an object we need to use in a lot of different places. That
ease comes at a cost, though -- it becomes equally easy to get our hands on the
object in places where we *don't* want it being used.

方便的获取是我们使用单例的一个主要原因。这让我们在很多不同的地方获取一个我们需要的对象更加容易。这种轻松是需要代价的——它很容易个让我们在我们*不想*要对象使用的地方被使用。

The general rule is that we want variables to be as narrowly scoped as possible
while still getting the job done. The smaller the scope an object has, the fewer
places we need to keep in our head while we're working with it. Before we take
the shotgun approach of a singleton object with *global* scope, let's consider
other ways our codebase can get access to an object:

通用原则是我们想让变量子啊能完成工作的情况写尽可能局部。对象影响的范围越小，我们在处理它的时候就越少需要把东西放在脑子里。在我们拿起有*全局*范围影响的单例对象猎枪时，让我们考虑其他我们代码库能够获取对象的方式：

 *  **Pass it in.** The <span name="di">simplest</span> solution, and often the
    best, is to simply pass the object you need as an argument to the functions
    that need it. It's worth considering before we discard it as too cumbersome.

 *  **传进来。**最简单的解决办法，通常也是最好的，简单的把你需要的对象作为参数传给需要它的函数。在我们用其他更加繁杂的方法前考虑这个解决方案。

    <aside name="di">

    Some use the term "dependency injection" to refer to this. Instead of code
    reaching *out* and finding its dependencies by calling into something
    global, the dependencies are pushed *in* to the code that needs it through
    parameters. Others reserve "dependency injection" for more complex ways of
    providing dependencies to code.

    </aside>

    Consider a function for rendering objects. In order to render, it needs
    access to an object that represents the graphics device and maintains the
    render state. It's very common to simply pass that in to all of the rendering
    functions, usually as a parameter named something like `context`.

    考虑一个渲染对象的函数。为了渲染，他需要接触一个代表图形设备的对象，管理渲染状态。将其传给所有渲染函数通常是很自然的，通常是一个参数名字像是`context`之类的。

    On the other hand, some objects don't belong in the signature of a method.
    For example, a function that handles AI may need to also write to a <span
    name="aop">log file</span>, but logging isn't its core concern. It would be
    strange to see `Log` show up in its argument list, so for cases like that
    we'll want to consider other options.

    另一方面，游戏对象不属于方法的署名。举个例子，一个出来AI的函数可能也需要写日志文件，但是日志不是他的核心关注点。看到`Log`出现在它的参数列表中是很奇怪的事情，由于像这样的情况我们需要考虑其他的选项。

    <aside name="aop">

    The term for things like logging that appear scattered throughout a codebase
    is "cross-cutting concern". Handling cross-cutting concerns gracefully is
    a continuing architectural challenge, especially in statically typed
    languages.

    [Aspect-oriented
    programming](http://en.wikipedia.org/wiki/Aspect-oriented_programming) was
    designed to address these concerns.

    </aside>

 *  **Get it from the base class.** Many game architectures have shallow but
    wide inheritance hierarchies, often only one level deep. For example, you
    may have a base `GameObject` class with derived classes for each enemy or
    object in the game. With architectures like this, a large portion of the
    game code will live in these "leaf" derived classes. This means that all
    these classes already have access to the same thing: their `GameObject` base
    class. We can use that to our advantage:

    **从基类中取出。**很逗游戏架构有浅层但是广泛的继承层次，通常只有一层深。举个例子，你也许有一个基础的`GameObject`类拥有每一个游戏中的敌人或者对象的推到类。通过使用这样的架构，很大一部分游戏代码会存在这些“子”推导类中。这就意味着其他这些类已经有啦对同样事物的相同获取方法：他们的`GameObject`基类。我们可以将其作为优势：

    <span name="gameobject"></span>

    ^code 10

    This ensures nothing outside of `GameObject` has access to its `Log` object,
    but every derived entity does using `getLog()`. This pattern of letting
    derived objects implement themselves in terms of protected methods provided
    to them is covered in the <a class="pattern"
    href="subclass-sandbox.html">Subclass Sandbox</a> chapter.

    这没有保证任何`GameObject`外部的东西可以获得他的`Log`对象，但是每一个推导的实体确实使用`getLog()`。这种通过提供给他们的保护方法让推导对象实现他们的模式在子类沙箱这章中被覆盖到了。

    <aside name="gameobject">

    This raises the question, "how does `GameObject` get the `Log` instance?" A
    simple solution is to have the base class simply create and own a static
    instance.

    If you don't want the base class to take such an active role, you can
    provide an initialization function to pass it in or use the <a
    class="pattern" href="service-locator.html">Service Locator</a> pattern to
    find it.

    </aside>

 *  **Get it from something already global.** The goal of removing *all* global
    state is admirable, but rarely practical. Most codebases will still have a
    couple of globally available objects, such as a single `Game` or `World`
    object representing the entire game state.

 *  **从已经是全局的东西中获取。**移除*所有*全局状态的目标令人钦佩，但是并不实际。大多数代码库还是会有一些可用全局对象，比如一个`Game`或`World`对象代表了整个游戏的状态。

    We can reduce the number of global classes by piggybacking on existing ones
    like that. Instead of making singletons out of `Log`, `FileSystem`, and
    `AudioPlayer`, do this:

    我们可以通过让现有的对象捎带来减少全局变量类的数目。不是让`Log`，`FileSystem`和`AudioPlayer`变成单例，像这样做：

    ^code 11

    With this, only `Game` is globally available. Functions can get to the
    other systems <span name="demeter">through</span> it:

    通过这样，只有`Game`是全局可用的。需要到其他系统的函数得穿过它：

    ^code 12

    <aside name="demeter">

    Purists will claim this violates the Law of Demeter. I claim that's still
    better than a giant pile of singletons.

    </aside>

    If, later, the architecture is changed to support multiple `Game` instances
    (perhaps for streaming or testing purposes), `Log`, `FileSystem`, and
    `AudioPlayer` are all unaffected -- they won't even know the difference. The
    downside with this, of course, is that more code ends up coupled to `Game`
    itself. If a class just needs to play sound, our example still requires it
    to know about the world in order to get to the audio player.

    如果，稍后，架构被改为支持多个`Game`实例（可能是为了流或者测试目的），`Log`，`FileSystem`，和`AudioPlayer`不会被影响到——他们甚至不知道有什么区别。这一点的缺陷是，单人，更多的代码耦合到了`Game`中。乳沟一个类只是需要播放声音，我们的例子仍然需要它知道游戏世界，只为了获得音频播放器。

    We solve this with a hybrid solution. Code that already knows about `Game`
    can simply access `AudioPlayer` directly from it. For code that doesn't, we
    provide access to `AudioPlayer` using one of the other options described
    here.

    我们通过混合方案解决这一点。知道`Game`的代码可以直接从那里获得`AudioPlayer`，对于不知道的代码，我们用上面描述的其他选项来提供`AudioPlayer`。

 *  **Get it from a Service Locator.** So far, we're assuming the global class
    is some regular concrete class like `Game`. Another option is to define a
    class whose sole reason for being is to give global access to objects. This
    common pattern is called a <a class="pattern"
    href="service-locator.html">Service Locator</a> and gets its own chapter.

 *  **从服务定位器中获得。**目前为止，我们假设全局类是通常具体的类比如`Game`，另外一种选项是定义一个类，唯一的目标就是为对象提供全局访问。这种常见的模式被称为服务定位器模式，有它单独的章节。

## What's Left for Singleton

## 单例中还剩下什么

The question remains, where *should* we use the real Singleton pattern?
Honestly, I've never used the full Gang of Four implementation in a game. To
ensure single instantiation, I usually simply use a static class. If that
doesn't work, I'll use a static flag to check at runtime that only one instance
of the class is constructed.

剩下的问题，何处我们*应该*使用真实的单例模式？说实话，我从来没有在游戏中完全使用全部的GoF。wield保证单一实例化，我通常简单的使用一个静态类。如果这没有效果，我使用一个静态标识在运行时检测是不是只有一个实例被创建了。

There are a couple of other chapters in this book that can also help here. The
<a class="pattern" href="subclass-sandbox.html">Subclass Sandbox</a> pattern
gives instances of a class access to some shared state without making it
globally available. The <a class="pattern" href="service-locator.html">Service
Locator</a> pattern *does* make an object globally available, but it gives you more
flexibility with how that object is configured.

这本书还有几个章节也可以帮忙。子类沙箱模式通过一些分享状态来给实例一个类的访问权限而无需让其全局可用。服务定位器模式*确实*让一个对象全局可用队列，但它给了你更大的对象如何设置的自由度。
