^title Component
^section Decoupling Patterns

^title 组件
^section Decoupling Patterns

## Intent

## 意图

*Allow a single entity to span multiple domains without coupling the domains to
each other.*

*允许单一的实体跨越多个领域，无需这些领域彼此耦合。*

## Motivation

## 动机

Let's say we're building a platformer. The Italian plumber demographic is
covered, so ours will star a Danish <span name="baker">baker</span>, Bjørn. It
stands to reason that we'll have a class representing our friendly pastry chef,
and it will contain everything he does in the game.

比方说，我们正构建平台跳跃游戏。
意大利水管工已经有人做了，因此我们将出动丹麦<span name="baker">面包师</span>，Bj?rn。
照理说，会有一个类来表示友好的糕点厨师，包含他在比赛中做的一切。

<aside name="baker">

Brilliant game ideas like this are why I'm a programmer and not a designer.

像这样的游戏创意是我是程序员而不是设计师的原因。

</aside>

Since the player controls him, that means reading controller input and
translating that input into motion. And, of course, he needs to interact with the level,
so some physics and collision go in there. Once that's done, he's got to show
up on screen, so toss in animation and rendering. He'll probably play some
sounds too.

由于玩家控制他，这意味着需要读取控制器的输入然后转化为运动。
而且，当然，他需要与关卡相互作用，所以要引入物理和碰撞。
一旦这样做了，他必须在屏幕上出现，所以要引入动画和渲染。
他可能还会播放一些声音。

Hold on a minute; this is getting out of control. Software Architecture 101
tells us that different domains in a program should be kept isolated from each
other. If we're making a word processor, the code that handles printing
shouldn't be affected by the code that loads and saves documents. A game doesn't
have the same domains as a business app, but the rule still applies.

等一下;这在失控。软件体系结构101课程告诉我们，程序的不同领域应保持分离。
如果我们做一个文字处理器，处理打印的代码不应该受加载和保存文件的代码影响。
游戏和企业应用程序不在相同的领域，但该规则仍然适用。

As much as possible, we don't want AI, physics, rendering, sound and other
domains to know about each other, but now we've got all of that crammed into one
class. We've seen where this road leads to: a 5,000-line dumping ground source
file so big that only the bravest ninja coders on your team even dare to go in
there.

我们希望AI，物理，渲染，声音和其他领域域尽可能相互不了解，
但现在我们将所有这一切挤在一个类中。
我们已经看到了这条路通往何处：5000行的巨大代码文件，哪怕是你们团队中最勇敢的程序员也不敢进去。

This is great job security for the few who can tame it, but it's hell for the rest of
us. A class that big means even the most seemingly trivial changes can have
far-reaching implications. Soon, the class collects *bugs* faster than it
collects *features*.

这对能驯服他的少数人是伟大的工作，但对其他人是地狱。
这么大的类意味着，即使是看似微不足道的变化亦可有深远的影响。
很快，为类添加*错误*速度明显快于添加*功能*。

### The Gordian knot

### 快刀斩乱麻

Even worse than the simple scale problem is the <span name="coupling">coupling
</span> one. All of the different systems in our game have been tied into a
giant knotted ball of code like:

比起单纯的规模问题，更糟糕的是<span name="coupling">耦合</span>。
在游戏中所有不同的系统被绑成了一个巨大的代码球：

^code gordian

Any programmer trying to make a change in code like that will need to know
something about physics, graphics, and sound just to make sure they don't break
anything.

任何试图改变代码的程序员，都需要知道一些关于物理，图形和声音的知识，只是为了确保不破坏什么。

<aside name="coupling">

While coupling like this sucks in *any* game, it's even worse on modern games
that use concurrency. On multi-core hardware, it's vital that code is running on
multiple threads simultaneously. One common way to split a game across threads
is along domain boundaries -- run AI on one core, sound on another, rendering on a
third, etc.

这样的耦合虽然在*任何*游戏中出现，但是对于使用并发的现代游戏更糟。
在多核硬件上，让代码同时在多个线程上运行是至关重要的。
一种将游戏分割到线程的通用方法是通过领域边界——在一个核上运行AI代码，在另一个上播放声音，在第三个上渲染，等等。

Once you do that, it's critical that those domains stay decoupled in order to
avoid deadlocks or other fiendish concurrency bugs. Having a single class with
an `UpdateSounds()` method that must be called from one thread and a
`RenderGraphics()` method that must be called from another is begging for
those kinds of bugs to happen.

一旦你这么做了，这些领域保持解耦就是至关重要的，这是为了避免死锁或者其他恶魔般的并发问题。
有从一个线程上调用的`UpdateSounds()`方法，从另一个线程上调用的`RenderGraphics()`方法是在祈求发生这样的问题。

</aside>

These two problems compound each other; the class touches so many domains that
every programmer will have to work on it, but it's so huge that doing so is a
nightmare. If it gets bad enough, coders will start putting hacks in other parts
of the codebase just to stay out of the hairball that this `Bjorn` class has
become.

这两个问题彼此混合;这个类涉及如此多的域，每个程序员都得接触它，
但它太过巨大，这就变成了一场噩梦。
如果变得够糟糕，程序员会黑入代码库的其他部分，仅仅为了躲开这个像毛球一样的`Bjorn`类。

### Cutting the knot

### 抽剑断结

We can solve this like Alexander the Great -- with a sword. We'll take our
monolithic `Bjorn` class and slice it into separate parts along domain
boundaries. For example, we'll take all of the code for handling user input and
move it into a separate `InputComponent` class. `Bjorn` will then own an
instance of this component. We'll repeat this process for each of the domains that
`Bjorn` touches.

我们可以像亚历山大大帝一样解决这个问题——用剑。
将`Bjorn`类切片成按领域独立的部分。
例如，我们抽出所有处理用户输入的代码，将其移动到一个单独的`InputComponent`类。
`Bjorn`拥有这个部件的一个实例。我们将对`Bjorn`接触的每个领域重复这一过程。

When we're done, we'll have moved almost everything out of `Bjorn`. All that
remains is a thin shell that binds the components together. We've solved our
huge class problem by simply dividing it up into multiple smaller classes, but
we've accomplished more than just that.

当完成后，我们将几乎所有`Bjorn`的东西都抽走了。
剩下的是一个薄壳包着所有的组件。
通过将类划分为多个小类，我们已经解决了这个问题。但我们完成了远远不止这些。

### Loose ends

### 宽松的结果

Our component classes are now decoupled. Even though `Bjorn` has a
`PhysicsComponent` and a `GraphicsComponent`, the two don't know about each
other. This means the person working on physics can modify their component
without needing to know anything about graphics and vice versa.

我们的组件类现在解耦了。
尽管`Bjorn`有`PhysicsComponent`和`GraphicsComponent`，
但这两部分都不知道对方的存在。
这意味着处理物理的人可以修改组件而不需要了解图形，反之亦然。

In practice, the components will need to have *some* interaction between
themselves. For example, the AI component may need to tell the physics component
where Bjørn is trying to go. However, we can restrict this to the
components that *do* need to talk instead of just tossing them all in the same
playpen together.

在实践中，这些部件需要在它们之间有*一些*相互作用。
例如，AI组件可能需要告诉物理组件
Bj?rn试图去哪里。
然而，我们可以限制这种交互在*确实*需要交互的组件之间，
而不是把它们围在同一个围栏里。

### Tying back together

### 绑到一起

Another feature of this design is that the <span
name="inheritance">components</span> are now reusable packages. So far, we've
focused on our baker, but let's consider a couple of other kinds of objects in
our game world. *Decorations* are things in the world the player sees but
doesn't interact with: bushes, debris and other visual detail. *Props* are like
decorations but can be touched: boxes, boulders, and trees. *Zones* are the
opposite of decorations -- invisible but interactive. They're useful for things
like triggering a cutscene when Bjørn enters an area.

这种设计的另一特性是，<span name="inheritance">组件</span>现在是可复用的包。
到目前为止，我们专注于面包师，但是让我们考虑几个游戏世界中其他类型的对象。
*装饰*是玩家看到但不能交互的事物：灌木，杂物等视觉细节。
*道具*像*装饰*，但可以交互：箱，巨石，树木。
*区*与装饰相反——无形的，但可互动。
它们是很好的触发器，比如在Bj?rn进入区域触发过场动画。

<aside name="inheritance">

When object-oriented programming first hit the scene, inheritance was the
shiniest tool in its toolbox. It was considered the ultimate code-reuse hammer,
and coders swung it often. Since then, we've learned the hard way that it's a
heavy hammer indeed. Inheritance has its uses, but it's often too cumbersome for
simple code reuse.

当面向对象语言第一次接触这个场景时，继承是它箱子里最闪耀的工具。
它被认为是代码无限重用之锤，编程者常常挥舞着它。
从那以后，我们以痛苦的方式学到，事实上它是一把重锤。
继承有它的用处，但对简单的代码重用太过复杂。

Instead, the growing trend in software design is to use composition instead of
inheritance when possible. Instead of sharing code between two classes by having
them *inherit* from the same class, we do so by having them both *own an
instance* of the same class.

相反，在今日软件设计的趋势是尽可能使用组件代替继承。不是让两个类*继承*同一类来分享代码，而是让它们*拥有同一个类的实例*。

</aside>

Now, consider how we'd set up an inheritance hierarchy for those classes if we
weren't using components. A first pass might look like:

现在，考虑如果不用组件，我们将如何建立这些类的继承层次。第一遍可能是这样的：

<img src="images/component-uml.png" alt="A class diagram. Zone has collision code and inherits from GameObject. Decoration also inherits from GameObject and has rendering code. Prop inherits from Zone but then has redundant rendering code." />

We have a base `GameObject` class that has common stuff like position and
orientation. `Zone` inherits from that and adds collision detection. Likewise,
`Decoration` inherits from `GameObject` and adds rendering. `Prop` inherits from
`Zone`, so it can reuse the collision code. However, `Prop` can't *also* inherit
from `Decoration` to reuse the *rendering* code without running into the <span
name="diamond">Deadly Diamond</span>.

我们有`GameObject`基类，包含位置和方向之类的通用部分。
`Zone`继承它，增加了碰撞检测。
同样，`Decoration`继承`GameObject`，并增加了渲染。
`Prop`继承`Zone`，因此它可以重用碰撞代码。
然而，`Prop`不能*同时*继承`Decoration`来重用*渲染*，
否则就会造成<span name="diamond">致命菱形</span>结构。

<aside name="diamond">

The "Deadly Diamond" occurs in class hierarchies with multiple inheritance where
there are two different paths to the same base class. The pain that causes is a
bit out of the scope of this book, but understand that they named it
"deadly" for a reason.

“致命菱形”发生在类继承了多个类，而这多个类中有两个继承同一基类。
介绍它引发的痛苦超过了本书的范围，但它被称为“致命”是有原因的。

</aside>

We could flip things around so that `Prop` inherits from `Decoration`, but then
we end up having to duplicate the *collision* code. Either way, there's no clean
way to reuse the collision and rendering code between the classes that need it
without resorting to multiple inheritance. The only other option is to push
everything up into `GameObject`, but then `Zone` is wasting memory on rendering
data it doesn't need and `Decoration` is doing the same with physics.

我们可以反过来让`Prop`继承`Decoration`，但随后不得不重复*碰撞*码。
无论哪种方式，没有干净方式重用碰撞和渲染代码而不诉诸多重继承。
唯一的其他选择是一切都继承`GameObject`，
但随后`Zone`浪费内存在并不需要的渲染数据上，
`Decoration`在物理效果上有同样的浪费。

Now, let's try it with components. Our <span name="menu">subclasses</span> disappear
completely. Instead, we have a single `GameObject` class and two component
classes: `PhysicsComponent` and `GraphicsComponent`. A decoration is simply a
`GameObject` with a `GraphicsComponent` but no `PhysicsComponent`. A zone is the
opposite, and a prop has both components. No code duplication, no multiple
inheritance, and only three classes instead of four.

现在，让我们尝试用组件。
<span name="menu">子类</span>彻底消失。
取而代之，一个`GameObject`类和两个组件类：`PhysicsComponent`和`GraphicsComponent`。
装饰是个简单的`GameObject`，包含`GraphicsComponent`但没有`PhysicsComponent`。
区与其恰好相反，而道具包含两种组件。
没有代码重复，没有多重继承，只有三个类，而不是四个。

<aside name="menu">

A restaurant menu is a good analogy. If each entity is a monolithic class, it's
like you can only order combos. We need to have a separate class for each
possible *combination* of features. To satisfy every customer, we would need
dozens of combos.

饭店菜单是个好类比。如果每个实体是一个类，那就像只能订套餐。
我们需要为每种*可能*组合定义分离的特性。
为了取悦每位用户，我们需要成打的套餐。

Components are &agrave; la carte dining -- each customer can select just the
dishes they want, and the menu is a list of the dishes they can choose
from.

组件是照单点菜——每位顾客都可以选他们想要的，菜单是可选的菜式。

</aside>

Components are basically plug-and-play for objects. They let us build complex
entities with rich behavior by plugging different reusable component
objects into sockets on the entity. Think software Voltron.

对对象而言，组件是即插即用的。
将不同的可重用部件插入对象，让我们构建复杂具有丰富的行为实体。
就像软件中的战神金刚。

## The Pattern

## 模式

A **single entity spans multiple domains**. To keep the domains isolated, the
code for each is placed in its own **<span name="component">component</span>
class**. The entity is reduced to a simple **container of components**.

**跨越多个领域的单一实体**。为了保持领域分离，将每部分代码放入<span name="component">**各自的组件类**</span>中。
实体被简化为*组件的容器*。

<aside name="component">

"Component", like "Object", is one of those words that means everything and
nothing in programming. Because of that, it's been used to describe a few
concepts. In business software, there's a "Component" design pattern that
describes decoupled services that communicate over the web.

“组件”，就像“对象”，在编程中意味任何东西也不意味任何东西。
因为这样，它被用来描述一些概念。
在商业软件中，“组件”设计模式描述了通过网络解耦的服务。

I tried to find a different name for this unrelated pattern found in games, but
"Component" seems to be the most common term for it. Since design patterns are
about documenting existing practices, I don't have the luxury of coining a new
term. So, following in the footsteps of XNA, Delta3D, and others, "Component" it
is.

我试图从游戏中找到这个无关模式的另一个名字，但“组件”看来是最常用的术语。
由于设计模式是关于记录已存的实践，我没有创建新术语的余地。
所以，跟着XNA，Delta3D和其他人的脚步，我称之为“组件”。

</aside>

## When to Use It

## 何时使用

Components are most commonly found within the core class that defines the
entities in a game, but they may be useful in other places as well. This pattern can
be put to good use when any of these are true:

组件通常在定义游戏实体的核心部分中使用，但它们在其他地方也有用。
这个模式在如下情况中可以使用：

 *  You have a class that touches multiple domains which you want to keep
    decoupled from each other.

* 有一个涉及了多个领域的类，而你想保持这些领域互相隔离。

 *  A class is getting massive and hard to work with.

* 一个类正在变大而且越来越难以使用。

 *  You want to be able to define a variety of objects that share different
    capabilities, but using inheritance doesn't let you pick the parts you
    want to reuse precisely enough.

* 想要能定义一系列分享不同能力的类，但是使用接口不足以得到足够的重用部分。

## Keep in Mind

## 记住

The Component pattern adds a good bit of complexity over simply making a class and
putting code in it. Each conceptual "object" becomes a cluster of objects that
must be instantiated, initialized, and correctly wired together. Communication
between the different components becomes more challenging, and controlling how
they occupy memory is more complex.

组件模式比简单地向类中添加代码增加了一点点复杂性。
每个概念上的“对象”要组成真正对象需要实例化，初始化，然后正确的连接。
不同组件间沟通会有些困难，而控制它们如何使用内存就更加复杂。

For a large codebase, this complexity may be worth it for the decoupling and
code reuse it enables, but take care to ensure you aren't over-engineering a
"solution" to a non-existent problem before applying this pattern.

对于大代码库，为了解耦和重用而付出这样的复杂度是值得的。
但是在使用这种模式之前，保证你没有为了不存在的问题而“过度设计”。

Another consequence of using components is that you often have to hop through a
level of indirection to get anything done. Given the container object, first you
have to get the component you want, *then* you can do what you need. In <span
name="perf">performance</span>-critical inner loops, this pointer following may
lead to poor performance.

使用组件的另一后果是，需要多一层跳转你才能做要做的事。
拿到一个容器对象，先得获得相应的组件，*然后*你才能获得你想要的组件。
在<span name="perf">性能攸关</span>的内部循环中，这种跳转也许会导致糟糕的性能。

<aside name="perf">

There's a flip side to this coin. The Component pattern can often *improve*
performance and cache coherence. Components make it easier to use the <a
class="pattern" href="data-locality.html"> Data Locality</a> pattern to organize
your data in the order that the CPU wants it.

这是硬币的两面。组件模式通常可以*增进*性能和缓存一致性。组件让使用<a class="pattern" href="data-locality.html">数据局部性</a>模式以CPU需要的方式组织数据更容易。

</aside>

## Sample Code

## 示例代码

One of the biggest challenges for me in writing this book is figuring out how to
isolate each pattern. Many design patterns exist to contain code that itself
isn't part of the pattern. In order to distill the pattern down to its essence,
I try to cut as much of that out as possible, but at some point it becomes a bit
like explaining how to organize a closet without showing any clothes.

我写这本书的最大挑战之一就是搞明白如何隔离各个模式。
许多设计模式包含了不属于这种模式的代码。
为了将模式提取到它的本质表现，我尽可能的消减代码，
但是在某种程度上，这就像没有衣服还要说明如何整理衣柜。

The Component pattern is a particularly hard one. You can't get a real feel for
it without seeing some code for each of the domains that it decouples, so I'll
have to sketch in a bit more of Bjørn's code than I'd like. The pattern is
really only the component *classes* themselves, but the code in them should
help clarify what the classes are for. It's fake code -- it calls into other
classes that aren't presented here -- but it should give you an idea of what
we're going for.

组件模式是特别难的一个。
如果看不到它解耦的各个领域的代码，你就不能获得正确的感觉，
因此我会多写一些有关于Bj?rn的代码。
这个模式事实上只关于将组件变为*类*，但类中的代码可以帮助表明类是做什么用的。
这是伪代码——它调用了其他不存在的类——但这应该可以让你理解我们正在做什么。

### A monolithic class

### 单块类

To get a clearer picture of how this pattern is applied, we'll start by showing
a monolithic <span name="cat">`Bjorn`</span> class that does everything we need
but *doesn't* use this pattern:

为了清晰的看到这个模式是如何应用的，
我们需要先展示一个<span name="cat">`Bjorn`</span>类，
它包含了所有我们需要但在模式中*不需要*的事物：

<aside name="cat">

I should point out that using the actual name of the character in the codebase
is usually a bad idea. The marketing department has an annoying habit of
demanding name changes days before you ship. "Focus tests show males between 11
and 15 respond negatively to &lsquo;Bjørn&rsquo;. Use &lsquo;Sven&rsquo;
instead."

我应指出在代码使用角色的名字总是个坏主意。市场需求有在发售之前改名字的坏习惯。
“焦点测试表面在11岁到15岁的男性不喜欢&lsquo;Bj?rn&rsquo;，改为&lsquo;Sven&rsquo”。

This is why many software projects use internal-only codenames. Well, that and
because it's more fun to tell people you're working on "Big Electric Cat" than
just "the next version of Photoshop."

这就是为什么很多软件项目使用内部代码名。好吧，那只是因为比起告诉人们你在完成“Photoshop的下一版本”，告诉他们你在完成“大电猫”更有趣。

</aside>

^code 1

`Bjorn` has an `update()` method that gets called once per frame by the game:

`Bjorn`有个每帧调用的`update()`方法。

^code monolithic-update

It reads the joystick to determine how to accelerate the baker. Then it resolves
its new position with the physics engine. Finally, it draws Bjørn onto the
screen.

它读取操纵杆以确定如何加速面包师。
然后，用物理引擎解析新位置。
最后，将Bj?rn渲染至屏幕。

The sample implementation here is trivially simple. There's no gravity,
animation, or any of the dozens of other details that make a character fun to
play. Even so, we can see that we've got a single function that several
different coders on our team will probably have to spend time in, and it's
starting to get a bit messy. Imagine this scaled up to a thousand lines and you
can get an idea of how painful it can become.

这里的示例实现平凡而简单。
没有重力，动画，或任何让人物有趣的其他细节。
即便如此，我们可以看到，已经出现了多个将消耗程序员时间的单一功能函数，而它开始变得有点混乱。
想象增加到一千行，你就知道这会有多难受了。

### Splitting out a domain

### 分离领域

Starting with one domain, let's pull a piece out of `Bjorn` and push it into a
separate component class. We'll start with the first domain that gets processed:
input. The first thing `Bjorn` does is read in user input and adjust his
velocity based on it. Let's move that logic out into a separate class:

从一个领域开始，让我们从`Bjorn`去除一部分，将之归入分离的组件类。
我们从首个开始执行的领域开始：输入。
`Bjorn`做的头件事就是读取玩家的输入，然后基于此调整它的速度。
让我们将这部分逻辑移入一个分离的类：

^code 2

Pretty simple. We've taken the first section of `Bjorn`&rsquo;s `update()`
method and put it into this class. The changes to `Bjorn` are also
straightforward:

很简单吧。我们将`Bjorn`的`update()`的第一部分取出，放入这个类中。
对`Bjorn`的改变也很直接：

^code 3

`Bjorn` now owns an `InputComponent` object. Where before he was handling user
input directly in the `update()` method, now he delegates to the component:

`Bjorn`现在拥有了一个`InputComponent`对象。
之前它在`update()`方法中直接处理用户输入，现在委托给组件：

^code 4

We've only started, but already we've gotten rid of some coupling -- the main
`Bjorn` class no longer has any reference to `Controller`. This will come in
handy later.

我们才刚开始，但已经摆脱了一些耦合——`Bjorn`主体现在已经与`Controller`无关了。这会派上用场的。

### Splitting out the rest

### 将剩下的分割出来

Now, let's go ahead and do the same cut-and-paste job on the physics and graphics
code. Here's our new `PhysicsComponent`:

现在让我们对物理和图像代码继续这种剪切粘贴的工作。
这是我们新的 `PhysicsComponent`：

^code 5

In addition to moving the physics *behavior* out of the main `Bjorn` class, you
can see we've also moved out the *data* too: The `Volume` object is now owned by
the component.

为了将物理*行为*移出`Bjorn`类，你可以看到我们也移出了*数据*：`Volume`对象已经是组件的一部分了。

Last but not least, here's where the rendering code lives now:

最后，这是现在的渲染代码：

^code 6

We've yanked almost everything out, so what's left of our humble pastry chef?
Not much:

我们几乎将所有的东西都移出来了，所以谦虚的面包师还剩下什么？没什么了：

^code 7

The `Bjorn` class now basically does two things: it holds the set of components
that actually define it, and it holds the state that is shared across multiple
domains. Position and velocity are still in the core `Bjorn` class for two
reasons. First, they are "pan-domain" state -- almost every component will make
use of them, so it isn't clear which component *should* own them if we did want
to push them down.

`Bjorn`类现在基本上就做两件事：拥有定义它的组件，以及在不同域间分享的数据。
有两个原因导致位置和速度仍然在`Bjorn`的核心类中：
首先，它们是“泛领域”状态——几乎每个组件都需要使用它们，
所以我们想要提取出来的时候，哪个组件*应该*拥有它们并不明确。

Secondly, and more importantly, it gives us an easy way for the components to
communicate without being coupled to each other. Let's see if we can put that to
use.

第二，也是更重要的一点，它给了我们无需让组件耦合就能沟通的简易方法。
让我们看看能不能利用这一点。

### Robo-Bjørn

### 机器人Bj?rn

So far, we've pushed our behavior out to separate component classes, but we
haven't *abstracted* the behavior out. `Bjorn` still knows the exact concrete
classes where his behavior is defined. Let's change that.

到目前为止，我们将行为归入了不同的组件类，但还没将行为*抽象*出来。
`Bjorn`仍知道每个类的具体定义的行为。让我们改变这一点。

We'll take our component for handling user input and hide it behind an
interface. We'll turn `InputComponent` into an abstract base class:

取出处理输入的部件，将其藏在接口之后，将`InputComponent`变为抽象基类。

^code 8

Then, we'll take our existing user input handling code and push it down into a
class that implements that interface:

然后，将现有的处理输入的代码取出，放进一个实现接口的类中。

^code 9

We'll change `Bjorn` to hold a pointer to the input component instead of having
an inline instance:

我们将`Bjorn`改为只拥有一个指向输入组件的指针，而不是拥有一个内联的实例。

^code 10

Now, when we instantiate `Bjorn`, we can pass in an input component for it to
use, like so:

现在当我们实例化`Bjorn`，我们可以传入输入组件使用，就像下面这样：

^code 11

This instance can be any concrete type that implements our abstract
`InputComponent` interface. We pay a price for this -- `update()` is now a virtual
method call, which is a little slower. What do we get in return for this cost?

这个实例可以是任何实现了抽象`InputComponent`接口的类型。
我们为此付出了代价——`update()`现在是虚方法调用了，这会慢一些。这一代价的回报是什么？

Most consoles require a game to support "demo mode." If the player sits at the
main menu without doing anything, the game will start playing automatically,
with the computer standing in for the player. This keeps the game from burning
the main menu into your TV and also makes the game look nicer when it's running
on a kiosk in a store.

大多数的主机需要游戏支持“演示模式”。
如果玩家停在主菜单没有做任何事情，游戏就会自动开始运行，直到接入一个玩家。
这让屏幕上的主菜单看上去更有生机，同时也是游戏厅中很好的展示。

Hiding the input component class behind an interface lets us get that working.
We already have our concrete `PlayerInputComponent` that's normally used when
playing the game. Now, let's make another one:

隐藏在输入组件后的类帮我们实现了这点，
我们已经有了具体的`PlayerInputComponent`供在玩游戏时使用。
现在让我们完成另一个：

^code 12

When the game goes into demo mode, instead of constructing Bjørn like we did
earlier, we'll wire him up with our new component:

当游戏进入演示模式，不像之前演示的那样构造Bj?rn，
我们将它和一个新组件连接起来：

^code 13

And now, just by swapping out a component, we've got a fully functioning
computer-controlled player for demo mode. We're able to reuse all of the other
code for Bjørn -- physics and graphics don't even know there's a difference.
Maybe I'm a bit strange, but it's stuff like this that gets me up in the <span
name="coffee">morning</span>.

现在，只需要更改组件，我们就有了为演示模式而设计的电脑控制的玩家。
我们可以重用所有Bj?rn的代码——物理和图像都不知道这里有了变化。
也许有些诡异，但这就是<span name="coffee">每天</span>能让我从起床的事物。

<aside name="coffee">

That, and coffee. Sweet, steaming hot coffee.

那个，还有咖啡。热气腾腾的咖啡。

</aside>

### No Bjørn at all?

### 完全没有Bj?rn？

If you look at our `Bjorn` class now, you'll notice there's nothing really
"Bjørn" about it -- it's just a component bag. In fact, it looks like a pretty
good candidate for a base "game object" class that we can use for *every* object
in the game. All we need to do is pass in *all* the components, and we can build
any kind of object by picking and choosing parts like Dr. Frankenstein.

如果你看看现在的`Bj?rn`类，你会意识到那里完全没有“Bj?rn”——那只是个组件包。
事实上，它是*每个*游戏中的对象都能继承的“游戏对象”基类的好候选人。
我们可以像弗兰肯斯坦一样，通过挑选拼装部件构建任何对象。

Let's take our two remaining concrete components -- physics and graphics -- and
hide them behind interfaces like we did with input:

让我们将剩下的两个具体组件——物理和图像——像输入那样藏到接口之后。

^code 14

Then we re-christen `Bjorn` into a <span name="id">generic</span> `GameObject`
class that uses those interfaces:

然后将`Bj?rn`改为使用这些接口的<span name="id">通用</span>`GameObject`类。

^code 15

<aside name="id">

Some component systems take this even further. Instead of a `GameObject` that
contains its components, the game entity is just an ID, a number. Then, you
maintain separate collections of components where each one knows the ID of the
entity its attached to.

有些人走的更远。
不使用包含组件的`GameObject`，游戏实体只是一个ID，一个数字。
然后在每个人都知道实体连接的ID的情况下，管理分离的组件。

These [entity component
systems](http://en.wikipedia.org/wiki/Entity_component_system) take decoupling
components to the extreme and let you add new components to an entity without
the entity even knowing. The <a href="data-locality.html" class="pattern">Data
Locality</a> chapter has more details.

这些[实体组件系统](http://en.wikipedia.org/wiki/Entity_component_system)将组件发挥到了极致，让你向实体添加组件而无需通知实体。
<a href="data-locality.html" class="pattern">数据局部性</a>一章有更多细节。

</aside>

Our existing concrete classes will get renamed and implement those interfaces:

我们现有的具体类被重命名并实现这些接口：

^code 16

And now we can build an object that has all of Bjørn's original behavior without
having to actually create a class for him, just like this:

现在我们无需为Bj?rn建立具体类，就能构建拥有所有Bj?rn行为的对象。

<span name="factory"></span>

<span name="factory"></span>

^code 17

<aside name="factory">

This `createBjorn()` function is, of course, an example of the classic Gang of
Four <a class="gof-pattern" href="http://c2.com/cgi/wiki?FactoryMethod">Factory
Method</a> pattern.

这个`createBjorn()`函数是，当然，经典GoF<a class="gof-pattern" href="http://c2.com/cgi/wiki?FactoryMethod">工厂模式</a>的例子。

</aside>

By defining other functions that instantiate `GameObjects` with different
components, we can create all of the different kinds of objects our game needs.

通过用不同组件实例化`GameObject`，我们可以构建游戏需要的任何组件。

## Design Decisions

## 设计决策

The most important design question you'll need to answer with this pattern is,
"What set of components do I need?" The answer there is going to depend on the
needs and genre of your game. The bigger and more complex your engine is, the
more finely you'll likely want to slice your components.

这章中你最需要回答的设计问题是“我需要什么样的组件？”
回答取决于你游戏的需求和风格。
引擎越大越复杂，你就越想将其划分成组件。

Beyond that, there are a couple of more specific options to consider:

除此之外，还有几个更具体的选项要回答：

### How does the object get its components?

### 对象如何获取组件？

Once we've split up our monolithic object into a few separate component parts,
we have to decide who puts the parts back together.

一旦将单块对象分割为多个分离的组件，就需要决定谁将它们拼到一起。

 *  **If the object creates its own components:**

 *  *如果对象创建它的组件：*

     *  *It ensures that the object always has the components it needs.* You
        never have to worry about someone forgetting to wire up the right
        components to the object and breaking the game. The container object
        itself takes care of it for you.

     *  *这保证了对象总是能拿到他需要的组件。*你永远不必担心某人忘记连接正确的组件然后破坏了整个游戏。容器类自己会处理这个问题。

     *  *It's harder to reconfigure the object.* One of the powerful features of
        this pattern is that it lets you build new kinds of objects simply by
        recombining components. If our object always wires itself with the same
        set of hard-coded components, we aren't taking advantage of that
        flexibility.

     *  *重新设置对象比较困难。*这个模式的强力特性之一就是只需重新组合组件就可以创建新的对象。如果对象总是自己用硬编码的组件组装自己，我们就无法利用这个灵活度。

 *  **If outside code provides the components:**

 *  *如果外部代码提供组件：*

     *  *The object becomes more flexible.* We can completely change the
        behavior of the object by giving it different components to work with.
        Taken to its fullest extent, our object becomes a generic component
        container that we can reuse over and over again for different purposes.

     *  *对象更加灵活。*我们可以提供不同的组件，这样就能改变对象的行为。通过共用组件，对象变成了组件容器，我们可以为不同目的一遍又一遍重用它。

     *  *The object can be decoupled from the concrete component types.* If
        we're allowing outside code to pass in components, odds are good that
        we're also letting it pass in *derived* component types. At that point,
        the object only knows about the component *interfaces* and not the
        concrete types themselves. This can make for a nicely encapsulated
        architecture.

     *  *对象可以与确定的组件类型解耦。*如果我们允许外部代码提供组件，好处是也可以传递*派生*的组件类型。这样，对象只知道组件*接口*而不知道组件的具体类型。这是一个很好的封装结构。

### How do components communicate with each other?

### 组件之间如何通信？

Perfectly decoupled components that function in isolation is a nice ideal, but
it doesn't really work in practice. The fact that these components are part of the
*same* object implies that they are part of a larger whole and need to
coordinate. That means communication.

完美解耦的组件不需要考虑这个问题，但在真正的实践中行不通。
事实上组件属于*同一*对象暗示它们属于需要相互协同的更大整体的一部分。
这就意味着通信。

So how can the components talk to each other? There are a couple of options, but
unlike most design "alternatives" in this book, these aren't exclusive -- you will
likely support more than one at the same time in your designs.

所以组件如何相互通信呢？这里有很多选项，但不像这本书中其他的“选项”，它们并不冲突——你可以在一个设计中支持多种方案。

 *  **By modifying the container object's state:**

 *  *通过修改容器对象的状态：*

     *  *It keeps the components decoupled.* When our `InputComponent` set
        Bjørn's velocity and the `PhysicsComponent` later used it, the two
        components had no idea that the other even existed. For all they knew,
        Bjørn's velocity could have changed through black magic.

    *   *这保持组件解耦。*当我们的`InputComponent`设置了Bj?rn的速度，而后`PhysicsComponent`使用它，这两个组件都不知道对方的存在。在它们的理解中，Bj?rn的速度是被黑魔法改变的。

     *  *It requires any information that components need to share to get pushed
        up into the container object.* Often, there's state that's really only
        needed by a subset of the components. For example, an animation and a
        rendering component may need to share information that's
        graphics-specific. Pushing that information up into the container object
        where *every* component can get to it muddies the object class.

     *  *需要将组件需要分享的任何数据存储在容器类中。*通常状态只在几个组件间共享。比如，动画组件和渲染组件需要共享图形专用的信息。将信息存入容器类会让*所有*组件都获得这样的信息。

        Worse, if we use the same container object class with different
        component configurations, we can end up wasting memory on state that
        isn't needed by *any* of the object's components. If we push some
        rendering-specific data into the container object, any invisible object
        will be burning memory on it with no benefit.

        更糟的是，如果我们为不同组件设置构建相同的容器类，最终会浪费内存存储没有*任何*对象组件需要的状态。如果我们将渲染专用的数据放入容器对象中，任何隐形对象都会无益的消耗内存。

     *  *It makes communication implicit and dependent on the order that
        components are processed.* In our sample code, the original monolithic
        `update()` method had a very carefully laid out order of operations. The
        user input modified the velocity, which was then used by the physics
        code to modify the position, which in turn was used by the rendering
        code to draw Bjørn at the right spot. When we split that code out into
        components, we were careful to preserve that order of operations.

     *  *这让组件的通信基于组件运行的顺序。*在同样的代码中，原先一整块的`update()`代码小心的排列这些操作。玩家的输入修改了速度，速度被物理代码使用修改位置，位置被渲染代码使用将Bj?rn绘到该有的地方。当我们将这些代码划入组件时，还是得小心翼翼的保持这种操作顺序。

        If we hadn't, we would have introduced <span name="pure">subtle</span>,
        hard-to-track bugs. For example, if we'd updated the graphics component
        *first*, we would wrongly render Bjørn at his position on the *last*
        frame, not this one. If you imagine several more components and lots
        more code, then you can get an idea of how hard it can be to avoid bugs
        like this.

        如果我们没有，就引入了 <span name="pure">微妙</span>而难以追踪的漏洞。比如，我们*先*更新图形组件，就错误地将Bj?rn渲染在他*上一帧*所处的位置上，而不是这一帧的。如果你考虑更多的组件和更多的代码，那你可以想象要避免这样的错误有多么困难了。

        <aside name="pure">

        Shared mutable state like this where lots of code is reading and writing
        the same data is notoriously hard to get right. That's a big part of why
        academics are spending time researching pure functional languages like
        Haskell where there is no mutable state at all.

        很难正确完成读写共享可变状态的相同数据的代码。这就是为什么学术界花时间研究完全函数式语言，比如Haskell，那里根本没有可变状态。

        </aside>

 *  **By referring directly to each other:**

 *  *通过他们之间相互引用：*

    The idea here is that components that need to talk will have direct
    references to each other without having to go through the container
    object at all.

    这里的思路是组件有要交流的组件的引用，这样它们直接交流，无需通过容器类。

    Let's say we want to let Bjørn jump. The graphics code needs to know if
    he should be drawn using a jump sprite or not. It can determine this by
    asking the physics engine if he's currently on the ground. An easy way
    to do this is by letting the graphics component know about the physics
    component directly:

    假设我们想让Bj?rn跳跃。图像代码想知道它需要用跳跃图像还是不用。这可以通过询问物理引擎它现在在不在地上来确定。一种简单的方式是图像组件直接知道物理组件的存在：

    ^code 18

    When we construct Bjørn's `GraphicsComponent`, we'll give it a reference
    to his corresponding `PhysicsComponent`.

    当构建Bj?rn的`GraphicsComponent`时，我们给它相应的`PhysicsComponent`引用。

     *  *It's simple and fast.* Communication is a direct method call from one
        object to another. The component can call any method that is supported
        by the component it has a reference to. It's a free-for-all.

     *  *简单快捷。*通信是一个对象到另一个的直接方法调用。组件可以调用任一引用对象的方法。做什么都可以。

     *  *The two components are tightly coupled.* The downside of the
        free-for-all. We've basically taken a step back towards our monolithic
        class. It's not quite as bad as the original single class though, since
        we're at least restricting the coupling to only the component pairs that
        need to interact.

     *  *两个组件紧绑在一起。*这是什么都可以的坏处。我们向使用整块类又退回了一步。这比只用单一类好一点，至少我们现在只是把需要通信的类绑在一起。

 *  **By sending messages:**

 *  *通过发送消息：*

     *  This is the most complex alternative. We can actually build a little
        messaging system into our container object and let the components
        broadcast information to each other.

     *  这是最复杂的选项。我们可以在容器类中建小小的消息系统，允许组件相互发送消息。

        Here's one possible implementation. We'll start by defining a base
        `Component` interface that all of our components will implement:

        这是一种可能的实现。我们从每个组件都会实现的`Component`接口开始：

        ^code 19

        It has a single `receive()` method that component classes implement in
        order to listen to an incoming message. Here, we're just using an `int`
        to identify the message, but a fuller implementation could attach
        additional data to the message.

        它有一个简单的`receive()`方法，每个需要接受消息的组件类都要实现它。这里，我们使用一个`int`来定义消息，但更完整的消息实现应该可以引用附加的消息。

        Then, we'll add a method to our container object for sending messages:

        然后，向容器类添加发送消息的方法。

        ^code 20

        <span name="queue">Now</span>, if a component has access to its
        container, it can send messages to the container, which will
        rebroadcast the message to all of the contained components. (That
        includes the original component that sent the message; be careful that
        you don't get stuck in a feedback loop!) This has a couple of
        consequences:

        <span name="queue">现在</span>，如果组件能够接触容器，它就能向容器发送消息，直接向所有的组件广播。（包括了原先发送消息的组件，小心别陷入消息的无限循环中！）这会有一些结果：

        <aside name="queue">

        If you really want to get fancy, you can even make this message system
        *queue* messages to be delivered later. For more on this, see <a
        href="event-queue.html" class="pattern">Event Queue</a>.

        如果你真的想突破限制，甚至可以将消息存储在*队列*中，晚些发送。要知道更多，看看<a href="event-queue.html" class="pattern">事件队列</a>。

        </aside>

     *  *Sibling components are decoupled.* By going <span
        name="mediator">through</span> the parent container object, like our
        shared state alternative, we ensure that the components are still
        decoupled from each other. With this system, the only coupling they have
        is the message values themselves.

     *  *同级组件解耦。*<span name="mediator">通过</span>父级容器对象，就像共享状态的方案一样，我们保证了组件之间仍然是解耦的。使用了这套系统，组件之间唯一的耦合是它们发送的消息值。

        <aside name="mediator">

        The Gang of Four call this the <a class="gof-pattern"
        href="http://c2.com/cgi-bin/wiki?MediatorPattern">Mediator</a> pattern --
        two or more objects communicate with each other indirectly by routing
        the message through an intermediate object. In this case, the container
        object itself is the mediator.

        GoF称之为<a class="gof-pattern" href="http://c2.com/cgi-bin/wiki?MediatorPattern">中介</a>模式——两个或更多的对象通过中介对象通信。在这种情况下，容器对象本身就是中介。

        </aside>

     *  *The container object is simple.* Unlike using shared state where the
        container object itself owns and knows about data used by the
        components, here, all it does is blindly pass the messages along. That
        can be useful for letting two components pass very domain-specific
        information between themselves without having that bleed into the
        container object.

     *  *容器类很简单。*不像使用共享状态那样，容器类无需知道组件使用了什么数据，它只是将消息发送出去。这可以让组件发送领域特有的数据而无需打扰容器对象。

Unsurprisingly, there's no one best answer here. What you'll likely end up doing
is using a bit of all of them. Shared state is useful for the really basic stuff
that you can take for granted that every object has -- things like position and
size.

不出意料的，这里没有最好的答案。这些方法你最终可能都会使用一些。
共享状态对于每个对象都有的数据是很好用的——比如位置和大小。

Some domains are distinct but still closely related. Think animation and
rendering, user input and AI, or physics and collision. If you have separate
components for each half of those pairs, you may find it easiest to just let
them know directly about their other half.

有些领域不同却仍然紧密相关。想想动画和渲染，输入和AI，或物理和粒子。
如果你有这样一对分离的组件，你会发现直接相互引用也许更加容易。

Messaging is useful for "less important" communication. Its fire-and-forget
nature is a good fit for things like having an audio component play a sound when
a physics component sends a message that the object has collided with something.

消息对于“不那么重要”的通信很有用。发送后不管的特性对物理组件发现事物碰撞后发送消息，让音乐组件播放声音这种事情是很有效的。

As always, I recommend you start simple and then add in additional
communication paths if you need them.

就像以前一样，我建议你从简单的开始，然后如果需要的话，加入其他的通信路径。

## See Also

## 参见

 *  The [Unity](http://unity3d.com) framework's core [`GameObject`](http://docs.unity3d.com/Documentation/Manual/GameObjects.html) class is designed
    entirely around [components](http://docs.unity3d.com/Manual/UsingComponents.html).

 *  [Unity](http://unity3d.com)核心架构中[`GameObject`](http://docs.unity3d.com/Documentation/Manual/GameObjects.html)类完全根据这样的原则设计 [components](http://docs.unity3d.com/Manual/UsingComponents.html)。

 *  The open source [Delta3D](http://www.delta3d.org) engine has a base
    `GameActor` class that implements this pattern with the appropriately named
    `ActorComponent` base class.

 *  开源的[Delta3D](http://www.delta3d.org)引擎有`GameActor`基类通过`ActorComponent`基类实现了这种模式。

 *  Microsoft's <a href="http://creators.xna.com/en-US/">XNA</a> game framework
    comes with a core `Game` class. It owns a collection of `GameComponent`
    objects. Where our example uses components at the individual game entity
    level, XNA implements the pattern at the level of the main game object
    itself, but the purpose is the same.

 *  微软的<a href="http://creators.xna.com/en-US/">XNA</a>游戏框架有一个核心的`Game`类。它拥有一系列`GameComponent`对象。我们在游戏实体层使用组件，XNA在游戏主对象上实现了这种模式，但意图是一样的。

 *  This pattern bears resemblance to the Gang of Four's <a class="gof-pattern"
    href="http://c2.com/cgi-bin/wiki?StrategyPattern">Strategy</a> pattern. Both
    patterns are about taking part of an object's behavior and delegating it to
    a separate subordinate object. The difference is that with the Strategy
    pattern, the separate "strategy" object is usually stateless -- it
    encapsulates an algorithm, but no data. It defines *how* an object behaves,
    but not *what* it is.

    这种模式与GoF的<a class="gof-pattern" href="http://c2.com/cgi-bin/wiki?StrategyPattern">策略模式</a> 类似。两种模式都是将对象的行为取出划入单独的重述对象。与对象模式不同的是，分离的策略模式通常是无状态的——它封装了算法，而没有数据。它定义了对象*如何*行动，但没有定义对象*是*什么。

    Components are a bit more self-important. They often hold state that
    describes the object and helps define its actual identity. However, the line
    may blur. You may have some components that don't need any local state. In
    that case, you're free to use the same component *instance* across multiple
    container objects. At that point, it really is behaving more akin to a
    strategy.


    组件本身更加重要。它们经常保存了对象的状态，这有助于确定其真正的身份。但是，这条界限很模糊。有一些组件也许根本没有任何状态。在这种情况下，你可以在不同的容器对象中使用相同的组件*实例*。这样看来，它的行为确实更像一种策略。
