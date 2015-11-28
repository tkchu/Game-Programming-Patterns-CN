^title Prototype
^section Design Patterns Revisited

The first time I heard the word "prototype" was in *Design Patterns*. Today, it
seems like everyone is saying it, but it turns out they aren't talking about the
<a href="http://en.wikipedia.org/wiki/Prototype_pattern"
class="gof-pattern">design pattern</a>. We'll cover that here, but I'll also
show you other, more interesting places where the term "prototype" and the
concepts behind it have popped up. But first, let's revisit the <span
name="original">original</span> pattern.

第一次我听到“原型”这个词是在*设计模式*中。今天，似乎每个人都在谈论它，谈事他们讨论的不是设计模式。我们等会会覆盖它，但是我同样会展示给你另一个，更加有趣的“原型”使用的地方以及他出现的背后观念。但是首先，让我们重访原型的设计模式。

<aside name="original">

I don't say "original" lightly here. *Design Patterns* cites Ivan Sutherland's
legendary [Sketchpad](http://en.wikipedia.org/wiki/Sketchpad) project in *1963*
as one of the first examples of this pattern in the wild. While everyone else
was listening to Dylan and the Beatles, Sutherland was busy just, you know,
inventing the basic concepts of CAD, interactive graphics, and object-oriented
programming.

Watch [the demo](http://www.youtube.com/watch?v=USyoT_Ha_bA) and prepare to be
blown away.

</aside>

## The Prototype Design Pattern

## 原型设计模式

Pretend we're making a game in the style of Gauntlet. We've got creatures and
fiends swarming around the hero, vying for their share of his flesh. These
unsavory dinner companions enter the arena by way of "spawners", and there is a
different spawner for each kind of enemy.

假设我们要用Gauntlet的风格做一款游戏。我们有动物和魔鬼蜂群围绕着英雄，竞争分享他的血肉。这些不可口的晚餐同伴通过“产卵者”进入这片区域，这对于每一种敌人有不同的产卵者。

For the sake of this example, let's say we have different classes for each kind
of monster in the game -- `Ghost`, `Demon`, `Sorcerer`, etc., like:

为了这个例子的利益，假设我们队每一种游戏中的怪物都有不同的类——`Ghost`，`Demon`，`Sorcerer`等等，像这样：

^code monster-classes

A spawner constructs instances of one particular monster type. To support every
monster in the game, we *could* brute-force it by having a spawner class for
each monster class, leading to a parallel class hierarchy:

一个产卵者构造特定种类的怪物的实例。为了在游戏中支持每一种怪物，我们*可以*残忍的强迫它，让每一个怪物类都有一个产卵者类，，得到了一个平行的类层次。

<span name="inherits-arrow"></span>

<img src="images/prototype-hierarchies.png" alt="Parallel class hierarchies. Ghost, Demon, and Sorceror all inherit from Monster. GhostSpawner, DemonSpawner, and SorcerorSpawner inherit from Spawner." />

<aside name="inherits-arrow">

I had to dig up a dusty UML book to make this diagram. The <img
src="images/arrow-inherits.png" class="arrow" alt="A UML arrow." /> means "inherits from".

</aside>

Implementing it would look like this:

实现这个看起来像是这个样：

^code spawner-classes

Unless you get paid by the line of code, this is obviously not a fun way
to hack this together. Lots of classes, lots of boilerplate, lots of redundancy,
lots of duplication, lots of repeating myself...

除非你会因为这行代码获得工资，这很明显不是有趣的方法将这些焊在一起。众多类，众多引用，众多冗余，众多副本，众多重复自我……

The Prototype pattern offers a solution. The key idea is that *an object can
spawn other objects similar to itself*. If you have one ghost, you can make more
ghosts from it. If you have a demon, you can make other demons. Any monster can
be treated as a *prototypal* monster used to generate other versions of
itself.

原型模式提供了一个解决方案。关键注意是*一个对象可以产出与它自己相近的对象。*如果你有一个鬼影，你可以从他制造更多的鬼影。如果你有一个恶魔，你可以制造其他恶魔。任何怪物都可以被视为*原型*怪物来产出它自己的其他版本。

To implement this, we give our base class, `Monster`, an abstract `clone()`
method:

为了实现这一点，我们给出了我们的基类`Monster`和一个抽象方法`clone()`：

^code virtual-clone

Each monster subclass provides an implementation that returns a new object
identical in class and state to itself. For example:

每一个怪兽子类提供一个实现，返回一个新的对象与他自己的类和状态都完全一样。举个例子：

^code clone-ghost

Once all our monsters support that, we no longer need a spawner class for each
monster class. Instead, we define a single one:

一旦我们所有的怪物都支持那个，我们不需要一个为每一个怪物类制作一个产卵类。取而代之的是，我们只定义一个：

^code spawner-clone

It internally holds a monster, a hidden one whose sole purpose is to be used by
the spawner as a template to stamp out more monsters like it, sort of like a
queen bee who never leaves the hive.

它内部有保存一个怪物，一个隐藏的怪物，唯一的目的就是被产卵者作为模板去产生更多像他一样的怪物，有点像一个从来不离开巢穴的蜂后。

<img src="images/prototype-spawner.png" alt="A Spawner contains a prototype field referencing a Monster. It calls clone() on the prototype to create new monsters." />

To create a ghost spawner, we create a prototypal ghost instance and
then create a spawner holding that prototype:

为了创造一个鬼魂产出者，我们创建一个原型鬼魂实例然后创建一个产出者拥有这个实例：

^code spawn-ghost-clone

One neat part about this pattern is that it doesn't just clone the *class* of
the prototype, it clones its *state* too. This means we could make a spawner for
fast ghosts, weak ghosts, or slow ghosts just by creating an appropriate
prototype ghost.

我们这个模式的灵巧部分是它不但拷贝原型的*类*，也拷贝它的*状态*。这就意味着我们可以创建一个产出者生产快速的鬼魂，虚弱的鬼魂，或者慢速的鬼魂，只需要创建一个合适的原型鬼魂。

I find something both elegant and yet surprising about this pattern. I can't
imagine coming up with it myself, but I can't imagine *not* knowing about it now
that I do.

我在这个模式中找到了一些既优雅又惊讶的东西。我无法相信我找到的东西，但我更无法想象*不知道*这些东西的现在的自己。

### How well does it work?

### 它工作的如何？

Well, we don't have to create a separate spawner class for each monster, so
that's good. But we *do* have to implement `clone()` in each monster class.
That's just about as much code as the spawners.

好吧，我们不需要为每一个怪物创建一个单独 产出者类，那很好。但我们*确实*需要在每个怪物类中实现实现`clone()`。这和产卵者中的代码一样多。

There are also some nasty semantic ratholes when you sit down to try to write a
correct `clone()`. Does it do a deep clone or shallow one? In other words, if a
demon is holding a pitchfork, does cloning the demon clone the pitchfork too?

这里有些令人不快的语义漏洞，当你坐下来试着写一个正确的`clone()`。做一个深层拷贝还是浅层的呢？换言之，如果一个恶魔拿着草叉，克隆恶魔也要克隆草叉吗？

Also, not only does this not look like it's saving us much code in this
contrived problem, there's the fact that it's a *contrived problem*. We had to
take as a given that we have separate classes for each monster. These days,
that's definitely *not* the way most game engines roll.

同时，这看上去既不减少我们在已存问题上的代码，这里事实上还有一些*人为的问题*。我们需要将每个怪物有独立的类作为前提条件。最近的日子，这绝对*不是*大多数游戏引擎运转的方法。

Most of us learned the hard way that big class hierarchies like this are a pain
to manage, which is why we instead use patterns like <a href="component.html"
class="pattern">Component</a> and <a href="type-object.html"
class="pattern">Type Object</a> to model different kinds of entities without
enshrining each in its own class.

我们中的大多数以痛苦的方式学到这样巨大的类层次管理起来是很痛苦的，那就是我们为什么用组件模式和类型对象来为不同种类的实体建模，无需让他们每一个都有自己的类。

### Spawn functions

### 产卵函数

Even if we do have different classes for each monster, there are other ways to
decorticate this *Felis catus*. Instead of making separate spawner *classes* for
each monster, we could make spawn *functions*, like so:

哪怕我们确实为每一怪物都有不同的类，这里有其他方式这里还有其他方式来为*家猫*剥皮。不是使用为每一个怪物建立分离的产卵*类*，我们可以创建产卵*函数*，就像这样：

^code callback

This is less boilerplate than rolling a whole class for constructing a monster
of some type. Then the one spawner class can simply store a function pointer:

这比将不同类型怪兽的构建包到一个有更少的样板。

^code spawner-callback

To create a spawner for ghosts, you do:

为了给鬼魂创建一个产卵者，你需要做：

^code spawn-ghost-callback

### Templates

### 模板

By <span name="templates">now</span>, most C++ developers are familiar with
templates. Our spawner class needs to construct instances of some type, but we
don't want to hard code some specific monster class. The natural solution then
is to make it a *type parameter*, which templates let us do:

到了现在，大多数C++开发者很熟悉模板了。我们的产卵者类需要构建各种类型的实例，但是五门不想Wie特定的怪物类硬编码。自然的解决方案是将它作为*类型参数*，就是模板让我们做的：

<aside name="templates">

I'm not sure if C++ programmers learned to love them or if templates just
scared some people completely away from C++. Either way, everyone I see using
C++ today uses templates too.

</aside>

<span name="base"></span>

^code templates

Using it looks like:

像这样使用它：

^code use-templates

<aside name="base">

The `Spawner` class here is so that code that doesn't care what kind of monster
a spawner creates can just use it and work with pointers to `Monster`.

If we only had the `SpawnerFor<T>` class, there would be no single supertype the
instantiations of that template all shared, so any code that worked with
spawners of any monster type would itself need to take a template parameter.

</aside>

### First-class types

### 第一公民类型

The previous two solutions address the need to have a class, `Spawner`, which is
parameterized by a type. In C++, types aren't generally first-class, so that
requires some <span name="type-obj">gymnastics</span>. If you're using a
dynamically-typed language like JavaScript, Python, or Ruby where classes *are*
 regular objects you can pass around, you can solve this much more directly.

前面的两个解决方案完成了需要一个类的需求，`Spawner`，那个以类型作为参数haunted的东西。在C++中，类型不是第一公民，所以需要一些艺术。如果你使用了一个动态类型语言比如JavaScript，Python，或者Ruby，他们的类*是*你可以传递的对象，你可以更加直接的解决这个问题。

<aside name="type-obj">

In some ways, the <a href="type-object.html" class="pattern">Type Object</a>
pattern is another workaround for the lack of first-class types. That pattern
can still be useful even in languages with them, though, because it lets *you*
define what a "type" is. You may want different semantics than what the
language's built-in classes provide.

</aside>

When you make a spawner, just pass in the class of monster that it should
construct -- the actual runtime object that represents the monster's
class. Easy as pie.

当你完成一个产出者，直接向他传递一个它应该构建的怪物的类型——那个真实的运行时对象，代表了怪物的类。小菜一碟。

With all of these options, I honestly can't say I've found a case where I felt
the Prototype *design pattern* was the best answer. Maybe your experience will
be different, but for now let's put that away and talk about something else:
prototypes as a *language paradigm*.

使用这些选项，我不能诚实的说我找到了一种情况，原型*设计模式*是最好的方案。也许你的经验有所不同，但是现在把它放到一边，讨论点别的：原型作为一种*语言范式*。

## The Prototype Language Paradigm

## 原型语言范式

Many people think "object-oriented programming" is synonymous with "classes".
Definitions of OOP tend to feel like credos of opposing religious denominations,
but a fairly non-contentious take on it is that *OOP lets you define "objects"
which bundle data and code together.* Compared to structured languages like C
and functional languages like Scheme, the defining characteristic of OOP is that
it tightly binds state and behavior together.

很多人认为“面向对象编程”和“类”是同义词。OOP的定义感觉像是相反宗教信仰的教义，公平无争议的是*OOP让你定义“对象”，将数据和代码绑定在一起。*与结构化的语言比如C相比，与函数语言比如Scheme相比，OOP的定义特性是它将状态和行为紧紧地绑在一起。

You may think classes are the one and only way to do that, but a handful of guys
including Dave Ungar and Randall Smith beg to differ. They created a language in
the 80s called Self. While as OOP as can be, it has no classes.

你也许认为类是唯一可以完成这个的方法，但是一大堆家伙包括Dave Ungar和Randall Smith一直拼命区分。他们在80年代床架了一种语言叫做Self。它可以是OOP的，同时没有类

### Self

### Self语言

In a pure sense, Self is *more* object-oriented than a class-based language. We
think of OOP as marrying state and behavior, but languages with classes actually
have a line of separation between them.

纯粹的感觉上，Self比一个基于类的语言*更加*面向对象。我们人为OOP是将状态和行为绑在一起，但是有类的于洋实际在他们之间画了一条线。

Consider the semantics of your favorite class-based language. To access some
state on an object, you look in the memory of the instance itself. State is
*contained* in the instance.

考虑你最喜欢的基于类的语言语义。为了接触一个对象中的一些状态，你查询内存中实例。状态是*包含*在实例之中的。

To invoke a <span name="vtable">method</span>, though, you look up the
instance's class, and then you look up the method *there*. Behavior is contained
in the *class*. There's always that level of indirection to get to a method,
which means fields and methods are different.

但是，为了调用一个方法，你查询实例的类，然后你在*那里*查询方法。行为被包含在*类*中。这里总有一次跳转来获得一个方法，这意味着字段和方法是不同的。

<img src="images/prototype-class.png" alt="A Class contains a list of Methods. An Instance contains a list of Fields and a reference to its Class." />

<aside name="vtable">

For example, to invoke a virtual method in C++, you look in the instance for the
pointer to its vtable, then look up the method there.

</aside>

Self eliminates that distinction. To look up *anything*, you just look on the
object. An instance can contain both state and behavior. You can have a single
object that has a method completely unique to it.

Self结束了这种分歧。找*任何东西*，你需要在对象中找。一个实例同时包含状态和行为。你可以有一个对象，他有一个完全独特的方法。

<span name="island"></span>

<img src="images/prototype-object.png" alt="An Object contains a mixed list of Fields and Methods." />

<aside name="island">

No man is an island, but this object is.

</aside>

If that was all Self did, it would be hard to use. Inheritance in class-based
languages, despite its faults, gives you a useful mechanism for reusing
polymorphic code and avoiding duplication. To accomplish something similar
without classes, Self has *delegation*.

如果这就是Self做的，那它很难使用。在基于类的语言中继承，不管他的缺陷，给你了有用的机制来重用多态代码和避免复制。为了不用类而实现一些相同的东西，Self拥有*授权*。

To find a field or call a method on some object, we first look in the object
itself. If it has it, we're done. If it doesn't, we look at the object's <span
name="parent">*parent*</span>. This is just a reference to some other object.
When we fail to find a property on the first object, we try its parent, and its
parent, and so on. In other words, failed lookups are *delegated* to an object's
parent.

为了在对象中寻找字段或者调用方法，我们首先在对象自己里查找。如果它有，我们就完成了。如果它没有，我吗在对象的*父类*中寻找。这是一个对其他对象的引用。当我们没能在第一个对象中找到属性，我们尝试它的父母，然后父母的父母，继续下去。换言之，失败的查找被*授权*给对象的父母。

<aside name="parent">

I'm simplifying here. Self actually supports multiple parents. Parents are just
specially marked fields, which means you can do things like inherit parents or
change them at runtime, leading to what's called *dynamic inheritance*.

</aside>

<img src="images/prototype-delegate.png" alt="An Object contains Fields and Methods and a reference to another object that it delegates to." />

Parent objects let us reuse behavior (and state!) across multiple objects, so
we've covered part of the utility of classes. The other key thing classes do is
give us a way to create instances. When you need a new thingamabob, you can just
do `new Thingamabob()`, or whatever your preferred language's syntax is. A class
is a factory for instances of itself.

父母对象让我们在不同对象间重用行为（还有状态！），所以我们覆盖了类的公共部分。另外一个类做的关键事情就是给了我们一种方法来创建实例。但你需要一个新的某物，你可以直接`new Thingamabob()`，或者无论什么你喜欢的语言表达法。一个类是实例本身的工厂。

Without classes, how do we make new things? In particular, how do we make a
bunch of new things that all have stuff in common? Just like the design pattern,
the way you do this in Self is by *cloning*.

不用类，我们怎样创建新东西？特别的，我们如何创建一堆所有东西都是一样的新东西？就像设计模式，你在Self中的方式是使用*克隆*。

In Self, it's as if *every* object supports the Prototype design pattern
automatically. Any object can be cloned. To make a bunch of similar objects, you:

在Self中，就好像*每一个*对象都自动支持原型设计模式。任何对象都能被克隆。为了获得一堆相似的对象，你：

1. Beat one object into the shape you want. You can just clone the base `Object`
   built into the system and then stuff fields and methods into it.

2. Clone it to make as many... uh... clones as you want.

1. 讲一个对象拍打成你想要的形状。你可以直接克隆系统内建的基本`Object`，然后向其中添加字段和方法。

2. 克隆它来产出……额……你想要的克隆数量。

This gives us the elegance of the Prototype design pattern without the tedium of
having to implement `clone()` ourselves; it's built into the system.

这就给了我们原型模式的优雅而无需我们自己沉闷的实现`clone()`；它被内建子啊系统中。

This is such a beautiful, clever, minimal system that as soon as I learned about
it, <span name="finch">I started</span> creating a prototype-based language to get more experience with it.

这是一个如此美妙，紧密最小化的系统，当我一听说它，我就开始创建一个基于原型的语言来获取更多经验。

<aside name="finch">

I realize building a language from scratch is not the most efficient way to
learn, but what can I say? I'm a bit peculiar. If you're curious, the language
is called [Finch](http://finch.stuffwithstuff.com/).

</aside>

### How did it go?

### 它运行的如何？

I was super excited to play with a pure prototype-based language, but once I had
mine up and running, I <span name="no-fun">discovered</span> an unpleasant fact:
it just wasn't that fun to program in.

我急切的想要使用一个纯粹的基于原型的语言，但是一旦我完成了并运行它，我发现了一个不愉快的事实：在其上编程没有那样有趣。

<aside name="no-fun">

I've since heard through the grapevine that many of the Self programmers came to
the same conclusion. The project was far from a loss, though. Self was so
dynamic that it needed all sorts of virtual machine innovations in order to run
fast enough.

The ideas they invented for just-in-time compilation, garbage collection, and
optimizing method dispatch are the exact same techniques -- often implemented by
the same people! -- that now make many of the world's dynamically-typed
languages fast enough to use for massively popular applications.

</aside>

Sure, the language was simple to implement, but that was because it punted the
complexity onto the user. As soon as I started trying to use it, I found myself
missing the structure that classes give. I ended up trying to recapitulate it at
the library level since the language didn't have it.

是的，语言很容易实现，那是因为它把复杂度推给了用户。一旦我们开始试着使用它，我发现我想念类给的结构。在库级别我最终停止了概括它，因为语言并没有。

Maybe this is because my prior experience is in class-based languages, so
my mind has been tainted by that paradigm. But my hunch is that most people just
like well-defined "kinds of things".

谢雨这是因为我之前的经验是在基于类的语言上，所以我的头脑被那个范式污染了。但是我的直觉是大部分人还是喜欢好好定义的“那些事物”。

In addition to the runaway success of class-based languages, look at how many
games have explicit character classes and a precise roster of different sorts
of enemies, items, and skills, each neatly labeled. You don't see many games
where each monster is a unique snowflake, like "sort of halfway between a troll
and a goblin with a bit of snake mixed in".

除去基于类的语言的成功，看看有多少游戏明确指定了玩家类和不同种的敌人物品技能的清单，每一个都有整齐的标签。你你在不会再游戏中看到每一个怪物都是独立的雪团，像“洞穴人和哥布林还有一些雪混合在一起”的东西。

While prototypes are a really cool paradigm and one that I wish more people
knew about, I'm glad that most of us aren't actually programming using them
every day. <span name="telling">The code</span> I've seen that fully embraces
prototypes has a weird mushiness to it that I find hard to wrap my head around.

原型是非常酷的范式，也是我希望有更多人了解的东西，我很庆幸我们不是每天都用他们编程。我看到的完全皈依原型的语言是一团浆糊，我很难用它完成点什么。

<aside name="telling">

It's also telling how *little* code there actually is written in a prototypal
style. I've looked.

</aside>

### What about JavaScript?

### JavaScript又怎么样呢？

OK, if prototype-based languages are so unfriendly, how do I explain JavaScript?
Here's a language with prototypes used by millions of people every day. More
computers run JavaScript than any other language on Earth.

好吧，如果基于原型的于洋是不那么友好，我怎么解释JavaScript呢？这是一个有原型的语言，每天都被数百万人使用。运行JavaScript的机器数量超过了地球上其他所有的语言。

<span name="ten">Brendan Eich</span>, the creator of JavaScript, took
inspiration directly from Self, and many of JavaScript's semantics are
prototype-based. Each object can have an arbitrary set of properties, both
fields and "methods" (which are really just functions stored as fields). An
object can also have another object, called its "prototype", that it delegates
to if a field access fails.

Brendan Eich，JavaScript的缔造者，从Self中直接吸收灵感，很多JavaScript的语义都是基于原型的。每一个对象都有任意集合的特性，字段和“方法”（事实上只是存储为字段的函数）都是如此。一个对象可以拥有另一个对象，被称为它的“原型”，如果字段获取失败就会获得它。

<aside name="ten">

As a language designer, one appealing thing about prototypes is that they are
simpler to implement than classes. Eich took full advantage of this: the first
version of JavaScript was created in ten days.

</aside>

But, despite that, I believe that JavaScript in practice has more in common with
class-based languages than with prototypal ones. One hint that JavaScript has
taken steps away from Self is that the core operation in a prototype-based
language, *cloning*, is nowhere to be seen.

但是不管那个，我相信JavaScript在事件中更像是基于类的而不是基于原型的。一个关键点是JavaScript将一些基于原型的语言核心操作取了出来，*克隆*，不见了。

There is no method to clone an object in JavaScript. The closest it has is
`Object.create()`, which lets you create a new object that
delegates to an existing one. Even that wasn't added until ECMAScript 5,
fourteen years after JavaScript came out. Instead of cloning, let me walk you
through the typical way you define types and create objects in JavaScript. You
start with a *constructor function*:

在JavaScript中没有方法来克隆一个对象。最接近的是`Object.create()`，允许你创建一个新的对象代表现有的。那个在ECMAScript5之前都没有添加，那是JavaScript出现后的十四年了。不用克隆，让我带你浏览一下JavaScript中in定义类和创建对象的经典方法。从一个*构造器函数*开始：

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

这里的`new`引入了`Weapon()`函数的实体，而`this`绑定到一个新的空对象上。实体添加了一系列字段到它上面，然后现在填满的对象自动返回了。

The `new` also does one other thing for you. When it creates that blank object,
it wires it up to delegate to a prototype object. You can get to that object
directly using `Weapon.prototype`.

`new`同样为你做了另外一件事情。当他创建那个空对象，它将它和一个原型对象连接起来了。你可以用`Weapon。prototype`来直接使用它。

While state is added in the constructor body, to define *behavior*, you usually
add methods to the prototype object. Something like this:

当状态被添加到构建器中，为了定义*行为*，你通常为原型对象添加方法。就像这样：

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

这添加了一个`attack`属性给武器原型，值是一个函数。由于每一个对象都通过代表`Weapon.prototype`的`new Weapon()`返回，你现在可以调用一个`sword.attack()`，他会调用那个函数。这看上去像是这样：

<img src="images/prototype-weapon.png" alt="A Weapon object contains an attack() method and other methods. A Sword object contains fields and delegates to Weapon." />

Let's review:

让我们回顾一下：

* The way you create objects is by a "new" operation that you invoke using an
  object that represents the type -- the constructor function.

* 你创建对象的方法是通过“new”操作，引入一个代表类型的对象——构造器函数。

* State is stored on the instance itself.

* 状态被存储在实例本身中。

* Behavior goes through a level of indirection -- delegating to the prototype --
  and is stored on a separate object that represents the set of methods shared
  by all objects of a certain type.

* 行为通过系列跳转——指向原型的代表——被存储在一个分散的对象中，代表了一系列特定类型的对象共享的方法。

Call me crazy, but that sounds a lot like my description of classes earlier. You
*can* write prototype-style code in JavaScript (*sans* cloning), but the syntax
and idioms of the language encourage a class-based approach.

说我疯了吧，但这听起来很像是我之前描述的类。你*可以*写原型风格的代码在JavaScript中（*没有*克隆），但是语言的语义和通常方法推荐一个基于类的方法。

Personally, I think that's a <span name="good">good thing</span>. Like I said, I
find doubling down on prototypes makes code harder to work with, so I like that
JavaScript wraps the core semantics in something a little more classy.

个人而言，我认为这是一个好事。就像我说的，我发现完全使用原型让代码很难处理，所以我喜欢JavaScript间整个核心语义包裹在一些更漂亮的东西中。

## Prototypes for Data Modeling

## 为数据模型建立原型

OK, I keep talking about things I *don't* like prototypes for, which is making
this chapter a real downer. I think of this book as more comedy than tragedy, so
let's close this out with an area where I *do* think prototypes, or more
specifically *delegation*, can be useful.

好吧，我不断的讨论我*不喜欢*原型的原因，这让这一章有些黯淡。我认为这本书该比惨案更加欢乐些，所以让我们结束这个话题在我*确实*考虑原型的地方，或者更加精确的，*委托*，是有用的。

If you were to count all the bytes in a game that are code compared to the ones
that are data, you'd see the fraction of data has been increasing steadily since
the dawn of programming. Early games procedurally generated almost everything so
they could fit on floppies and old game cartridges. In many games today, the
code is just an "engine" that drives the game, which is defined entirely in
data.

如果你想要计算中游戏的字节数并与数据的相比，你可以看到游戏的部分随着程序的进行稳定的增长。早期的游戏在程序中生成几乎任何东西，这样他们可以适应磁盘和老式游戏盒。子啊今日的游戏中，代码只是驱动游戏的“引擎”，游戏是完全由数据定义的。

That's great, but pushing piles of content into data files doesn't magically
solve the organizational challenges of a large project. If anything, it makes it
harder. The reason we use programming languages is because they have tools for
managing complexity.

这很好，但是将内容推到数据文件中不会魔法般的解决组织大工程的挑战。如果有什么区别的话，它让它更难了。我们使用编程语言的原因就是他们有办法处理复杂性。

Instead of copying and pasting a chunk of code in ten places, we move it into a
function that we can call by name. Instead of copying a method in a bunch of
classes, we can put it in a separate class that those classes inherit from or
mix in.

不是讲一堆代码拷来拷去，我们将其移入一个函数中，我们可以通过名字调用。不是在一堆类之间复制方法，我可以将其放入分离的类中，其他类可以从它继承或者把它混合进来。

When your game's data reaches a certain size, you really start wanting similar
features. Data modeling is a deep subject that I can't hope to do justice here,
but I do want to throw out one feature for you to consider in your own games:
using prototypes and delegation for reusing data.

当你的游戏数据达到一定规模时，你真的需要考虑更加简单的特性。数据模式是一个我不能指望在这里能说清的问题，但是我确实希望将一个特性扔给你在你的游戏中考虑：使用原型和委托来重要数据。

Let's say we're defining the data model for the <span name="shameless">shameless
Gauntlet rip-off</span> I mentioned earlier. The game designers need to specify
the attributes for monsters and items in some kind of files.

让我们假设我们为我早先提到的无耻挑战定义数据模型。游戏设计者需要特定怪物和事物在很多文件中定义它们的属性。

<aside name="shameless">

I mean completely original title in no way inspired by any previously existing
top-down multi-player dungeon crawl arcade games. Please don't sue me.

</aside>

One common approach is to use JSON. Data entities are basically *maps*, or
*property bags*, or any of a dozen other terms because there's nothing
programmers like more than <span name="inventing">inventing</span> a new name
for something that already has one.

一个通用的方法是使用JSON。数据实体是基本“图”，或者“属性包”，或者其他什么术语，因为程序员就喜欢为已有的东西发明新名字。

<aside name="inventing">

We've re-invented them so many times that Steve Yegge calls them ["The Universal
Design Pattern"](http://steve-yegge.blogspot.com/2008/10/universal-design-patter
n.html).

</aside>

So a goblin in the game might be defined something like this:

所以游戏中的一个哥布林也许被定义为像这样的东西：

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

这看上去很直观，哪怕是最讨厌文字的设计者也能完成。所以你给哥布林大家族添加几个兄弟分支：

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

现在，如果这是代码，我们的美感就被刺激到了。这里在实体间有很多的重复性，训练优良的程序员*讨厌*它。它浪费了空间，消耗了作者更多时间。你需要仔细阅读代码才知道这些数据*是不是*相同的。这让维护头疼。如果我们决定让所有的哥布林在游戏中变强，我们需要记住为他们三个都更新一遍。糟糕糟糕糟糕。

If this was code, we'd create an abstraction for a "goblin" and reuse that
across the three goblin types. But dumb JSON doesn't know anything about that.
So let's make it a bit smarter.

如果这是代码，我们会为“哥布林”创造一个抽象，然后子啊是那种哥布林类型中重用他们。但是愚蠢的JSON不知道这么做。素以让我们把它做得更加巧妙。

We'll declare that if an object has a <span name="meta">`"prototype"`</span>
field, then that defines the name of another object that this one delegates to.
Any properties that don't exist on the first object fall back to being looked up
on the prototype.

我们声明它好像一个对象有“原型”字段，然后定义这个对象委托给的另一个对象。任何不在第一个对象中的原型都退回在原型中查找。

<aside name="meta">

This makes the `"prototype"` a piece of *meta*data instead of data. Goblins have
warty green skin and yellow teeth. They don't have prototypes. Prototypes are a
property of the *data object representing the goblin*, and not the goblin
itself.

</aside>

With that, we can simplify the JSON for our goblin horde:

通过这样，我们可以简单的定义一大群哥布林：

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

由于弓箭手有他们grunt作为原型，我们不需要在他们中重复健康，防御和弱点。我们给数据增加的新逻辑超简单——基本的单一委托——但是我们已经摆脱了一堆重复的东西。

One interesting thing to note here is that we didn't set up a fourth "base
goblin" *abstract* prototype for the three concrete goblin types to delegate to.
Instead, we just picked one of the goblins who was the simplest and delegated to
it.

一个有趣的事情是我们没有为这三个具体的哥布林委托的*抽象*原型设置任何“基本哥布林”。相反，我们选择了一个最简单的哥布林，然后委托给它。

That feels natural in a prototype-based system where any object can be used as a
clone to create new refined objects, and I think it's equally natural here too.
It's a particularly good fit for data in games where you often have one-off
special entities in the game world.

在一个基于原型的系统中，当一个对象可以被克隆产生一个新对象是很自然的，而我认为在这里也一样自然。这特别适合于那些游戏世界中你有一项特殊之处的实体的数据。

Think about bosses and unique items. These are often refinements of a more
common object in the game, and prototypal delegation is a good fit for defining
those. The magic Sword of Head-Detaching, which is really just a longsword with
some bonuses, can be expressed as that directly:

考虑Boss和独特的事物，他们通常是更加平凡事物的重新定义，而原型委托是一个定义他们的好方法。断头魔剑，就是一个拥有加成的长剑，可以向下面这样表示：

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

游戏引擎中一点额外的能力就能让设计者更加方便地添加多种不同的武器和游戏世界中的动物，这种丰富度会取悦玩家。
