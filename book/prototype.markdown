^title Prototype
^section Design Patterns Revisited

^title 原型
^section Design Patterns Revisited

The first time I heard the word "prototype" was in *Design Patterns*. Today, it
seems like everyone is saying it, but it turns out they aren't talking about the
<a href="http://en.wikipedia.org/wiki/Prototype_pattern"
class="gof-pattern">design pattern</a>. We'll cover that here, but I'll also
show you other, more interesting places where the term "prototype" and the
concepts behind it have popped up. But first, let's revisit the <span
name="original">original</span> pattern.

我第一次听到“原型”这个词是在*设计模式*中。
如今，似乎每个人都在用这个词，但他们讨论的不是<a href="http://en.wikipedia.org/wiki/Prototype_pattern" class="gof-pattern">设计模式</a>。
我们会讲述设计模式，同样也会展示术语“原型”的有趣之处，和其出现的背后理念。
但首先，让我们重访<span name="original">原先的</span>设计模式。

<aside name="original">

I don't say "original" lightly here. *Design Patterns* cites Ivan Sutherland's
legendary [Sketchpad](http://en.wikipedia.org/wiki/Sketchpad) project in *1963*
as one of the first examples of this pattern in the wild. While everyone else
was listening to Dylan and the Beatles, Sutherland was busy just, you know,
inventing the basic concepts of CAD, interactive graphics, and object-oriented
programming.

我不是随意地在这里说“原先的”。
*设计模式*引自*1963年*Ivan Sutherland的传说级的[Sketchpad](http://en.wikipedia.org/wiki/Sketchpad)项目，那是这个模式出现的首个例子。
当其他人在听迪伦和甲壳虫乐队时，Sutherland正忙于，你知道的，发明CAD，交互图形和面向对象编程的基本概念。

Watch [the demo](http://www.youtube.com/watch?v=USyoT_Ha_bA) and prepare to be
blown away.

看看这个[demo](http://www.youtube.com/watch?v=USyoT_Ha_bA)，准备接受震撼吧。

</aside>

## The Prototype Design Pattern

## 原型设计模式

Pretend we're making a game in the style of Gauntlet. We've got creatures and
fiends swarming around the hero, vying for their share of his flesh. These
unsavory dinner companions enter the arena by way of "spawners", and there is a
different spawner for each kind of enemy.

假设我们要用Gauntlet的风格做款游戏。
我们有野兽和魔鬼蜂群围绕着英雄，争着要吃他的血肉。
这些不可口的晚餐同伴通过“生产者”进入这片区域，每种敌人有不同的生产者。

For the sake of this example, let's say we have different classes for each kind
of monster in the game -- `Ghost`, `Demon`, `Sorcerer`, etc., like:

为了这个例子考虑，假设我们游戏中每种怪物都有不同的类——`Ghost`，`Demon`，`Sorcerer`等等，像这样：

^code monster-classes

A spawner constructs instances of one particular monster type. To support every
monster in the game, we *could* brute-force it by having a spawner class for
each monster class, leading to a parallel class hierarchy:

一个生产者构造特定种类怪物的实例。
为了在游戏中支持每一种怪物，我们*可以*残忍地逼迫它，
让每个怪物类都有生产者类，得到平行的类层次。

<span name="inherits-arrow"></span>

<span name="inherits-arrow"></span>

<img src="images/prototype-hierarchies.png" alt="Parallel class hierarchies. Ghost, Demon, and Sorceror all inherit from Monster. GhostSpawner, DemonSpawner, and SorcerorSpawner inherit from Spawner." />

<aside name="inherits-arrow">

I had to dig up a dusty UML book to make this diagram. The <img
src="images/arrow-inherits.png" class="arrow" alt="A UML arrow." /> means "inherits from".

我得翻出落满灰尘的UML书来写这个图表。<img src="images/arrow-inherits.png" class="arrow" alt="A UML arrow." />代表“继承”。

</aside>

Implementing it would look like this:

实现它看起来像是这样：

^code spawner-classes

Unless you get paid by the line of code, this is obviously not a fun way
to hack this together. Lots of classes, lots of boilerplate, lots of redundancy,
lots of duplication, lots of repeating myself...

除非你会因为这行代码获得工资，
将这些焊在一起很明显不是好方法。
众多类，众多引用，众多冗余，众多副本，众多重复自我……

The Prototype pattern offers a solution. The key idea is that *an object can
spawn other objects similar to itself*. If you have one ghost, you can make more
ghosts from it. If you have a demon, you can make other demons. Any monster can
be treated as a *prototypal* monster used to generate other versions of
itself.

原型模式提供了一个解决方案。
关键思路是*一个对象可以产出与它自己相近的对象。*
如果你有一个鬼影，你可以制造更多鬼影。
如果你有一个恶魔，你可以制造其他恶魔。
任何怪物都可以被视为*原型*怪物来产出它自己的其他版本。

To implement this, we give our base class, `Monster`, an abstract `clone()`
method:

为了实现这一点，我们给出基类`Monster`一个抽象方法`clone()`：

^code virtual-clone

Each monster subclass provides an implementation that returns a new object
identical in class and state to itself. For example:

每个怪兽子类提供一个实现，返回与它自己的类和状态都完全一样的新对象。举个例子：

^code clone-ghost

Once all our monsters support that, we no longer need a spawner class for each
monster class. Instead, we define a single one:

一旦我们所有的怪物都支持这个，
我们不需要为每个怪物类创建生产者类。我们只定义一个：

^code spawner-clone

It internally holds a monster, a hidden one whose sole purpose is to be used by
the spawner as a template to stamp out more monsters like it, sort of like a
queen bee who never leaves the hive.

它内部有保存一个怪物，一个隐藏的怪物，
它唯一的目标就是被生产者当做模板，去产生更多一样的怪物，
有点像一个从来不离开巢穴的蜂后。

<img src="images/prototype-spawner.png" alt="A Spawner contains a prototype field referencing a Monster. It calls clone() on the prototype to create new monsters." />

To create a ghost spawner, we create a prototypal ghost instance and
then create a spawner holding that prototype:

为了创造鬼魂生产者，我们创建一个原型鬼魂实例，然后创建拥有这个实例的产出者：

^code spawn-ghost-clone

One neat part about this pattern is that it doesn't just clone the *class* of
the prototype, it clones its *state* too. This means we could make a spawner for
fast ghosts, weak ghosts, or slow ghosts just by creating an appropriate
prototype ghost.

我们这个模式的灵巧部分是它不但拷贝原型的*类*，也拷贝它的*状态*。
这就意味着我们可以创建生产者生产快速鬼魂，虚弱鬼魂，慢速鬼魂，只需创建一个合适的原型鬼魂。

I find something both elegant and yet surprising about this pattern. I can't
imagine coming up with it myself, but I can't imagine *not* knowing about it now
that I do.

我在这个模式中找到了一些既优雅又惊讶的东西。
我无法相信我找到的东西，但我更无法想象现在*不知道*这些东西的自己。

### How well does it work?

### 它工作的如何？

Well, we don't have to create a separate spawner class for each monster, so
that's good. But we *do* have to implement `clone()` in each monster class.
That's just about as much code as the spawners.

好吧，我们不需要为每个怪物创建单独的生产者类，那很好。
但我们*确实*需要在每个怪物类中实现`clone()`。
这和生产者中的代码一样多。

There are also some nasty semantic ratholes when you sit down to try to write a
correct `clone()`. Does it do a deep clone or shallow one? In other words, if a
demon is holding a pitchfork, does cloning the demon clone the pitchfork too?

当你坐下来试着写一个正确的`clone()`，这里有些令人不快的语义漏洞。
做深层拷贝还是浅层拷贝呢？换言之，如果恶魔拿着草叉，克隆恶魔也要克隆草叉吗？

Also, not only does this not look like it's saving us much code in this
contrived problem, there's the fact that it's a *contrived problem*. We had to
take as a given that we have separate classes for each monster. These days,
that's definitely *not* the way most game engines roll.

同时，这看上去没减少已存问题上的代码，
事实上这里还有些*人为的问题*。
我们需要将每个怪物有独立的类作为前提条件。
这绝对*不是*当今大多数游戏引擎运转的方法。

Most of us learned the hard way that big class hierarchies like this are a pain
to manage, which is why we instead use patterns like <a href="component.html"
class="pattern">Component</a> and <a href="type-object.html"
class="pattern">Type Object</a> to model different kinds of entities without
enshrining each in its own class.

我们中大部分以痛苦地方式学到，这样庞杂的类层次管理起来是很痛苦的，那就是我们为什么用<a href="component.html" class="pattern">组件模式</a>和<a href="type-object.html" class="pattern">类型对象</a>为不同的实体建模，这样无需一一建构自己的类。

### Spawn functions

### 生产函数

Even if we do have different classes for each monster, there are other ways to
decorticate this *Felis catus*. Instead of making separate spawner *classes* for
each monster, we could make spawn *functions*, like so:

哪怕每个怪物都有不同的类，这里还有其他方式来为*家猫*剥皮。（译著：无法将这句翻译出来）
不是使用为每个怪物建立分离的生产者*类*，我们可以创建生产*函数*，就像这样：

^code callback

This is less boilerplate than rolling a whole class for constructing a monster
of some type. Then the one spawner class can simply store a function pointer:

这比将不同类型怪兽的构建包到一起使用了更少的样板。生产者类可以简单地存储一个函数指针：

^code spawner-callback

To create a spawner for ghosts, you do:

为了给鬼魂创建生产者，你需要做：

^code spawn-ghost-callback

### Templates

### 模板

By <span name="templates">now</span>, most C++ developers are familiar with
templates. Our spawner class needs to construct instances of some type, but we
don't want to hard code some specific monster class. The natural solution then
is to make it a *type parameter*, which templates let us do:

到了<span name="templates">现在</span>，大多数C++开发者很熟悉模板了。
生产者类需要构建各种类型的实例，但是我们不想为特定的怪物类硬编码。
自然的解决方案是将它作为*类型参数*，就是模板允许我们做的：

<aside name="templates">

I'm not sure if C++ programmers learned to love them or if templates just
scared some people completely away from C++. Either way, everyone I see using
C++ today uses templates too.

我不太确定C++程序员是学着喜欢它还是模板完全吓阻了人们学习C++。
不管怎样，我今日见到使用C++的也都使用模板。

</aside>

<span name="base"></span>

<span name="base"></span>

^code templates

Using it looks like:

像这样使用它：

^code use-templates

<aside name="base">

The `Spawner` class here is so that code that doesn't care what kind of monster
a spawner creates can just use it and work with pointers to `Monster`.

这里的`Spawner`类是这样，代码不必担心生产者生产了什么样的怪物，
它可以使用它然后与指向`Monster`的指针打交道。

If we only had the `SpawnerFor<T>` class, there would be no single supertype the
instantiations of that template all shared, so any code that worked with
spawners of any monster type would itself need to take a template parameter.

如果我们只有`SpawnerFor<T>`类，那就没有类型实例化共享模板，这样任何与产生多种怪物类型的生产者打交道的代码都需要接受模板参数。

</aside>

### First-class types

### 第一公民类型

The previous two solutions address the need to have a class, `Spawner`, which is
parameterized by a type. In C++, types aren't generally first-class, so that
requires some <span name="type-obj">gymnastics</span>. If you're using a
dynamically-typed language like JavaScript, Python, or Ruby where classes *are*
 regular objects you can pass around, you can solve this much more directly.

前面的两个解决方案完成了需要类的需求，`Spawner`，它用类型参数化。
在C++中，类型不是第一公民，所以需要一些<span name="type-obj">改动</span>。
如果你使用了动态类型语言比如JavaScript，Python，或者Ruby，
它们的类*是*可以传递的对象，你可以更加直接的解决这个问题。

<aside name="type-obj">

In some ways, the <a href="type-object.html" class="pattern">Type Object</a>
pattern is another workaround for the lack of first-class types. That pattern
can still be useful even in languages with them, though, because it lets *you*
define what a "type" is. You may want different semantics than what the
language's built-in classes provide.

某种程度上， <a href="type-object.html" class="pattern">类型对象</a>也是为了弥补缺少第一公民类型。
但那个模式在拥有第一公民类型的语言中也有用，因为它让*你*决定什么是“类型”。
你也许想要与语言内建的类不同的语义。

</aside>

When you make a spawner, just pass in the class of monster that it should
construct -- the actual runtime object that represents the monster's
class. Easy as pie.

当你完成一个生产者，直接向它传递该构建的怪物类——那个真实的运行时对象，代表了怪物的类。小菜一碟。

With all of these options, I honestly can't say I've found a case where I felt
the Prototype *design pattern* was the best answer. Maybe your experience will
be different, but for now let's put that away and talk about something else:
prototypes as a *language paradigm*.

使用这些选项，我不能诚实的说我找到了一种情况，
原型*设计模式*是最好的方案。
也许你的经验有所不同，但现在把它搁到一边，讨论点别的：作为一种*语言范式*的原型。

## The Prototype Language Paradigm

## 原型语言范式

Many people think "object-oriented programming" is synonymous with "classes".
Definitions of OOP tend to feel like credos of opposing religious denominations,
but a fairly non-contentious take on it is that *OOP lets you define "objects"
which bundle data and code together.* Compared to structured languages like C
and functional languages like Scheme, the defining characteristic of OOP is that
it tightly binds state and behavior together.

很多人认为“面向对象编程”和“类”是同义词。
OOP的定义感觉像是相反宗教信仰的教义，
公平而无争议的是*OOP让你定义“对象”，将数据和代码绑定在一起。*
与结构化的语言比如C相比，与函数语言比如Scheme相比，
OOP定义的特性是它将状态和行为紧紧地绑在一起。

You may think classes are the one and only way to do that, but a handful of guys
including Dave Ungar and Randall Smith beg to differ. They created a language in
the 80s called Self. While as OOP as can be, it has no classes.

你也许认为类是唯一可以完成这个的方法，
但是一大堆家伙包括Dave Ungar和Randall Smith一直在拼命区分。
他们在80年代创建了一种叫做Self的语言。它可以是OOP的，同时没有类

### Self

### Self语言

In a pure sense, Self is *more* object-oriented than a class-based language. We
think of OOP as marrying state and behavior, but languages with classes actually
have a line of separation between them.

纯粹的感觉上，Self比一个基于类的语言*更加*面向对象。
我们认为OOP将状态和行为绑在一起，但是有类的语言实际在他们之间画了一条线。

Consider the semantics of your favorite class-based language. To access some
state on an object, you look in the memory of the instance itself. State is
*contained* in the instance.

考虑你最喜欢的基于类的语言的语义。
为了接触对象中的一些状态，在实例的内存中查询。状态是*包含*在实例之中的。

To invoke a <span name="vtable">method</span>, though, you look up the
instance's class, and then you look up the method *there*. Behavior is contained
in the *class*. There's always that level of indirection to get to a method,
which means fields and methods are different.

但是，为了调用<span name="vtable">方法</span>，你查询实例的类，
然后你在*那里*查询方法。行为被包含在*类*中。
获得方法总需要通过中间层，这意味着字段和方法是不同的。

<img src="images/prototype-class.png" alt="A Class contains a list of Methods. An Instance contains a list of Fields and a reference to its Class." />

<aside name="vtable">

For example, to invoke a virtual method in C++, you look in the instance for the
pointer to its vtable, then look up the method there.

举个例子，为了调用C++中的虚方法，你需要在实例中找指向虚方法表的指针，然后再在那里找方法。

</aside>

Self eliminates that distinction. To look up *anything*, you just look on the
object. An instance can contain both state and behavior. You can have a single
object that has a method completely unique to it.

Self结束了这种分歧。找*任何东西*，都只需在对象中找。
实例同时包含状态和行为。你可以有一个对象，它有一个完全独特的方法。

<span name="island"></span>

<span name="island"></span>

<img src="images/prototype-object.png" alt="An Object contains a mixed list of Fields and Methods." />

<aside name="island">

No man is an island, but this object is.

没人是一座孤岛，但这个对象是。

</aside>

If that was all Self did, it would be hard to use. Inheritance in class-based
languages, despite its faults, gives you a useful mechanism for reusing
polymorphic code and avoiding duplication. To accomplish something similar
without classes, Self has *delegation*.

如果这就是Self做的，那它很难使用。
在基于类的语言中，继承，不管它的缺陷，给了有用的机制来重用多态代码和避免复制。
为了不用类而实现一些相同的东西，Self拥有*委托*。

To find a field or call a method on some object, we first look in the object
itself. If it has it, we're done. If it doesn't, we look at the object's <span
name="parent">*parent*</span>. This is just a reference to some other object.
When we fail to find a property on the first object, we try its parent, and its
parent, and so on. In other words, failed lookups are *delegated* to an object's
parent.

为了在对象中寻找字段或者调用方法，首先在对象内部查找。
如果有，就完成了。如果没有，在对象的<span name="parent">*父类*</span>中寻找。
这是一个对其他对象的引用。
当我们没能在第一个对象中找到属性，我们尝试它的父母，然后父母的父母，继续下去。
换言之，失败的查找被*委托*给对象的父母。

<aside name="parent">

I'm simplifying here. Self actually supports multiple parents. Parents are just
specially marked fields, which means you can do things like inherit parents or
change them at runtime, leading to what's called *dynamic inheritance*.

我在这里简化了。Self实际上支持多个父对象。
父父母只是特别标明的字段，意味着你可以继承它们或者在运行时改变他们，
最终得到了“动态继承”。

</aside>

<img src="images/prototype-delegate.png" alt="An Object contains Fields and Methods and a reference to another object that it delegates to." />

Parent objects let us reuse behavior (and state!) across multiple objects, so
we've covered part of the utility of classes. The other key thing classes do is
give us a way to create instances. When you need a new thingamabob, you can just
do `new Thingamabob()`, or whatever your preferred language's syntax is. A class
is a factory for instances of itself.

父对象让我们在不同对象间重用行为（还有状态！），所以我们覆盖了类的公共部分。
另外一个类做的关键事情就是给了创建实例的方法。
当你需要新的某物，你可以直接`new Thingamabob()`，或者随便什么你喜欢的语言表达法。
类是它实例的工厂。

Without classes, how do we make new things? In particular, how do we make a
bunch of new things that all have stuff in common? Just like the design pattern,
the way you do this in Self is by *cloning*.

不用类，我们怎样创建新东西？
特别的，我们如何创建一堆有共同点的新东西？
就像这个设计模式，你在Self中的方式是使用*克隆*。

In Self, it's as if *every* object supports the Prototype design pattern
automatically. Any object can be cloned. To make a bunch of similar objects, you:

在Self中，就好像*每个*对象都自动支持原型设计模式。
任何对象都能被克隆。为了获得一堆相似的对象，你：

1. Beat one object into the shape you want. You can just clone the base `Object`
   built into the system and then stuff fields and methods into it.

1. 将对象拍打成你想要的形状。你可以直接克隆系统内建的基本`Object`，然后向其中添加字段和方法。

2. Clone it to make as many... uh... clones as you want.

2. 克隆它来产出……额……随你想要多少克隆。

This gives us the elegance of the Prototype design pattern without the tedium of
having to implement `clone()` ourselves; it's built into the system.

这给了我们原型模式的优雅，而无需自己沉闷的实现`clone()`；它被内建在系统中。

This is such a beautiful, clever, minimal system that as soon as I learned about
it, <span name="finch">I started</span> creating a prototype-based language to get more experience with it.

这是一个如此美妙，紧密，最小化的系统，
一听说它，<span name="finch">我就开始</span>创建一个基于原型的语言来获取更多经验。

<aside name="finch">

I realize building a language from scratch is not the most efficient way to
learn, but what can I say? I'm a bit peculiar. If you're curious, the language
is called [Finch](http://finch.stuffwithstuff.com/).

我知道从头开始构建语言不是学它最有效率的办法，但我能说什么呢？我是有点特权的。
如果你很好奇，那语言被称为[Finch](http://finch.stuffwithstuff.com/).

</aside>

### How did it go?

### 它运行的如何？

I was super excited to play with a pure prototype-based language, but once I had
mine up and running, I <span name="no-fun">discovered</span> an unpleasant fact:
it just wasn't that fun to program in.

我急切的想要使用一个纯粹的基于原型的语言，但是当我完成并运行它，
我<span name="no-fun">发现</span>了一个令人不快的事实：用它编程没那么有趣。

<aside name="no-fun">

I've since heard through the grapevine that many of the Self programmers came to
the same conclusion. The project was far from a loss, though. Self was so
dynamic that it needed all sorts of virtual machine innovations in order to run
fast enough.

我从小道消息听说很多Self程序员得出了相同的结论。
但项目并不是一个失败。
Self非常的灵活，它需要虚拟机各种创新来跑得快一些。

The ideas they invented for just-in-time compilation, garbage collection, and
optimizing method dispatch are the exact same techniques -- often implemented by
the same people! -- that now make many of the world's dynamically-typed
languages fast enough to use for massively popular applications.

在这种思路下他们发明了just-in-time编译，垃圾回收，以及优化方法分配——由同样的人实现的——让很多动态类型语言快到可以构建超流行的应用。

</aside>

Sure, the language was simple to implement, but that was because it punted the
complexity onto the user. As soon as I started trying to use it, I found myself
missing the structure that classes give. I ended up trying to recapitulate it at
the library level since the language didn't have it.

是的，语言很容易实现，那是因为它把复杂度推给了用户。
一旦我们开始试着使用，我发现我想念类给的结构。
我最终停在构建语言没有的库概念。

Maybe this is because my prior experience is in class-based languages, so
my mind has been tainted by that paradigm. But my hunch is that most people just
like well-defined "kinds of things".

鉴于我之前的经验都来自基于类的语言，因此我的头脑可能被那个范式污染了。
但是我的直觉是大部分人还是喜欢认真定义的“那些事物”。

In addition to the runaway success of class-based languages, look at how many
games have explicit character classes and a precise roster of different sorts
of enemies, items, and skills, each neatly labeled. You don't see many games
where each monster is a unique snowflake, like "sort of halfway between a troll
and a goblin with a bit of snake mixed in".

除去基于类语言的成功，看看有多少游戏明确指定了角色类以及不同敌人物品技能的清单，
每个都有整齐的标签。你不会在游戏中看到每个怪物都是独特的雪团，
比如“洞穴人和哥布林还有雪混合在一起”的东西。

While prototypes are a really cool paradigm and one that I wish more people
knew about, I'm glad that most of us aren't actually programming using them
every day. <span name="telling">The code</span> I've seen that fully embraces
prototypes has a weird mushiness to it that I find hard to wrap my head around.

原型是非常酷的范式，也是我希望有更多人了解的东西，
但我很庆幸我们不是每天都用它编程。
我看到的完全皈依原型的<span name="telling">代码是一团浆糊</span>，
我很难用它完成点什么。

<aside name="telling">

It's also telling how *little* code there actually is written in a prototypal
style. I've looked.

同时证明使用原型风格的代码*很少*。我查过了。

</aside>

### What about JavaScript?

### JavaScript又怎么样呢？

OK, if prototype-based languages are so unfriendly, how do I explain JavaScript?
Here's a language with prototypes used by millions of people every day. More
computers run JavaScript than any other language on Earth.

好吧，如果基于原型的语言不那么友好，怎么解释JavaScript呢？
这是一个有原型的语言，每天被数百万人使用。运行JavaScript的机器数量超过了地球上其他所有的语言。

<span name="ten">Brendan Eich</span>, the creator of JavaScript, took
inspiration directly from Self, and many of JavaScript's semantics are
prototype-based. Each object can have an arbitrary set of properties, both
fields and "methods" (which are really just functions stored as fields). An
object can also have another object, called its "prototype", that it delegates
to if a field access fails.

<span name="ten">Brendan Eich</span>，JavaScript的缔造者，
从Self中直接吸收灵感，很多JavaScript的语义都是基于原型的。
每个对象都有属性的集合，字段和“方法”（事实上只是存储为字段的函数）都在其中。
A对象可以拥有B对象，B对象被称为A对象的“原型”，
如果A对象的字段获取失败就会委托给B对象。

<aside name="ten">

As a language designer, one appealing thing about prototypes is that they are
simpler to implement than classes. Eich took full advantage of this: the first
version of JavaScript was created in ten days.

作为语言设计者，原型的诱人之处是它们比类更易于实现。
Eich充分利用了这一点，JavaScript的第一个版本是在十天内创建的。

</aside>

But, despite that, I believe that JavaScript in practice has more in common with
class-based languages than with prototypal ones. One hint that JavaScript has
taken steps away from Self is that the core operation in a prototype-based
language, *cloning*, is nowhere to be seen.

但是不管那个，我相信JavaScript在实践中更像是基于类的而不是基于原型的。
一个关键点是JavaScript将一些基于原型的语言核心操作取了出来，*克隆*，不见了。

There is no method to clone an object in JavaScript. The closest it has is
`Object.create()`, which lets you create a new object that
delegates to an existing one. Even that wasn't added until ECMAScript 5,
fourteen years after JavaScript came out. Instead of cloning, let me walk you
through the typical way you define types and create objects in JavaScript. You
start with a *constructor function*:

在JavaScript中没有方法来克隆一个对象。
最接近的是`Object.create()`，允许你创建新对象委托给现有的。
甚至在ECMAScript5之前都没有添加，那已是JavaScript出现后的十四年了。
不用克隆，让我带你浏览一下JavaScript中定义类和创建对象的经典方法。
从一个*构造器函数*开始：

    :::javascript
    function Weapon(range, damage) {
      this.range = range;
      this.damage = damage;
    }

This creates a new object and initializes its fields. You invoke it like:

这创建了一个新对象并初始化了它的字段。你像这样引入它：

    :::javascript
    var sword = new Weapon(10, 16);

The `new` here invokes the body of the `Weapon()` function with `this` bound to a
new empty object. The body adds a bunch of fields to it, then the now-filled-in
object is automatically returned.

这里的`new`引入了`Weapon()`函数的实体，
而`this`绑定到新的空对象上。
实体在它上面添加了一系列字段，然后现在填满的对象自动返回了。

The `new` also does one other thing for you. When it creates that blank object,
it wires it up to delegate to a prototype object. You can get to that object
directly using `Weapon.prototype`.

`new`也为你做了另外一件事。
当它创建那个空对象，它将它和一个原型对象连接起来。
你可以用`Weapon.prototype`来直接使用它。

While state is added in the constructor body, to define *behavior*, you usually
add methods to the prototype object. Something like this:

当状态添加到构建器中时，为了定义*行为*，你通常向原型对象添加方法。就像这样：

    :::javascript
    Weapon.prototype.attack = function(target) {
      if (distanceTo(target) > this.range) {
        console.log("Out of range!");
      } else {
        target.health -= this.damage;
      }
    }

This adds an `attack` property to the weapon prototype whose value is a
function. Since every object returned by `new Weapon()` delegates to
`Weapon.prototype`, you can now call `sword.attack()` and it will call that
function. It looks a bit like this:

这添加了一个`attack`属性给武器原型，属性的值是一个函数。
由于每个对象都通过`new Weapon()`委托给了`Weapon.prototype`，
你现在可以调用一个`sword.attack()`，它就会调用那个函数。
看上去像是这样：

<img src="images/prototype-weapon.png" alt="A Weapon object contains an attack() method and other methods. A Sword object contains fields and delegates to Weapon." />

Let's review:

让我们回顾一下：

* The way you create objects is by a "new" operation that you invoke using an
  object that represents the type -- the constructor function.

* 创建对象的方法是通过“new”操作，引入代表类型的对象——构造器函数。

* State is stored on the instance itself.

* 状态被存储在实例中。

* Behavior goes through a level of indirection -- delegating to the prototype --
  and is stored on a separate object that represents the set of methods shared
  by all objects of a certain type.

* 行为通过一个间接层——原型的委托——被存储在分散的对象中，代表了一系列特定类型对象的共享方法。

Call me crazy, but that sounds a lot like my description of classes earlier. You
*can* write prototype-style code in JavaScript (*sans* cloning), but the syntax
and idioms of the language encourage a class-based approach.

说我疯了吧，但这听起来很像是我之前描述的类。
你*可以*在JavaScript中写原型风格的代码（*没有*克隆），
但是语言语义和常用方法更推荐基于类实现。

Personally, I think that's a <span name="good">good thing</span>. Like I said, I
find doubling down on prototypes makes code harder to work with, so I like that
JavaScript wraps the core semantics in something a little more classy.

个人而言，我认为这是<span name="good">好事</span>。
就像我说的，我发现完全使用原型让代码很难处理，
所以我喜欢JavaScript将整个核心语义包裹在更漂亮的东西中。

## Prototypes for Data Modeling

## 为数据模型建立原型

OK, I keep talking about things I *don't* like prototypes for, which is making
this chapter a real downer. I think of this book as more comedy than tragedy, so
let's close this out with an area where I *do* think prototypes, or more
specifically *delegation*, can be useful.

好吧，我不断的讨论*不喜欢*原型的原因，这让这一章稍有褪色。
我认为这本书该比惨案更加欢乐些，所以让我们在原型*确实*有用，或者更加精确，*委托*有用的地方结束这个话题。

If you were to count all the bytes in a game that are code compared to the ones
that are data, you'd see the fraction of data has been increasing steadily since
the dawn of programming. Early games procedurally generated almost everything so
they could fit on floppies and old game cartridges. In many games today, the
code is just an "engine" that drives the game, which is defined entirely in
data.

如果你比较了程序与数据的字节数，
你可以看到数据的占比随着编程的进行稳定地增长。
早期的游戏在程序中生成几乎任何东西，这样可以适应磁盘和老式游戏盒。
在今日的游戏中，代码只是驱动游戏的“引擎”，游戏是完全由数据定义的。

That's great, but pushing piles of content into data files doesn't magically
solve the organizational challenges of a large project. If anything, it makes it
harder. The reason we use programming languages is because they have tools for
managing complexity.

这很好，但是将内容推到数据文件中不会魔术般解决组织大工程的挑战。
如果有什么区别的话，那就是变难了。
我们使用编程语言的原因就是它们有办法处理复杂性。

Instead of copying and pasting a chunk of code in ten places, we move it into a
function that we can call by name. Instead of copying a method in a bunch of
classes, we can put it in a separate class that those classes inherit from or
mix in.

不再将一堆代码拷来拷去，我们将其移入函数中，这样可以通过名字调用。
不再一堆类之间复制方法，我们将其放入分离的类中，其他类可以继承或者组合它。

When your game's data reaches a certain size, you really start wanting similar
features. Data modeling is a deep subject that I can't hope to do justice here,
but I do want to throw out one feature for you to consider in your own games:
using prototypes and delegation for reusing data.

当游戏数据达到一定规模时，你真的需要考虑更加简单的特性。
数据模式是个我不指望在这里能说清的问题，
但我确实希望将一个思路扔给你，让你在游戏中考虑：使用原型和委托来重用数据。

Let's say we're defining the data model for the <span name="shameless">shameless
Gauntlet rip-off</span> I mentioned earlier. The game designers need to specify
the attributes for monsters and items in some kind of files.

假设我们为我早先提到的<span name="shameless">无耻Gauntlet rip-off</span>定义数据模型。游戏设计者需要在很多文件中设定怪物和物品的属性。

<aside name="shameless">

I mean completely original title in no way inspired by any previously existing
top-down multi-player dungeon crawl arcade games. Please don't sue me.

这是完全原创的主题，没有受到已存的多人地下城游戏的影响。
请不要告我。

</aside>

One common approach is to use JSON. Data entities are basically *maps*, or
*property bags*, or any of a dozen other terms because there's nothing
programmers like more than <span name="inventing">inventing</span> a new name
for something that already has one.

一个常用的方法是使用JSON。
数据实体是基本*图*，或者*属性包*，或者其他什么术语，
因为程序员就喜欢为已有的东西<span name="inventing">发明</span>新名字。

<aside name="inventing">

We've re-invented them so many times that Steve Yegge calls them ["The Universal
Design Pattern"](http://steve-yegge.blogspot.com/2008/10/universal-design-patter
n.html).

我们重新发明了它们太多次，Steve Yegge称之为["通用设计模式"](http://steve-yegge.blogspot.com/2008/10/universal-design-pattern.html).

</aside>

So a goblin in the game might be defined something like this:

所以游戏中的哥布林也许被定义为像这样的东西：

    :::json
    {
      "name": "goblin grunt",
      "minHealth": 20,
      "maxHealth": 30,
      "resists": ["cold", "poison"],
      "weaknesses": ["fire", "light"]
    }



This is pretty straightforward and even the most text-averse designer can handle
that. So you throw in a couple of sibling branches on the Great Goblin Family
Tree:

这看上去很易懂，哪怕是最讨厌文字的设计者也能完成。
所以你给哥布林大家族添加几个兄弟分支：

    :::json
    {
      "name": "goblin wizard",
      "minHealth": 20,
      "maxHealth": 30,
      "resists": ["cold", "poison"],
      "weaknesses": ["fire", "light"],
      "spells": ["fire ball", "lightning bolt"]
    }

    {
      "name": "goblin archer",
      "minHealth": 20,
      "maxHealth": 30,
      "resists": ["cold", "poison"],
      "weaknesses": ["fire", "light"],
      "attacks": ["short bow"]
    }

Now, if this was code, our aesthetic sense would be tingling. There's a lot of
duplication between these entities, and well-trained programmers *hate* that. It
wastes space and takes more time to author. You have to read carefully to tell
if the data even *is* the same. It's a maintenance headache. If we decide to
make all of the goblins in the game stronger, we need to remember to update the
health of all three of them. Bad bad bad.

现在，如果这是代码，我们的美感就被刺激了。
在实体间有很多的重复性，训练优良的程序员*讨厌*它。
它浪费了空间，消耗了作者更多时间。
你需要仔细阅读代码才知道这些数据*是不是*相同的。
这让维护头疼。
如果我们决定让所有哥布林变强，需要记得将三个都更新一遍。糟糕糟糕糟糕。

If this was code, we'd create an abstraction for a "goblin" and reuse that
across the three goblin types. But dumb JSON doesn't know anything about that.
So let's make it a bit smarter.

如果这是代码，我们会为“哥布林”创造一个抽象，然后在三个哥布林类型中重用它。
但是愚蠢的JSON不知道这么做。所以让我们把它做得更加巧妙。

We'll declare that if an object has a <span name="meta">`"prototype"`</span>
field, then that defines the name of another object that this one delegates to.
Any properties that don't exist on the first object fall back to being looked up
on the prototype.

我们像对象有<span name="meta">`"prototype"`</span>字段那样声明它，
然后定义它委托给的对象的名字。
任何不在第一个对象中的原型都退回在原型中查找。

<aside name="meta">

This makes the `"prototype"` a piece of *meta*data instead of data. Goblins have
warty green skin and yellow teeth. They don't have prototypes. Prototypes are a
property of the *data object representing the goblin*, and not the goblin
itself.

这让`"prototype"`不再是数据，而是*元*数据的一部分。
哥布林有疣状绿色皮和黄牙齿。
它们没有原型。
原型是*表示哥布林的数据模型*的属性，而不是哥布林本身。

</aside>

With that, we can simplify the JSON for our goblin horde:

通过这样做，我们可以简单地定义大群哥布林：

    :::json
    {
      "name": "goblin grunt",
      "minHealth": 20,
      "maxHealth": 30,
      "resists": ["cold", "poison"],
      "weaknesses": ["fire", "light"]
    }

    {
      "name": "goblin wizard",
      "prototype": "goblin grunt",
      "spells": ["fire ball", "lightning bolt"]
    }

    {
      "name": "goblin archer",
      "prototype": "goblin grunt",
      "attacks": ["short bow"]
    }

Since the archer and wizard have the grunt as their prototype, we don't have to
repeat the health, resists, and weaknesses in each of them. The logic we've added
to our data model is super simple -- basic single delegation -- but we've
already gotten rid of a bunch of duplication.

由于弓箭手有grunt作原型，我们不需要在他们中重复血量，防御和弱点。
我们给数据模型增加的新逻辑超简单——基本的单一委托——但已经摆脱了一堆冗余。

One interesting thing to note here is that we didn't set up a fourth "base
goblin" *abstract* prototype for the three concrete goblin types to delegate to.
Instead, we just picked one of the goblins who was the simplest and delegated to
it.

有趣的事情是，我们没有为具体的哥布林委托的*抽象*原型设置更进一步“基本哥布林”。
相反，我们选择了最简单的哥布林，然后委托给它。

That feels natural in a prototype-based system where any object can be used as a
clone to create new refined objects, and I think it's equally natural here too.
It's a particularly good fit for data in games where you often have one-off
special entities in the game world.

在基于原型的系统中，一个对象可以克隆产生新对象是很自然的，
我认为在这里也一样自然。这特别适合记录那些只有一项特别之处实体的游戏数据。

Think about bosses and unique items. These are often refinements of a more
common object in the game, and prototypal delegation is a good fit for defining
those. The magic Sword of Head-Detaching, which is really just a longsword with
some bonuses, can be expressed as that directly:

想想Boss和其他独特事物，它们通常是更加平凡事物的重新定义，
而原型委托是定义它们的好方法。
断头魔剑，就是一把拥有加成的长剑，可以像下面这样表示：

    :::json
    {
      "name": "Sword of Head-Detaching",
      "prototype": "longsword",
      "damageBonus": "20"
    }

A little extra power in your game engine's data modeling system can make it
easier for designers to add lots of little variations to the armaments and
beasties populating your game world, and that richness is exactly what delights
players.


在游戏引擎上耗费一点额外的力量，就能让设计者更方便地添加多种不同的武器和野兽，这种丰富度会取悦玩家。
