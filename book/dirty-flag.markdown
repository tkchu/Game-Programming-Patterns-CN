^title Dirty Flag
^section Optimization Patterns

## Intent

## 意图

*Avoid unnecessary work by deferring it until the result is needed.*

*将工作延期至其结果需要时才去执行，避免不必要的工作。*

## Motivation

## 动机

Many games have something called a *scene graph*. This is a big data structure
that contains all of the objects in the world. The rendering engine uses this to
determine where to draw stuff on the screen.

很多游戏有*场景图*。那是一个巨大的数据结构包含了游戏世界中所有的对象。渲染引擎使用之来决定在屏幕哪里画事物。

At its simplest, a scene graph is just a flat list of objects. Each object has a
model, or some other graphic primitive, and a <span
name="transform">*transform*</span>. The transform describes the object's
position, rotation, and scale in the world. To move or turn an object, we simply
change its transform.

最简单的实现中场景图只是一个对象列表。每一额无比都有一个模型，或者一些其他的原始图形，以及一个*转换*。一个转换描述了对象的字啊世界中的位置，方向，拉伸。为了移动或者移动一个对象，我们只需简单的改变它的转换。

<aside name="transform">

The mechanics of *how* this transform is stored and manipulated are unfortunately
out of scope here. The comically abbreviated summary is that it's a 4x4 matrix.
You can make a single transform that combines two transforms -- for example,
translating and then rotating an object -- by multiplying the two matrices.

How and why that works is left as an exercise for the reader.

</aside>

When the renderer draws an object, it takes the object's model, applies the
transform to it, and then renders it there in the world. If we had a scene
*bag* and not a scene *graph*, that would be it, and life would be simple.

当渲染系统描绘一个对象，他取出对下岗的模型，对其使用转换，然后将其渲染到游戏世界中。如果我们有一个场景*包*而不是场景*图*，那就是这样了，生活就很简单。

However, most scene graphs are <span name="hierarchical">*hierarchical*</span>.
An object in the graph may have a parent object that it is anchored to. In that
case, its transform is relative to the *parent's* position and isn't an
absolute position in the world.

但是，大多数场景图都是*按等级划分的*。一个图像中的对象也许拥有它锚定的父对象。在这种情况下，他的转换与其*父对象*的位置相关，不再是游戏世界上的绝对位置。

For example, imagine our game world has a pirate ship at sea. Atop the ship's
mast is a crow's nest. Hunched in that crow's nest is a pirate. Clutching the
pirate's shoulder is a parrot. The ship's local transform positions the ship in
the sea. The crow's nest's transform positions the nest on the ship, and so on.

举个例子，将游戏世界想象为海上的海盗船。桅杆的顶端是一个乌鸦巢。在乌鸦巢中的是一个海盗，海盗肩上是一只鹦鹉。船本身的转换定位穿在海上的位置。乌鸦巢的转换定位器在船上的位置，诸如此类。

<span name="pirate"></span>
<img src="images/dirty-flag-pirate.png" alt="A pirate ship containing a crow's nest with a pirate in it with a parrot on his shoulder." />

<aside name="pirate">

Programmer art!

</aside>

This way, when a parent object moves, its children move with it automatically.
If we change the local transform of the ship, the crow's nest, pirate, and
parrot go along for the ride. It would be a total <span
name="slide">headache</span> if, when the ship moved, we had to manually adjust
the transforms of all the objects on it to keep them from sliding off.

这样的话，当一个人物对象移动时，他的孩子节点也自动的跟着他移动。吴国我们改变了船本身的转换，乌鸦巢，海盗和鹦鹉都会随之移动。如果当船移动时，我们就得手动调整每一个对象来保持他们不滑出去，那可相当令人头疼。

<aside name="slide">

To be honest, when you are at sea you *do* have to keep manually adjusting your
position to keep from sliding off. Maybe I should have chosen a drier example.

</aside>

But to actually draw the parrot on screen, we need to know its absolute position
in the world. I'll call the parent-relative transform the object's *local
transform*. To render an object, we need to know its *world transform*.

当时为了在屏幕上真正的猫会鹦鹉，我需要知道它咋世界上的绝对位置。我会我会调用父节点相关的转换对象的*自身转换*。为了渲染一个对象，我需要知道他的*世界转换*。

### Local and world transforms

### 自身转换和世界转换

Calculating an object's world transform is pretty straightforward -- you just walk
its parent chain starting at the root all the way down to the object, combining
transforms as you go. In other words, the parrot's world transform is:

计算对象的世界转换很直接——你只需要从他的父节点一直追踪到对象，将你经过的所有转换绑在一起。换言之，鹦鹉的世界转换如下：

<span name="degenerate"></span>
<img src="images/dirty-flag-multiply.png" alt="The parrot's world position comes from multiplying the local positions for the ship, nest, pirate, and parrot." />

<aside name="degenerate">

In the degenerate case where the object has no parent, its local and world
transforms are equivalent.

</aside>

We need the world transform for every object in the world every frame, so even
though there are only a handful of matrix multiplications per model, it's on the hot
code path where performance is critical. Keeping them up to date is tricky
because when a parent object moves, that affects the world transform of itself
and all of its children, recursively.

我们需要为游戏世界的每个对象每一帧做世界转换，因此哪怕每个模型只有一大堆矩阵乘法，当性能攸关时它也在热点代码路径上。保持他们更新是有技巧的，因为当父对象移动时，它影响他自己的世界转换，并递归影响他的所有孩子节点。

The simplest approach is to calculate transforms on the fly while
rendering. Each frame, we recursively traverse the scene graph starting at the
top of the hierarchy. For each object, we calculate its world transform right
then and draw it.

最简单的方法是在渲染时计算转换。每一帧，我们从最高层递归遍历整个场景图。对每一个对象我们计算他的世界转换然后绘制它。

But this is terribly wasteful of our precious CPU juice! Many objects in the
world are *not* moving every frame. Think of all of the static geometry that
makes up the level. Recalculating their world transforms each frame even though
they haven't changed is a waste.

但这完全是在浪费CPU！很多游戏世界的对象*不*在每帧都移动。想想那些构成关卡的静态几何图形。在没有改变的情况下每帧计算他们的世界转换是一种浪费。

### Cached world transforms

### 缓存世界转换

The obvious answer is to *cache* it. In each object, we store its local
transform and its derived world transform. When we render, we only use the
precalculated world transform. If the object never moves, the cached transform
is always up to date and everything's happy.

明显的回答是*缓存*它。在每个对象中，我们存储他的本身转换和他的世界转换。当我们渲染时我们使用预计算的世界转换。如果对象从未移动，缓存的转换永远跟得上变化然后每个人都很开心。

When an object *does* move, the simple approach is to refresh its world
transform right then. But don't forget the hierarchy! When a parent moves, we
have to recalculate its world transform *and all of its children's,
recursively*.

当一个对象*确实*移动了，简单的解决方式是之后就更新他的世界转换。但是不要忘记层次性！当父节点移动时，我们得计算它的世界转换以及*递归计算它所有的子对象*。

Imagine some busy gameplay. In a single frame, the ship gets tossed on the
ocean, the crow's nest rocks in the wind, the pirate leans to the edge, and the
parrot hops onto his head. We changed four local transforms. If we recalculate
world transforms eagerly whenever a local transform changes, what ends up
happening?

想象一些忙碌的游戏。在一帧中，船在海上颠簸，乌鸦巢在风中摇晃，海盗甩到了边缘，而鹦鹉撞上了他的脑袋。我们改变了四个本身转换。如果每一次自身转换都立即更新世界转换，会发生什么？

<span name="stars"></span>
<img src="images/dirty-flag-update-bad.png" alt="Any time an object moves, the world coordinates are recalculated eagerly and redundantly." />

<aside name="stars">

You can see on the lines marked &#x2605; that we're recalculating the parrot's
world transform *four* times when we only need the result of the final one.

</aside>

We only moved four objects, but we did *ten* world transform calculations.
That's six pointless calculations that get thrown out before they are ever used
by the renderer. We calculated the parrot's world transform *four* times, but it
is only rendered once.

我只移动四个对象，但我们做了*十个*世界转换计算。那就有六个在被渲染器使用前浪费了。我们计算了鹦鹉的世界转换*四*次，但它只需渲染一次。

The problem is that a world transform may depend on several local transforms.
Since we recalculate immediately each time *one* of the transforms changes, we end up
recalculating the same transform multiple times when more than one of the local
transforms it depends on changes in the same frame.

问题是一个世界转换也许会依赖于多个自身转换。由于我们*每一次*转换变化就立即重新计算，当自身转换依赖的多个世界转换同时发生变化的时候，我们就对同一转换做了多次重新计算。

### Deferred recalculation

### 延期重计算

We'll solve this by <span name="decoupling">decoupling</span> changing local
transforms from updating the world transforms. This lets us change a bunch of
local transforms in a single batch and *then* recalculate the affected world
transform just once after all of those modifications are done, right before we
need it to render.

我们会通过解耦自身转换和世界转换的更新来解决这个问题。这让我们先在一批中改变自身转换，在这些改变完成*之后*重新计算他们的世界转换，就在我们需要渲染它之前

<aside name="decoupling">

It's interesting how much of software architecture is intentionally
engineering a little slippage.

</aside>

To do this, we add a *flag* to each object in the graph. "Flag" and "bit" are
synonymous in programming -- they both mean a single micron of data that can be
in one of two states. We call those "true" and "false", or sometimes "set" and
"cleared". I'll use all of these interchangeably.

为了做到这一点，我们为图中的每一个对象添加一个*标识位*。“标识”和“位”在编程中密切相关——他们都代表一小块处在两种状态之一的数据。我们称之为“真”和“假”，或者有时“设置”和“清除”。我之后会交替使用它们。

When the local transform
changes, we set it. When we need the object's world transform, we check the
flag. If it's set, we calculate the world transform and then clear the flag. The
flag represents, "Is the world transform out of date?" For reasons that aren't
entirely clear, the traditional name for this "out-of-date-ness" is "dirty".
Hence: *a dirty flag*. "Dirty bit" is an equally
<span name="specific">common</span> name for this pattern, but I figured I'd
stick with the name that didn't seem as prurient.

当自身转换改变了，我们设置它。当我们需要对象的世界转换时，我们坚持这一位。如果它被设置了，我们计算世界转换然后清除标识。那个标识代表着，“世界转换过时了吗？”由于他们没有清除，这种“过时的杂乱”被称为“脏”。也就是*脏标识*。“脏位”也是这章通常使用的名字，但是我决定使用不那么下流的称呼。

<aside name="specific">

Wikipedia's editors don't have my level of self-control and went with [dirty
bit](http://en.wikipedia.org/wiki/Dirty_bit).

</aside>

If we apply this pattern and then move all of the objects in our previous
example, the game ends up doing:

如果我们应用这一章然后移动我们之前例子的所有对象，那么游戏最终是这样的：

<img src="images/dirty-flag-update-good.png" alt="By deferring until all moves are done, we only recalculate once." />

That's the best you could hope to do -- the world transform for each affected
object is calculated exactly once. With only a single bit of data, this pattern
does a few things for us:

这就是您能希望得到的最好结果了——每一个受到影响的对象的世界转换只被计算一次。使用仅仅一位数据，这个模式为我们做了以下事情：

 *  It collapses modifications to multiple local transforms along an object's
    parent chain into a single recalculation on the object.

 *  他将对象的父节点链上的众多自身转换变化归并成对象上的一次计算。

 *  It avoids recalculation on objects that didn't move.

 *  它避免了在没有移动的对象上重新计算。

 *  And a minor bonus: if an object gets removed before it's rendered, it
    doesn't calculate its world transform at all.

 *  还有一个小小的意外收获：如果对象在渲染前被删除了，不必再计算它的世界转换。

## The Pattern

## 模式

A set of **primary data** changes over time. A set of **derived data** is
determined from this using some **expensive process**. A **"dirty" flag** tracks
when the derived data is out of sync with the primary data. It is **set when the
primary data changes**. If the flag is set when the derived data is needed, then
**it is reprocessed and the flag is cleared.** Otherwise, the previous **cached
derived data** is used.

一组*根本数据*随着时间变化而改变。一组*导出数据*使用一些*昂贵的过程*由他们推定。一个*“脏”标识*追踪导出数据是否与根本数据保持一致。它在*基本数据改变时被设置。*否则的话，使用之前的*缓存的导出数据*。

## When to Use It

## 何时使用

Compared to some other patterns in this book, this one solves a pretty specific
problem. Also, like most optimizations, you should only reach for it when you
have a performance problem big enough to justify the added code complexity.

与这本书中的其他模式相比，这一个解决了一个非常特殊的问题。同时，就像其他优化一样，只在有足够大的性能问题时再使用这一章增加代码 复杂度。

Dirty flags are applied to two kinds of work: *calculation* and
*synchronization*. In both cases, the process of going from the primary data to
the derived data is time-consuming or otherwise costly.

脏标识在两种工作上应用：“计算”和“同步”。在两种情况下，从基本数据转换到导出数据消耗很多时间或者有其他方面的消耗。

In our scene graph example, the process is slow because of the amount of math to
perform. When using this pattern for synchronization, on the other hand, it's
more often that the derived data is *somewhere else* -- either on disk or over
the network on another machine -- and simply getting it from point A to point B
is what's expensive.

在我们的场景图例子中，这个过程非常缓慢是因为需要执行很多数学运算。当你使用这个章节为了同步时，另一个应用场景，更通常的是导出数据在*别的地方*——要么在磁盘上或者在网格络另一头的终端机上——从点A到点B消耗很大。

There are a couple of other requirements too:

这里是一些其他应用场景：

 *  **The primary data has to change more often than the derived data is used.**
    This pattern works by avoiding processing derived data when a subsequent
    primary data change would invalidate it before it gets used. If you find
    yourself always needing that derived data after every single modification
    to the primary data, this pattern can't help.

 *  *基本数据的变化速度远高于导出数据的使用速度。*避免在导出数据使用前基本数据再一次变化而需要做的不必要的计算。如果你发现你总是在基本数据变化后使用导出数据，这个模式额密友帮助。

 *  **It should be hard to update incrementally.** Let's say the
    pirate ship in our game can only carry so much booty. We need to
    know the total weight of everything in the hold. We
    *could* use this pattern and have a dirty flag for the total weight. Every
    time we add or remove some loot, we set the flag. When we need the
    total, we add up all of the booty and clear the flag.

 *  *增量更新应该是困难的。*架设我们海盗船只能携带特定数量的战利品。我们需要获取带着的每件事物的总重量。我们*可以*使用这个模式让后为总重量设一个脏标识。每一次我们添加或者移除一些战利品，我们设置这个标识。当我们需要总量，我们将所有战利品重量加起来然后清除标识。

    But a simpler solution is to *keep a running total*. When we add or
    remove an item, just add or remove its weight from the current total. If
    we can "pay as we go" like this and keep the derived data updated, then
    that's often a better choice than using this pattern and calculating the
    derived data from scratch when needed.

    但是一个更简单的解决方法是*保持计算总量*。当我们添加或删除一个事物，直接从现在的总重量添加或者删除它的重量。如果我们可以*承担得起消耗*保持导出数据更新，那么更好的选择是不用这个模式，每次需要时重新计算导出数据。

This makes it sound like dirty flags are rarely appropriate, but you'll
find a place here or there where they help. <span name="hacks">Searching</span>
your average game codebase for the word "dirty" will often turn up uses of this
pattern.

这听起来脏标识很少使用，但你会找到一两个部分它会帮得上忙。直接字啊你的邮箱代码库中搜索“dirty”通常会发现使用这个模式。

<aside name="hacks">

From my research, it also turns up a lot of comments apologizing for "dirty"
hacks.

</aside>

## Keep in Mind

## 记住

Even after you've convinced yourself this pattern is a good fit, there are a few
wrinkles that can cause you some discomfort.

哪怕是在我们说服自己这个模式在这里很恰当，这里还有一些东西可能会让你不舒服。

### There is a cost to deferring for too long

### 延期太久是有代价的

This pattern defers some slow work until the result is actually needed, but when
it is, it's often needed *right now*. But the reason we're using this pattern to
begin with is because calculating that result is slow!

这个模式将一些缓慢的工作延期到结果真正需要的时候，但是当它要的时候，通常是*现在就要*。但是我们之所有使用这个模式的原因是计算结果很慢！

This isn't a problem in our example because we can still calculate world
coordinates fast enough to fit within a frame, but you can imagine other cases
where the work you're doing is a big chunk that takes noticeable time to chew
through. If the game doesn't *start* chewing until right when the player expects
to see the result, that can cause an unpleasant visible <span
name="gc">pause</span>.

在我们的例子中这不是问题，因为我们还是可以在一帧之内计算世界的位置，但是可以想象其他情况下，工作是一大块需要可观时间来进行。如果在玩家想要结构的时候才*开始*计算，这会引起不愉快的可见暂停。

Another problem with deferring is that if something goes wrong, you may fail to
do the work at all. This can be particularly problematic when you're using this
pattern to save some state to a more persistent form.

另一个延期的问题是，如果有东西出错率，你也许根本做不了事情。想要将模式存储为固定模式也许会造成问题。

For example, text editors know if your document has "unsaved changes". That
little bullet or star in your file's title bar is literally the dirty flag
visualized. The primary data is the open document in memory, and the derived
data is the file on disk.

举个例子，文本编辑器知道你的文档有“没保存的改变”。在你文件的标题栏的小点或者星号就是一个可见的脏标识。原始数据是在内存中打开的文档，推导数据是在磁盘上的文件。

<img src="images/dirty-flag-title-bar.png" alt="A window titlebar showing the little icon representing unsaved changes." />

Many programs don't save to disk until either the document is closed or the
application is exited. That's fine most of the time, but if you accidentally
kick the power cable out, there goes your masterpiece.

很多程序直到文档关闭或者应用退出才保存到磁盘上。在大多数情况下这都很好，但是如果你一不小心踢到了插线板，你的主要工作也就随风而逝了。

Editors that auto-save a backup in the background are compensating specifically
for this shortcoming. The auto-save frequency is a point on the continuum
between not losing too much work when a crash occurs and not thrashing the file
system too much by saving all the time.

在后台自动保存备份的编辑器是这一失误的补偿。这种自动保存的频率保持在在崩溃时不要丢失太多数据和不要总是打扰文件系统保存文件之间。

<aside name="gc">

This mirrors the different garbage collection strategies in systems that
automatically manage memory. Reference counting frees memory the second it's no
longer needed, but it burns CPU time updating ref counts eagerly every time
references are changed.

Simple garbage collectors defer reclaiming memory until it's really needed, but
the cost is the dreaded "GC pause" that can freeze your entire game until the
collector is done scouring the heap.

In between the two are more complex systems like deferred ref-counting and
incremental GC that reclaim memory less eagerly than pure ref-counting but more
eagerly than stop-the-world collectors.

</aside>

### You have to make sure to set the flag *every* time the state changes

### *每一次*状态改变你都得保证设置标识。

Since the derived data is calculated from the primary data, it's essentially a
cache. Whenever you have cached data, the trickiest aspect of it is <span
name="cache">*cache invalidation*</span> -- correctly noting when the cache is
out of sync with its source data. In this pattern, that means setting the dirty
flag when *any* primary data changes.

由于推导数据是从基本数据推导而来的，它实际上是缓存。无论何时你缓存了数据，有技巧的部分都是保证*缓存一致性*——在缓存与它原来数据不同步的时候通知之。字啊这种模式上，这意味着*任何*基本数据变化时设置脏标识。

<aside name="cache">

Phil Karlton famously said, "There are only two hard things in Computer Science:
cache invalidation and naming things."

</aside>

Miss it in one place, and your program will incorrectly use stale derived data.
This leads to confused players and bugs that are very hard to track down. When you use
this pattern, you'll have to take care that any code that modifies the primary
state also sets the dirty flag.

一处遗漏，你的程序就使用了不正确的推导数据。这引起了了玩家的困惑和非常难以追踪的漏洞。当你使用合格模式id时候，你也得注意任何修改了原始数据的代码设置了脏标识。

One way to mitigate this is by encapsulating modifications to the primary data
behind some interface. If anything that can change the state goes through a
single narrow API, you can set the dirty flag there and rest assured that it
won't be missed.

一种解决它的方法是将原始数据的修改隐藏在接口之后。任何想要改变状态的代码都要通过一个狭小的API，你可以在哪里设置脏标识来保证不会遗漏。

### You have to keep the previous derived data in memory

### 你得之前的推导数据保存在内存中。

<span name="sync"></span>

When the derived data is needed and the dirty flag *isn't* set, it uses the
previously calculated data. This is obvious, but that does imply that you have
to keep that derived data around in memory in case you end up needing it later.

当推导数据被请求而脏标识*没有*设置，他使用之间计算出的数据。这很明显，但这需要你在内存中保存推导数据在内存中，以防之后你需要再次使用。

<aside name="sync">

This isn't much of an issue when you're using this pattern to synchronize the
primary state to some other place. In that case, the derived data isn't usually
in memory at all.

</aside>

If you weren't using this pattern, you could calculate the derived data on the
fly whenever you needed it, then discard it when you were done. That avoids the
expense of keeping it cached in memory at the cost of having to do that
calculation every time you need the result.

如果你没有使用这个模式，你可以在你需要的时候计算推导数据，然后在你完成之后释放。这避免了每一次计算后都将其存储回内存的代价。

Like many optimizations, then, this pattern <span name="trade">trades</span>
memory for speed. In return for keeping the previously calculated data in
memory, you avoid having to recalculate it when it hasn't changed. This
trade-off makes sense when the calculation is slow and memory is cheap. When
you've got more time than memory on your hands, it's better to calculate it
as needed.

就像很多优化一样，这种模式以内存换速度。通过在内存中保存之前计算的结果值，你避免了在它没有改变的情况下重新计算。这种就爱哦一在内存拍内衣而计算昂贵的时候是划算的。当你手头有更多空闲的时间而不是内存的时候，最好按需求重新计算之。

<aside name="trade">

Conversely, compression algorithms make the opposite trade-off: they optimize
*space* at the expense of the processing time needed to decompress.

</aside>

## Sample Code

## 示例代码

Let's assume we've met the surprisingly long list of requirements and see how
the pattern looks in code. As I mentioned before, the actual math behind
transform matrices is beyond the humble aims of this book, so I'll just
encapsulate that in a class whose implementation you can presume exists
somewhere out in the æther:

假设我们满足了出奇长的需求列表，看看模式字啊代码中是如何应用的。就像我在之前提到的那样，矩阵变换后面的数学超出了本书的范围，因此我将其封装在一个类中，假设它们在其他某处实现了：

^code transform

The only operation we need here is `combine()` so that we can get an object's
world transform by combining all of the local transforms along its parent chain.
It also has a method to get an "origin" transform -- basically an identity
matrix that means no translation, rotation, or scaling at all.

这里我们唯一需要的操作就是`combine()`这样我们可以通过将所有父节点链上的自身转换组合起来获得一个对象的世界转换。同样有一个方法来获得“原先”的转换——通常是一个身份矩阵，没有平移，旋转，或者拉伸。

Next, we'll sketch out the class for an object in the scene graph. This is the
bare minimum we need *before* applying this pattern:

下面我们勾勒出场景图中的对象类。这是在我们应用模式*之前*，我们需要的最低限度的东西。

^code graph-node

Each node has a local transform which describes where it is relative to its
parent. It has a mesh which is the actual graphic for the object. (We'll allow
`mesh_` to be `NULL` too to handle non-visual nodes that are used just to group
their children.) Finally, each node has a possibly empty collection of child
nodes.

每一个节点都有一个原始转换描述了他和它的父节点之间的关系。他有对象的真实网格状结构。（我们将`mesh_`置为`NULL`来处理不可见的用来组织子节点的结点。）最终每一个节点都有一个可能为空的子节点集合。

With this, a "scene graph" is really only a single root `GraphNode` whose
children (and grandchildren, etc.) are all of the objects in the world:

通过这样，一个“场景图”只有一个简单的`GraphNode`，它是世界里所有的子节点（以及孙子节点）的根。

^code scene-graph

In order to render a scene graph, all we need to do is traverse that tree of
nodes, starting at the root, and call the following function for each node's mesh
with the right world transform:

为了渲染场景图，我们需要的就是遍历节点树，从根开始，然后使用正确的世界变换为每个节点的王调用函数：

^code render

We won't implement this here, but if we did, it would do whatever magic the
renderer needs to draw that mesh at the given location in the world. If we can
call that correctly and efficiently on every node in the scene graph, we're
happy.

我们还没有直接在这里实现，但是如果我们这么做了，那么它就会做渲染需要的事情将那张网格绘制在世界上给定的位置。如果我们对场景图中的每一个节点都正确有效的调用，我们就很高兴了。

### An unoptimized traversal

### 一个没有优化的遍历

To get our hands dirty, let's throw together a basic traversal for rendering the
scene graph that calculates the world positions on the fly. It won't be optimal,
but it will be simple. We'll add a new method to `GraphNode`:

为了把我们的手弄脏，我们做一个简单的遍历，在渲染需要的时候去计算所有的位置。这没有优化，但它很简单。我们添加一个新方法给`GraphNode`：

^code render-on-fly

We pass the world transform of the node's parent into this using `parentWorld`.
With that, all that's left to get the correct world transform of *this* node is
to combine that with its own local transform. We don't have to walk *up* the
parent chain to calculate world transforms because we calculate as we go while
walking *down* the chain.

我们使用`parentWorld`将父节点的世界转换传入节点。通过这样，需要获得*这个*节点的世界转换只需要将其和节点本身转换相结合。我们不需要*向上*遍历父节点去计算世界转换，因为我们可以在*向下*遍历时计算。

We calculate the node's world transform and store it in `world`, then we render
the mesh, if we have one. Finally, we recurse into the child nodes, passing in
*this* node's world transform. All in all, it's a tight, simple recursive
method.

我们计算了节点的世界转换然后将其存储到`world`，然后我们渲染网格，如果有的话。最后我们遍历进入子节点，传入*这个*节点的世界转换。无论如何，这是一个紧密的，简单的遍历方法。

To draw an entire scene graph, we kick off the process at the root node:

为了绘制整个场景图，我们从根节点开始整个过程。

^code render-root

### Let's get dirty

### 让我们使用脏

So this code does the right thing -- it renders all the meshes in the right place
-- but it doesn't do it efficiently. It's calling `local_.combine(parentWorld)`
on every node in the graph, every frame. Let's see how this pattern fixes that.
First, we need to add two fields to `GraphNode`:

所以代码做了正确的事情——他在正确的地方渲染正确的网格——但是他没有有效的的完成。他在途中的每一个节点每帧调用`local_.combine(parentWorld)`。让我们看看这个模式是如何修复这一点的。首先，我们给`GraphNode`添加两个字段。

^code dirty-graph-node

The `world_` field caches the previously calculated world transform, and
`dirty_`, of course, is the dirty flag. Note that the flag starts out `true`.
When we create a new node, we haven't calculated its world transform yet. At
birth, it's already out of sync with the local transform.

`world_`字段缓存了前一个计算出来的世界转换，和`dirty_`字段，当然，是脏标识。注意标识开始的时候是`true`。当我们创建一个新节点的时候，我们还没有计算出它的世界转换。在开始时，他与自身转换是同步的。

The only reason we need this pattern is because objects can *move*, so let's add
support for that:

我们需要这个模式的唯一原因是对象可以*移动*，因此让我们添加这一点的支持：

^code set-transform

The important part here is that it sets the dirty flag too. Are we forgetting
anything? Right -- the child nodes!

这里重要的部分是设置脏标识。我们忘了什么事吗？是的——子节点！

When a parent node moves, all of its children's world coordinates are
invalidated too. But here, we aren't setting their dirty flags. We *could* do
that, but that's recursive and slow. Instead, we'll do something clever when we
go to render. Let's see:

当父节点移动时，所有它自己的的世界坐标也改变了。但是这里，我们还没有设置他们的脏标识。我们*可以*这样做，但这样做是递归而且缓慢。我们可以在渲染时做点更聪明的事情。让我们看看：

<span name="branch"></span>

^code dirty-render

<aside name="branch">

There's a subtle assumption here that the `if` check is faster than a matrix
multiply. Intuitively, you would think it is; surely testing a single bit is
faster than a bunch of floating point arithmetic.

However, modern CPUs are fantastically complex. They rely heavily on
*pipelining* -- queueing up a series of sequential instructions. A branch like
our `if` here can cause a *branch misprediction* and force the CPU to lose cycles
refilling the pipeline.

The <a href="data-locality.html" class="pattern">Data Locality</a> chapter has
more about how modern CPUs try to go faster and how you can avoid tripping them
up like this.

</aside>

This is similar to the original naïve implementation. The key changes are that
we check to see if the node is dirty before calculating the world transform and
we store the result in a field instead of a local variable. When the node is
clean, we skip `combine()` completely and use the old-but-still-correct `world_`
value.

这与原先的实现很相似。光剑改变是我们在计算他的世界转换之前去检查节点是不是脏的，然后将结果存在字段中而不是本地变量。如果节点是干净的，我们完全跳过了`combine()`使用了老的但是正确的`world_`值。

The <span name="clever">clever</span> bit is that `dirty` parameter. That will
be `true` if any node above this node in the parent chain was dirty. In much the
same way that `parentWorld` updates the world transform incrementally as we
traverse down the hierarchy, `dirty` tracks the dirtiness of the parent chain.

这个聪明的位是那个`dirty`参数。如果任何他父节点练上有任何是脏的，那么这个就是`true`。当我们顺着层次遍历下来，`parentWorld`用同样的方式更新他的世界转换，`dirty`标识了父节点链的脏。

This lets us avoid having to recursively mark each child's `dirty_` flag
in `setTransform()`. Instead, we pass the parent's dirty flag down to its
children when we render and look at that too to see if we need to recalculate
the world transform.

这让我们避免递归地调用`setTransform()`标注每一个子节点的`dirty_`标识。相反，我们将父节点的脏标识传递给他们的子节点，然后看看我们是否需要重新计算它的世界转换。

The end result here is exactly what we want: changing a node's local transform
is just a couple of assignments, and rendering the world calculates the exact
minimum number of world transforms that have changed since the last frame.

这里结果正是我们系那个要的：改变节点的自身转换只是一些什么，渲染世界的时候只计算从上一帧开始所需的最小数量的世界转换。

<aside name="clever">

Note that this clever trick only works because `render()` is the *only* thing in
`GraphNode` that needs an up-to-date world transform. If other things accessed
it, we'd have to do something different.

</aside>

## Design Decisions

## 设计决策

This pattern is fairly specific, so there are only a couple of knobs to twiddle:

这种模式非常具体的，所以只有几个旋钮来摆弄：

### When is the dirty flag cleaned?

### 什么时候清空脏标识？

* **When the result is needed:**

* *当结果被请求时？*

    * *It avoids doing calculation entirely if the result is never used.* For
        primary data that changes much more frequently than the derived data is
        accessed, this can be a big win.

    * *如果不需要结果，可以完全避免计算。*如果原始数据变化的速度比推导数据获取的速度快的多，这可以是很大的胜利。

    * *If the calculation is time-consuming, it can cause a noticeable pause.*
        Postponing the work until the player is expecting to see the result can
        affect their gameplay experience. It's often fast enough that this
        isn't a problem, but if it is, you'll have to do the work earlier.

    * *如果计算是消耗大量时间，这可以造成一个可察觉的暂停。*将工作推迟到玩家想要结果的时候会严重影响他们的游戏体验。这部分工作一般足够快，不会构成问题，但是如果是问题，你就需要早一些做这些工作。

* **At well-defined checkpoints:**

* *在好好定义的检查点处：*

    Sometimes, there is a point in time or in the progression of the game where it's
    natural to do the deferred processing. For example,
    we may want to save the game only when the pirate sails into port. Or the
    sync point may not be part of the game mechanics. We may just want to hide the
    work behind a loading screen or a cut scene.

    有时候，有一个时间点或者在游戏的过程中很自然的需要推迟处理。例如，我们只有海盗驶入港口才会去保存游戏。或者同步点不是游戏的机制，我们的将这些工作隐藏在加载画面或者过程动画之后。

    * *Doing the work doesn't impact the user experience.* Unlike the previous
      option, you can often give something to
        distract the player while the game is busy processing.

    * *做这种工作不会影响到玩家体验。*不像前一个选项，游戏在紧张运行的时候，你总能转移玩家的注意力。

    * *You lose control over when the work happens.* This is sort of the
        opposite of the earlier point. You have micro-scale control over when you
        process, and can make sure the game handles it gracefully.

    * *你会丧失何时工作的控制权。*这将工作提前的反面。你在进行的时候有微观控制，能确保有效优雅的处理它。

        What you *can't* do is ensure the player actually makes it to the
        checkpoint or meets whatever criteria you've defined. If they get lost
        or the game gets in a weird state, you can end up deferring
        longer than you expect.

        你*不能*做的是保证玩家真的到了检查点或者满足了你定义的条件。如果他们在游戏中迷失了，或者游戏进入了一个奇怪的状态，你最终会将工作推迟到超过你预料的晚。

* **In the background:**

* *在后台处理：*

    Usually, you start a fixed <span name="hysteresis">timer</span>
    on the first modification and then process all of the changes that happen
    between then and when the timer fires.

    通常情况下，你为每个更改启动一个固定的计时器，然后在计时器开始计时后处理所哟丶变化。

    <aside name="hysteresis">

    The term in human-computer interaction for an intentional delay between
    when a program receives user input and when it responds is [*hysteresis*](http://en.wikipedia.org/wiki/Hysteresis).

    </aside>

    * *You can tune how often the work is performed.* By adjusting the timer
        interval, you can ensure it happens as frequently (or infrequently) as
        you want.

    * *你可以控制工作进行的多么频繁。*通过调节计时器，你可以保证他处理的像你预期一样频繁（或者不频繁）。

    * *You can do more redundant work.* If the primary state only changes a
        tiny amount during the timer's run, you can end up processing a large
        chunk of mostly unchanged data.

    * *你会做更多冗余工作。*如果原始状态在计时器运行之间只改变了很少的部分，最终你会处理的大部分都是没有改变的数据。

    * *You need support for doing work asynchronously.*
        Processing the data "in the background" implies that the player can
        keep doing whatever it is that they're doing at the same time. That
        means you'll likely need threading or some other kind of concurrency
        support so that the game can work on the data while it's still
        being played.

        Since the player is likely interacting with
        the same primary state that you're processing, you'll need to think
        about making that safe for concurrent modification too.

    * *你需啊哟同步支持工作。*在“后台”处理数据意味着玩家可以同时继续做他们正在做的事情。这就意味着你将会需要线程或者其他并行支持，这样游戏在处理数据的同时仍然可以保持游玩。

### How fine-grained is your dirty tracking?

### 你的脏追踪做的有多细粒度？

Imagine our pirate game lets players build and customize their pirate ship.
Ships are automatically saved online so the player can resume where they left
off. We're using dirty flags to determine which decks of the ship have been
fitted and need to be sent to the server. Each chunk of data we send to the
server contains some modified ship data and a bit of metadata describing where
on the ship this modification occurred.

* **If it's more fine-grained:**

* *用越细的粒度：*

    Say you slap a dirty flag on each tiny plank of each deck.

    假设你为甲板上的每一个小木板都拍上一个脏标识。

    * *You only process data that actually changed.* You'll send exactly the
        facets of the ship that were modified to the server.

    * *你只需处理真正改变的数据。*你只处理修改了船舶方面的数据到服务器。

* **If it's more coarse-grained:**

* *如果粒度更粗：*

    Alternatively, we could associate a dirty bit with each deck.
    Changing anything on it marks the entire deck <span name="swab">dirty</span>.

    或者，我们可以每层甲板关联一个脏标识。改变它上面的任何东西都会让整个甲板变脏。

    <aside name="swab">

    I could make some terrible joke about it needing to be swabbed here, but
    I'll refrain.

    </aside>

    * *You end up processing unchanged data.* Add a single barrel to a deck
        and you'll have to send the whole thing to the server.

    * *你最终需要处理没有变化的数据。*在甲板上添加一个桶，就要将全部的东西发送到服务器。

    * *Less memory is used for storing dirty flags.* Add ten barrels to a deck
        and you only need a single bit to track them all.

    * *更少的内存被用在存储脏标识上。*为甲板上添加十个桶只需要一位来追踪他们。

    * *Less time is spent on fixed overhead.* When processing some changed data,
       there's often a bit of fixed work you have to do on top of handling the
       data itself. In the example here, that's the metadata required to
       identify where on the ship the changed data is. The bigger your
       processing chunks, the fewer of them there are, which means the less
       overhead you have.

    * *较少的时间花在固定开销上。*当处理某些改变了的数据时，通常需要处理顶部的数据本身。在这个例子中，有些元数据需要确认船上改变的数据在哪里。你处理的块越大，那么要处理的数量就越少，这就意味着有更小的开销。

## See Also

## 参见

 *  This pattern is common outside of games in browser-side web frameworks like
    [Angular](http://angularjs.org/). They use dirty flags to track which data
    has been changed in the browser and needs to be pushed up to the server.

 *  这个模式在游戏之外的领域比如浏览器方向的框架是很常见的。他们使用脏标识来追踪哪个数据在浏览器中被改变了，需要被推向服务器。

 *  Physics engines track which objects are in motion and which are resting.
    Since a resting body won't move until an impulse is applied to it, they
    don't need processing until they get touched. This "is moving" bit is a
    dirty flag to note which objects have had forces applied and need to have
    their physics resolved.

 *  物理引擎最终哪些对那些对象在运动中哪些在休息，由于休息的骨骼知道有力施加在上面才会移动，他们知道被碰到才会需要处理。这种“正在移动”的位是一个脏标识来标注那一个对象上面有力施加，需要他们的物理解析。