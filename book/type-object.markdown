^title Type Object
^section Behavioral Patterns

## Intent

## 意图

*Allow the flexible creation of new "classes" by creating a single class, each
instance of which represents a different type of object.*

*创造一个类来允许灵活的创造新“类”，类的每个实例都代表了不同类型的对象。*

## Motivation

## 动机

Imagine we're working on a fantasy role-playing game. Our task is to write the
code for the hordes of vicious monsters that seek to slay our brave hero.
Monsters have a bunch of different attributes: health, attacks, graphics,
sounds, etc., but for example purposes we'll just worry about the first two.

想象我们在制作一个超棒的角色扮演游戏。我们的任务是为一群想要杀死我们英雄的恶毒怪物编写代码。怪物有多个的属性：健康，攻击，图形，声音，等等。但是为了演示目的我们先只考虑前面两个。

Each monster in the game has a value for its current health. It starts out full,
and each time the monster is wounded, it diminishes. Monsters also have an
attack string. When the monster attacks our hero, that text will be shown to the
user somehow. (We don't care how here.)

游戏中的每一个怪物都有一个当前健康的值。它开始时是满的，每一次怪物受伤，它就下降。关务也有一个攻击字符。当怪物攻击我们的英雄，那个文本就会以某种方式展示给用户。（我们不在乎这里怎样实现。）

The designers tell us that monsters come in a variety of different *breeds*,
like "dragon" or "troll". Each breed describes a *kind* of monster that exists
in the game, and there can be multiple monsters of the same breed running around
in the dungeon at the same time.

设计者告诉我们怪物有不同*品种*，像“龙”或者“巨魔”。每一个品种都描述了一*种*存在于游戏中的怪物，同时可能有多个同种怪物在地牢里游荡。

The breed determines a monster's starting health -- dragons start off with more
than trolls, making them harder to kill. It also determines the attack string --
all monsters of the same breed attack the same way.

品种决定了怪物的初始健康——龙开始比巨魔多，让他们更难被杀死。这也决定了攻击字符——同种的所有怪物都以相同方式攻击。

### The typical OOP answer

### 传统的面向对象方案

With that game design in mind, we fire up our text editor and start coding. According
to the design, a dragon <span name="isa">is a</span> kind of monster, a troll is
another kind, and so on with the other breeds. Thinking object-oriented, that
leads us to a `Monster` base class:

脑中有这样的设计方案，我们启动了我们的文本编辑器开始编程。根据设计，龙是一种怪物，巨魔是另一种，其他种类的也是一样。考虑面向对象，这引导我们创建一个`Monster`基类。

<aside name="isa">

This is the so-called "is-a" relationship. In conventional OOP thinking, since a
dragon "is-a" monster, we model that by making `Dragon` a subclass of `Monster`. As
we'll see, subclassing is only one way of enshrining a conceptual relation like
that into code.

</aside>

^code 1

The public `getAttack()` function lets the combat code get the string that
should be displayed when the monster attacks the hero. Each derived breed class
will override this to provide a different message.

公开的`getAttack()`函数让战斗代码获得在怪物攻击英雄时需要显示的文字。每一个推导类都需要重载它来提供一个不同的消息。

The constructor is protected and takes the starting health for the monster.
We'll have derived classes for each breed that provide their own public
constructors that call this one, passing in the starting health that is
appropriate for that breed.

构造器是被保护的，需要传入怪物的初始健康。我们为每个品种的推导类提供公共构造器调用这一个，传入对于该品种适合的起始血量。

Now let's see a couple of breed subclasses:

现在让我们看看一些品种子类：

<span name="exclaim"></span>

^code 2

<aside name="exclaim">

Exclamation points make everything more exciting!

</aside>

Each class derived from `Monster` passes in the starting health and overrides
`getAttack()` to return the attack string for that breed. Everything works as
expected, and before long, we've got our hero running around slaying a variety of
beasties. We keep slinging code, and before we know it, we've got dozens of
monster subclasses, from acidic slimes to zombie goats.

每一个个从`Monster`推导出来的类都传入起始血量并重载`getAttack()`来返回那个品种的攻击字符串。所有事情都一如所料的工作，不久以后，我们的英雄就可以跑来跑去杀死各种野兽了。我们继续编程，在我们意识到之前，我们就有了成打的怪物子类，从酸泥怪到僵尸羊。

Then, strangely, things start to bog down. Our designers ultimately want to have
*hundreds* of breeds, and we find ourselves spending all of our time writing
these little seven-line subclasses and recompiling. It gets worse -- the designers
want to start tuning the breeds we've already coded. Our formerly productive
workday degenerates to:

然后，奇怪的是，事情陷入困境。我们的设计者最终想要*几百个*品种，但是我们发现我们花费了所有的时间在写这些启航子类和重新编译上。这会继续变糟——设计者想要协调我们已经编码的品种。我们之前的工作日退化成了：

1.  Get email from designer asking to change health of troll from
    48 to 52.

1. 收到设计者将巨魔的血量从48改到52的邮件。

2.  Check out and change `Troll.h`.

2. 签出并修改`Troll.h`。

3.  Recompile game.

3. 重新编译游戏。

4.  Check in change.

4. 签入修改。

5.  Reply to email.

5. 回复邮件。

6.  Repeat.

6. 重复。

We spend the day frustrated because we've turned into data monkeys. Our
designers are frustrated because it takes them forever to get a simple
number tuned. What we need is the ability to change breed stats without having
to recompile the whole game every time. Even better, we'd like designers to be
able to create and tune breeds without *any* programmer intervention at all.

我们度过了失意的一天，因为我们变成了数据猴子。我们的设计者感到挫败因为修改一个数据就要老久。我们需要的是无需每次重新编译游戏就能修改品种的状态。甚至更好的，我们想要设计者恩狗创建和修改品类无需*任何*程序员的介入。

### A class for a class

### 为类建类

At a very high level, the problem we're trying to solve is pretty simple. We
have a bunch of different monsters in the game, and we want to share certain
attributes between them. A horde of monsters are beating on the hero, and we
want some of them to use the same text for their attack. We define that by
saying that all of those monsters are the same "breed", and that the breed
determines the attack string.

在一个非常高的层次，我们试图解决的问题非常简单。游戏中有很多不同的怪物，我们想要在他们之间分享属性。一大群怪物子啊攻击英雄，我们想要他们其中的一些使用相同的攻击文本。我们通过声明这些怪物是相同“品种”，而那个品种决定了攻击字符串。

We decided to implement this concept using inheritance since it lines up with
our intuition of classes. A dragon is a monster, and each dragon in the game is
an instance of this dragon "class". Defining each breed as a subclass of an
abstract base `Monster` class, and having each monster in the game be an
instance of that derived breed class mirrors that. We end up with a class
hierarchy like this:

由于它与我们和类的直觉相关，我们决定实现这个模式。一条龙是一个怪物，每一条龙都是龙“类”的实例。定义每个品种是抽象基类`Monster` 的子类，让每个游戏中的怪物都是推导类的实例反映了那一点。我们最终由一个这样的类层次：

<span name="inherits-arrow"></span>

<img src="images/type-object-subclasses.png" alt="A Monster base class with derived classes for Dragon, Troll, etc." />

<aside name="inherits-arrow">

Here, the <img src="images/arrow-inherits.png" class="arrow" alt="A UML arrow representing inheritance." /> means "inherits
from".

</aside>

Each instance of a monster in the game will be of one of the derived monster
types. The more breeds we have, the bigger the class hierarchy. That's the
problem of course: adding new breeds means adding new code, and each breed has
to be compiled in as its own type.

每一个怪物的实例都是一个推导怪物类型。我们有的品种越多，类层次越大。这当然是问题：添加新品种就是添加新代码，而每一个品种都需要被编译为它自己的类型。

This works, but it isn't the only option. We could also architect our code so
that each monster *has* a breed. Instead of subclassing `Monster` for each
breed, we have a single `Monster` class and a single `Breed` class:

这可行，但不是唯一的选项。我们也可以架构代码为每一个怪物*有*一个品种。不是让每个品种继承`Monster`，我们现在有单一的`Monster`类和`Breed`类。

<span name="references-arrow"></span>

<img src="images/type-object-breed.png" alt="A Monster object has a reference to a Breed object." />

<aside name="references-arrow">

Here, the <img src="images/arrow-references.png" class="arrow" alt="A UML arrow for an object reference." /> means "is
referenced by".

</aside>

That's it. Two classes. Notice that there's no inheritance at all. With this
system, each monster in the game is simply an instance of class `Monster`. The
`Breed` class contains the information that's shared between all monsters of the
same breed: starting health and the attack string.

这就成了。两个类。注意这里完全没有继承。通过这个系统，游戏中的每一个怪物都是`Monster`的一个实例。`Breed`类包含了在不同品种怪物间分享的信息：开始健康和攻击字符串。

To associate monsters with breeds, we give each `Monster` instance a reference
to a `Breed` object containing the information for that breed. To get the attack
string, a monster just calls a method on its breed. The `Breed` class
essentially defines a monster's "type". Each breed instance is an *object* that
represents a different conceptual *type*, hence the name of the pattern: Type
Object.

为了将怪物与品种相关联，我们给了每一个`Monster`实例一个对包含品种信息的`Breed`对象的引用。为了获得攻击字符串，一个关守可以在对她的子类调用方法。`Breed`类本质上定义了一个怪物的*类型*，因此模式的名字是类型对象。

What's especially powerful about this pattern is that now we can define new
*types* of things without complicating the codebase at all. We've essentially
lifted a portion of the type system out of the hard-coded class hierarchy into
data we can define at runtime.

这个模式特别有用的一点是我们现在可以定义全新的*类型*而无需搅乱代码库。我们本质上讲一部分类型系统从硬编码的继承结构中拉出放到我们可以在运行时定义的数据中去。

We can create hundreds of different breeds by instantiating more instances
of `Monster` with different values. If we create breeds by initializing them from
data read from some configuration file, we have the ability to define new types
of monsters completely in data. So easy, a designer could do it!

我们可以通过通过用不同值实例化`Monster`来创建成百上千的新品种。如果通过从一些配置文件读取不同的数据来初始化品种，我们就有能力完全靠数据定义新怪物品种。这么容易，一个设计者也可以做到！

## The Pattern

## 模式

Define a **type object** class and a **typed object** class. Each type object
instance represents a different logical type. Each typed object stores a
**reference to the type object that describes its type**.

定义一个**类型对象**类和一个**有类型的对象**类。每一种类型对象实例代表一种不同的逻辑类型，每一种有类型的对象保存一个**描述它的类型的对类型对象的引用**。

Instance-specific data is stored in the typed object instance, and data or
behavior that should be shared across all instances of the same conceptual type
is stored in the type object. Objects referencing the same type object will
function as if they were the same type. This lets us share data and behavior
across a set of similar objects, much like subclassing lets us do, but without
having a fixed set of hard-coded subclasses.

实例相关的数据被存储在有类型对象实例中，应该被同种类型分享的数据或者行为存储子啊类型对象中。以用户相同类型对象的对象将会向他们是同一类型一样运作。这让我们在一组相托的对象间分享行为和数据，就像子类让我们做的那样，到那时没有固定的硬编码子类集合。

## When to Use It

## 何时使用

This pattern is useful anytime you need to define a variety of different "kinds"
of things, but baking the kinds into your language's type system is too rigid.
In particular, it's useful when either of these is true:

这个模式在任何你需要定义不同“种”，但是直接运用你语言的类型系统过于僵硬的时候。特别的，下面两者之一都是有用的：

 *  You don't know what types you will need up front. (For example, what if our
    game needed to support downloading content that contained new breeds of
    monsters?)

 *  你不知道你后面还需要什么类型。（举个例子，如果你的游戏需要支持下载后续新包含进来的怪物品种呢？）

 *  You want to be able to modify or add new types without having to recompile
    or change code.

 *  你想要能不编译或改变代码就能修改或添加新类型。

## Keep in Mind

## 记住

This pattern is about moving the definition of a "type" from the imperative but
rigid language of code into the more flexible but less behavioral world of
objects in memory. The flexibility is good, but you lose some things by hoisting
your types into data.

这个模型关于将“类型”的定义从必要但是僵硬的语言世界移到灵活但是更少行为的对象行为世界。灵活性很好，但是将类型提高到数据丧失了一些东西。

### The type objects have to be tracked manually

### 类型对象需要手动追踪

One advantage of using something like <span name="vtable">C++'s</span> type
system is that the compiler handles all of the bookkeeping for the classes
automatically. The data that defines each class is automatically compiled into
the static memory segment of the executable and just works.

使用C++类型系统的好处之一就是编译器控制了自动记录类的工作。定义每个类的数据自动编译到可执行的静态内存段然后工作。

With the Type Object pattern, we are now responsible for managing not only our
monsters in memory, but also their *types* -- we have to make sure all of the
breed objects are instantiated and kept in memory as long as our monsters need
them. Whenever we create a new monster, it's up to us to ensure that it's
correctly initialized with a reference to a valid breed.

使用类型对象模式，我们现在不但要负责管理内存中的怪物，同时要管理他们的*类型*——我们要保证所有的品种对象都被实例化并保存子啊内存中只要我的怪物需要它。无论何时我们创建了一个新的怪物，这由我们来保证它别初始化为有一个对合理种类的引用。

We've freed ourselves from some of the limitations of the compiler, but the cost
is that we have to re-implement some of what it used to be doing for us.

我们解放了一些编译器的限制，但是代价是我们需要重新实现一些它以前为我们做的事情。

<aside name="vtable">

Under the hood, C++ virtual methods are implemented using something called a
"virtual function table", or just "vtable". A vtable is a simple `struct`
containing a set of function pointers, one for each virtual method in a class.
There is one vtable in memory for each class. Each instance of a class has a
pointer to the vtable for its class.

When you call a virtual function, the code first looks up the vtable for the
object, then it calls the function stored in the appropriate function pointer in
the table.

Sound familiar? The vtable is our breed object, and the pointer to the vtable is
the reference the monster holds to its breed. C++ classes are the Type Object
pattern applied to C, handled automatically by the compiler.

</aside>

### It's harder to define *behavior* for each type

### 更难为每种对象定义*行为*

With subclassing, you can override a method and do whatever you want to --
calculate values procedurally, call other code, etc. The sky is the limit. We
could define a monster subclass whose attack string changed based on the phase
of the moon if we wanted to. (Handy for werewolves, I suppose.)

通过子类，你可以重载方法然后做你想做的事——用程序计算值，调用其他代码，等等。天就是限制。如果我们想的话，我们可以定义一个怪物子类，根据月亮的阶段改变它的攻击字符串。（就像是狼人，我觉得。）

When we use the Type Object pattern instead, we replace an overridden method
with a member variable. Instead of having monster subclasses that override a
method to *calculate* an attack string using different *code*, we have a breed
object that *stores* an attack string in a different *variable*.

当我们使用类型对象模式的时候，我们将重载方法替换成了一个成员变量。不是让怪物的子类重载一个方法用不同的*代码*来*计算*一个攻击字符串，我们有一个品种对象在不同的*变量*中*存储*攻击字符串。

This makes it very easy to use type objects to define type-specific *data*, but
hard to define type-specific *behavior*. If, for example, different breeds of
monster needed to use different AI algorithms, using this pattern becomes more
challenging.

这让使用类型对象定义类相关的*数据*变得容易，但是定义类型相关的*行为*困难。如果，举个例子，不同的品种的怪物需要使用不同的AI算法，使用这个模式就更加成为一个挑战。

There are a couple of ways we can get around this limitation. A simple solution
is to have a fixed set of pre-defined behaviors and then use data in the type
object to simply *select* one of them. For example, let's say our monster AI
will always be either "stand still", "chase hero", or "whimper and cower in
fear" (hey, they can't all be mighty dragons). We can define <span
name="fn">functions</span> to implement each of those behaviors. Then, we can
associate an AI algorithm with a breed by having it store a pointer to
the appropriate function.

这有很多方式我们可以跨越这个限制。一个简单的方式是使用一个固定的预先定义的行为然后使用类型对象中的数据简单的*选择*他们中的一个。举例，假设我们的怪物AI总是要么“站着不动”“追逐英雄”或者“恐惧地呜咽颤抖”（嘿，他们不可能都是强势的龙）。我们可以定义函数来实现每一种行为。然后，我们通过存储指向合适函数的方法将AI算法与品种相关联。

<aside name="fn">

Sound familiar again? Now we're back to really implementing vtables in *our*
type objects.

</aside>

Another more powerful solution is to actually support defining behavior
completely in <span name="data">data</span>. The <a class="gof-pattern"
href="http://c2.com/cgi-bin/wiki?InterpreterPattern">Interpreter</a> and <a
class="pattern" href="bytecode.html">Bytecode</a> patterns both let us build
objects that represent behavior. If we read in a data file and use that to
create a data structure for one of these patterns, we've moved the behavior's
definition completely out of code and into content.

另一个更加有力的解决方案是真正的子啊数据中支持定义行为。翻译器模式和字节码模式让我们定义有行为的对象。如果我们阅读数据文件然后用它为两者之一构建数据结构，我们就将行为完全从代码中移出放入了内容。

<aside name="data">

Over time, games are getting more data-driven. Hardware gets more powerful, and
we find ourselves limited more by how much content we can author than how hard
we can push the hardware. With a 64K cartridge, the challenge was *cramming* the
gameplay into it. With a double-sided DVD, the challenge is *filling* it with
gameplay.

Scripting languages and other higher-level ways of defining game behavior can
give us a much needed productivity boost, at the expense of less optimal runtime
performance. Since hardware keeps getting better but our brainpower doesn't,
that trade-off starts to make more and more sense.

</aside>

## Sample Code

## 示例代码

For our first pass at an implementation, let's start simple and build the basic
system described in the motivation section. We'll start with the `Breed`
class:

在我们的第一次实现过程中，让我们从简单的开始，只建构动机那节提到的基础系统。我们从`Breed`类开始：

^code 3

Very simple. It's basically just a container for two data fields: the starting
health and the attack string. Let's see how monsters use it:

很简单。它基本上只是两个数据字段的容器：起始血量和攻击字符串。让我们看看怪物怎么使用它：

^code 4

When we construct a monster, we give it a reference to a breed object. This
defines the monster's breed instead of the subclasses we were previously using.
In the constructor, `Monster` uses the breed to determine its starting health.
To get the attack string, the monster simply forwards the call to its breed.

当我们建构怪物，我给它一个种类对象的引用。这定义了了关务的种类而不是用我们之前的子类。在构造器中，`Monster`使用种类决定它的起始血量。为了得到攻击字符串，怪物简单的将调用转发给它的种类。

This very simple chunk of code is the core idea of the pattern. Everything from
here on out is bonus.

这种非常简单的代码是这章的核心思路。由此得出的任何东西都是红利。

### Making type objects more like types: constructors

### 让类型对象更像类型：构造器

With what we have now, we construct a monster directly and are responsible for
passing in its breed. This is a bit backwards from how regular objects are
instantiated in most OOP languages -- we don't usually allocate a blank chunk of
memory and then *give* it its class. Instead, we call a constructor function on
the class itself, and it's responsible for giving us a new instance.

使用我们现有的，我们可以直接构造一个怪物然后负责传入它的种类。这比常用的OOP语言实现的对象有些退步——我们通常不分配一块空白内存然后*给*它它的类。相反，我们根据类调用构造器，它负责给我们一个新实例。

We can apply this same <span name="pattern">pattern</span> to our type objects:

我们可以在我们的类型对象上应用同样的模式。

^code 5

<aside name="pattern">

"Pattern" is the right word here. What we're talking about is one of the classic
patterns from Design Patterns: <a class="gof-pattern"
href="http://c2.com/cgi/wiki?FactoryMethodPattern">Factory Method</a>.

In some languages, this pattern is applied for constructing *all* objects. In
Ruby, Smalltalk, Objective-C, and other languages where classes are objects, you
construct new instances by calling a method on the class object itself.

</aside>

And the class that uses them:

以及那个使用他们的类：

^code 6

The key <span name="friend">difference</span> is the `newMonster()` function in
`Breed`. That's our "constructor" factory method. With our original
implementation, creating a monster looked like:

核心不同是`Breed`中的`newMonster()`。这是我们的“构造器”工厂方法。使用我们原先的是吸纳，创建一个怪物看上去像：

<aside name="friend">

There's another minor difference here. Because the sample code is in C++, we can
use a handy little feature: *friend classes.*

We've made `Monster`&rsquo;s constructor private, which prevents anyone from
calling it directly. Friend classes sidestep that restriction so `Breed` can
still access it. This means the *only* way to create monsters is by going
through `newMonster()`.

</aside>

^code 7

After our changes, it's like this:

在我们改动后，它看上去是：

^code 8

So, why do this? There are two steps to creating an object: allocation and
initialization. `Monster`&rsquo;s constructor lets us do all of the
initialization we need. In our example, that's only storing the breed, but a
full game would be loading graphics, initializing the monster's AI, and doing
other set-up work.

所以，为什么这么做？这里有两步来创作一个对象：分配和初始化。`Monster`的构造器让我们做完了所有我们需要的初始化。在我们的例子中，暗示存储类型的唯一地方，但是一个完整的游戏需要加载图形，初始化怪物AI以及做其他的设置工作。

However, that all happens *after* allocation. We've already got a chunk of
memory to put our monster into before its constructor is called. In games, we
often want to control that aspect of object creation too: we'll typically use
things like custom allocators or the <a class="pattern"
href="object-pool.html">Object Pool</a> pattern to control where in memory our
objects end up.

但是，那都发生在分配*之后*。我们已经在构造器调用前找到了一块内存放置我们的怪物。在游戏中，我们通常也想控制对象创造的层面：我们通常使用自定义的分配器或者对象池模式来控制我们的对象最终在内存的何处。

Defining a "constructor" function in `Breed` gives us a place to put that logic.
Instead of simply calling `new`, the `newMonster()` function can pull the memory
from a pool or custom heap before passing control off to `Monster` for
initialization. By putting this logic inside `Breed`, in the *only* function
that has the ability to create monsters, we ensure that all monsters go through
the memory management scheme we want.

在`Breed`中定义一个“构造器”函数给了我们地方放置这些逻辑。不是简单的调用`new`,`newMonster()`函数可以在将控制传递给`Monster`初始化之前从池中或堆中获取内存。通过放置这些逻辑在`Breed`内部*唯一*有能力创建怪物的函数中，我们保证了所有的怪物变量遵守了内存管理规范。

### Sharing data through inheritance

### 通过继承分享数据

What we have so far is a perfectly serviceable type object system, but it's
pretty basic. Our game will eventually have *hundreds* of different breeds, each
with dozens of attributes. If a designer wants to tune all of the thirty
different breeds of troll to make them a little stronger, she's got a lot of
tedious data entry ahead of her.

我们现在有的是一个完美服务性的类型对象系统，但是它非常简单。我们的游戏最终由*上百*中不同种类，每种都有成打的特性。如果一个设计者想要协调30中不同的巨魔让它们变得强壮一点，她就有冗长的数据入口摆在它面前。

What would help is the ability to share attributes across multiple *breeds* in
the same way that breeds let us share attributes across multiple *monsters*.
Just like we did with our original OOP solution, we can solve this using
inheritance. Only, this time, instead of using our language's inheritance
mechanism, we'll implement it ourselves within our type objects.

能帮上忙的是在不同的*种类*间分享属性的能力，一如种类让我们在不同的*怪物*间分享属性的能力。就像我们对之前OOP方案做的那样，我们可以使用继承完成这一点。只是，这次，不使用语言的继承机制，我们用类型对象实现它。

To keep things simple, we'll only support single inheritance. In the same way
that a class can have a parent base class, we'll allow a breed to have a parent
breed:

为了让事保持简单，我们只支持单继承。某种程度上一个类可以有一个父基类，我们允许品种有一个父品种。

^code 9

When we construct a breed, we give it a parent that it inherits from. We can
pass in `NULL` for a base breed that has no ancestors.

当我们构建一个品种，我们把它交给他继承的父母。我们可以为基础种类传入一个`NULL`表明它没有祖先。

To make this useful, a child breed needs to control which attributes are
inherited from its parent and which attributes it overrides and specifies itself. For our
example system, we'll say that a breed overrides the monster's health by having
a non-zero value and overrides the attack by having a non-`NULL` string.
Otherwise, the attribute will be inherited from its parent.

为了让这个有用，一个子品种需要控制它从父品种继承了什么东西，以及哪些属性它重载并由自己指定。对我们的例子系统，我们可以说一个种类用非零值重载了怪物的健康，通过非空字符串重载了攻击字符串。否则，这些属性要从它的父母继承。

There are two ways we can implement this. One is to handle the delegation
dynamically every time the attribute is requested, like this:

有两种方式我们可以实现之。一种是每次属性被请求是东塔处理委托，就像这样：

^code 10

This has the advantage of doing the right thing if a breed is modified at
runtime to no longer override, or no longer inherit some attribute. On the other
hand, it takes a bit more memory (it has to retain a pointer to its parent), and
it's slower. It has to walk the inheritance chain each time you look up an
attribute.

当种类在运行时修改他不为重载，或者不再继承某些属性时，这个有做正确的事的优势。另一方面，他需要更多的内存（他需要保存一个指针指向它的父母）而且更慢。每一次你寻找一个属性都需要回溯继承链。

If we can rely on a breed's attributes not changing, a faster solution is to
apply the inheritance at *construction time*. This is called "copy-down"
delegation because we *copy* inherited attributes *down* into the derived type
when it's created. It looks like this:

如果我们可以依赖一个品种的属性不改变，一个更快的解决方案是在*构造时*引用继承。这被称为“复制下来”委托，因为当创建对象时，我们*复制*继承的属性*吓到*推导的类型。它看上去是这样的：

^code copy-down

Note that we no longer need a field for the parent breed. Once the constructor
is done, we can forget the parent since we've already copied all of its
attributes in. To access a breed's attribute, now we just return the field:

现在我们不再需要一个字段给父种类了。一旦构造器完成，我们可以忘了父品种，因为我们已经拷贝了它的所有属性。为了获得一个种类的属性，我们现在直接返回字段：

^code copy-down-access

Nice and fast!

又好又快！

Let's say our game engine is set up to create the breeds by loading a JSON file
that defines them. It could look like:

假设我们的游戏引擎设置为加载一个定义品种的JSON文件后创建类型。他看上去是这样的：

    :::json
    {
      "Troll": {
        "health": 25,
        "attack": "The troll hits you!"
      },
      "Troll Archer": {
        "parent": "Troll",
        "health": 0,
        "attack": "The troll archer fires an arrow!"
      },
      "Troll Wizard": {
        "parent": "Troll",
        "health": 0,
        "attack": "The troll wizard casts a spell on you!"
      }
    }


We'd have a chunk of code that reads each breed entry and instantiates a new
breed instance with its data. As you can see from the `"parent": "Troll"`
fields, the `Troll Archer` and `Troll Wizard` breeds inherit from the base
`Troll` breed.

我们有一堆代码读取每个品种的入口，用新的数据实例化一个品种实例。就像你从`"parent": "Troll"`字段看到的， `Troll Archer`和`Troll Wizard`种类都由基础`Troll`种类继承而来。

Since both of them have zero for their health, they'll inherit it from the base
`Troll` breed instead. This means now our designer can tune the health in
`Troll` and all three breeds will be updated. As the number of breeds and the
number of different attributes each breed has increase, this can be a big
time-saver. Now, with a pretty small chunk of code, we have an open-ended system
that puts control in our designers' hands and makes the best use of their time.
Meanwhile, we can get back to coding other features.

由于他们的初始血量都是0，他们会从基础`Troll`种类继承。这意味着无论我们怎么协调`Troll`的学历三个种类都会被更新。随着种类的数量和属性的数量增加，这节约了很多时间。现在，哦听过一小块嗲吗，我们有一个开放式的系统给了我们设计者控制权，并让他们好好利用时间。同时，我们可以回去编码其他特性了。

## Design Decisions

## 设计决策

The Type Object pattern lets us build a type system as if we were designing our
own programming language. The design space is wide open, and we can do all sorts
of interesting stuff.

类型对象模式让我们建立一个类型系统就好像我们在设计我们自己的编程语言。设计空间是开发的，我们可以做很多有趣的事情。

In practice, a few things curtail our fancy. Time and maintainability will
discourage us from anything particularly complicated. More importantly, whatever
type object system we design, our users (often non-programmers) will need to be
able to easily understand it. The simpler we can make it, the more usable it
will be. So what we'll cover here is the well-trodden design space, and we'll
leave the far reaches for the academics and explorers.

在实践中，有些东西削减了我们的幻想。时间和可维护性阻止我们创建特别复杂的东西。更重要的是，无论我们设计了怎样的类型系统，我们的用户（通常不是程序员）要能轻松理解它。我们将其做的越简单，它就越有用。所以我们在这里覆盖的是陈旧的设计空间，我们会将其留给艺术家和探索者。

### Is the type object encapsulated or exposed?

### 类型对象是封装的还是暴露的？

In our sample implementation, `Monster` has a reference to a breed, but it
doesn't publicly expose it. Outside code can't get directly at the monster's
breed. From the codebase's perspective, monsters are essentially typeless, and
the fact that they have breeds is an implementation detail.

在我们的简单实现中，`Monster`有一个对种类的引用，但是它没有显式暴露它。外部代码不能直接获取怪物的品种。从代码库的角度看来，怪物基本上没有类型，事实上他们有种类是一个实现的细节。

We can easily change this and allow `Monster` to return its `Breed`:

我们可以很容易的改变这点，允许`Monster`返回`Breed`：

<span name="null"></span>

^code 11

<aside name="null">

As in other examples in this book, we're following a convention where we return
objects by reference instead of pointer to indicate to users that `NULL` will
never be returned.

</aside>

Doing this changes the design of `Monster`. The fact that all monsters have
breeds is now a publicly visible part of its API. There are benefits with either
choice.

这样做改变了`Monster`的设计。事实是所有这些由品种的怪物都是API的可见部分了，下面是这两者的好处：

 *  **If the type object is encapsulated:**

 *  **如果类型对象是封装的：**

     *  *The complexity of the Type Object pattern is hidden from the rest of
        the codebase.* It becomes an implementation detail that only the typed
        object has to worry about.

     *  *类型对象模式的复杂性对代码库的其他部分是隐藏的。*它成为了一种实现细节，只有有类型的对象需要考虑。

     *  *The typed object can selectively override behavior from the type
        object*. Let's say we wanted to change the monster's attack string when
        it's near death. Since the attack string is always accessed through
        `Monster`, we have a convenient place to put that code:

     *  *有联系的对象可以选择新的重载类型对象的行为*。假设我们想要怪物在它解决死亡的时候改变它的攻击字符串。由于攻击字符串总是通过`Monster`获取的，我们有一个方便的地方放置代码：

        ^code 12

        If outside code was calling `getAttack()` directly on the breed, we
        wouldn't have the opportunity to insert that logic.

        如果外部代码直接在种类上调用`getAttack()`，我们就没有机会能插入逻辑。

     *  *We have to write forwarding methods for everything the type object
        exposes.* This is the tedious part of this design. If our type object
        class has a large number of methods, the object class will have to have
        its own methods for each of the ones that we want to be publicly
        visible.

     *  *我们的为每件类型对象暴露的方法写转发。*这是这个设计的冗长之处。如果类型对象有很多方法，对象类也得为每一个建立它自己的公共可见方法。

 *  **If the type object is exposed:**

 *  **如果类型对象是暴露的：**

     *  *Outside code can interact with type objects without having an instance
        of the typed class.* If the type object is encapsulated, there's no way
        to use it without also having a typed object that wraps it. This
        prevents us, for example, from using our constructor pattern where new
        monsters are created by calling a method on the breed. If users can't
        get to breeds directly, they wouldn't be able to call it.

     *  *外部代码可以与类型对象直接交互，无需拥有一个类型对象的实例。*如果类型对象是封装的，如果没有一个包装它的有类型对象就没法使用它。这防止我们，举个例子，在调用种类的方法来创建一个新的怪物时使用构造器模式。如果用户不能直接获得种类，他们就没办法调用它。

     *  *The type object is now part of the object's public API.* In general,
        narrow interfaces are easier to maintain than wide ones -- the less you
        expose to the rest of the codebase, the less complexity and maintenance
        you have to deal with. By exposing the type object, we widen the
        object's API to include everything the type object provides.

     *  *类型对象现在是对象的公共API的一部分了。*大体上，狭窄的接口较宽的接口更容易掌控——你暴露给代码库其他部分的越少，你需要处理的复杂度和维护就越少。通过暴露类型对象，我们扩宽了对象的API来包含任何类型对象提供的。

### How are typed objects created?

### 有类型的对象是如何创建的？

With this pattern, each "object" is now a pair of objects: the main object and
the type object it uses. So how do we create and bind the two together?

使用这个模式，每一个“对象”现在都是一对对象：主对象和它用的类型对象。所以我们怎样创建并绑定两者呢？

 *  **Construct the object and pass in its type object:**

 *  **构造对象然后传入类型对象：**

     *  *Outside code can control allocation.* Since the calling code is
        constructing both objects itself, it can control where in memory that
        occurs. If we want our objects to be usable in a variety of different
        memory scenarios (different allocators, on the stack, etc.) this gives
        us the flexibility to do that.

     *  *外部代码可以控制分配。*由于调用代码也是构建对象的代码，他可以控制它放到内存的何处。乳沟我们想要我们的UI想在很多内存场景中使用（不同的分配器，在栈中，等等），这给了我们完成它的灵活度。

 *  **Call a "constructor" function on the type object:**

 *  **在类型对象上调用一个“构造器”函数：**

     *  *The type object controls memory allocation.* This is the other side of
        the coin. If we *don't* want users to choose where in memory our objects
        are created, requiring them to go through a factory method on the type
        object gives us control over that. This can be useful if we want to
        ensure all of our objects come from a certain <a class="pattern"
        href="object-pool.html">Object Pool</a> or other memory allocator.

     *  *类型对象控制了内存分配。*这是硬币的另一面。如果我们*不想*让玩家选择他们在内存中何处创建对象，需要他们在类型对象上调用工厂方法给了我们控制它的权利。在我们想抱着所有的对象都来自一个具体的对象池或者其他的内存分配器也有用。

### Can the type change?

### 类型能改变吗？

So far, we've presumed that once an object is created and bound to its type
object that that binding will never change. The type an object is created with
is the type it dies with. This isn't strictly necessary. We *could* allow an
object to change its type over time.

到目前为止，我们假设一旦对象创建并绑定到类型对象上，这种永远不会改变。对象创建时的类型就是他死亡时的类型。这没有那么必要。我们*可以*允许一个对象随着时间改变它的类型。

Let's look back at our example. When a monster dies, the designers tell us
sometimes they want its corpse to become a reanimated zombie. We could implement
this by spawning a new monster with a zombie breed when a monster dies, but
another option is to simply get the existing monster and change its breed to a
zombie one.

让我们看看我们的例子。当怪物死去，设计者告诉我们有时它的尸体会复活成僵尸。我们可以通过在怪物死亡时产生新的有僵尸类型的怪兽，但另一个选项是拿到现有的怪物然后将它的种类改为僵尸。

 *  **If the type doesn't change:**

 *  **如果类型不改变：**

     *  *It's simpler both to code and to understand.* At a conceptual level,
        "type" is something most people probably will not expect to change. This
        codifies that assumption.

     *  *编码和理解都更容易。*在概念上，“类型”是大多数人不期望改变的东西。这改变了那个假设。

     *  *It's easier to debug.* If we're trying to track down a bug where a
        monster gets into some weird state, it simplifies our job if we can take
        for granted that the breed we're looking at *now* is the breed the
        monster has always had.

     *  *更容易查漏。*如果再怪物进入一个奇怪状态的时候我们试图追踪漏洞，如果我们知道我们*现在*看到的种类就是怪物一直有的种类可以大大简化工作。

 *  **If the type can change:**

 *  **如果类型可以改变：**

     *  *There's less object creation.* In our example, if the type can't
        change, we'll be forced to burn CPU cycles creating a new zombie
        monster, copying over any attributes from the original monster that need
        to be preserved, and then deleting it. If we can change the type,
        all that work gets replaced by a simple assignment.

     *  *需要创建的对象更少。*在我们的例子中，如果类型不能改变，我们需要消耗CPU循环创建新的僵尸怪物对象，把原先对象中需要保留的属性都拷贝过来，然后删除它。乳沟我们可以改变类型，所有的工作都被一个简单的声明替代。

     *  *We need to be careful that assumptions are met.* There's a fairly tight
        coupling between an object and its type. For example, a breed might
        assume that a monster's *current* health is never above the starting
        health that comes from the breed.

     *  *我们需要小心符合假设。*在对象和它的类型间有强耦合是很自然的事情。举个例子，一个种类也许假设怪物*当前的*血量永远高于种类中的初始血量。

        If we allow the breed to change, we need to make sure that the new
        type's requirements are met by the existing object. When we change the
        type, we will probably need to execute some validation code to make sure
        the object is now in a state that makes sense for the new type.

        如果我们允许种类改变，我们需要确保新类型的需求被已存对象满足。但我们改变类型，我们也许需要执行一些验证代码保证对象现在的状态对新类型是有意义的。

### What kind of inheritance is supported?

### 它支持何种继承？

 *  **No inheritance:**

 *  **没有继承：**

     *  *It's simple.* Simplest is often best. If you don't have a ton of data
        that needs sharing between your type objects, why make things hard on
        yourself?

     *  *简单。*最简单的通常是最好的。如果你在类型对象间没有大量数据共享，为什么要为难自己呢？

     *  *It can lead to duplicated effort.* I've yet to see an authoring system
        where designers *didn't* want some kind of inheritance. When you've got
        fifty different kinds of elves, having to tune their health by changing
        the same number in fifty different places *sucks*.

     *  *这会带来重复的工作。*我从未见过看到设计者*不*想要继承的编码系统。当你有十五种不同的精灵时，协调血量就要修改十五处同样的数字糟透了。

 *  **Single inheritance:**

 *  **单一继承：**

     *  *It's still relatively simple.* It's easy to implement, but, more
        importantly, it's also pretty easy to understand. If non-technical users
        are going to be working with the system, the fewer moving parts, the
        better. There's a reason a lot of programming languages only support
        single inheritance. It seems to be a sweet spot between power and
        simplicity.

     *  *这还是相对简单。*它易于实现，但是，更重要的是，它也易于理解。如果非技术用户正在使用这个系统，移动的部分越少越好。这就是很多编程语言只支持单继承的原因。这看起来是能力和简洁之间的平衡点。

     *  *Looking up attributes is slower.* To get a given piece of data from a
        type object, we might need to walk up the inheritance chain to find
        the type that ultimately decides the value. If we're in
        performance-critical code, we may not want to spend time on this.

     *  *查询属性更慢。*为了从一个类型对象中获取一块数据，我们也许需要回溯继承链寻找哪一个类型最终决定了值。如果我们在性能攸关的代码桑，我们也许不想花时间在这上面。

 *  **Multiple inheritance:**

 *  **多重继承：**

     *  *Almost all data duplication can be avoided.* With a good multiple
        inheritance system, users can build a hierarchy for their type objects
        that has almost no redundancy. When it comes time to tune numbers, we
        can avoid a lot of copy and paste.

     *  *几乎可以避免所有的代码重复。*通过一个好的多继承系统，用户可以为他们的类型对象建立几乎没有冗余的层次。但是改变数字的时候，我们可以避免很多复制和粘贴。

     *  *It's complex.* Unfortunately, the benefits for this seem to be more
        theoretical than practical. Multiple inheritance is hard to understand
        and reason about.

     *  *复杂。* 不幸的是，它的好处看上去更多的是理论上的而非实际。多重继承很难理解。

        If our Zombie Dragon type inherits both from Zombie and Dragon, which
        attributes come from Zombie and which come from Dragon? In order to use
        the system, users will need to understand how the inheritance graph is
        traversed and have the foresight to design an intelligent hierarchy.

        如果我们的僵尸龙类型继承僵尸和龙，哪些属性来自僵尸，哪些来自于龙？为了使用系统，用户要理解如何横跨继承图并由设计一个优秀层次的愿景。

        Most C++ coding standards I see today tend to ban multiple inheritance,
        and Java and C# lack it completely. That's an acknowledgement of a sad
        fact: it's so hard to get it right that it's often best to not use it at
        all. While it's worth thinking about, it's rare that you'll want to use
        multiple inheritance for the type objects in your games. As always,
        simpler is better.

        我看到的大多数C++编码标准趋向于禁止多重继承，Java和C#完全移除了它。这承认了一个悲伤的事实：它太难掌握了，最好根本不要用。尽管它值得考虑，但很少你想要在类型对象上实现多重继承。就像往常一样，简单的总是最好的。

## See Also

## 参见

 *  The high-level problem this pattern addresses is sharing data and behavior
    between several objects. Another pattern that addresses the same problem in
    a different way is <a class="gof-pattern"
    href="prototype.html">Prototype</a>.

 *  这个模式处理的高层问题是子啊多个对象间分享数据和行为。另一个用另一种方式解决了相同问题的模式是原型模式。

 *  Type Object is a close cousin to <a class="gof-pattern"
    href="flyweight.html">Flyweight</a>. Both let you share data across
    instances. With Flyweight, the intent is on saving memory, and the shared
    data may not represent any conceptual "type" of object. With the Type Object
    pattern, the focus is on organization and flexibility.

 *  类型对象是享元模式的近亲。两者都让你在实例间分享代码。使用享元，意图是节约内存，而分享的数据也许不代表任何概念上的对象的“类型”使用类型对象模式，注意力集中在组织和灵活性。

 *  There's a lot of similarity between this pattern and the <a
    class="gof-pattern" href="state.html">State</a> pattern. Both patterns let
    an object delegate part of what defines itself to another object. With a
    type object, we're usually delegating what the object *is*: invariant data
    that broadly describes the object. With State, we delegate what an object
    *is right now*: temporal data that describes an object's current
    configuration.

 *  这个模式和状态模式有很多相似之处。两者都让它的部分委托给另外一个对象。拖过类型对象，我们通常委托对象*是*什么：不变的数据概括描述对象。通过状态，我们委托了对象*现在是什么*：暂时的数据描述对象当前的设置。

    When we discussed having an object change its type, you can look at that as
    having our Type Object serve double duty as a State too.

    当我们讨论一个对象改变它的类型时，你可以将其认为类型对象负起了和状态相似的职责。
