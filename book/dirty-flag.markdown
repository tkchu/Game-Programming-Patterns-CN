^title Dirty Flag
^section Optimization Patterns

^title 脏标识
^section Optimization Patterns

## Intent

## 意图

*Avoid unnecessary work by deferring it until the result is needed.*

*将工作延期至需要其结果时才去执行，避免不必要的工作。*

## Motivation

## 动机

Many games have something called a *scene graph*. This is a big data structure
that contains all of the objects in the world. The rendering engine uses this to
determine where to draw stuff on the screen.

很多游戏有*场景图*。
那是一个巨大的数据结构，包含了游戏世界中所有的对象。
渲染引擎使用它决定在屏幕哪里画东西。

At its simplest, a scene graph is just a flat list of objects. Each object has a
model, or some other graphic primitive, and a <span
name="transform">*transform*</span>. The transform describes the object's
position, rotation, and scale in the world. To move or turn an object, we simply
change its transform.

最简单的实现中，场景图只是对象列表。
每个对象都有模型，或者其他的原始图形，以及<span name="transform">*转换*</span>。
转换描述了对象在世界中的位置，方向，拉伸。
为了移动或者旋转对象，只需简单地改变它的转换。

<aside name="transform">

The mechanics of *how* this transform is stored and manipulated are unfortunately
out of scope here. The comically abbreviated summary is that it's a 4x4 matrix.
You can make a single transform that combines two transforms -- for example,
translating and then rotating an object -- by multiplying the two matrices.

*如何*存储和操作转换的机制很不幸超出了本书范围。
一个简单的总结是一个4x4的矩阵。
你可以通过矩阵相乘来组合两个转换，获得单一转换——举个例子，平移之后旋转对象。

How and why that works is left as an exercise for the reader.

它如何工作，以及为什么那样工作是留给读者的练习。

</aside>

When the renderer draws an object, it takes the object's model, applies the
transform to it, and then renders it there in the world. If we had a scene
*bag* and not a scene *graph*, that would be it, and life would be simple.

当渲染系统描绘对象，它取出对象的模型，对其应用转换，然后将其渲染到游戏世界中。
如果我们有场景*包*而不是场景*图*，那就是这样了，生活很简单。

However, most scene graphs are <span name="hierarchical">*hierarchical*</span>.
An object in the graph may have a parent object that it is anchored to. In that
case, its transform is relative to the *parent's* position and isn't an
absolute position in the world.

但是，大多数场景图都是<span name="hierarchical">*有等级的*</span>。
图像中的对象也许拥有它锚定的父对象。
这种情况下，它的转换与其*父对象*的位置相关，不再是游戏世界上的绝对位置。

For example, imagine our game world has a pirate ship at sea. Atop the ship's
mast is a crow's nest. Hunched in that crow's nest is a pirate. Clutching the
pirate's shoulder is a parrot. The ship's local transform positions the ship in
the sea. The crow's nest's transform positions the nest on the ship, and so on.

举个例子，将游戏世界想象为海上的海盗船。
桅杆的顶端有瞭望塔，瞭望塔中有海盗，海盗肩上有鹦鹉。
船本身的转换定位船在海上的位置。瞭望塔的转换定位它在船上的位置，诸如此类。

<span name="pirate"></span>
<img src="images/dirty-flag-pirate.png" alt="A pirate ship containing a crow's nest with a pirate in it with a parrot on his shoulder." />

<span name="pirate"></span>
<img src="images/dirty-flag-pirate.png" alt="A pirate ship containing a crow's nest with a pirate in it with a parrot on his shoulder." />

<aside name="pirate">

Programmer art!

编程艺术！

</aside>

This way, when a parent object moves, its children move with it automatically.
If we change the local transform of the ship, the crow's nest, pirate, and
parrot go along for the ride. It would be a total <span
name="slide">headache</span> if, when the ship moved, we had to manually adjust
the transforms of all the objects on it to keep them from sliding off.

这样的话，当父对象移动时，孩子节点也自动的跟着移动。
如果改变了船自身的转换，瞭望塔，海盗和鹦鹉都会随之移动。
如果当船移动时，就得手动调整每个对象来保证它们不滑出去，那可相当<span name="slide">令人头疼</span>。

<aside name="slide">

To be honest, when you are at sea you *do* have to keep manually adjusting your
position to keep from sliding off. Maybe I should have chosen a drier example.

老实说，当你在海上，你*确实*需要手动调整姿势来保证不滑出去。
也许我应该选一个更好的例子。

</aside>

But to actually draw the parrot on screen, we need to know its absolute position
in the world. I'll call the parent-relative transform the object's *local
transform*. To render an object, we need to know its *world transform*.

但是为了在屏幕上真正的描绘鹦鹉，我需要知道它在世界上的绝对位置。
我会调用父节点相关的转换对象的*自身转换*。
为了渲染一个对象，我需要知道它的*世界转换*。

### Local and world transforms

### 自身转换和世界转换

Calculating an object's world transform is pretty straightforward -- you just walk
its parent chain starting at the root all the way down to the object, combining
transforms as you go. In other words, the parrot's world transform is:

计算对象的世界转换很直接——从它的父节点一直追踪到对象，将经过的所有转换绑在一起。
换言之，鹦鹉的世界转换如下：

<span name="degenerate"></span>
<img src="images/dirty-flag-multiply.png" alt="The parrot's world position comes from multiplying the local positions for the ship, nest, pirate, and parrot." />

<span name="degenerate"></span>
<img src="images/dirty-flag-multiply.png" alt="The parrot's world position comes from multiplying the local positions for the ship, nest, pirate, and parrot." />

<aside name="degenerate">

In the degenerate case where the object has no parent, its local and world
transforms are equivalent.

在对象没有父对象的情况下，它的自身转换和世界转换是一样的。

</aside>

We need the world transform for every object in the world every frame, so even
though there are only a handful of matrix multiplications per model, it's on the hot
code path where performance is critical. Keeping them up to date is tricky
because when a parent object moves, that affects the world transform of itself
and all of its children, recursively.

我们每帧需要为游戏世界的每个对象做世界转换，因此哪怕每个模型只有一部分矩阵乘法，当性能攸关时，它也在热点代码路径上。
保持它们更新是有技巧的，因为当父对象移动时，它影响自己的世界转换，并递归影响所有孩子节点。

The simplest approach is to calculate transforms on the fly while
rendering. Each frame, we recursively traverse the scene graph starting at the
top of the hierarchy. For each object, we calculate its world transform right
then and draw it.

最简单的方法是在渲染时计算转换。
每一帧，我们从最高层递归遍历整个场景图。
我们计算每个对象的世界转换然后绘制它。

But this is terribly wasteful of our precious CPU juice! Many objects in the
world are *not* moving every frame. Think of all of the static geometry that
makes up the level. Recalculating their world transforms each frame even though
they haven't changed is a waste.

但这完全是在浪费CPU！
很多游戏世界的对象*不是*在每帧都移动。
想想那些构成关卡的静态几何图形。
在没有改变的情况下每帧计算它们的世界转换是一种浪费。

### Cached world transforms

### 缓存世界转换

The obvious answer is to *cache* it. In each object, we store its local
transform and its derived world transform. When we render, we only use the
precalculated world transform. If the object never moves, the cached transform
is always up to date and everything's happy.

明显的答案是*缓存*它。
在每个对象中，我们存储它的自身转换和世界转换。
当我们渲染时使用预计算的世界转换。
如果对象从未移动，缓存的转换永远跟得上变化，每个人都很开心。

When an object *does* move, the simple approach is to refresh its world
transform right then. But don't forget the hierarchy! When a parent moves, we
have to recalculate its world transform *and all of its children's,
recursively*.

当一个对象*确实*移动了，简单的解决方式是之后就更新世界转换。
但是不要忘记层次性！当父节点移动时，我们得计算它的世界转换并*递归计算它所有的子对象*。

Imagine some busy gameplay. In a single frame, the ship gets tossed on the
ocean, the crow's nest rocks in the wind, the pirate leans to the edge, and the
parrot hops onto his head. We changed four local transforms. If we recalculate
world transforms eagerly whenever a local transform changes, what ends up
happening?

想象游戏中忙碌的时刻。
在一帧中，船在海上颠簸，瞭望塔在风中摇晃，海盗甩到了边缘，而鹦鹉撞上了他的脑袋。
我们改变了四个自身转换。如果每次自身转换都立即更新世界转换，会发生什么？

<span name="stars"></span>
<img src="images/dirty-flag-update-bad.png" alt="Any time an object moves, the world coordinates are recalculated eagerly and redundantly." />

<span name="stars"></span>
<img src="images/dirty-flag-update-bad.png" alt="Any time an object moves, the world coordinates are recalculated eagerly and redundantly." />

<aside name="stars">

You can see on the lines marked &#x2605; that we're recalculating the parrot's
world transform *four* times when we only need the result of the final one.

你可以看到在标为&#x2605;的线上，我们重复计算了鹦鹉世界转换四次，但我们只需要最终的结果。

</aside>

We only moved four objects, but we did *ten* world transform calculations.
That's six pointless calculations that get thrown out before they are ever used
by the renderer. We calculated the parrot's world transform *four* times, but it
is only rendered once.

我只移动四个对象，但我们做了*十次*世界转换计算。
那就有六次在被渲染器使用前浪费了。
我们计算了鹦鹉的世界转换*四*次，但它只需渲染一次。

The problem is that a world transform may depend on several local transforms.
Since we recalculate immediately each time *one* of the transforms changes, we end up
recalculating the same transform multiple times when more than one of the local
transforms it depends on changes in the same frame.

问题在于世界转换也许会依赖于多个自身转换。
由于我们*每次*变化就立即重新计算，当自身转换依赖的多个世界转换同一帧发生变化时，我们就对同一转换做了多次重新计算。

### Deferred recalculation

### 延期重计算

We'll solve this by <span name="decoupling">decoupling</span> changing local
transforms from updating the world transforms. This lets us change a bunch of
local transforms in a single batch and *then* recalculate the affected world
transform just once after all of those modifications are done, right before we
need it to render.

我们会通过<span name="decoupling">解耦</span>自身转换和世界转换的更新来解决这个问题。
这让我们先在一次批处理中改变自身转换，在这些改变完成*之后*，在我们需要渲染它之前，重新计算它们世界转换。

<aside name="decoupling">

It's interesting how much of software architecture is intentionally
engineering a little slippage.

很有趣，不少软件架构是故意为工程增加灵活度。

</aside>

To do this, we add a *flag* to each object in the graph. "Flag" and "bit" are
synonymous in programming -- they both mean a single micron of data that can be
in one of two states. We call those "true" and "false", or sometimes "set" and
"cleared". I'll use all of these interchangeably.

为了做到这点，我们为图中的每个对象添加*标识位*。
“标识”和“位”在编程中密切相关——都代表一小块处在两种状态之一的数据。
我们称之为“真”和“假”，或者有时“设置”和“清除”。
我之后会交替使用它们。

When the local transform
changes, we set it. When we need the object's world transform, we check the
flag. If it's set, we calculate the world transform and then clear the flag. The
flag represents, "Is the world transform out of date?" For reasons that aren't
entirely clear, the traditional name for this "out-of-date-ness" is "dirty".
Hence: *a dirty flag*. "Dirty bit" is an equally
<span name="specific">common</span> name for this pattern, but I figured I'd
stick with the name that didn't seem as prurient.

当自身转换改变了，我们设置它。
当我们需要对象的世界转换时，我们检查这位。
如果它被设置了，计算世界转换然后清除标识。
那个标识代表着，“世界转换过时了吗？”
由于它们没有清除，这种“过时的杂乱”被称为“脏”。
也就是*脏标识*。“脏位”也是这模式<span name="specific">通常</span>使用的名字，但是我决定使用不那么下流的称呼。

<aside name="specific">

Wikipedia's editors don't have my level of self-control and went with [dirty
bit](http://en.wikipedia.org/wiki/Dirty_bit).

维基百科的编辑者没有我这样的自我控制水平，使用了[dirty bit](http://en.wikipedia.org/wiki/Dirty_bit).

</aside>

If we apply this pattern and then move all of the objects in our previous
example, the game ends up doing:

如果我们应用这一章，然后移动之前例子中所有对象，那么游戏最终是这样的：

<img src="images/dirty-flag-update-good.png" alt="By deferring until all moves are done, we only recalculate once." />

That's the best you could hope to do -- the world transform for each affected
object is calculated exactly once. With only a single bit of data, this pattern
does a few things for us:

这就是你能希望得到的最好结果了——每个受到影响的对象的世界转换只被计算一次。使用仅仅一位数据，这个模式为我们做了以下事情：

 *  It collapses modifications to multiple local transforms along an object's
    parent chain into a single recalculation on the object.

 *  它将对象的父节点链上的众多自身转换变化归并成对象上的一次计算。

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

一组*原始数据*随着时间变化而改变。
使用*昂贵的过程*推定一组*导出数据*。
一个*“脏”标识*追踪导出数据是否与原始数据保持一致。
它在*原始数据改变时被设置。*
否则的话，使用之前*缓存的导出数据*。

## When to Use It

## 何时使用

Compared to some other patterns in this book, this one solves a pretty specific
problem. Also, like most optimizations, you should only reach for it when you
have a performance problem big enough to justify the added code complexity.

与这本书中的其他模式相比，这个解决了一个非常特殊的问题。
同时，就像其他优化一样，只在有足够大的性能问题时，再使用这一章增加代码复杂度。

Dirty flags are applied to two kinds of work: *calculation* and
*synchronization*. In both cases, the process of going from the primary data to
the derived data is time-consuming or otherwise costly.

脏标识在两种任务上应用：“计算”和“同步”。
在两种情况下，从原始数据转换到导出数据消耗很多时间，或者有其他方面的消耗。

In our scene graph example, the process is slow because of the amount of math to
perform. When using this pattern for synchronization, on the other hand, it's
more often that the derived data is *somewhere else* -- either on disk or over
the network on another machine -- and simply getting it from point A to point B
is what's expensive.

在我们的场景图例子中，这个过程非常缓慢是因为需要执行很多数学运算。
应用这个章节在同步上，是另一个应用场景，
导出数据在*别的地方*——在磁盘上或者在网络另一头的终端机上——从点A到点B消耗很大。

There are a couple of other requirements too:

这里是一些其他应用场景：

 *  **The primary data has to change more often than the derived data is used.**
    This pattern works by avoiding processing derived data when a subsequent
    primary data change would invalidate it before it gets used. If you find
    yourself always needing that derived data after every single modification
    to the primary data, this pattern can't help.

 *  *原始数据的变化速度远高于导出数据的使用速度。*避免在导出数据使用前原始数据多次变化带来的不必要计算。如果你发现总在原始数据变化后使用导出数据，这个模式无法帮忙。

 *  **It should be hard to update incrementally.** Let's say the
    pirate ship in our game can only carry so much booty. We need to
    know the total weight of everything in the hold. We
    *could* use this pattern and have a dirty flag for the total weight. Every
    time we add or remove some loot, we set the flag. When we need the
    total, we add up all of the booty and clear the flag.

 *  *增量更新应该是困难的。*假设海盗船只能携带特定数量的战利品。我们需要获取携带事物的总重量。我们*可以*使用这个模式，然后为总重量设脏标识。每次添加或者移除一些战利品，我们设置这个标识。当我们需要总量，将所有战利品重量加起来，然后清除标识。

    But a simpler solution is to *keep a running total*. When we add or
    remove an item, just add or remove its weight from the current total. If
    we can "pay as we go" like this and keep the derived data updated, then
    that's often a better choice than using this pattern and calculating the
    derived data from scratch when needed.

    但是更简单的解决方法是*保存计算总量*。当我们添加或删除事物，直接从现在的总重量添加或者删除它的重量。如果我们可以*承担得起消耗*，保持导出数据更新，那么更好的选择是不用这个模式，每次需要时重新计算导出数据。

This makes it sound like dirty flags are rarely appropriate, but you'll
find a place here or there where they help. <span name="hacks">Searching</span>
your average game codebase for the word "dirty" will often turn up uses of this
pattern.

这听起来脏标识很少使用，但你会找到一两个部分它能帮得上忙。
直接在你的游戏代码库中<span name="hacks">搜索</span>“dirty”，通常会发现这个模式的使用。

<aside name="hacks">

From my research, it also turns up a lot of comments apologizing for "dirty"
hacks.

根据我的研究，也能找到很多对“dirty”黑魔法的抱怨注释。

</aside>

## Keep in Mind

## 记住

Even after you've convinced yourself this pattern is a good fit, there are a few
wrinkles that can cause you some discomfort.

哪怕是在说服自己这个模式在这里很恰当之后，这里还有一些东西可能会让你不舒服。

### There is a cost to deferring for too long

### 延期太久是有代价的

This pattern defers some slow work until the result is actually needed, but when
it is, it's often needed *right now*. But the reason we're using this pattern to
begin with is because calculating that result is slow!

这个模式将缓慢的工作延期到结果真正需要的时候，但是当它要的时候，通常是*现在就要*。
但是我们使用这个模式的原因是计算结果很慢！

This isn't a problem in our example because we can still calculate world
coordinates fast enough to fit within a frame, but you can imagine other cases
where the work you're doing is a big chunk that takes noticeable time to chew
through. If the game doesn't *start* chewing until right when the player expects
to see the result, that can cause an unpleasant visible <span
name="gc">pause</span>.

在例子中，这不是问题，因为我们还是可以在一帧之内计算世界坐标，但是可以想象其他情况下，工作需要可观时间来进行。
如果玩家想要结果时才*开始*计算，这会引起不愉快的可见<span name="gc">暂停</span>。

Another problem with deferring is that if something goes wrong, you may fail to
do the work at all. This can be particularly problematic when you're using this
pattern to save some state to a more persistent form.

延期的另一个问题是，如果有东西出错了，你也许根本做不了事情。
想要将模式存储为固定模式也许会造成问题。

For example, text editors know if your document has "unsaved changes". That
little bullet or star in your file's title bar is literally the dirty flag
visualized. The primary data is the open document in memory, and the derived
data is the file on disk.

举个例子，文本编辑器知道文档有“没保存的改变”。
在文件标题栏的小点或者星号就是可见的脏标识。
原始数据是在内存中打开的文档，推导数据是在磁盘上的文件。

<img src="images/dirty-flag-title-bar.png" alt="A window titlebar showing the little icon representing unsaved changes." />

Many programs don't save to disk until either the document is closed or the
application is exited. That's fine most of the time, but if you accidentally
kick the power cable out, there goes your masterpiece.

很多程序直到文档关闭或者应用退出才保存到磁盘上。
在大多数情况下这很好，但是如果一不小心踢到了插线板，你的主要工作也就随风而逝了。

Editors that auto-save a backup in the background are compensating specifically
for this shortcoming. The auto-save frequency is a point on the continuum
between not losing too much work when a crash occurs and not thrashing the file
system too much by saving all the time.

在后台自动保存备份的编辑器是这一失误的补偿。
自动保存的频率保持在崩溃时不丢失太多数据和不总是打扰文件系统保存文件之间。

<aside name="gc">

This mirrors the different garbage collection strategies in systems that
automatically manage memory. Reference counting frees memory the second it's no
longer needed, but it burns CPU time updating ref counts eagerly every time
references are changed.

这反映了系统中自动内存管理的不同的垃圾回收策略。
引用计数在不需要内存时立即释放它，但每次引用改变时都会更新引用计数，那消耗了大量CPU时间。

Simple garbage collectors defer reclaiming memory until it's really needed, but
the cost is the dreaded "GC pause" that can freeze your entire game until the
collector is done scouring the heap.

简单的垃圾回收器将回收内存拖延到需要内存时，但是代价是可怕的，“垃圾回收过程”会冻住整个游戏，直到回收器完成了对堆的处理。

In between the two are more complex systems like deferred ref-counting and
incremental GC that reclaim memory less eagerly than pure ref-counting but more
eagerly than stop-the-world collectors.

在两者之间是更复杂的系统，像推迟的引用计数和增量的垃圾回收，比起纯粹的引用计数回收要消极，但比冻住游戏回收的系统更积极。

</aside>

### You have to make sure to set the flag *every* time the state changes

### *每次*状态改变你都得保证设置标识。

Since the derived data is calculated from the primary data, it's essentially a
cache. Whenever you have cached data, the trickiest aspect of it is <span
name="cache">*cache invalidation*</span> -- correctly noting when the cache is
out of sync with its source data. In this pattern, that means setting the dirty
flag when *any* primary data changes.

由于推导数据是从原始数据推导而来的，它实际上是缓存。
无论何时缓存了数据，都是需要保证<span name="cache">*缓存一致性*</span>——在缓存与原始数据不同步时通知之。
在这个模式上，这意味着在*任何*原始数据变化时设置脏标识。

<aside name="cache">

Phil Karlton famously said, "There are only two hard things in Computer Science:
cache invalidation and naming things."

Phil Karlton有句名言：“计算机科学中只有两件难事：缓存一致性和命名。”

</aside>

Miss it in one place, and your program will incorrectly use stale derived data.
This leads to confused players and bugs that are very hard to track down. When you use
this pattern, you'll have to take care that any code that modifies the primary
state also sets the dirty flag.

一处遗漏，你的程序就使用了不正确的推导数据。
这引起了玩家的困惑和非常难以追踪的漏洞。
当使用这个模式时，你也得注意，任何修改了原始数据的代码都得设置脏标识。

One way to mitigate this is by encapsulating modifications to the primary data
behind some interface. If anything that can change the state goes through a
single narrow API, you can set the dirty flag there and rest assured that it
won't be missed.

一种解决它的方法是将原始数据的修改隐藏在接口之后。
任何想要改变状态的代码都要通过一个API，你可以在那里设置脏标识来保证不会遗漏。

### You have to keep the previous derived data in memory

### 得将之前的推导数据保存在内存中。

<span name="sync"></span>

<span name="sync"></span>

When the derived data is needed and the dirty flag *isn't* set, it uses the
previously calculated data. This is obvious, but that does imply that you have
to keep that derived data around in memory in case you end up needing it later.

当推导数据被请求而脏标识*没有*设置，它使用之前计算出的数据。
这很明显，但这需要在内存中保存推导数据，以防之后需要再次使用。

<aside name="sync">

This isn't much of an issue when you're using this pattern to synchronize the
primary state to some other place. In that case, the derived data isn't usually
in memory at all.

如果你用这个模式将原始状态同步到其他地方，这不是问题。
那样的话，推导数据通常不再在内存里了。

</aside>

If you weren't using this pattern, you could calculate the derived data on the
fly whenever you needed it, then discard it when you were done. That avoids the
expense of keeping it cached in memory at the cost of having to do that
calculation every time you need the result.

如果你没有使用这个模式，可在需要时计算推导数据，然后在完成后释放。
这避免了每次计算后都将其存储回内存的代价。

Like many optimizations, then, this pattern <span name="trade">trades</span>
memory for speed. In return for keeping the previously calculated data in
memory, you avoid having to recalculate it when it hasn't changed. This
trade-off makes sense when the calculation is slow and memory is cheap. When
you've got more time than memory on your hands, it's better to calculate it
as needed.

就像很多优化一样，这种模式以内存<span name="trade">换</span>速度。
通过在内存中保存之前计算的结果，避免了在它没有改变的情况下重新计算。
这种交易在内存便宜而计算昂贵时是划算的。
当你手头有更多空闲的时间而不是内存的时候，最好按需求重新计算之。

<aside name="trade">

Conversely, compression algorithms make the opposite trade-off: they optimize
*space* at the expense of the processing time needed to decompress.

相反，压缩算法做了反向的交易：
它们优化*空间*，代价是解压时额外的处理时间。

</aside>

## Sample Code

## 示例代码

Let's assume we've met the surprisingly long list of requirements and see how
the pattern looks in code. As I mentioned before, the actual math behind
transform matrices is beyond the humble aims of this book, so I'll just
encapsulate that in a class whose implementation you can presume exists
somewhere out in the æther:

假设我们满足了出奇长的需求列表，看看在代码中模式是如何应用的。
就像我之前提到的那样，矩阵变换背后的数学超出了本书的范围，因此我将其封装在类中，假设在其他某处已经实现了：

^code transform

The only operation we need here is `combine()` so that we can get an object's
world transform by combining all of the local transforms along its parent chain.
It also has a method to get an "origin" transform -- basically an identity
matrix that means no translation, rotation, or scaling at all.

这里我们唯一需要的操作就是`combine()`，
这样可以将父节点链上所有的自身转换组合起来获得对象的世界转换。
同样有办法来获得“原点”转换——通常是一个单位矩阵，没有平移，旋转，或者拉伸。

Next, we'll sketch out the class for an object in the scene graph. This is the
bare minimum we need *before* applying this pattern:

下面，我们勾勒出场景图中的对象类。这是在应用模式*之前*，需要的最低限度的东西。

^code graph-node

Each node has a local transform which describes where it is relative to its
parent. It has a mesh which is the actual graphic for the object. (We'll allow
`mesh_` to be `NULL` too to handle non-visual nodes that are used just to group
their children.) Finally, each node has a possibly empty collection of child
nodes.

每个节点都有原始转换描述了它和父节点之间的关系。
它有对象的真实网格。（将`mesh_`置为`NULL`来处理子节点的不可见节点。）
最终每个节点都有可能为空的子节点集合。

With this, a "scene graph" is really only a single root `GraphNode` whose
children (and grandchildren, etc.) are all of the objects in the world:

通过这样，“场景图”只是简单的`GraphNode`，它是所有的子节点（以及孙子节点）的根。

^code scene-graph

In order to render a scene graph, all we need to do is traverse that tree of
nodes, starting at the root, and call the following function for each node's mesh
with the right world transform:

为了渲染场景图，我们需要的就是从根开始遍历节点树，然后使用正确的世界变换为每个节点的网格调用函数：

^code render

We won't implement this here, but if we did, it would do whatever magic the
renderer needs to draw that mesh at the given location in the world. If we can
call that correctly and efficiently on every node in the scene graph, we're
happy.

我们不会直接在这里实现，但是如果我们这么做了，那么它就会做渲染需要的事，将网格绘制在世界上给定的位置。
如果对场景图中的每个节点都正确有效地调用，这就愉快地完成了。

### An unoptimized traversal

### 一个没有优化的遍历

To get our hands dirty, let's throw together a basic traversal for rendering the
scene graph that calculates the world positions on the fly. It won't be optimal,
but it will be simple. We'll add a new method to `GraphNode`:

为了把手弄脏，我们做一个简单的遍历，在渲染需要时去计算所有的位置。
这没有优化，但它很简单。我们添加一个新方法给`GraphNode`：

^code render-on-fly

We pass the world transform of the node's parent into this using `parentWorld`.
With that, all that's left to get the correct world transform of *this* node is
to combine that with its own local transform. We don't have to walk *up* the
parent chain to calculate world transforms because we calculate as we go while
walking *down* the chain.

使用`parentWorld`将父节点的世界转换传入节点。
这样，需要获得*这个*节点的世界转换只需要将其和节点本身转换相结合。
不需要*向上*遍历父节点去计算世界转换，因为我们可以在*向下*遍历时计算。

We calculate the node's world transform and store it in `world`, then we render
the mesh, if we have one. Finally, we recurse into the child nodes, passing in
*this* node's world transform. All in all, it's a tight, simple recursive
method.

我们计算了节点的世界转换，将其存储到`world`，如果有网格，渲染它。
最后我们遍历进入子节点，传入*这个*节点的世界转换。
无论如何，这是一个紧密的，简单的遍历方法。

To draw an entire scene graph, we kick off the process at the root node:

为了绘制整个场景图，我们从根节点开始整个过程。

^code render-root

### Let's get dirty

### 让我们变脏

So this code does the right thing -- it renders all the meshes in the right place
-- but it doesn't do it efficiently. It's calling `local_.combine(parentWorld)`
on every node in the graph, every frame. Let's see how this pattern fixes that.
First, we need to add two fields to `GraphNode`:

所以代码做了正确的事情——它在正确的地方渲染正确的网格——但是它没有有效地完成。
它在途中的每个节点上每帧调用`local_.combine(parentWorld)`。
让我们看看这个模式是如何修复这一点的。首先，我们给`GraphNode`添加两个字段。

^code dirty-graph-node

The `world_` field caches the previously calculated world transform, and
`dirty_`, of course, is the dirty flag. Note that the flag starts out `true`.
When we create a new node, we haven't calculated its world transform yet. At
birth, it's already out of sync with the local transform.

`world_`字段缓存了前一个计算出来的世界转换，和`dirty_`字段，当然，脏标识。
注意标识初始为`true`。当我们创建新节点时，我们还没有计算它的世界转换。
初始时，它与自身转换不是同步的。

The only reason we need this pattern is because objects can *move*, so let's add
support for that:

我们需要这个模式的唯一原因是对象可以*移动*，因此让我们添加对这点的支持：

^code set-transform

The important part here is that it sets the dirty flag too. Are we forgetting
anything? Right -- the child nodes!

这里重要的部分是设置脏标识。我们忘了什么事吗？是的——子节点！

When a parent node moves, all of its children's world coordinates are
invalidated too. But here, we aren't setting their dirty flags. We *could* do
that, but that's recursive and slow. Instead, we'll do something clever when we
go to render. Let's see:

当父节点移动时，它所有子节点的世界坐标也改变了。
但是这里，我们还没有设置它们的脏标识。
我们*可以*那样做，但是要递归而且缓慢。我们可以在渲染时做点更聪明的事。让我们看看：

<span name="branch"></span>

<span name="branch"></span>

^code dirty-render

<aside name="branch">

There's a subtle assumption here that the `if` check is faster than a matrix
multiply. Intuitively, you would think it is; surely testing a single bit is
faster than a bunch of floating point arithmetic.

这里有一个微妙的假设：`if`检查比矩阵乘法快。直观上，你会这么想，检测一位当然比一堆浮点计算要快。

However, modern CPUs are fantastically complex. They rely heavily on
*pipelining* -- queueing up a series of sequential instructions. A branch like
our `if` here can cause a *branch misprediction* and force the CPU to lose cycles
refilling the pipeline.

但是，现代CPU超级复杂。它们严重依赖于*流水线*——入队的一系列连续指令。
像我们这里的`if`造成的分支会引发*分支预测失败*，强迫CPU消耗周期在填满流水线上。

The <a href="data-locality.html" class="pattern">Data Locality</a> chapter has
more about how modern CPUs try to go faster and how you can avoid tripping them
up like this.

<a href="data-locality.html" class="pattern">数据局部性</a>一章有更多现代CPU是如何试图变快的，以及如何避免这样颠簸它们。

</aside>

This is similar to the original naïve implementation. The key changes are that
we check to see if the node is dirty before calculating the world transform and
we store the result in a field instead of a local variable. When the node is
clean, we skip `combine()` completely and use the old-but-still-correct `world_`
value.

这与原先的原始实现很相似。
关键改变是我们在计算世界转换之前去检查节点是不是脏的，然后将结果存在字段中而不是本地变量中。
如果节点是干净的，我们完全跳过了`combine()`使用了老的但是正确的`world_`值。

The <span name="clever">clever</span> bit is that `dirty` parameter. That will
be `true` if any node above this node in the parent chain was dirty. In much the
same way that `parentWorld` updates the world transform incrementally as we
traverse down the hierarchy, `dirty` tracks the dirtiness of the parent chain.

这个<span name="clever">聪明</span>的位是`dirty`参数。
如果父节点链上有任何节点是脏的，那么就是`true`。
当我们顺着层次遍历下来，`parentWorld`用同样的方式更新它的世界转换，`dirty`追踪父节点链的是否有脏。

This lets us avoid having to recursively mark each child's `dirty_` flag
in `setTransform()`. Instead, we pass the parent's dirty flag down to its
children when we render and look at that too to see if we need to recalculate
the world transform.

这让我们避免递归地调用`setTransform()`标注每个子节点的`dirty_`标识。
相反，我们将父节点的脏标识传递给子节点，然后看看是否需要重新计算它的世界转换。

The end result here is exactly what we want: changing a node's local transform
is just a couple of assignments, and rendering the world calculates the exact
minimum number of world transforms that have changed since the last frame.

这里结果正是我们需要的：改变节点的自身转换只是一些声明，渲染世界时只计算从上一帧开始所需的最小数量的世界转换。

<aside name="clever">

Note that this clever trick only works because `render()` is the *only* thing in
`GraphNode` that needs an up-to-date world transform. If other things accessed
it, we'd have to do something different.

注意这个技巧能有用是因为`render()`是`GraphNode`中*唯一*需要最新世界转换的。
如果其他东西也要获取，我们就得做点不同的事。

</aside>

## Design Decisions

## 设计决策

This pattern is fairly specific, so there are only a couple of knobs to twiddle:

这种模式非常具体，所以只有几个旋钮来摆弄：

### When is the dirty flag cleaned?

### 什么时候清空脏标识？

* **When the result is needed:**

* *当结果被请求时？*

    * *It avoids doing calculation entirely if the result is never used.* For
        primary data that changes much more frequently than the derived data is
        accessed, this can be a big win.

    * *如果不需要结果，可以完全避免计算。*如果原始数据变化的速度比推导数据获取的速度快得多，这可以是很大的胜利。

    * *If the calculation is time-consuming, it can cause a noticeable pause.*
        Postponing the work until the player is expecting to see the result can
        affect their gameplay experience. It's often fast enough that this
        isn't a problem, but if it is, you'll have to do the work earlier.

    * *如果计算消耗大量时间，这会造成可察觉的暂停。*将工作推迟到玩家想要结果的时候会严重影响游戏体验。这部分工作一般足够快，不会构成问题，但是如果构成问题，你就需要早一些做这些工作。

* **At well-defined checkpoints:**

* *在好好定义的检查点处：*

    Sometimes, there is a point in time or in the progression of the game where it's
    natural to do the deferred processing. For example,
    we may want to save the game only when the pirate sails into port. Or the
    sync point may not be part of the game mechanics. We may just want to hide the
    work behind a loading screen or a cut scene.

    有时候，某个时间点或在游戏过程中很自然的需要推迟处理。例如，只有海盗船驶入港口才会去保存游戏。如果同步点不是游戏的机制，我们将这些工作隐藏在加载画面或者过场动画之后。

    * *Doing the work doesn't impact the user experience.* Unlike the previous
      option, you can often give something to
        distract the player while the game is busy processing.

    * *做这种工作不会影响到玩家体验。*不像前一个选项，游戏在紧张运行时，你总能转移玩家的注意力。

    * *You lose control over when the work happens.* This is sort of the
        opposite of the earlier point. You have micro-scale control over when you
        process, and can make sure the game handles it gracefully.

    * *会丧失何时工作的控制权。*这和前一个选项相反。你在进行时能微观控制，确保有效优雅地处理它。

        What you *can't* do is ensure the player actually makes it to the
        checkpoint or meets whatever criteria you've defined. If they get lost
        or the game gets in a weird state, you can end up deferring
        longer than you expect.

        你*不能*做的是保证玩家真的到了检查点或者满足了定义的条件。如果他们在游戏中迷失了，或者游戏进入了奇怪的状态，你最终会将工作推迟晚的超乎预料。

* **In the background:**

* *在后台处理：*

    Usually, you start a fixed <span name="hysteresis">timer</span>
    on the first modification and then process all of the changes that happen
    between then and when the timer fires.

    通常情况下，你为每个更改启动固定间隔的<span name="hysteresis">计时器</span>，然后在计时器到时间后处理间隔中的所有变化。

    <aside name="hysteresis">

    The term in human-computer interaction for an intentional delay between
    when a program receives user input and when it responds is [*hysteresis*](http://en.wikipedia.org/wiki/Hysteresis).

    在人机交互界，描述程序接受用户的输入和回应之间的故意延迟的术语是[*hysteresis*](http://en.wikipedia.org/wiki/Hysteresis).

    </aside>

    * *You can tune how often the work is performed.* By adjusting the timer
        interval, you can ensure it happens as frequently (or infrequently) as
        you want.

    * *可以控制工作进行的频率。*通过调节计时器，可以保证它发生的像预期一样频繁（或者不频繁）。

    * *You can do more redundant work.* If the primary state only changes a
        tiny amount during the timer's run, you can end up processing a large
        chunk of mostly unchanged data.

    * *更多冗余工作。*如果原始状态在计时器运行之间只改变了很少的部分，最终处理的大部分都是没有改变的数据。

    * *You need support for doing work asynchronously.*
        Processing the data "in the background" implies that the player can
        keep doing whatever it is that they're doing at the same time. That
        means you'll likely need threading or some other kind of concurrency
        support so that the game can work on the data while it's still
        being played.

    * *需要同步支持工作。*
        在“后台”处理数据意味着玩家可以同时继续做正在做的事情。这就意味着你将会需要线程或者其他并行支持，这样游戏在处理数据的同时仍然可以继续游玩。

        Since the player is likely interacting with
        the same primary state that you're processing, you'll need to think
        about making that safe for concurrent modification too.

        由于玩家很可能与处理中的状态交互，你也需要考虑保持同步修改的安全。

### How fine-grained is your dirty tracking?

### 脏追踪做的有多细粒度？

Imagine our pirate game lets players build and customize their pirate ship.
Ships are automatically saved online so the player can resume where they left
off. We're using dirty flags to determine which decks of the ship have been
fitted and need to be sent to the server. Each chunk of data we send to the
server contains some modified ship data and a bit of metadata describing where
on the ship this modification occurred.

假设我们的海盗游戏允许玩家建造并个性化自己的船。
船在线自动保存，这样玩家可以从离开的地方继续。
我们使用脏标识记录船的哪块甲板被修改了，从而需要发送到服务器。
每一块发送给服务器的数据都包括了修改的船数据和一些描述船的哪里被修改的元数据。

* **If it's more fine-grained:**

* *如果粒度更细：*

    Say you slap a dirty flag on each tiny plank of each deck.

    假设你为甲板上的每个小木板都拍上一个脏标识。

    * *You only process data that actually changed.* You'll send exactly the
        facets of the ship that were modified to the server.

    * *你只需处理真正改变的数据。*你只处理船上修改了的数据到服务器。

* **If it's more coarse-grained:**

* *如果粒度更粗：*

    Alternatively, we could associate a dirty bit with each deck.
    Changing anything on it marks the entire deck <span name="swab">dirty</span>.

    或者，我们可以每层甲板关联一个脏标识。改变它上面的任何东西都会让整个甲板<span name="swab">变脏</span>。

    <aside name="swab">

    I could make some terrible joke about it needing to be swabbed here, but
    I'll refrain.

    我可以说些这里需要清洗的糟糕笑话，但我克制住了。

    </aside>

    * *You end up processing unchanged data.* Add a single barrel to a deck
        and you'll have to send the whole thing to the server.

    * *最终需要处理没有变化的数据。*在甲板上添加一个桶，就要将全部的东西发送到服务器。

    * *Less memory is used for storing dirty flags.* Add ten barrels to a deck
        and you only need a single bit to track them all.

    * *更少的内存被用在存储脏标识上。*为甲板上添加十个桶只需要一位来追踪。

    * *Less time is spent on fixed overhead.* When processing some changed data,
       there's often a bit of fixed work you have to do on top of handling the
       data itself. In the example here, that's the metadata required to
       identify where on the ship the changed data is. The bigger your
       processing chunks, the fewer of them there are, which means the less
       overhead you have.

    * *较少的时间花在固定开销上。*当处理某些改变的数据时，通常处理数据之前有些固定的工作。在这个例子中，是需要确认船上改变的数据在哪里的元数据。处理的块越大，那么要处理的数量就越少，这就意味着有更小的开销。

## See Also

## 参见

 *  This pattern is common outside of games in browser-side web frameworks like
    [Angular](http://angularjs.org/). They use dirty flags to track which data
    has been changed in the browser and needs to be pushed up to the server.

 *  这个模式在游戏之外的像[Angular](http://angularjs.org/)的浏览器方向框架，是很常见的。它们使用脏标识来追踪哪个数据在浏览器中被改变了，需要将数据推向服务器。

 *  Physics engines track which objects are in motion and which are resting.
    Since a resting body won't move until an impulse is applied to it, they
    don't need processing until they get touched. This "is moving" bit is a
    dirty flag to note which objects have had forces applied and need to have
    their physics resolved.


 *  物理引擎追踪哪些对象在运动中哪些在休息。由于休息的骨骼直到有力施加在上面才会移动，它们被碰到才会需要处理。这种“正在移动”的位是一个脏标识，标注哪个对象上面有力施加，需要物理解析。
