^title Double Buffer
^section Sequencing Patterns

^title 双缓冲
^section Sequencing Patterns

## Intent

## 意图

*Cause a series of sequential operations to appear instantaneous or
simultaneous.*

*用序列的操作展示瞬间或者同时。*

## Motivation

## 动机

In their hearts, computers are <span name="sequential">sequential</span> beasts.
Their power comes from being able to break down the largest tasks into tiny
steps that can be performed one after another. Often, though, our users need to
see things occur in a single instantaneous step or see multiple tasks performed
simultaneously.

在我们心中，电脑是<span name="sequential">连续</span>运行的野兽。
它的力量来自于将大的任务分解为小的步骤，这样可以一个接一个完成。
但是，通常用户需要看到事情在瞬间发生或者让多个任务同时进行。

<aside name="sequential">

With threading and multi-core architectures this is becoming less true, but
even with several cores, only a few operations are running concurrently.

使用线程和多核结构让这点不那么正确了，但哪怕有多核，只有一些操作可以同步运行。

</aside>

A typical example, and one that every game engine must address, is rendering.
When the game draws the world the users see, it does so one piece at a time -- the
mountains in the distance, the rolling hills, the trees, each in its turn. If
the user *watched* the view draw incrementally like that, the illusion of a
coherent world would be shattered. The scene must update smoothly and quickly,
displaying a series of complete frames, each appearing instantly.

一个典型的例子，也是每个游戏引擎都得掌控的问题，渲染。
当游戏渲染玩家见到的世界时，它同时处理了一大块——远处的山，起伏的丘陵，树木，每种都在各自的循环中。
如果在用户*观察*时增量做这些，连续世界的幻觉就会被打破。
场景必须快速流畅地更新，显示一系列完整的帧，每个都是立即出现。

Double buffering solves this problem, but to understand how, we first need to
review how a computer displays graphics.

双缓冲解决了这个问题，但是为了理解怎么去做，我们首先的复习计算机是如何显示图形的。

### How computer graphics work (briefly)

### 计算机图形是如何工作的（简短介绍）

A <span name="scan">video</span> display like a computer monitor draws one pixel
at a time. It sweeps across each row of pixels from left to right and then
moves down to the next row. When it reaches the bottom right corner, it scans
back up to the top left and starts all over again. It does this so fast --
around sixty times a second -- that our eyes can't see the scanning. To us, it's
a single static field of colored pixels -- an image.

在电脑屏幕上显示<span name="scan">视频</span>是一次绘制一个像素点。
它从左到右扫描每行像素点，然后移动至下一行。
当抵达了右下角，它退回左上角重新开始。
它做的飞快——每秒六十次——我们的眼睛看不出扫描。
对我们来说，这是一整张固定的有色像素——一张图像。

<aside name="scan">

This explanation is, err, "simplified". If you're a low-level hardware person
and you're cringing right now, feel free to skip to the next section. You
already know enough to understand the rest of the chapter. If you *aren't* that
person, my goal here is to give you just enough context to understand the
pattern we'll discuss later.

这个解释是，额，“简化过的”。
如果你是底层软件开发人员，跳过下一节吧。
你对这章的其余部分已经了解的够多了。
如果你不是，这里的目标是给你足够的背景知识理解等会讨论的模式。

</aside>

You can think of this process like a tiny hose that pipes pixels to the display.
Individual colors go into the back of the hose, and it sprays them out across
the display, one bit of color to each pixel in its turn. So how does the hose
know what colors go where?

你可以将整个过程想象为软管向屏幕喷洒像素。
独特的像素从软管的后面流入，然后在屏幕上喷洒，每次对一个像素涂一位的颜色。
所以软管怎么知道哪种颜色要喷到哪里去？

In most computers, the answer is that it pulls them from a *framebuffer*. A
framebuffer is an array of pixels in memory, a chunk of RAM where each couple of
<span name="color">bytes</span> represents the color of a single pixel. As the
hose sprays across the display, it reads in the color values from this array,
one byte at a time.

在大多数电脑上，答案是从*帧缓冲*中取出它们。
帧缓冲是内存中的色素数组，RAM中有块<span name="color">字节</span>表示像素的颜色。
当软管喷洒屏幕时，它从这个数组中读取颜色值，一次一个字节。

<aside name="color">

The specific mapping between byte values and colors is described by the *pixel
format* and the *color depth* of the system. In most gaming consoles today, each
pixel gets 32 bits: eight each for the red, green, and blue channels, and another
eight left over for various other purposes.

在字节值和颜色之间的映射通常由系统的*像素格式*和*色深*来指定。
在今日多数游戏主机上，每个像素都有32位，红绿蓝三个频道各八位，剩下的八位有不同的用途。

</aside>

Ultimately, in order to get our game to appear on screen, all we do is write to
that array. All of the crazy advanced graphics algorithms we have boil down to
just that: setting byte values in the framebuffer. But there's a little problem.

最终，为了让游戏显示在屏幕中，我们需要做的就是写入这个数组。
我们使用的疯狂图形算法最终都到了这里：设置帧缓冲中的字节值。但这里有个小问题。

Earlier, I said computers are sequential. If the machine is executing a chunk of
our rendering code, we don't expect it to be doing anything else at the same
time. That's mostly accurate, but a couple of things *do* happen in the middle
of our program running. One of those is that the video display will be reading
from the framebuffer *constantly* while our game runs. This can cause a problem
for us.

早先，我说过计算机是顺序的。
如果机器在运行一块渲染代码，我们不期望它同时还能做些别的什么事。
这通常是准确的，但是有些事*确实*在程序运行中发生。
其中一件事是，当游戏运行时，视频输出正在*不断*从帧缓冲中读取数据。
这可能会对我们造成问题。

Let's say we want a happy face to appear on screen. Our program starts looping
through the framebuffer, coloring pixels. What we don't realize is that the
video driver is pulling from the framebuffer right as we're writing to it. As it
scans across the pixels we've written, our face starts to appear, but then it
outpaces us and moves into pixels we haven't written yet. The result is
*tearing*, a hideous visual bug where you see half of something drawn on screen.

假设我们要在屏幕上显示一张笑脸。
程序在帧缓冲上开始循环为像素点涂色。
我们没有意识到的是视频驱动在写入的同时正在读取它。
当它扫描过整个我们已写的像素时，笑脸正在开始出现，但是之后它进入了我们没有写的部分，将没有写的像素绘制到了屏幕上。结果就是*哭脸*，让你在屏幕上看到绘制了一半的图像，这是可见的可怕漏洞。

<span name="tearing"></span>

<span name="tearing"></span>

<img src="images/double-buffer-tearing.png" alt="A series of images of an in-progress frame being rendered. A pointer writes pixels while another reads them. The reader outpaces the writer until it starts reading pixels that haven't been rendered yet." />

<aside name="tearing">

We start drawing pixels just as the video driver starts reading from the
framebuffer (Fig. 1). The video driver eventually catches up to the renderer and
then races past it to pixels we haven't written yet (Fig. 2). We finish drawing
(Fig. 3), but the driver doesn't catch those new pixels.

我们在视频驱动从缓冲帧上读取时开始绘制像素(Fig. 1)。
视频驱动最终追上了渲染器，然后超过它，读取了还没有写入的像素(Fig. 2)。
我们完成了绘制，但驱动没有收到那些新像素。

The result (Fig. 4) is that the user sees half of the drawing. The name
"tearing" comes from the fact that it looks like the bottom half was torn off.

结果(Fig. 4)是用户只看到了一半的绘制结果。
“哭脸”这个名字来自看上去下半部是撕裂的。

</aside>

This is why we need this pattern. Our program renders the pixels one at a time,
but we need the display driver to see them all at once -- in one frame the face
isn't there, and in the next one it is. Double buffering solves this. I'll explain how
by analogy.

这就是为什么我们需要这个模式。
程序一次渲染一个像素，但是显示驱动需要一次全部看到——在一帧中没有出现脸，下一帧全部出现。
双缓冲解决了这个。我会用类比来解释。

### Act 1, Scene 1

### 表演1，场景1

Imagine our users are watching a play produced by ourselves. As scene one ends
and scene two starts, we need to change the stage setting. If we have the
stagehands run on after the scene and start dragging props around, the illusion
of a coherent place will be broken. We could dim the lights while we do that
(which, of course, is what real theaters do), but the audience still knows
*something* is going on. We want there to be no gap in time between scenes.

想象玩家正在看直接演出的表演。
在场景一结束而场景二开始时，我们需要改变舞台设置。
如果让场务在场景结束后进去拖动东西，在连贯地点演出的幻觉就被打破了。
我们可以减弱灯光（这当然是剧院真正做的），但是观众还是知道*有什么*在进行，我们想在场景间毫无跳跃的转换。

With a bit of real estate, we come up with this clever solution: we build
<span name="stage">*two*</span> stages set up so the audience can see both. Each
has its own set of lights. We'll call them stage A and stage B. Scene one is shown on
stage A. Meanwhile, stage B is dark as the stagehands are setting up scene two. As soon as scene one ends, we cut the lights on stage A and bring them
up on stage B. The audience looks to the new stage and scene two begins
immediately.

消耗一些地皮，我们想到了一个聪明的解决方案：建<span name="stage">*两个*</span>舞台，观众两个都能看到。
每个有它自己的一组灯光。我们称这些舞台为舞台A和舞台B。
场景一在舞台A上。同时在场务设置场景二时，舞台B是黑的。
当场景一完成后，切断场景A的灯光，将灯光指向场景B。观众看向新舞台，场景二立即开始。

At the same time, our stagehands are over on the now darkened stage *A*,
striking scene one and setting up scene *three*. As soon as scene two ends, we
switch the lights back to stage A again. We continue this process for the entire
play, using the darkened stage as a work area where we can set up the next
scene. Every scene transition, we just toggle the lights between the two stages.
Our audience gets a continuous performance with no delay between scenes. They
never see a stagehand.

同时，场务到了黑咕隆咚的舞台*A*，收拾了场景一然后设置场景*三*。
一旦场景二结束，将灯光转回舞台A。
我们在整场表演中进行这样的活动，使用黑暗的舞台作为设置下一场景的工作区域。
每一次场景转换，只是在两个舞台间切换灯光。
观众获得了连续的体验，没有在场景转换间感到延迟。他们从来没有见到场务。

<aside name="stage">

Using a half-silvered mirror and some very smart layout, you could actually
build this so that the two stages would appear to the audience in the same *place*. As
soon as the lights switch, they would be looking at a different stage, but they would
never have to change where they look. Building this is left as an exercise for
the reader.

使用半边镀银镜还有其他的巧妙布置，你可以真正的在同一*位置*布置两个舞台。
随着灯光转换，观众看到了不同的舞台，但他们不会看向不同的地方。
这样布置舞台是一个给读者的练习。

</aside>

### Back to the graphics

### 重新回到图形

That is <span name="sync">exactly</span> how double buffering works, and this
process underlies the rendering system of just about every game you've ever
seen. Instead of a single framebuffer, we have *two*. One of them represents the
current frame, stage A in our analogy. It's the one the video hardware is
reading from. The GPU can scan through it as much as it wants whenever it wants.

这<span name="sync">就是双缓冲</span>是如何工作的，
这就是几乎每个你看到的游戏背后的渲染系统。
不是使用一个帧缓冲，我们使用*两个*。其中一个代表现在的帧，我们类比中的舞台A。
是图形硬件渲染的那一个。GPU可以想什么时候扫就什么时候扫。

<aside name="sync">

Not *all* games and consoles do this, though. Older and simpler consoles where
memory is limited carefully sync their drawing to the video refresh
instead. It's tricky.

但不是*所有*的游戏盒主机都是这么做的。更简单更老的主机中，内存有限，需要小心的同步描绘和视频更新。那很有技巧。

</aside>

Meanwhile, our rendering code is writing to the *other* framebuffer. This is our
darkened stage B. When our rendering code is done drawing the scene, it switches
the lights by *swapping* the buffers. This tells the video hardware to start
reading from the second buffer now instead of the first one. As long as it times
that switch at the end of a refresh, we won't get any tearing, and the entire
scene will appear all at once.

同时，我们的渲染代码正在写入*另一个*帧缓冲。
这是我们黑暗中的舞台B。当渲染代码完成了描绘场景，它将通过*交换*缓存来改变灯光。
这告诉图形硬件开始从第二块缓存中读取而不是第一块。
只要在更新之前交换，我们就不会有任何悲伤，整个场景都会一下子出现。

Meanwhile, the old framebuffer is now available for use. We start rendering the
next frame onto it. Voil&agrave;!

同时，现在可以使用以前的帧缓冲了。我们可以将下一帧渲染在它上面了。Voil！

## The Pattern

## 模式

A **buffered class** encapsulates a **buffer**: a piece of state that can be
modified. This buffer is edited incrementally, but we want all outside code to
see the edit as a single atomic change. To do this, the class keeps *two*
instances of the buffer: a **next buffer** and a **current buffer**.

一个*双缓冲类*封装了一个**缓冲**：一段可改变的状态。
这个缓冲被增量的修改，但我们想要外部的代码将其视为单一的原子修改。
为了达到这点，类保存了*两个*缓冲的实例：**下一缓存**和**当前缓存**。

When information is read *from* a buffer, it is always from the *current*
buffer. When information is written *to* a buffer, it occurs on the *next*
buffer. When the changes are complete, a **swap** operation swaps the next and
current buffers instantly so that the new buffer is now publicly visible. The
old current buffer is now available to be reused as the new next buffer.

当信息*从*缓存中读取，它总是读取*当前的*缓存。
当信息需要写*到*缓存，它总是在*下一*缓存上操作。
当改变完成后，一个**交换**操作会立刻将当前缓存和下一缓存交换，
这样新缓存就是公共可见的了。以前的当前缓存被视为下一个重用的缓存。

## When to Use It

## 何时使用

This pattern is one of those ones where you'll know when you need it. If you
have a system that lacks double buffering, it will probably look visibly wrong
(tearing, etc.) or will behave incorrectly. But saying, "you'll know when you
need it" doesn't give you much to go on. More specifically, this pattern is
appropriate when all of these are true:

这个模式是那种你需要它时，你自然就会知道的那种。
如果你有一个系统需要双缓冲，它可能有可见的错误（泪水之类的）或者行为不正确。
但是，“当你需要时你会知道的”并不会给你太多线索。
更加特殊的，以下情况都满足时，使用这个模式就很恰当：

 *  We have some state that is being modified incrementally.

 *  我们需要增量修改一些状态。

 *  That same state may be accessed in the middle of modification.

 *  在修改中，状态可能会被外部请求。

 *  We want to prevent the code that's accessing the state from
    seeing the work in progress.

 *  我们想要防止请求状态的外部代码知道内部是如何工作的。

 *  We want to be able to read the state and we don't want to have to
    wait while it's being written.

 *  我们想要读取状态，而且不想在修改的时候等待。

## Keep in Mind

## 记住

Unlike larger architectural patterns, double buffering exists at a lower
implementation level. Because of this, it has fewer consequences for the rest of
the codebase -- most of the game won't even be aware of the difference. There
are a couple of caveats, though.

不像更大的架构模式，双缓冲模式在底层。
正因如此，它对代码库的其他部分有更少的影响——大多数游戏甚至不会感到有区别。
虽然这里还是有几个警告。

### The swap itself takes time

### 交换本身需要时间

Double-buffering requires a *swap* step once the state is done being modified.
That operation must be atomic -- no code can access *either* state while they
are being swapped. Often, this is as quick as assigning a pointer, but if it
takes longer to swap than it does to modify the state to begin with, then we haven't
helped ourselves at all.

在状态被修改后，双缓冲需要一个*swap*步步骤。
这个操作必须是原子的——在它们交换时，没有代码可以接触到*任何一个*状态。
通常，这有修改一个指针那么快，但是如果交换消耗的时间长于修改状态的时间，那我们一点也没有帮到自己。

### We have to have two buffers

### 我们得保存两个缓存

The other consequence of this pattern is increased memory usage. As its name implies, the
pattern requires you to keep *two* copies of your state in memory at all times.
On memory-constrained devices, this can be a heavy price to pay. If you can't
afford two buffers, you may have to look into other ways to ensure your state
isn't being accessed during modification.

这个模式的另一个结果是增加了内存的使用。
就像名字暗示的一样，这个模式需要你在内存中一直保留*两个*状态的拷贝。
在内存攸关订的设备上，这可能要付出惨痛的代价。
如果你不能接受使用两个缓存，你需要使用别的方法保证状态在修改时不会被请求。

## Sample Code

## 示例代码

Now that we've got the theory, let's see how it works in practice. We'll write a
very bare-bones graphics system that lets us draw pixels on a framebuffer. In
most consoles and PCs, the video driver provides this low-level part of the
graphics system, but implementing it by hand here will let us see what's going on.
First up is the buffer itself:

现在我们知道了理论，让我们看看它在实践中如何使用。
我们写一个非常基础的图形系统，允许我们在缓冲帧上描绘像素。
在大多数主机和电脑上，图形驱动提供了这种底层的图形系统，但是在这里手动实现有助于理解发生了什么。首先是缓存本身：

^code 1

It has basic operations for clearing the entire buffer to a default color and
setting the color of an individual pixel. It also has a function, `getPixels()`,
to expose the raw array of memory holding the pixel data. We won't see this in
the example, but the video driver will call that function frequently to stream
memory from the buffer onto the screen.

它有操作设置整个缓存成默认的颜色，也有操作将其一像素设置为特定颜色。
它也有函数`getPixels()`，将保存像素数据的数组暴露出来。
虽然从例子中看不出，但实时视频驱动会频繁调用这个函数，将缓存中的数据输送到屏幕上。

We wrap this raw buffer in a `Scene` class. It's job here is to render something
by making a bunch of `draw()` calls on its buffer:

我们将整个缓存包在`Scene`类中。渲染某物需要做的是在这块缓存上调用一系列`draw()`。

<span name="draw"></span>

<span name="draw"></span>

^code 2

<aside name="draw">

Specifically, it draws this artistic masterpiece:

特别的，它画出来这幅旷世杰作：

<img src="images/double-buffer-face.png" width="240" alt="A pixellated smiley face." />

</aside>

Every frame, the game tells the scene to draw. The scene clears the buffer and
then draws a bunch of pixels, one at a time. It also provides access to the
internal buffer through `getBuffer()` so that the video driver can get to it.

每一帧，游戏告诉场景去描绘。场景清空缓存然后一个接一个绘制像素。
这也提供了`getBuffer()`获得缓存，这样视频驱动可以接触到它。

This seems pretty straightforward, but if we leave it like this, we'll run into
problems. The trouble is that the video driver can call `getPixels()` on the
buffer at *any* point in time, even here:

这看起来很直观，但是如果就这样做，我们会遇到麻烦。
问题在于图形驱动可以在*任何*时间调用`getBuffer()`，甚至在这里：

^code 3

When that happens, the user will see the eyes of the face, but the mouth will
disappear for a single frame. In the next frame, it could get interrupted at some
other point. The end result is horribly flickering graphics. We'll fix this with
double buffering:

当那发生时，用户就会看到脸的眼睛，但是一帧中嘴就会消失了。
下一帧，又会在某些地方冲突。最终结果是可怕的闪烁图形。我们会用双缓冲修复这点：

^code 4

Now `Scene` has two buffers, stored in the `buffers_` array. We don't directly
reference them from the array. Instead, there are two members, `next_` and
`current_`, that point into the array. When we draw, we draw onto the next
buffer, referenced by `next_`. When the video driver needs to get the pixels,
it always accesses the *other* buffer through `current_`.

现在`Scene`有两个缓存了，存储在`buffers_`数组中。
我们不会直接从数组引用它们。这里会有两个成员，`next_`和`current_`，指向这个数组。
当描绘时，我们绘制在`next_`引用的缓存上。
当图形驱动需要获得像素时，它总是通过`current_`获取*另一个*缓存。

This way, the video driver never sees the buffer that we're working on. The only
remaining piece of the puzzle is the call to `swap()` when the scene is done
drawing the frame. That swaps the two buffers by simply switching the `next_`
and `current_` references. The next time the video driver calls `getBuffer()`,
it will get the new buffer we just finished drawing and put our recently
drawn buffer on screen. No more tearing or unsightly glitches.

通过这种方式，视频驱动永远看不到我们正在工作的缓存。
谜题的最后一片碎片就是在场景完成绘制一帧的时候调用`swap()`。
这通过交换`next_`和`current_`的引用完成这一点。
下一次视频驱动调用getBuffer()`，它会获得我们刚刚完成描绘的新缓存，
然后将刚刚描绘好的缓存放在屏幕上。没有哭脸，也没有可以看到的漏洞。

### Not just for graphics

### 不仅仅是图形

The core problem that double buffering solves is state being accessed while it's
being modified. There are two common causes of this. We've covered the first one
with our graphics example -- the state is directly accessed from code on another
thread or interrupt.

双缓冲解决的核心问题是状态被修改的同时被请求。
这通常有两种原因。我们用图形例子覆盖了第一种——状态被另一线程的代码或中断请求。

There is another equally common cause, though: when the code *doing the
modification* is accessing the same state that it's modifying. This can manifest
in a variety of places, especially physics and AI where you have entities
interacting with each other. Double-buffering is often helpful here too.

但是，还有一个同样常见的原因：当*负责修改的*代码请求正在修改的同样的状态。
这可能发生在很多实体中，特别是物理和AI中，有实体相互交互时。
双缓冲在那里也十分有用。

### Artificial unintelligence

### 人工不智能

Let's say we're building the behavioral system for, of all things, a game based
on slapstick comedy. The game has a stage containing a bunch of actors that run
around and get up to various hijinks and shenanigans. Here's our base actor:

假设我们正在为基于趣味喜剧的游戏构建行为系统。
这个游戏包括一堆跑来跑去寻欢作乐恶作剧的角色。这里是我们的基础角色：

^code 5

Every frame, the game is responsible for calling <span
name="update">`update()`</span> on the actor so that it has a chance to do some
processing. Critically, from the user's perspective, *all actors should appear
to update simultaneously*.

每一帧，游戏要在角色身上调用<span name="update">`update()`</span>这样它可以做些事情。特别的，从玩家的角度，*所有的角色都看上去应该同时更新*。

<aside name="update">

This is an example of the <a class="pattern" href="update-method.html">Update
Method</a> pattern.

这是<a class="pattern" href="update-method.html">更新方法</a>模式的例子。

</aside>

Actors can also interact with each other, if by "interacting", we mean "they can
slap each other around". When updating, the actor can call `slap()` on another
actor to slap it and call `wasSlapped()` to determine if it has been slapped.

角色也可以相互交互，这里的“交互”，我指“可以互相扇对方巴掌”。
当更新时，角色可以在另一个角色身上调用`slap()`来扇它一巴掌，然后调用`wasSlapped()`决定它是不是被扇了。

The actors need a stage where they can interact, so let's build that:

角色需要一个可以交互的状态，所以让我们来设置它：

^code 6

`Stage` lets us add actors, and provides a single `update()` call that updates each
actor. To the user, actors appear to move simultaneously, but internally,
they are updated one at a time.

`Stage`允许增加角色，然后提供一个简单的`update()`调用来更新每个角色。
对于用户，角色看起来是同时移动的，但是实际上，它们是依次更新的。

The only other point to note is that each actor's "slapped" state is cleared
immediately after updating. This is so that an actor only responds to a given
slap once.

这里需要注意的另一点是每个角色的“被扇”状态在更新后就立刻被清除。
这是为什么一个角色对一巴掌只反应一次。

To get things going, let's define a concrete actor subclass. Our comedian here
is pretty simple. He faces a single actor. Whenever he gets slapped -- by
anyone -- he responds by slapping the actor he faces.

作为一切的开始，让我们定义一个具体的角色子类。
我们这里的喜剧演员很简单。
他面向一个角色。当他被扇的时候——无论是谁扇的——他扇面前的人一巴掌作为回应。

^code 7

Now, let's throw some comedians on a stage and see what happens. We'll set up
three comedians, each facing the next. The last one will face the first, in a
big circle:

现在我们把一些喜剧演员丢到舞台上看看发生了什么。
我们设置三个演员，每个面朝另一个，最后一个面朝第一个，形成一个环状：

^code 8

The resulting stage is set up as shown in the following image. The arrows
show who the actors are facing, and the numbers show their index in the stage's array.

最终舞台被像下面图像上那样设置。箭头描述了角色朝向谁，然后数字显示他们在舞台数组中的索引。

<img src="images/double-buffer-slaps-1.png" alt="Boxes for Harry, Baldy, and Chump, in that order. Harry has an arrow pointing to Baldy, who has an arrow pointing to Chump, who has an arrow pointing back to Harry." />

We'll slap Harry to get things going and see what happens when we start
processing:

我们扇哈利一巴掌让事件开始，然后看看我们开始处理后会发生什么：

^code 9

Remember that the `update()` function in `Stage` updates each actor in turn, so if
we step through the code, we'll find that the following occurs:

记住`Stage`中的`update()`函数在每轮中更新每个角色，
因此如果检视整个代码，我们会发现以下事件发生：

    Stage updates actor 0 (Harry)
      Harry was slapped, so he slaps Baldy
    Stage updates actor 1 (Baldy)
      Baldy was slapped, so he slaps Chump
    Stage updates actor 2 (Chump)
      Chump was slapped, so he slaps Harry
    Stage update ends

    Stage updates actor 0 (Harry)
      Harry was slapped, so he slaps Baldy
    Stage updates actor 1 (Baldy)
      Baldy was slapped, so he slaps Chump
    Stage updates actor 2 (Chump)
      Chump was slapped, so he slaps Harry
    Stage update ends

In a single frame, our initial slap on Harry has propagated through all of
the comedians. Now, to mix things up a bit, let's say we reorder the comedians
within the stage's array but leave them facing each other the same way.

在单独的一帧中，初始给哈利的一巴掌传给了所有的喜剧演员。
现在，将事物混乱起来，让我们重新排列舞台数组中角色的排序，
但是让他们继续保持面向对方的方式。

<img src="images/double-buffer-slaps-2.png" alt="The same boxes as before with the same arrows, but now they are ordered Chump, Baldy, Harry." />

We'll leave the rest of the stage setup alone, but we'll replace the chunk of code
where we add the actors to the stage with this:

我们不管舞台的其余部分，将添加角色到舞台的代码块改为如下：

^code 10

Let's see what happens when we run our experiment again:

让我们看看再次运行时会发生什么：

    Stage updates actor 0 (Chump)
      Chump was not slapped, so he does nothing
    Stage updates actor 1 (Baldy)
      Baldy was not slapped, so he does nothing
    Stage updates actor 2 (Harry)
      Harry was slapped, so he slaps Baldy
    Stage update ends

    Stage updates actor 0 (Chump)
      Chump was not slapped, so he does nothing
    Stage updates actor 1 (Baldy)
      Baldy was not slapped, so he does nothing
    Stage updates actor 2 (Harry)
      Harry was slapped, so he slaps Baldy
    Stage update ends

Uh, oh. Totally different. The problem is straightforward. When we
update the actors, we modify their "slapped" states, the exact same
state we also *read* during the update. Because of this, changes to
that state early in the update affect <span name="cascade">later</span> parts of that *same*
update step.

哦吼。完全不一样了。问题很明显。
更新角色时，我们修改了他们的“被扇”状态，这也是我们在更新时*读取*状态。
因为这样，在早先的更新中修改状态会影响<span name="cascade">之后</span>更新*同一*状态的步骤。

<aside name="cascade">

If you continue to update the stage, you'll see the slaps gradually
cascade through the actors, one per frame. In the first frame, Harry
slaps Baldy. In the next frame, Baldy slaps Chump, and so on.

如果你继续更新舞台，你会看的巴掌在角色间扩散，每帧一个。
在第一帧 Harry扇了Baldy。下一帧，Baldy扇了Chump，就这样下去。

</aside>

The ultimate result is that an actor may respond to being slapped in
either the *same* frame as the slap or in the *next* frame based
entirely on how the two actors happen to be ordered on the stage. This
violates our requirement that actors need to appear to run in
parallel -- the order that they update within a single frame shouldn't
matter.

最终的结果是，一个角色响应被扇可能是在被扇的*同一*帧或者*下一*帧，
这完全取决于两个角色在舞台上是如何排序的。
这违背了我们需要角色同时出现的需求——它们在同一帧中更新的顺序不该对结果有影响。

### Buffered slaps

### 缓存巴掌

Fortunately, our Double Buffer pattern can help. This time, instead of
having two copies of a monolithic "buffer" object, we'll be buffering
at a much finer granularity: each actor's "slapped" state:

幸运的是，双缓冲模式可以帮忙。
这次，不是保存两大块“缓冲”，我们会用更好的粒度缓存：每个角色的“被扇”状态。

^code 11

Instead of a single `slapped_` state, each actor now has two. Just
like the previous graphics example, the current state is used for
reading, and the next state is used for writing.

不是使用一个`slapped_`状态，每个演员现在有两个了。
就像我们之前图像的例子一样，当前状态为读准备，下一状态为写准备。

The `reset()` function has been replaced with `swap()`. Now, right
before clearing the swap state, it copies the next state into the
current one, making it the new current state. This also requires a
small change in `Stage`:

`reset()`函数被替换为`swap()`。
现在，就在清除交换状态前，它将下一状态拷贝到当前状态上，让它成为新的当前状态，这也需要在`Stage`中做出小小改变：

^code 12

The `update()` function now updates all of the actors and *then* swaps
all of their states. The end result of this is that an actor will only see a
slap in the frame *after* it was actually slapped. This way, the actors will
behave the same no matter their order in the stage's array. As far as the user
or any outside code can tell, all of the actors update simultaneously within a
frame.

`update()`函数现在更新所有的角色，*然后*交换它们的状态。
最终结果是角色在实际被扇*之后*的那帧才能看到巴掌。
通过这种方式，角色无论在舞台数组中如何排列，都会保持相同的行为。
无论外部的代码如何调用，所有的角色在一帧内同时更新。

## Design Decisions

## 设计决策

Double Buffer is pretty straightforward, and the examples we've seen so far
cover most of the variations you're likely to encounter. There are two main
decisions that come up when implementing this pattern.

双缓冲很直观，我们现在看到的例子也覆盖了大多数你需要的场景。
使用这个模式之前，还有两个主要的设计决策需要去做。

### How are the buffers swapped?

### 缓存是如何被交换的？

The swap operation is the most critical step of the process since we
must lock out all reading and modification of both buffers while it's
occurring. To get the best performance, we want this to happen as
quickly as possible.

交换操作是整个过程的最重要的一步，
因为我们必须在其发生时，锁住两个缓存上的读取和修改。
为了得到最好的性能表现，我们需要这个进行的越快越好。

 *  **Swap pointers or references to the buffer:**

 *  **交换缓存的指针或者引用：**

    This is how our graphics example works, and it's the most common
    solution for double-buffering graphics.

    这是我们图形例子工作的方式，这也是大多数双缓冲图形通用的解决方法。

     *  *It's fast.* Regardless of how big the buffer is, the swap is simply a
        couple of pointer assignments. It's hard to beat that for speed and
        simplicity.

     *  *速度快。*不管缓存有多大，交换都只是一对指针作业。很难在速度和简易性上击败它。

     *  *Outside code cannot store persistent pointers to the buffer.* This is
        the main limitation. Since we don't actually move the *data*, what we're
        essentially doing is periodically telling the rest of the codebase to
        look somewhere else for the buffer, like in our original stage analogy.
        This means that the rest of the codebase can't store pointers directly
        to data within the buffer -- they may be pointing at the wrong one a
        moment later.

     *  *外部代码不能存储对缓存的永久指针。*这是主要限制。由于我们实际上不能移动*数据*，本质上做的是周期性的通知代码库的其他部分去别处去寻找缓存，就像前面的舞台类比一样。这就意味着代码库的其他部分不能存储指向缓存中数据的指针——它们也许错误的指向了一段时间后的那个。

        This can be particularly troublesome on a system where the video driver
        expects the framebuffer to always be at a fixed location in memory. In
        that case, we won't be able to use this option.

        这会严重误导那些期待缓冲帧永远在内存中的固定地址的视频驱动。在这种情况下，我们不能这么做。

     *  *Existing data on the buffer will be from two frames ago, not the last
        frame.* Successive frames are drawn on alternating buffers with no data
        copied between them, like so:

     *  *在缓存中的数据是两帧之前的，而不是上一帧的。*连续的帧绘制在候选的帧缓存上，无需再在它们之间传递任何数据拷贝，就像这样：

            :::text
            Frame 1 drawn on buffer A
            Frame 2 drawn on buffer B
            Frame 3 drawn on buffer A
            ...

        You'll note that when we go to draw the third frame, the data already on
        the buffer is from frame *one*, not the more recent second frame. In
        most cases, this isn't an issue -- we usually clear the whole
        buffer right before drawing. But if we intend to <span
        name="blur">reuse</span> some of the existing data on the buffer, it's
        important to take into account that that data will be a frame older than
        we might expect.

        你会注意到当我们绘制第三帧的时候，缓存上的数据是帧*一*的，而不是最近的帧二的。大多数情况下，这不是什么问题——我们通常在绘制之前清空整个帧。但如果想<span name="blur">沿用</span>某些缓存中已有的数据，就需要考虑数据也许比期望的更老。

        <aside name="blur">

        One classic use of old framebuffer data is simulating motion blur. The
        current frame is blended with a bit of the previously rendered frame to
        make a resulting image that looks more like what a real camera captures.

        旧帧缓存数据的经典用法是模拟动态模糊。
        当前的帧混合一点之前的帧可以造成像是相机捕获的图景。

        </aside>

 *  **Copy the data between the buffers:**

 *  **在缓存之间拷贝数据：**

    If we can't repoint users to the other buffer, the only other option is
    to actually copy the data from the next frame to the current frame. This is
    how our slapstick comedians work. In that case, we chose this method because
    the state -- a single Boolean flag -- doesn't take any longer to copy than a
    pointer to the buffer would.

    如果我们不能将用户指向其他缓存，唯一的选项就是将下帧的数据实实在在的拷贝到现在这帧上。这是我们的扇巴掌喜剧的工作方法。那种情况下，这种方式因为状态——一个简单的布尔标识——不比修改指向缓存的指针开销更大。

     *  *Data on the next buffer is only a single frame old.* This is the nice
        thing about copying the data as opposed to ping-ponging back and forth
        between the two buffers. If we need access to previous buffer data, this
        will give us more up-to-date data to work with.

     *  *下一帧的数据只比之前老一帧。*拷贝数据这点与乒乒乓乓地跳转缓存正相反。如果我们需要前一帧的数据，这会给出更新的数据来工作。

     *  *Swapping can take more time.* This, of course, is the big negative
        point. Our swap operation now means copying the entire buffer in memory.
        If the buffer is large, like an entire framebuffer, it can take a
        significant chunk of time to do this. Since nothing can read or write to
        *either* buffer while this is happening, that's a big limitation.

     *  *交换也许很花时间。*这个，当然，是最大的负面。交换操作现在意味着在内存中拷贝整个缓存。如果缓存很大比如一整个缓冲帧，这需要花费可观的时间。由于此事发生时没有东西可以读取或者写入*每个*缓存，这是一个巨大的限制。

### What is the granularity of the buffer?

### 缓存的粒度如何？

The other question is how the buffer itself is organized -- is it a single monolithic
chunk of data or distributed among a collection of objects? Our graphics
example uses the former, and the actors use the latter.

这里另一问题是缓存本身是如何组织的——是整体的数据块还是散布在对象集合中？
图形例子是前一个，而角色是后一个。

Most of the time, the nature of what you're buffering will lead to the answer,
but there's some flexibility. For example, our actors all could have stored
their messages in a single message block that they all reference into by their
index.

大多数情况下，你缓存的方式自然而然会引导你找到答案，但是这里有些灵活度。
比如，角色总能将消息存在独立的消息块中，他们都使用索引来引用。

 *  **If the buffer is monolithic:**

 *  **如果缓存是一整块：**

     *  <p>*Swapping is simpler.* Since there is only one pair of buffers, a
        single swap does it. If you can swap by changing pointers, then you can
        swap the entire buffer, regardless of size, with just a couple of
        assignments.</p>

     *  <p>*交换更简单。*由于只有一对缓存，一个简单的交换就完成了。如果可以改变指针来交换，那么就可以交换整个缓存，不必在意大小，只需几步操作。</p>

 *  **If many objects have a piece of data:**

 *  **如果很多对象都有一块数据：**

     *  *Swapping is slower.* In order to swap, we need to iterate through the
        entire collection of objects and tell each one to swap.

     *  *交换更慢。*为了交换，需要遍历整个对象集合，通知每个都得交换。

        In our comedian example, that was OK since we needed to clear the next
        slap state anyway -- every piece of buffered state needed to be touched
        each frame. If we don't need to otherwise touch the old buffer, there's
        a simple optimization we can do to get the same performance of a
        monolithic buffer while distributing the buffer across multiple objects.

        在喜剧例子中，这没问题，因为反正需要清除扇巴掌状态——每块缓存的数据每帧都需要接触。如果不需要接触老的那帧，可以用一个简单操作在多个对象间分散状态，获得使用整块缓存一样的性能。

        The idea is to get the "current" and "next" pointer concept and apply it
        to each of our objects by turning them into object-relative *offsets*.
        Like so:

        思路是将“当前”和“下一”指针概念改为对象相关的*偏移量*。就像这样：

        ^code 13

        Actors access their current slap state by using `current_` to index into
        the state array. The next state is always the other index in the array,
        so we can calculate that with `next()`. Swapping the state simply
        alternates the `current_` index. The clever bit is that `swap()` is now
        a *static* function -- it only needs to be called once, and *every*
        actor's state will be swapped.

        角色使用`current_`在状态数组中查询，获得当前的被扇状态，下一状态总是数组中的另一索引，这样可以用`next()`来计算。交换状态只需交替`current_`索引。聪明之处在于`swap()`现在是*静态*函数，只需被调用一次，*每个*角色的状态都会被交换。

## See Also

## 参见

 *  <p>You can find the Double Buffer pattern in use in almost every graphics
    API out there. For example, OpenGL has `swapBuffers()`, Direct3D has "swap
    chains", and Microsoft's XNA framework swaps the framebuffers within its
    `endDraw()` method.</p>


*   <p>你可以在几乎每个图形API中找到双缓冲。举个例子，OpenGL有`swapBuffers()`，Direct3D有"swap chains", Microsoft的XNA框架有`endDraw()`方法。</p>
