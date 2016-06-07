^title Flyweight
^section Design Patterns Revisited

^title 享元
^section Design Patterns Revisited

The fog lifts, revealing a majestic old growth forest. Ancient hemlocks,
countless in number, tower over you forming a cathedral of greenery. The stained
glass canopy of leaves fragments the sunlight into golden shafts of mist.
Between giant trunks, you can make out the massive forest receding into the
distance.

迷雾散尽，露出了古朴庄严的森林。古老的铁杉，在头顶编成绿色穹顶。阳光在树叶间破碎成金色顶棚。从树干间远眺，远处的森林渐渐隐去。

This is the kind of otherworldly setting we dream of as game developers, and
scenes like these are often enabled by a pattern whose name couldn't possibly be
more modest: the humble Flyweight.

这是我们游戏开发者梦想的超凡设置，这样的场景通常由一个模式支撑着，它的名字谦虚至极：享元模式。

## Forest for the Trees

## 森林

I can describe a sprawling woodland with just a few sentences, but actually
*implementing* it in a realtime game is another story. When you've got an entire
forest of individual trees filling the screen, all that a graphics programmer
sees is the millions of polygons they'll have to somehow shovel onto the GPU
every sixtieth of a second.

用几句话就能描述一片巨大的森林，但是在实时游戏中做这件事就完全是另外一件事了。
当屏幕上需要一整个森林时，图形程序员看到的是每秒需要送到GPU六十次的百万多边形。

We're talking thousands of trees, each with detailed geometry containing
thousands of polygons. Even if you have enough *memory* to describe that forest,
in order to render it, that data has to make its way over the bus from the CPU
to the GPU.

我们讨论的是成千上万的树，每棵都由上千的多边形组成。就算有足够的*内存*描述森林，渲染的过程中，CPU到GPU的部分也太过繁忙了。

Each tree has a bunch of bits associated with it:

每棵树都有一系列与之相关的位：

* A mesh of polygons that define the shape of the trunk, branches, and greenery.
* Textures for the bark and leaves.
* Its location and orientation in the forest.
* Tuning parameters like size and tint so that each tree looks different.

* 定义树干，树枝和树叶的形状的多边形网格。
* 树皮和树叶的纹理。
* 在森林中它的位置和方向。
* 大小和色彩的参数，使每棵树都看起来与众不同。

If you were to sketch it out in code, you'd have something like this:


如果用代码表示，那么会得到这样的东西：

^code heavy-tree

That's a lot of data, and the mesh and textures are particularly large. An entire
forest of these objects is too much to throw at the GPU in one frame.
Fortunately, there's a time-honored trick to handling this.

这是一大堆数据，多边形网格和纹理体积非常大。
描述这样的森林整个对象在一帧的时间就交给GPU是太过了。
幸运的是，有一种时间技巧来处理它。

The key observation is that even though there may be thousands of trees in the
forest, they mostly look similar. They will likely all use the <span
name="same">same</span> mesh and textures. That means most of the fields in
these objects are the *same* between all of those instances.

关键点在于，哪怕森林里有千千万万的树，他们大多数长得一模一样。
它们也许使用了<span name="same">相同的</span>网格和纹理。
这意味着这些树对象的实例的大部分字段是*一样的*。

<aside name="same">

You'd have to be crazy or a billionaire to budget for the artists to
individually model each tree in an entire forest.

你要么是疯了，要么是亿万富翁，才能让美术给整个森林的每棵树建一个独立模型。

</aside>

<span name="trees"></span>

<span name="trees"></span>

<img src="images/flyweight-trees.png" alt="A row of trees, each of which has its own Mesh, Bark, Leaves, Params, and Position." />

<aside name="trees">

Note that the stuff in the small boxes is the same for each tree.

注意小盒子中的东西对每一棵树都是一样的。

</aside>

We can model that explicitly by splitting the object in half. First, we pull
out the data that all trees have <span name="type">in common</span> and move it
into a separate class:

我们可以通过显式将对象切为两部分来更加明确地模拟。
第一，将树<span name="type">共有的</span>数据拿出来放到另一个类中：

^code tree-model

The game only needs a single one of these, since there's no reason to have the
same meshes and textures in memory a thousand times. Then, each *instance* of a
tree in the world has a *reference* to that shared `TreeModel`. What remains in
`Tree` is the state that is instance-specific:

游戏只需要一个这种类，
因为没有必要在内存中把相同的网格和纹理重复一千遍。
然后每个游戏世界中树的实例有一个对这个共享`TreeModel`的*引用*。
留在`Tree`中的是那些实例相关的数据：

^code split-tree

You can visualize it like this:

你可以将其想象成这样：

<img src="images/flyweight-tree-model.png" alt="A row of trees each with its own Params and Position, but pointing to a shared Model with a Mesh, Bark, and Leaves." />

<aside name="type">

This looks a lot like the <a href="type-object.html" class="pattern">Type
Object</a> pattern. Both involve delegating part of an object's state to some
other object shared between a number of instances. However, the intent behind
the patterns differs.

这有点像<a href="type-object.html" class="pattern">类型对象</a>模式。
两者都涉及将一个类中的状态委托给另外的类，来达到在不同实例间分享状态的目的。
但是，这两种模式背后的意图不同。

With a type object, the goal is to minimize the number of classes you have to
define by lifting "types" into your own object model. Any memory sharing you get
from that is a bonus. The Flyweight pattern is purely about efficiency.

使用类型对象，目标是通过将类型引入对象模型，减少需要定义的类。
伴随而来的内容分享是额外的好处。享元模式则是纯粹的为了效率。

</aside>

This is all well and good for storing stuff in main memory, but that doesn't
help rendering. Before the forest gets on screen, it has to work its way over to
the GPU. We need to express this resource sharing in a way that the graphics
card understands.

把所有的东西都存在主存里是很好的，但是这不利于渲染。
在森林到屏幕上之前，它得先到GPU。我们需要用显卡可以理解的方式共享

## A Thousand Instances

## 一千个实例

To minimize the amount of data we have to push to the GPU, we want to be able to
send the shared data -- the `TreeModel` -- just *once*. Then, separately, we
push over every tree instance's unique data -- its position, color, and scale.
Finally, we tell the GPU, "Use that one model to render each of these
instances."

为了减少需要推送到GPU的数据量，我们想把共享的数据——`TreeModel`——只发送*一次*。
然后，我们分别发送每个树独特的数据——位置，颜色，大小。
最后，我们告诉GPU，“使用一个模型渲染每一个实例”。

Fortunately, today's graphics APIs and <span name="hardware">cards</span>
support exactly that. The details are fiddly and out of the scope of this book,
but both Direct3D and OpenGL can do something called [*instanced
rendering*](http://en.wikipedia.org/wiki/Geometry_instancing).

幸运的是，今日的图形接口和<span name="hardware">显卡</span>正好支持这一点。
这些细节繁琐且超出了这部书的范围，但是Direct3D和OpenGL都可以做[*实例渲染*](http://en.wikipedia.org/wiki/Geometry_instancing)。

In both APIs, you provide two streams of data. The first is the blob of common
data that will be rendered multiple times -- the mesh and textures in our
arboreal example. The second is the list of instances and their parameters that
will be used to vary that first chunk of data each time it's drawn. With a
single draw call, an entire forest grows.

在这些API中，你提供两个数据流。
第一部分是一块需要渲染多次的共同数据——在例子中是树的网格和纹理。
第二部分是实例的列表以及绘制第一部分时需要使用的参数。
只要调用一次绘图，整个森林都会生长出来。

<aside name="hardware">

The fact that this API is implemented directly by the graphics card means the
Flyweight pattern may be the only Gang of Four design pattern to have actual
hardware support.

API是由显卡直接实现意味着享元模式也许是唯一的有硬件支持的GoF设计模式。

</aside>

## The Flyweight Pattern

## 享元模式

Now that we've got one concrete example under our belts, I can walk you through
the general pattern. Flyweight, like its name implies, comes into play when you
have objects that need to be more lightweight, generally because you have too
many of them.

好了，我们已经看了一个具体的例子，下面我介绍模式的通用部分。
享元，就像它名字暗示的那样，
当你需要共享类时使用，通常是因为你有太多这种类了。

With instanced rendering, it's not so much that they take up too much memory as
it is they take too much *time* to push each separate tree over the bus to the
GPU, but the basic idea is the same.

渲染的实例中，消耗的内存没有将每棵树通过总线送到GPU消耗的*时间*多，但是基本要点是一样的。

The pattern solves that by separating out an object's data into two kinds. The
first kind of data is the stuff that's not specific to a single *instance* of
that object and can be shared across all of them. The Gang of Four calls this
the *intrinsic* state, but I like to think of it as the "context-free" stuff. In
the example here, this is the geometry and textures for the tree.

这个模式通过将对象的数据分为两种来解决这个问题。
第一种数据没有特定指明是哪个对象的*实例*，因此可以在它们间分享。
Gof称之为*固有*状态，但是我更喜欢将其视为“上下文无关”部分。
在这里的例子中，是树的网格和纹理。

The rest of the data is the *extrinsic* state, the stuff that is unique to that
instance. In this case, that is each tree's position, scale, and color. Just
like in the chunk of sample code up there, this pattern saves memory by sharing
one copy of the intrinsic state across every place where an object appears.

数据的剩余部分是*外在*状态，那些对实例独一无二的东西。
在这个例子中，是每棵树的位置，拉伸和颜色。
就像这里的示例代码块一样，这种模式通过在每个对象出现时共享一份固有状态，来节约内存。

From what we've seen so far, this seems like basic resource sharing,
hardly worth being called a pattern. That's partially because in this example
here, we could come up with a clear separate *identity* for the shared state:
the `TreeModel`.

就目前而言而言，这看上去像是基础的资源共享，很难被称为一种模式。
部分原因是在这个例子中，我们可以为共享状态划出一个清晰的*身份*：`TreeModel`。

I find this pattern to be less obvious (and thus more clever) when used in cases
where there isn't a really well-defined identity for the shared object. In those
cases, it feels more like an object is magically in multiple places at the same
time. Let me show you another example.

我发现，当共享对象没有有效定义的实体时，使用这种模式就不那么明显（使用它也就越发显得精明）。
在那些情况下，这看上去是一个对象在同时被魔术般的分配到了多个地方。
让我展示给你另外一个例子。

## A Place To Put Down Roots

## 放根的地方

The ground these trees are growing on needs to be represented in our game too.
There can be patches of grass, dirt, hills, lakes, rivers, and whatever other
terrain you can dream up. We'll make the ground *tile-based*: the surface of the
world is a huge grid of tiny tiles. Each tile is covered in one kind of terrain.

这些树长出来的地方也需要在游戏中表示。
这里可能有草，泥土，丘陵，湖泊，河流，以及其它任何你可以想到的地形。
我们*基于区块*建立地表：世界的表面被划分为由微小区块组成的巨大网格。
每个区块都是由一种地形覆盖。

Each terrain type has a number of properties that affect gameplay:

每种地形类型都有一系列特性会影响游戏玩法：

* A movement cost that determines how quickly players can move through it.
* A flag for whether it's a watery terrain that can be crossed by boats.
* A texture used to render it.

* 一个移动代价决定了玩家能够多快的穿过它。
* 一个标识表明这是不是一个能够用船穿过的有水地形。
* 一个纹理用来渲染它。

Because we game programmers are paranoid about efficiency, there's no way we'd
store all of that state in <span name="learned">each</span> tile in the world.
Instead, a common approach is to use an enum for terrain types:

因为我们游戏程序员偏执于效率，我们不会在<span name="learned">每个</span>区块中保存这些状态。
相反，一个通用的方式是为每种地形使用一个枚举。

<aside name="learned">

After all, we already learned our lesson with those trees.

再怎么样，我们都已经学过树的例子了。

</aside>

^code terrain-enum

Then the world maintains a huge grid of those:

然后，这个世界管理巨大的网格：

<span name="grid"></span>

<span name="grid"></span>

^code enum-world

<aside name="grid">

Here I'm using a nested array to store the 2D grid. That's efficient in C/C++
because it will pack all of the elements together. In Java or other memory-
managed languages, doing that will actually give you an array of rows where each
element is a *reference* to the array of columns, which may not be as memory-
friendly as you'd like.

这里我使用嵌套数组存储2D网格。
在C/C++中这样很有效率的，因为它会将所有元素打包在一起。
在Java或者其他内存管理语言中，那样做会实际给你一数组的行，其中每个元素都是对数组的列的*引用*，那就不像你喜欢的那样内存友好了。

In either case, real code would be better served by hiding this implementation
detail behind a nice 2D grid data structure. I'm doing this here just to keep it
simple.

在两种情况中，隐藏2D网格数据结构背后的实现细节都能更好的为真实代码服务。
我在这里做的只是为了让其保持简单。

</aside>

To actually get the useful data about a tile, we do something like:

为了获得区块的实际有用数据，我们做了一些这样的事情：

^code enum-data

You get the idea. This works, but I find it ugly. I think of movement cost and
wetness as *data* about a terrain, but here that's embedded in code. Worse, the
data for a single terrain type is smeared across a bunch of methods. It would be
really nice to keep all of that encapsulated together. After all, that's what
objects are designed for.

你知道我的意思了。这可行，但是我觉得很丑。
移动代价和有没有水是地形的*数据*，但这嵌入了代码。
更糟的是，简单地形的数据横跨在一堆代码中。
如果能够将这些包裹起来就好了。毕竟，那是我们设计对象的目的。

It would be great if we could have an actual terrain *class*, like:

如果我们有实际的地形*类*就好了，像这样：

<span name="const"></span>

<span name="const"></span>

^code terrain-class

<aside name="const">

You'll notice that all of the methods here are `const`. That's no coincidence.
Since the same object is used in multiple contexts, if you were to modify it,
the changes would appear in multiple places simultaneously.

你会注意这里所有的方法都是`const`。这不是巧合。
由于同样的方法在多个上下文中使用，如果你打算修改它，
改变会同时在多个地方出现。

That's probably not what you want. Sharing objects to save memory should be an
optimization that doesn't affect the visible behavior of the app. Because of
this, Flyweight objects are almost always immutable.

这也许不是你想要的。
分享对象节约内存应该是不会影响应用显性行为的优化。
因为这一点，享元对象几乎总是不可变的。

</aside>

But we don't want to pay the cost of having an instance of that for each tile in
the world. If you look at that class, you'll notice that there's actually
*nothing* in there that's specific to *where* that tile is. In flyweight terms,
*all* of a terrain's state is "intrinsic" or "context-free".

但是我们不想为每个区块都保存一个实例。
如果你看看这个类里面，你会发现里面实际上*什么也没有*，
唯一特别的是区块在*哪里*。
用享元的术语讲，区块的*所有*状态都是“内在的”或者说“上下文无关的”。

Given that, there's no reason to have more than one of each terrain type. Every
grass tile on the ground is identical to every other one. Instead of having the
world be a grid of enums or Terrain objects, it will be a grid of *pointers* to
`Terrain` objects:

鉴于此，我们没有必要保存多余一个地形类型。
每种地面上的草区块和其他的没有什么不同。
游戏世界不是由地形区块对象网格组成的，而是由`Terrain`对象*指针*网格组成的：

^code world-terrain-pointers

Each tile that uses the same terrain will point to the same terrain instance.

每一个使用相同地形的区块会指向相同的地形实例。

<img src="images/flyweight-tiles.png" alt="A row of tiles. Each tile points to either a shared Grass, River, or Hill object." />

Since the terrain instances are used in multiple places, their lifetimes would
be a little more complex to manage if you were to dynamically allocate them.
Instead, we'll just store them directly in the world:

由于地形实例在很多地方使用，如果你想要动态分配，它们的生命周期会有点复杂。
取而代之的是，我们直接在游戏世界中存储。

^code world-terrain

Then we can use those to paint the ground like this:

然后我们可以像这样来描绘地面：

<span name="generate"></span>

<span name="generate"></span>

^code generate

<aside name="generate">

I'll admit this isn't the world's greatest procedural terrain generation
algorithm.

我承认这不是世界上最好的地形生成算法。

</aside>

Now instead of methods on `World` for accessing the terrain properties, we can
expose the `Terrain` object directly:

现在不需要`World`中的方法来接触地形属性，我们可以直接暴露出`Terrain`对象。

^code get-tile

This way, `World` is no longer coupled to all sorts of details of terrains. If
you want some property of the tile, you can get it right from that object:

用这种方式，`World`不再与各种地形的细节耦合。
如果你想要某一区块的属性，可直接从那个对象获得：

^code use-get-tile

We're back to the pleasant API of working with real objects, and we did this
with almost no overhead -- a pointer is often no larger than an enum.

我们回到了与真实对象工作的愉快API，也几乎没有开销——指针通常不比枚举大。

## What About Performance?

## 性能如何？

I say "almost" here because the performance bean counters will rightfully want
to know how this compares to using an enum. Referencing the terrain by pointer
implies an indirect lookup. To get to some terrain data like the movement cost,
you first have to follow the pointer in the grid to find the terrain object and
then find the movement cost there. Chasing a pointer like this can cause a <span
name="cache">cache miss</span>, which can slow things down.

我在这里说几乎，是因为性能偏执狂肯定会想要知道它和枚举比起来如何。
通过解引用指针获取地形需要一次间接跳转。
为了获得移动代价这样的地形数据，你首先需要跟着网格中的指针找到地形对象，
然后再找到移动代价。跟踪这样的指针会导致<span name="cache">缓存不命中</span>，降低运行速度。

<aside name="cache">

For lots more on pointer chasing and cache misses, see the chapter on <a href
="data-locality.html" class="pattern">Data Locality</a>.

对于更多指针追逐和缓存不命中，看看<a href ="data-locality.html" class="pattern">数据局部性</a>这章。

</aside>

As always, the golden rule of optimization is *profile first*. Modern computer
hardware is too complex for performance to be a game of pure reason anymore. In
my tests for this chapter, there was no penalty for using a flyweight over an
enum. Flyweights were actually noticeably faster. But that's entirely dependent
on how other stuff is laid out in memory.

就像往常一样，优化的金科玉律是*需求优先*。
现代计算机硬件过于复杂，性能只是游戏的一个考虑方面。
在我这章做的测试中，享元较枚举没有什么性能的优势。
享元实际上明显更快。但是这也完全取决于内存中的事物是如何排列的。

What I *am* confident of is that using flyweight objects shouldn't be dismissed
out of hand. They give you the advantages of an object-oriented style without
the expense of tons of objects. If you find yourself creating an enum and doing
lots of switches on it, consider this pattern instead. If you're worried about
performance, at least profile first before changing your code to a less
maintainable style.

我*可以*自信使用享元对象而不会搞到不可收拾。
它给了你面向对象的优势，而没有牺牲一堆对象。
如果你发现创建了一个枚举，又在它上面做了很多分支跳转，考虑一下这个模式吧。
如果你担心性能，在把代码编程为更加不可控的风格之前，至少测试一下。

## See Also

## 参见

 *  In the tile example, we just eagerly created an instance for each terrain
    type and stored it in `World`. That made it easy to find and reuse the
    shared instances. In many cases, though, you won't want to create *all* of
    the flyweights up front.

 *  在区块的例子中，我们只是为每种地形创建一个实例然后存储在`World`中。
    这也许能更好找到和重用这些实例。
    但是在多数情况下，你不会在一开始就创建*所有*享元。

    If you can't predict which ones you actually need, it's better to create
    them on demand. To get the advantage of sharing, when you request one, you
    first see if you've already created an identical one. If so, you just return
    that instance.

    如果你不能预料到哪个是实际上需要的，最好在需要时才创建。
    为了保持共享的优势，当你需要一个时，首先看看是否已经创建了一个相同的实例。
    如果确实如此，那么只需返回那个实例。

    This usually means that you have to encapsulate construction behind some
    interface that can first look for an existing object. Hiding a constructor
    like this is an example of the <a
    href="http://en.wikipedia.org/wiki/Factory_method_pattern" class="gof-
    pattern">Factory Method</a> pattern.

    这通常意味需要将构建指令封装在首先查询对象是否存在的接口之后。
    像这样隐藏构造指令是<a href="http://en.wikipedia.org/wiki/Factory_method_pattern" class="gof-pattern">工厂方法</a>的一个例子。

 *  In order to return a previously created flyweight, you'll have to keep track
    of the pool of ones that you've already instantiated. As the name implies,
    that means that an <a href="object-pool.html" class="pattern">Object
    Pool</a> might be a helpful place to store them.

 *  为了返回一个已经创建的享元，需要追踪那些已经实例化的对象池。就像名字暗示的那样，这就意味着<a href="object-pool.html" class="pattern">对象池</a>是存储它们的好地方。

 *  When you're using the <a class="pattern" href="state.html">State</a>
    pattern, you often have "state" objects that don't have any fields specific
    to the machine that the state is being used in. The state's
    identity and methods are enough to be useful. In that case, you can apply
    this pattern and reuse that same state instance in multiple state machines
    at the same time without any problems.


 * 当使用<a class="pattern" href="state.html">状态</a>模式时，你通常没有任何拥有特定字段的“状态对象”。这个状态的身份和方法都够用。在这种情况下，你可以使用这个模式，然后在不同的状态机上使用相同的对象实例。
