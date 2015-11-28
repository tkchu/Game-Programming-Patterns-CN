^title Subclass Sandbox
^section Behavioral Patterns

## Intent

## 意图

*Define behavior in a subclass using a set of operations provided by its
base class.*

*用一系列由基类定义的操作定义子类中的行为。*

## Motivation

## 动机

Every kid has dreamed of being a superhero, but unfortunately, cosmic rays are
in short supply here on Earth. Games that let you pretend to be a superhero are
the closest approximation. Because our game designers have never learned to say,
"no", *our* superhero game aims to feature dozens, if not hundreds, of
different superpowers that heroes may choose from.

每个孩子都梦想过变成超级英雄，但是不幸的是，高能射线在地球上很短缺。游戏是让你假装是超级英雄的最简单的方法。因为我们的游戏设计者从来没有学会说“不”，*我们的*超级英雄游戏面向成打，如果不是成百的，不同的超级能力可以选择。

Our plan is that we'll have a `Superpower` base class. Then, we'll have a <span
name="lots">derived</span> class that implements each superpower. We'll divvy up
the design doc among our team of programmers and get coding. When we're done,
we'll have a hundred superpower classes.

我们的集合是有一个`Superpower`基类。然后我们有它推导出各种超级能力的实现。我们在我们的程序员队伍中分发设计文档，然后开始变成，当我们完成后，我们就会有上百的超级能力类。

<aside name="lots">

When you find yourself with a *lot* of subclasses, like in this example, that
often means a data-driven approach is better. Instead of lots of *code* for
defining different powers, try finding a way to define that behavior in *data*
instead.

Patterns like <a class="pattern" href="type-object.html">Type Object</a>, <a
class="pattern" href="bytecode.html">Bytecode</a>, and <a class="gof-pattern"
href="http://en.wikipedia.org/wiki/Interpreter_pattern">Interpreter</a> can all
help.

</aside>

We want to immerse our players in a world teeming with variety. Whatever power
they dreamed up when they were a kid, we want in our game. That means these
superpower subclasses will be able to do just about everything: play sounds,
spawn visual effects, interact with AI, create and destroy other game entities,
and mess with physics. There's no corner of the codebase that they won't touch.

我们想让玩家处于充满变化的游戏世界中。无论他们在孩子时想象过什么能力，我们都要在游戏中表现。这就意味着这些超级能力子类需要做任何事情：播放声音，产生视觉此熬过，与AI交互，创建和销毁其他游戏实体，与物理打交道。没有哪处嗲吗是他们不能碰到的。

Let's say we unleash our team and get them writing superpower classes. What's
going to happen?

假设我们让团队信马由缰的写超级能力类。会发生什么？

 *  *There will be lots of redundant code.* While the different powers will be
    wildly varied, we can still expect plenty of overlap. Many of them will
    spawn visual effects and play sounds in the same way. A freeze ray, heat
    ray, and Dijon mustard ray are all pretty similar when you get down to it.
    If the people implementing those don't coordinate, there's going to be a lot
    of duplicate code and effort.

 *  *会有很多冗余代码。*当有非常不同的各种能力，我们可以预期有很多重叠。他们很多都会用相同的方式发出视觉效果并播放声音。当你坐下来看看，冷冻光线，加热光线，芥末酱光线都很相似。如果人们实现这些的时候没有协同，那就会有很多冗余的代码和付出。

 *  *Every part of the game engine will get coupled to these classes.* Without
    knowing better, people will write code that calls into subsystems that were
    never meant to be tied directly to the superpower classes. If our renderer
    is organized into several nice neat layers, only one of which is intended to
    be used by code outside of the graphics engine, we can bet that we'll end up
    with superpower code that pokes into every one of them.

 *  *游戏引擎中的每一部分都会与这些类耦合。*没有其他信息的话，任何会写直接调用子系统的代码，它们从来没打算直接与超级能力类绑定。如果我们的渲染系统被好好组织成多个层次，只有其中之一能被外部的图形引擎使用，我们可以大度我的超级能力代码与它们中的每一个回到它们中的每一个。

 *  *When these outside systems need to change, odds are good some random
    superpower code will get broken.* Once we have different superpower classes
    coupling themselves to various and sundry parts of the game engine, it's
    inevitable that changes to those systems will impact the power classes.
    That's no fun because your graphics, audio, and UI programmers probably
    don't want to also have to be gameplay programmers *too*.

 *  *当外部代码需要改变是，有很大几率一些随机超能力代码会损坏。*一旦我们有了不同的超能力类绑定到多种杂乱的游戏引擎部分，改变那些期限必然影响能力类。这没什么意思，因为你的图形，音频，UI程序员很可能不想*也*成为玩法程序员。

 *  *It's hard to define invariants that all superpowers obey.* Let's say we
    want to make sure that all audio played by our powers gets properly queued
    and prioritized. There's no easy way to do that if our hundred classes are
    all directly calling into the sound engine on their own.

 *  *很难定义所有超能力遵守的不变量。*假设我们想保证我们能力播放的所有音频都正确的排序和优先化。如果我们几百个类都直接自己调用音频引擎就没什么好办法来完成那一点。

What we want is to give each of the gameplay programmers who is implementing a
superpower a set of primitives they can play with. You want your power to play a
sound? Here's your `playSound()` function. You want particles? Here's
`spawnParticles()`. We'll make sure these operations cover everything you need
to do so that you don't need to `#include` random headers and nose your way into
the rest of the codebase.

我们想要的是给么一个实现超能力的玩法程序员一系列他们可以使用的基本单元。你想要播放声音？这是你的`playSound()`函数。你想要粒子？这是`spawnParticles()`。我们保证了这些操作覆盖了你需要做的事情，所以你不需要`#include`随机的头文件，干扰到代码库的其他部分。

We do this by making these operations *protected methods of the* `Superpower`
*base class*. Putting them in the base class gives every power subclass direct,
easy access to the methods. Making them protected (and likely non-virtual) communicates
that they exist specifically to be *called* by subclasses.

我们通过定义这些操作为`Superpower`*基类*的*保护方法*。将它们放在基类给了每个能力子类直接，方便的途径获取方法。让它们保护（有些像非虚）暗示了他们存在就是为了被子类*调用*。

Once we have these toys to play with, we need a place to use them. For that,
we'll define a *sandbox method*, an abstract protected method that subclasses
must implement. Given those, to implement a new kind of power, you:

一旦我们有了这些东西来玩耍，我们需要一个地方使用他们。为了那个，我们定义一个*沙箱方法*，一个子类必须实现的抽象的保护方法。获得了那些，要实现一种新的能力，你需要：

1.  Create a new class that inherits from `Superpower`.

2.  Override `activate()`, the sandbox method.

3.  Implement the body of that by calling the protected methods that
    `Superpower` provides.

1. 创建一个从`Superpower`继承的新类。

2. 重载`activate()`，沙箱方法。

3. 通过调用`Superpower`提供的保护方法实现主体。

We can fix our redundant code problem now by making those provided operations as
high-level as possible. When we see code that's duplicated between lots of the
subclasses, we can always roll it up into `Superpower` as a new operation that
they can all use.

我们现在可以使用这些尽可能高层次的操作来解决我们的冗余代码问题了。当我们看到代码在多个子类间重复，我们总可以将其打包到`Superpower`中，作为他们都可以使用的新操作。

We've addressed our coupling problem by constraining the coupling to one place.
`Superpower` itself will end up coupled to the different game systems, but our
hundred derived classes will not. Instead, they are *only* coupled to their base
class. When one of those game systems changes, modification to `Superpower` may
be necessary, but dozens of subclasses shouldn't have to be touched.

我们通过将耦合约束到一个地方解决了我们的耦合问题。`Superpower`最终与不同的系统耦合，但是我们继承的几百个类不会。相反，他们*唯一*耦合他们的基类。当其中一个游戏系统改变时，修改`Superpower`也许是必须的，但是成打的子类不需要修改。

This pattern leads to an architecture where you have a shallow but wide class
hierarchy. Your <span name="wide">inheritance</span> chains aren't *deep*, but
there are a *lot* of classes that hang off `Superpower`. By having a single
class with a lot of direct subclasses, we have a point of leverage in our
codebase. Time and love that we put into `Superpower` can benefit a wide set of
classes in the game.

这个模式指向浅层但是广泛的类层次。你的继承链不*深*，但是有*很多*类挂在`Superpower`上。通过有很多直接子类的基类，我们在代码库中创造了一个支撑点。我们投入到`Superpower`的时间和爱可以让游戏中广泛的类收益。

<aside name="wide">

Lately, you find a lot of people criticizing inheritance in object-oriented
languages. Inheritance *is* problematic -- there's really no deeper coupling in
a codebase than the one between a base class and its subclass -- but I find
*wide* inheritance trees to be easier to work with than *deep* ones.

</aside>

## The Pattern

## 模式

A **base class** defines an abstract **sandbox method** and several **provided
operations**. Marking them protected makes it clear that they are for use by
derived classes. Each derived **sandboxed subclass** implements the sandbox
method using the provided operations.

一个**基类**顶一个一个抽象的**沙箱方法**和几个**提供的操作**。让他们处于保护表面他们只为继承类所使用。每一个推导出的**沙箱子类**用提供的操作实现了沙箱方法。

## When to Use It

## 何时使用

The Subclass Sandbox pattern is a very simple, common pattern lurking in lots of codebases, even outside
of games. If you have a non-virtual protected method laying around, you're
probably already using something like this. Subclass Sandbox is a good fit
when:

子类沙箱模式是潜伏在代码库中简单常用的模式，哪怕是在游戏之外的地方。乳沟你有一个非虚方法的保护方法，你可能已经在用某些类似的东西了。沙箱方法在以下情况适用：

 *  You have a base class with a number of derived classes.

 *  你有一个能推导很多子类的基类。

 *  The base class is able to provide all of the operations that a derived class
    may need to perform.

 *  基类可以提供推导类需要的所有操作。

 *  There is behavioral overlap in the subclasses and you want to make it easier
    to share code between them.

 *  在在子类总有行为重合，你想要更容易的在他们间分享代码。

 *  You want to minimize coupling between those derived classes and the rest of
    the program.

 *  你想要最小化推导类和程序的其他部分的耦合。

## Keep in Mind

## 记住

"Inheritance" is a bad word in many programming circles these days, and one
reason is that base classes tend to accrete more and more code. This pattern is
particularly susceptible to that.

“继承”现在在很多编程圈子是一个坏词，一个原因是基类趋向于增加越来越多的代码。这个模式特别容易被感染。

Since subclasses go through their base class to reach the rest of the game, the
base class ends up coupled to every system *any* derived class needs to talk to.
Of course, the subclasses are also intimately tied to their base class. That
spiderweb of coupling makes it very hard to change the base class without
breaking something -- you've got the [brittle base class problem][].

由于子类通过他们的基类接触游戏的剩余部分，基类最后和*每个*推导类需要的所有系统耦合。当然，子类也紧密的与基类相绑定。这种蛛网耦合让你很难在不破坏什么的情况下改变基类——你获得了易碎基类问题。

The flip side of the coin is that since most of your coupling has been pushed up
to the base class, the derived classes are now much more cleanly separated from
the rest of the world. Ideally, most of your behavior will be in those
subclasses. That means much of your codebase is isolated and easier to maintain.

硬币的另一面是由于你耦合的大部分都被推到了基类，推导类现在与世界的其他部分分离。理想的，你大多数的行为都在哪些子类中。这意味着你的代码库大部分是孤立的，很容易管理。

Still, if you find this pattern is turning your base class into a giant bowl of
code stew, consider pulling some of the provided operations out into separate
classes that the base class can dole out responsibility to. The <a
class="pattern" href="component.html">Component</a> pattern can help here.

如果你发现这个模式正把你的基类变成一锅代码糊糊，考虑将它提供的一些操作提出放入分离的类中，这样基类可以分散它的责任。组件模式可以帮得到忙。

[brittle base class problem]: http://en.wikipedia.org/wiki/Fragile_base_class

## Sample Code

## 示例代码

Because this is such a simple pattern, there isn't much to the sample code. That
doesn't mean it isn't useful -- the pattern is about the *intent*, not the
complexity of its implementation.

因为则是一个如此简单的模式，示例代码中没有太多东西。这不是说它没有用——这个模式是关于“意图”，而不是它实现复杂度。

We'll start with our `Superpower` base class:

我们从`Superpower`基类开始：

^code 1

The `activate()` method is the sandbox method. Since it is virtual and abstract,
subclasses *must* override it. This makes it clear to someone creating a power
subclass where their work has to go.

`activate()`方法是沙箱方法。由于它是虚和抽象的，子类*必须*重载它。这让那些需要创建子类的人知道他们需要在哪里做工作。

The other protected methods, `move()`, `playSound()`, and `spawnParticles()`, are
the provided operations. These are what the subclasses will call in their
implementation of `activate()`.

其他的保护方法，`move()`，`playSound()`，和`spawnParticles()`都是提供的操作。它们是子类在实现`activate()`要调用的。

We didn't implement the provided operations in this example, but an actual game
would have real code there. Those methods are where `Superpower` gets coupled to
other systems in the game -- `move()` may call into physics code, `playSound()`
will talk to the audio engine, etc. Since this is all in the *implementation* of
the base class, it keeps that coupling encapsulated within `Superpower` itself.

在这个例子中，我们没有实现提供的操作，但一个真正的游戏会在那里有真正的代码。那些代码是`Superpower`与游戏中其他部分的耦合——`move()`也许调用物理代码，`playSound()`会与音频引擎交互，等等。由于这都在基类的*实现*中，保证了耦合封闭在`Superpower`中。

OK, now let's get our radioactive spiders out and create a power. Here's one:

好了，让我们的放射蜘蛛侠处理创建一个能力。这里是一个：

<span name="jump"></span>

^code 2

<aside name="jump">

OK, maybe being able to *jump* isn't all that *super*, but I'm trying to keep
things basic here.

</aside>

This power springs the superhero into the air, playing an appropriate sound and
kicking up a little cloud of dust. If all of the superpowers were this simple --
just a combination of sound, particle effect, and motion -- then we wouldn't
need this pattern at all. Instead, `Superpower` could have a baked-in
implementation of `activate()` that accesses fields for the sound ID, particle
type, and movement. But that only works when every power essentially works the
same way with only some differences in data. Let's elaborate on it a bit:

这种能力将超级英雄弹射到天空，播放一阵适合的声音，踢起一堆尘土。如果所有的超能力都这样简单——声音，粒子效果，动作的组合——那么我就根本不想要这个模式了。相反，`Superpower`有内置的`activate()`获得了声音ID，粒子类型和运动的字段。但是那样只在所有能力都以相同方式工作，只有一些数据上的不同的时候可行。让我们精细一些：

^code 3

Here, we've added a couple of methods to get the hero's position. Our
`SkyLaunch` subclass can now use those:

这里我们增加了些方法获取英雄的位置。我们的`SkyLaunch`现在可以使用那些了：

^code 4

Since we have access to some state, now our sandbox method can do actual,
interesting control flow. Here, it's still just a couple of simple `if`
statements, but you can do <span name="data">anything</span> you want. By having
the sandbox method be an actual full-fledged method that contains arbitrary
code, the sky's the limit.

由于我们接触了些状态，现在我们的沙箱方法可以做实际有趣的控制流了。这，还只有几个简单的`if`声明，但你可以做任何你想的东西。使用完全成熟的沙箱方法，天高任鸟飞了。

<aside name="data">

Earlier, I suggested a data-driven approach for powers. This is one reason why
you may decide *not* to do that. If your behavior is complex and imperative,
it is more difficult to define in data.

</aside>

## Design Decisions

## 设计决策

As you can see, Subclass Sandbox is a fairly "soft" pattern. It describes a basic idea, but
it doesn't have a lot of detailed mechanics. That means you'll be making some
interesting choices each time you apply it. Here are some questions to consider.

如你所见，子类沙箱是一个“软”模式。它报数了一个基本思路，但是没有很多细节机制。这意味着每次使用可以做出有趣的选择。这里是一些需要思考的问题。

### What operations should be provided?

### 应该提供什么的操作？

This is the biggest question. It deeply affects how this pattern feels and how
well it works. At the minimal end of the spectrum, the base class doesn't
provide *any* operations. It just has a sandbox method. To implement it, you'll
have to call into systems outside of the base class. If you take that angle,
it's probably not even fair to say you're using this pattern.

这是最大的问题。这深深影响了这个模式感觉上证明样和它能工作的有多好。，在一种极端中，基类几乎不提供*任何*操作。他只有一个沙箱方法。为了实现之，你需要调用基类外部的系统。如果你从那个角度看，很难说你在使用这个模式。

On the other end of the spectrum, the base class provides <span
name="include">*every*</span> operation that a subclass may need. Subclasses are
*only* coupled to the base class and don't call into any outside systems
whatsoever.

另一个极端，基类提供了*所有*子类也许需要的操作。子类*只*与基类耦合，不调用任何外部系统的东西。

<aside name="include">

Concretely, this means each source file for a subclass would only need a single
`#include` -- the one for its base class.

</aside>

Between these two points, there's a wide middle ground where some operations are
provided by the base class and others are accessed directly from the outside
system that defines it. The more operations you provide, the less coupled
subclasses are to outside systems, but the *more* coupled the base class is. It
removes coupling from the derived classes, but it does so by pushing that up to the
base class itself.

在这两点之间，操作由基类提供还是直接调用外部有很大的操作余地。你提供的操作越多，子类与外部系统耦合越少，但是与基类耦合*更多*。它从推导类中移除了耦合，但它是通过将其推给基类完成的。

That's a win if you have a bunch of derived classes that were all coupled to
some outside system. By moving the coupling up into a provided operation, you've
centralized it into one place: the base class. But the more you do this, the
bigger and harder to maintain that one class becomes.

如果你有一堆推导类与外部系统耦合的话，这是一个胜利。通过将耦合移到一个提供的操作，你将其移动到了一个地方：基类。但是你越这么做，那个类就越大越难管理。

So where should you draw the line? Here are a few rules of thumb:

所以分界线在哪里？这里是一些首要原则：

 *  If a provided operation is only used by one or a few subclasses, you don't
    get a lot of bang for your buck. You're adding complexity to the base class,
    which affects everyone, but only a couple of classes benefit.

 *  如果一个提供的操作只被一个或几个子类使用，你就获益不了太多。你向基类添加了复杂性，会影响所有事物，但是只有几个类收益。

    This may be worth it to make the operation consistent with other
    provided operations, or it may be simpler and cleaner to let those
    special case subclasses call out to the external systems directly.

    也许让操作与其他提供的操作一致更有价值，或者让这些特殊情况子类直接调用外部系统更加简单明了。

 *  When you call a method in some other corner of the game, it's less intrusive
    if that method doesn't modify any state. It still creates a coupling, but
    it's a <span name="safe">"safe"</span> coupling because it can't break
    anything in the game.

 *  当你调用游戏中其他交流的一个方法，如果方法没有修改任何状态就有更少的干扰。它仍然制造耦合，但是这是“安全”耦合，因为它没有打破游戏中的任何东西。

    <aside name="safe">

    "Safe" is in quotes here because technically, even just accessing data can
    cause problems. If your game is multi-threaded, you could read something at
    the same time that it's being modified. If you aren't careful, you could end up
    with bogus data.

    Another nasty case is if your game state is strictly deterministic (which
    many online games are in order to keep players in sync). If you access
    something outside of the set of synchronized game state, you can cause
    incredibly painful non-determinism bugs.

    </aside>

    Calls that do modify state, on the other hand, more deeply tie you to those
    parts of the codebase, and you need to be much more cognizant of that. That
    makes them good candidates for being rolled up into provided operations in
    the more visible base class.

    调用修改状态的，另一方面，将你和代码库的其他方面紧密绑定，你需要更多的审视。打包他们进一个显式基类提供的操作是一个好的候选项。

 *  If the implementation of a provided operation only forwards a call to some
    outside system, then it isn't adding much value. In that case, it may be
    simpler to call the outside method directly.

 *  如果提供操作的实现之是增加了向外部系统转发调用，那它就没增加什么价值。那种情况下，也许直接调用外部方法更简单。

    However, even simple forwarding can still be useful -- those methods often
    access state that the base class doesn't want to directly expose to
    subclasses. For example, let's say `Superpower` provided this:

    但是，哪怕简单的转发也是有用的——那些接触基类不想直接暴露被子类的状态的方法。举个例子，假设`Superpower`提供这个：

    ^code 5

    It's just forwarding the call to some `soundEngine_` field in `Superpower`.
    The advantage, though, is that it keeps that field encapsulated in
    `Superpower` so subclasses can't poke at it.

    它只是转发`Superpower`中`soundEngine_`字段。但是，好处是褒词字段封装在`Superpower`中，子类不会接触它。

### Should methods be provided directly, or through objects that contain them?

### 方法应该直接提供，还是通过包含他们的对象？

The challenge with this pattern is that you can end up with a painfully large
number of methods crammed into your base class. You can mitigate that by moving
some of those methods over to other classes. The provided operations in the base
class then just return one of those objects.

这个模式的挑战是基类中最终闯进了很多方法。你可以将一些方法移到其他类中来缓和。基类提供的方法只是放回这些对象中的一个。

For example, to let a power play sounds, we could add these directly to
`Superpower`:

举个例子，为了让一个超能力播放声音，我们可以直接将它们加到`Superpower`中：

^code 6

But if `Superpower` is already getting large and unwieldy, we might want to
avoid that. Instead, we create a `SoundPlayer` class that exposes that
functionality:

但是如果`Superpower`已经很大很宽泛，我们也许想要避免那一点。相反，我们创建一个`SoundPlayer`类暴露那个函数：

^code 7

Then `Superpower` provides access to it:

`Superpower`提供了对其的接触：

^code 8

Shunting provided operations into auxiliary classes like this can do a few
things for you:

将提供的操作分到辅助类可以为你做一些事情：

 *  *It reduces the number of methods in the base class.* In the example here,
    we went from three methods to just a single getter.

 *  *它减少了基类的方法。*在这里的例子，将三个方法变成了一个简单的获取。

 *  *Code in the helper class is usually easier to maintain.* Core base classes
    like `Superpower`, despite our best intentions, tend to be tricky to change
    since so much depends on them. By moving functionality over to a less
    coupled secondary class, we make that code easier to poke at without
    breaking things.

 *  *在辅助类中的代码通常更好管理。*核心基类像`Superpower`，不管我们滴最好意图，由于被如此多类依赖而很难改变。通过将函数移到一个更少耦合的二阶类，我们让代码更容易接触而没有破坏任何东西。

 *  *It lowers the coupling between the base class and other systems.* When
    `playSound()` was a method directly on `Superpower`, our base
    class was directly tied to `SoundId` and whatever audio code the
    implementation called into. Moving that over to `SoundPlayer` reduces
    `Superpower`'s coupling to the single `SoundPlayer` class, which then
    encapsulates all of its other dependencies.

 *  *减少了基类和其他系统的耦合度。*当`playSound()`是`Superpower`中一个直接的方法，我们的基类与`SoundId`以及其他实现涉及的代码直接绑定。将它移动到`SoundPlayer`减少了`Superpower`与`SoundPlayer`类的耦合，这就封装了它其他的依赖。

### How does the base class get the state that it needs?

### 基类如何获得它需要的状态？

Your base class will often need some data that it wants to encapsulate and keep
hidden from its subclasses. In our first example, the `Superpower` class
provided a `spawnParticles()` method. If the implementation of that needs some
particle system object, how would it get one?

你的基类经常需要想要封装和从子类隐藏的数据。在我们的第一个例子中，`Superpower`类提过了`spawnParticles()`方法。乳沟那个的实现需要一些粒子系统对象，它怎么获得一个呢？

 *  **Pass it to the base class constructor:**

 *  **将它传给基类构造器：**

    The simplest solution is to have the base class take it as a constructor
    argument:

    最简单的解决方案是让基类将其作为构造器变量：

    ^code pass-to-ctor-base

    This safely ensures that every superpower does have a particle system by the
    time it's constructed. But let's look at a derived class:

    这安全的保证了每个超能力在构造时有一个粒子系统。但是让我们看看推导类：

    ^code pass-to-ctor-sub

    Here we see the problem. Every derived class will need to have a constructor
    that calls the base class one and passes along that argument. That exposes
    every derived class to a piece of state that we don't want them to know
    about.

    这我们看到了问题。每一个推导类都需要一个构造器调用基类传递那个变量。这项推导类暴露了我们不想要他知道的状态。

    This is also a maintenance headache. If we later add another piece of state
    to the base class, every constructor in each of our derived classes will
    have to be modified to pass it along.

    这也让维护头疼。如果我们后续向基类添加了一个状态，每一个推导类都需要修改并传递它。

 *  **Do two-stage initialization:**

 *  **使用两阶初始化：**

    To avoid passing everything through the constructor, we can split
    initialization into two steps. The constructor will take no parameters and
    just create the object. Then, we call a separate method defined directly on
    the base class to pass in the rest of the data that it needs:

    为了避免通过构造器传递所有东西，我们可以将初始化划分为两个部分。构造器不接受任何参数只是创建对象。然后，我们调用一个定义在基类的分离方法传入所有他需要的数据。

    ^code 9

    Note here that since we aren't passing anything into the constructor for
    `SkyLaunch`, it isn't coupled to anything we want to keep private in
    `Superpower`. The trouble with this approach, though, is that you have to
    make sure you always remember to call `init()`. If you ever forget, you'll
    have a power that's in some twilight half-created state and won't work.

    注意我们没有为`SkyLaunch`的构造器传入任何东西，它与`Superpower`中想要保持 私有的任何东西都不耦合。这种实现的问题，是你需要保证你用于记得调用`init()`，你会获得一个处于半完成的超能力，无法运行。

    You can fix that by encapsulating the entire process into a single function,
    like so:

    你可以将整个过程封装到一个函数中来修复这一点，就像这样：

    <span name="friend"></span>

    ^code 10

    <aside name="friend">

    With a little trickery like private constructors and friend classes, you can
    ensure this `createSkylaunch()` function is the *only* function that can
    actually create powers. That way, you can't forget any of the initialization
    stages.

    </aside>

 *  **Make the state static:**

 *  **让状态变为静态：**

    In the previous example, we were initializing each `Superpower` *instance*
    with a particle system. That makes sense when every power needs its own
    unique state. But let's say that the particle system is a <a class="pattern"
    href="singleton.html">Singleton</a>, and every power will be sharing the
    same state.

    在前一个例子中，我们用粒子系统初始化每一个`Superpower`*实例*。这在每个能力都需要它独特的状态时有意义。但是假设粒子系统是一个单例，每一个能力都会分享相同的状态。

    In that case, we can make the state private to the base class and also make
    it <span name="singleton">*static*</span>. The game will still have to make
    sure that it initializes the state, but it only has to initialize the
    `Superpower` *class* once for the entire game, and not each instance.

    在那种情况下，我们可以让状态时基类私有而*静态*的。游戏仍然要保证初始化转台，但是它只需要为整个游戏初始化`Superpower`*类*一次，而不是为每一个实例。

    <aside name="singleton">

    Keep in mind that this still has many of the problems of a singleton. You've
    got some state shared between lots and lots of objects (all of the
    `Superpower` instances). The particle system is encapsulated, so it isn't
    globally *visible*, which is good, but it can still make reasoning about
    powers harder because they can all poke at the same object.

    </aside>

    ^code 11

    Note here that `init()` and `particles_` are both static. As long as the
    game calls `Superpower::init()` once early on, every power can access the
    particle system. At the same time, `Superpower` instances can be created
    freely by calling the right derived class's constructor.

    注意这里的`init()`和`particles_`都是静态的。主要游戏早先调用过一次`Superpower::init()`，每种能力都能接触粒子系统。同时，`Superpower`实例可以通过调用推导类的构造器自由创建。

    Even better, now that `particles_` is a *static* variable, we don't have to
    store it for each instance of `Superpower`, so we've made the class use less
    memory.

    甚至更好的，现在`particles_`是一个*静态*变量，我们不需要在每一个`Superpower`中存储它，这样我们的类需要更少的内存。

 *  **Use a service locator:**

 *  **使用服务定位器：**

    The previous option requires that outside code specifically remembers to push
    in the state that the base class needs before it needs it. That places the
    burden of initialization on the surrounding code. Another option is to let
    the base class handle it by pulling in the state it needs. One way to do
    that is by using the <a class="pattern" href="service-locator.html">Service
    Locator</a> pattern:

    前一个选项需要外部代码记得在基类需要前设置状态。这将初始化的责任落在了周围的代码。另一个选项是让基类通过调出它需要的状态来实现。一种实现的方法是使用一个服务定位器模式：

    ^code 12

    Here, `spawnParticles()` needs a particle system. Instead of being *given*
    one by outside code, it fetches one itself from the service locator.

    这儿，`spawnParticles()`需要一个粒子系统，不是外部系统*给*它，它自己从服务定位器中拿了一个。

## See Also

## 参见

 *  When you apply the <a class="pattern" href="update-method.html">Update
    Method</a> pattern, your update method will often also be a sandbox method.

 *  但你使用更新模式时，你的更新模式通常也是沙箱方法。

 *  This pattern is a role reversal of the <a class="gof-pattern"
    href="http://en.wikipedia.org/wiki/Template_method_pattern">Template
    Method</a> pattern. In both patterns, you implement a method using a set of
    primitive operations. With Subclass Sandbox, the method is in the derived
    class and the primitive operations are in the base class. With Template
    Method, the *base* class has the method and the primitive operations are
    implemented by the *derived* class.

 *  这个模式时模板方法的反面角色。在两种模式中，你都使用一系列受限操作实现方法。子啊子类沙箱总，方法在推导类中，限制操作在基类中。在模板方法中，*基*类有方法，而受限操作被*推导*类实现。

 *  You can also consider this a variation on the <a class="gof-pattern"
    href="http://en.wikipedia.org/wiki/Facade_Pattern">Facade</a> pattern. That
    pattern hides a number of different systems behind a single simplified API.
    With Subclass Sandbox, the base class acts as a facade thats hides the
    entire game engine from the subclasses.

 *  你也可以考虑这是外观模式的变异。那个模式将一系列不同系统藏在一个简化的API后。使用子类沙箱，基类起到了在子类前隐藏整个游戏引擎的外观作用。
