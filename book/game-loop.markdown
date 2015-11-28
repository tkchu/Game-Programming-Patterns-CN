^title Game Loop
^section Sequencing Patterns

## Intent

## 意图

*Decouple the progression of game time from user input and processor speed.*

*将游戏的处理过程与玩家输入和处理器速度解耦。*

## Motivation

## 动机

If there is one pattern this book couldn't live without, this is it. Game loops
are the quintessential example of a "game programming pattern". Almost every
game has one, no two are exactly alike, and relatively few programs outside of
games use them.

如果这本书有一个不可或缺模式，那就是这个了。游戏循环是“游戏设计模式”的精华例子。几乎每一个游戏都有一个，没有两个是相同的，而在游戏以外机会没有程序使用它。

To see how they're useful, let's take a quick trip down memory lane. In the
olden days of computer programming when everyone had <span
name="beard">beards</span>, programs worked like your dishwasher. You dumped a
load of code in, pushed a button, waited, and got results out. Done. These were
*batch mode* programs -- once the work was done, the program stopped.

为了看看他们是多有用，让我们快速检视一下内存小巷。这计算机程序的早先日子，每个人都长着胡子，程序像你的洗碗机一样工作。你输入一堆代码，按个按钮，等待，然后获得结果，完成。这些都是*批处理*程序，一旦工作完成，程序就停止了。

<aside name="beard">

Ada Lovelace and Rear Admiral Grace Hopper had honorary beards.

</aside>

You still see these today, though thankfully we don't have to write them on
punch cards anymore. Shell scripts, command line programs, and even the little
Python script that turns a pile of Markdown into this book are all batch mode
programs.

你在今日仍然能看到这恶心，虽然感谢上天我们不必在打孔纸上面编写他们了。终端脚本，命令行程序，甚至将Markdown翻译成这本书的Python脚本都是批处理程序。

### Interview with a CPU

### 采访CPU

Eventually, programmers realized having to drop off a batch of code at the
computing office and come back a few hours later for the results was a terribly
slow way to get the bugs out of a program. They wanted immediate feedback.
*Interactive* programs were born. Some of the first interactive programs were
games:

最终，程序意识到将批处理代码留在计算办公室，然后几个小时后获得结果是一个糟糕的慢方法来找出程序的漏洞。他们想要立即的反馈。*交互式*程序诞生了。第一批交互式程序中的一些事游戏：

<span name="cave"></span>

    YOU ARE STANDING AT THE END OF A ROAD BEFORE A SMALL BRICK
    BUILDING . AROUND YOU IS A FOREST. A SMALL
    STREAM FLOWS OUT OF THE BUILDING AND DOWN A GULLY.

    > GO IN
    YOU ARE INSIDE A BUILDING, A WELL HOUSE FOR A LARGE SPRING.

<aside name="cave">

This is [Colossal Cave
Adventure](http://en.wikipedia.org/wiki/Colossal_Cave_Adventure), the first
adventure game.

</aside>

You could have a live conversation with the program. It waited for your input,
then it would respond to you. You would reply back, taking turns just like you
learned to do in kindergarten. When it was your turn, it sat there doing
nothing. Something like:

你可以和这个程序进行一些交流。他等待你的熟人，然后它会回应你。你再回复，就像你再幼儿园中学到的那样轮流。当你的回合，他坐在那里啥也不做。像这样：

<span name="exit"></span>

^code 1

<aside name="exit">

This loops forever, so there's no way to quit the game. A real game would do
something like `while (!done)` and set `done` to exit. I've omitted that to keep
things simple.

</aside>

### Event loops

### 事件循环

Modern graphic UI applications are surprisingly similar to old adventure games
once you shuck their skin off. Your word processor usually just sits there doing
nothing until you press a key or click something:

如果你剥开现代的图形UI的外皮，会惊讶地发现他们与老旧的冒险游戏差不多。你的文本处理器通常呆在那里什么也不做直到你按了个键或者点击了什么东西：

^code 2

The main difference is that instead of *text commands*, the program is waiting
for *user input events* -- mouse clicks and key presses. It still works
basically like the old text adventures where the program <span
name="idle">*blocks*</span> waiting for user input, which is a problem.

最大的不同是，不使用*文本命令*，程序等待*用户输入事件*——鼠标点击和按键按下。他还是和以前的老式文本冒险游戏一样，程序*阻塞*在等待用户输入，这是一个问题。

Unlike most other software, games keep moving even when the user isn't providing
input. If you sit staring at the screen, the game doesn't freeze. Animations
keep animating. Visual effects dance and sparkle. If you're unlucky, that
monster keeps chomping on your hero.

不想大多数其他软件，游戏即使在玩家没有输入时也在继续运行。入股你站在那里看着屏幕，游戏不会冻结。动画继续动着。视觉效果跳跃闪烁。如果你不幸的话，怪物继续咬噬你的英雄。

<aside name="idle">

Most event loops do have "idle" events so you can intermittently do stuff
without user input. That's good enough for a blinking cursor or a progress bar,
but too rudimentary for games.

</aside>

This is the first key part of a real game loop: *it processes user input, but
doesn't wait for it*. The loop always keeps spinning:

这是游戏循环的第一个关键部分：*他处理用户输入，但是不等待它*。循环总是继续旋转：

^code 3

We'll refine this later, but the basic pieces are here. `processInput()` handles
any user input that has happened since the last call. Then, <span
name="update">`update()`</span> advances the game simulation one step. It runs
AI and physics (usually in that order). Finally, `render()` draws the game so
the player can see what happened.

我们之后会改善它，但是基本部分都在这里了。`processInput()`处理上次调用到现在的任何玩家输入。然后`update()`运行游戏模拟的下一步。这运行了AI和物理（通常是这种顺序）。最终，`render()`绘制游戏这样玩家可以看到发生了什么。

<aside name="update">

As you might guess from the name, `update()` is a good place to use the <a
href="update-method.html" class="pattern">Update Method</a> pattern.

</aside>

### A world out of time

### 时间之外的世界

If this loop isn't blocking on input, that leads to the obvious question: how
*fast* does it spin? Each turn through the game loop advances the state of the
game by some amount. From the perspective of an inhabitant of the game world,
the hand of their clock has <span name="tick">ticked</span> forward.

如果这个循环不是在输入中阻塞，这就带来了一个明显的问题，要转*多快*？么每次穿过游戏循环都会发展一定的游戏状态。从游戏世界的居民看来，他们手上的表就会滴答一下。

<aside name="tick">

The common terms for one crank of the game loop are "tick" and "frame".

</aside>

Meanwhile, the *player's* actual clock is ticking. If we measure how quickly the
game loop cycles in terms of real time, we get the game's "frames per second".
If the game loop cycles quickly, the FPS is high and the game moves smoothly and
quickly. If it's slow, the game jerks along like a stop motion movie.

同时，*玩家的*真实手表也在滴答着。如果我们用实际时间来测算游戏循环运行的速度，我们得到了游戏的“帧每秒”。如果游戏循环的更快，FPS就更高，游戏运行的更流畅更快。若干他很慢，游戏看上去就像是慢动作电影。

With the crude loop we have now where it just cycles as quickly as it can, two
factors determine the frame rate. The first is *how much work it has to do each
frame*. Complex physics, a bunch of game objects, and lots of graphic detail all
will keep your CPU and GPU busy, and it will take longer to complete a frame.

我们现在写的这个循环可以能转多快转多快，两个因素决定了帧率。第一个是每帧要做多少工作。复杂物理，一对游戏对象，很多图形细节购汇让你的CPU和GPU繁忙，这决定了需要多久完成一帧。

The second is *the speed of the underlying platform.* Faster chips churn through
more code in the same amount of time. Multiple cores, GPUs, dedicated audio
hardware, and the OS's scheduler all affect how much you get done in one tick.

第二个是*潜在平台的速度。*更快的芯片可以在同样的回家按运行更多的嗲吗。多核，GPU组，独立声卡，以及系统的调度都影响了在一次滴答中能够做多少东西。

### Seconds per second

### 每秒的秒数

In early video games, that second factor was fixed. If you wrote a game for the
NES or Apple IIe, you knew *exactly* what CPU your game was running on and you
could (and did) code specifically for that. All you had to worry about was how
much work you did each tick.

在早期的视频游戏中，第二个参数是固定的。如果你为NES或者Apple IIe写游戏，你*明确*知道你的游戏运行在什么CPU上你可以（而且确实）为他特别的写代码。你需要担忧的是每一次滴答要做多少工作。

Older games were carefully coded to do just enough work each frame so that the
game ran at the speed the developers wanted. But if you tried to play that same
game on a faster or slower <span name="turbo">machine</span>, then the game
itself would speed up or slow down.

老些的游戏被仔细地编码，一帧只做足够的工作，这样游戏可以以开发者想要的速率运行。但是如果你想要在快一些或者慢一些的机器上运行同一个游戏，游戏速度本身就会加速或者减速。

<aside name="turbo">

This is why old PCs used to have
"[turbo](http://en.wikipedia.org/wiki/Turbo_button )" buttons. New PCs were
faster and couldn't play old games because the games would run too fast. Turning
the turbo button *off* would slow the machine down and make old games playable.

</aside>

These days, though, few developers have the luxury of knowing exactly what
hardware their game will run on. Instead, our games must intelligently adapt to
a variety of devices.

这些天，很少有开发者可以奢侈的知道他们游戏需要运行的硬件条件。相反，我们的游戏必须自动适应多种设备。

This is the other key job of a game loop: *it runs the game at a consistent
speed despite differences in the underlying hardware.*

这就是游戏循环的另一个关键工作：*不管潜在硬件的条件，他在一个固定的速度运行。*

## The Pattern

## 模式

A **game loop** runs continuously during gameplay. Each turn of the loop, it
**processes user input** without blocking, **updates the game state**, and
**renders the game**. It tracks the passage of time to **control the rate of
gameplay**.

一个**游戏循环**在游玩中不断运行。循环的每一回合，他无阻塞地**处理玩家的输入**，**更新游戏状态**，**渲染游戏**。他追踪时间的消息来**控制游玩的速度。**

## When to Use It

## 何时使用

Using the wrong pattern can be worse than using no pattern at all, so this
section is normally here to caution against over-enthusiasm. The goal of design
patterns isn't to cram as many into your codebase as you can.

使用错误的模式比不使用模式更糟，所以这节通常是要小心过于热衷。设计模式的目标不是往代码库里尽可能的塞东西。

But this pattern is a bit different. I can say with pretty good confidence that
you *will* use this pattern. If you're using a game <span
name="engine">engine</span>, you won't write it yourself, but it's still there.

但是这个模式有所不同。我可以很自信的说你*会*使用这个模式。如果你使用一个游戏引擎，你不需要主角写，但是它还在那里。

<aside name="engine">

For me, this is the difference between an "engine" and a "library". With
libraries, you own the main game loop and call into the library. An engine owns
the loop and calls into *your* code.

</aside>

You might think you won't need this if you're making a turn-based game. But even
there, though the *game state* won't advance until the user takes their turn,
the *visual* and *audible* states of the game usually do. Animation and music
keep running even when the game is "waiting" for you to take your turn.

你可能认为你在做回合制游戏的时候不需要这个。但是哪怕是那里，就算*游戏状态*到玩家回合才改变，*视觉*和*听觉*状态仍会改变。动画和音乐会继续运行，哪怕游戏在“等待”你进行你的回合。

## Keep in Mind

## 记住

The loop we're talking about here is some of the most important code in your
game. They say a program spends <span name="percent">90%</span> of its time in
10% of the code. Your game loop will be firmly in that 10%. Take care with this
code, and be mindful of its efficiency.

我们这里谈到的循环是你游戏代码中最重要的部分。他们说一个程序会花费90%的事件在10%的代码上。你的游戏循环代码肯定在这10%中。好好对待代码，然后注意他的效率。

<aside name="percent">

Made up statistics like this are why "real" engineers like mechanical and
electrical engineers don't take us seriously.

</aside>

### You may need to coordinate with the platform's event loop

### 你需要与平台的事件循环协调

If you're building your game on top of an OS or platform that has a graphic UI
and an event loop built in, then you have *two* application loops in play. They'll
need to play nice together.

如果你在操作系统的顶层或者有图形UI和内建的事件循环的平台，那你就有了两个应用循环在运作。他们需要很好的配合。

Sometimes, you can take control and make your loop the only one. For
example, if you're writing a game against the venerable Windows API, your
`main()` can just have a game loop. Inside, you can call `PeekMessage()` to
handle and dispatch events from the OS. Unlike `GetMessage()`, `PeekMessage()`
doesn't block waiting for user input, so your game loop will keep cranking.

有时候，你可以控制让你之循环其中一个。举个例子，如果用舍弃了Windows的珍贵API，你的`main()`可以只有一个游戏循环。在里面，你可以调用`PeekMessage()`来处理和分发系统的事件。不像`GetMessage()`，`PeekMessage()`没有阻塞等待用户输入，因此你的游戏循环会保持运作。

Other platforms don't let you opt out of the event loop so easily. If you're
targeting a web browser, the event loop is deeply built into browser's execution
model. There, the event loop will run the show, and you'll use it as your game
loop too. You'll call something like `requestAnimationFrame()` and it will
call back into your code to keep the game running.

其他的平台不会让你这么轻松的选择事件循环。如果你的目标是一个网页浏览器，事件徐汇被内建在浏览器的执行模型的深处。这样，你会用事件循环和游戏循环。你会调用`requestAnimationFrame()`之类的他会返回你的代码让游戏继续运行。

## Sample Code

## 示例代码

For such a long introduction, the code for a game loop is actually pretty
straightforward. We'll walk through a couple of variations and go over their
good and bad points.

对于一个如此长的介绍，游戏循环的代码实际上很直观。我们会浏览一堆变种并他们的好处和坏处。

The game loop drives AI, rendering, and other game systems, but those aren't the
point of the pattern itself, so we'll just call into fictitious methods here.
Actually implementing `render()`, `update()` and others is left as a
(challenging!) exercise for the reader.

游戏循环驱动了AI，渲染和其他游戏系统，但这些不是模式的要点，所以我们会调用虚拟的方法。在实际实现了`render()`，`update()`，剩下的作为一个（挑战性的）练习给读者。

### Run, run as fast as you can

### 跑，能跑多快跑多快

We've already seen the simplest possible game loop:

我们已经看到了可能是最简单的游戏循环：

^code 3

The problem with it is you have no control over how fast the game runs. On a
fast machine, that loop will spin so fast users won't be able to see what's
going on. On a slow machine, the game will crawl. If you have a part of the game
that's content-heavy or does more AI or physics, the game will actually play
slower there.

它的问题是你不能控制游戏运行的有多快。在一个快速机器上，循环会运行的如此之快以至于玩家看不清发生了什么。在慢速机器上，游戏在爬行。如果游戏的一部分时有大量内容或者做了很多AI或物理，游戏就会在那里慢一些。

### Take a little nap

### 休息一下

The first variation we'll look at adds a simple fix. Say you want your game to
run at 60 FPS. That gives you about <span name="16">16</span> milliseconds per
frame. As long as you can reliably do all of your game processing and rendering
in less than that time, you can run at a steady frame rate. All you do is process
the frame and then *wait* until it's time for the next one, like so:

我们看到的第一个问题可以增加一个简单的修复。假设你想要你的游戏一60FPS运行。这就给了你大约每帧16毫秒。只要你可以用少于这个时间可靠地处理所有的游戏内容和渲染，你就可以以一个稳定的帧率运行。你需要做的就是处理这一帧然后*等待*知道是时候处理下一个，就像这样：

<img src="images/game-loop-simple.png" alt="A simple game loop flowchart. Process Input &rarr; Update Game &rarr; Render &rarr; Wait, then loop back to the beginning." />

The code looks a bit like this:

代码看上去像这样：

<aside name="16">

*1000 ms / FPS = ms per frame*.

</aside>

^code 4

The `sleep()` here makes sure the game doesn't run too *fast* if it processes a
frame quickly. It *doesn't* help if your game runs too *slowly*. If it takes
longer than 16ms to update and render the frame, your sleep time goes
*negative*. If we had computers that could travel back in time, lots of things
would be easier, but we don't.

这里的`sleep()`保证了游戏不会运行太*快*如果它很快的处理一帧。这无法帮忙如果你的游戏运行太*慢*。如果需要超过16ms来更新并渲染一帧，你休眠的时间就变成了*负的*。如果我们的计算机可以回退时间，很多事情就很容易了，但是不可以。

Instead, the game slows down. You can work around this by doing less work each
frame -- cut down on the graphics and razzle dazzle or dumb down the AI. But that
impacts the quality of gameplay for all users, even ones on fast machines.

相反，游戏变慢了。你可以通过每帧少做一些工作来解决这个问题——减少物理效果和绚丽光影，或者把AI变笨。但是这影响了玩家的游戏体验，哪怕是那些在快速机器上的。

### One small step, one giant step

### 一小步，一大步

Let's try something a bit more sophisticated. The problem we have basically
boils down to:

让我们尝试一些更加复杂的东西。我们拥有的问题基本上是：

  1. Each update advances game time by a certain amount.

  2. It takes a certain amount of *real* time to process that.

  1. 每一次更新将游戏时间推动一个固定量。

  2. 这消耗一定量的*真实*时间来处理它。

If step two takes longer than step one, the game slows down. If it takes more
than 16 ms of processing to advance game time by 16ms, it can't possibly keep
up. But if we can advance the game by *more* than 16ms of game time in a single
step, then we can update the game less frequently and still keep up.

如果第二步消耗的时间超过第一步，游戏变慢了。如果它需要超过16ms来来推动游戏时间16ms，它永远跟不上。但是如果我们一步中推动游戏时间*超过*16ms，纳米我们可以减少更新的频率仍然可以跟得上。

The idea then is to choose a time step to advance based on how much *real* time
passed since the last frame. The longer the frame takes, the bigger steps the
game takes. It always keeps up with real time because it will take bigger and
bigger steps to get there. They call this a *variable* or *fluid* time step. It
looks like:

之后的主意是选择一个事件间隔基于上一帧到现在有多少*真实*时间过去了。这一帧花费的时间越长，游戏的间隔越大。它总能跟上真实时间，因为它走的步子越来越大。他们称之为*变化的*或者*流动的*时间间隔。它看上去像是：

^code 5

Each frame, we determine how much *real* time passed since the last game update
(`elapsed`). When we update the game state, we pass that in. The engine is then
responsible for advancing the game world forward by that amount of time.

每一帧，我们决定上一次游戏更新（`elapsed`）到现在有多少时间过去了。当我们更新游戏状态时，我们将其传入。游戏引擎复杂退浆游戏世界一个确定的时间量。

Say you've got a bullet shooting across the screen. With a fixed time step, in each
frame, you'll move it according to its velocity. With a variable time step, you
*scale that velocity by the elapsed time*. As the time step gets bigger, the
bullet moves farther in each frame. That bullet will get across the screen in
the *same* amount of *real* time whether it's twenty small fast steps or four
big slow ones. This looks like a winner:

假设你有一颗子弹跨过屏幕。使用一个固定的时间间隔，在每一帧中，你根据他的速度移动它。使用一个变化的时间间隔，你*根据过去的时间决定速度的比例*。随着时间间隔加大，子弹在每一帧间移动的更远。那个子弹在*真实*时间里移动*同样*多的距离，无论是二十个快的小间隔还是四个慢的大间隔。这看上去是胜利者：

 *  The game plays at a consistent rate on different hardware.

 *  游戏在不同 的硬件上以固定的速度运行。

 *  Players with faster machines are rewarded with smoother gameplay.

 *使用更快机器的玩家获得了更流畅的游戏体验。

But, alas, there's a serious problem lurking ahead: we've made the game <span
name="deterministic">non-deterministic</span> and unstable. Here's one example
of the trap we've set for ourselves:

但是，悲哀，这里有一个严重的问题：我们让游戏不再是确定性的了也不再稳定。这里是我们给自己挖的一个坑：

<aside name="deterministic">

"Deterministic" means that every time you run the program, if you give it the
same inputs, you get the exact same outputs back. As you can imagine, it's much
easier to track down bugs in deterministic programs -- find the inputs that caused
the bug the first time, and you can cause it every time.

Computers are naturally deterministic; they follow programs mechanically.
Non-determinism appears when the messy real world creeps in. For example,
networking, the system clock, and thread scheduling all rely on bits of the
external world outside of the program's control.

</aside>

Say we've got a two-player networked game and Fred has some beast of a gaming
machine while George is using his grandmother's antique PC. That aforementioned
bullet is flying across both of their screens. On Fred's machine, the game is
running super fast, so each time step is tiny. We cram, like, 50 frames in the
second it takes the bullet to cross the screen. Poor George's machine can only fit in about
five frames.

假设我们有一个双人联网游戏，Fred有一台游戏性能猛兽机，而George正在使用at祖母的老爷机。前面提到的子弹在他们两人的屏幕上飞行。在Fred的机器上，游戏跑的超级快，每一个时间间隔都很小。我们塞了，比如，50帧在子弹穿过屏幕的那一秒。可怜的George的机器只能跑到大约5帧。

This means that on Fred's machine, the physics engine updates the bullet's position
50 times, but George's only does it five times. Most games use floating point
numbers, and those are subject to *rounding error*. Each time you add two
floating point numbers, the answer you get back can be a bit off. Fred's machine
is doing ten times as many operations, so he'll accumulate a bigger error than
George. The *same* bullet will end up in *different places* on their machines.

这就意味着在Fred 的机器上，物理引擎每秒更新50次位置，但是George的只能做5次。大多数游戏使用浮点数，他们有*每轮误差*。每一次你讲两个浮点数加在一起，你获得的结果就会有一点偏差。Fred的机器做了10倍多的操作，所以它的误差要比George的更大。*同样*的子弹最终在他们的机器上到了*不同的位置*。

This is just one nasty problem a variable time step can cause, but there are more.
In order to run in real time, game physics engines are approximations of the
real laws of mechanics. To keep those approximations from <span
name="blowup">blowing up</span>, damping is applied. That damping is carefully
tuned to a certain time step. Vary that, and the physics gets unstable.

这是我们使用变化的时间可以引起的一个问题，但是还有更多。为了在真实时间运行，游戏图形引擎是实际机制法则的近似值。为了从爆炸计算这些近似值，阻尼被添加了。这个阻尼每次时间间隔被小心的调整。改变它，物理就不再稳定。

<aside name="blowup">

"Blowing up" is literal here. When a physics engine flakes out, objects can get
completely wrong velocities and launch themselves into the air.

</aside>

This instability is bad enough that this example is only here as a cautionary
tale and to lead us to something better...

这种不稳定性太糟了，这个例子在这里的唯一原因是作为警示寓言，带领我们到更好的东西……

### Play catch up

### 追逐游戏

One part of the engine that usually *isn't* affected by a variable time step is
<span name="render">rendering</span>. Since the rendering engine captures an
instant in time, it doesn't care how much time advanced since the last one. It
renders things wherever they happen to be right then.

我们游戏中通常*不会*被动态时间间隔影响到的是渲染。由于渲染引擎捕捉一个时间上的瞬间，它不再会从上一次到现在过了多久。它渲染事物在所在的地方。

<aside name="render">

This is more or less true. Things like motion blur can be affected by time step,
but if they're a bit off, the player doesn't usually notice.

</aside>

We can use this fact to our advantage. We'll *update* the game using a fixed
time step because that makes everything simpler and more stable for physics and
AI. But we'll allow flexibility in when we *render* in order to free up some
processor time.

我们可以利用这点作为优势。我们用固定的时间间隔*更新*游戏，因为这让所有事情变得简单，对于物理和AI也更加稳定。但是我们允许*渲染*上的灵活性来释放一些处理器时间。

It goes like this: A certain amount of real time has elapsed since the last turn
of the game loop. This is how much game time we need to simulate for the game's
"now" to catch up with the player's. We do that using a *series* of *fixed* time
steps. The code looks a bit like:

它像这样运作：固定量的时间自上一次游戏循环逝去。这就是我们需要为游戏的“现在”模拟的来追上玩家的时间。我们使用一*系列*的*固定*时间间隔。代码看上去像是这样的：

^code 6

There's a few pieces here. At the beginning of each frame, we update `lag` based
on how much real time passed. This measures how far the game's clock is behind
compared to the real world. We then have an inner loop to update the game, one
fixed step at a time, until it's caught up. Once we're caught up, we render and
start over again. You can visualize it sort of like this:

这里还有几个小块。在每一帧的开头，我们基于有多少真实时间过去更新`lag`。这测量了游戏世界是时钟比真实世界落后了多少，直到追上。一旦我们追上，我们就渲染然后重新开始。你可以将这种视觉化如下：

<img src="images/game-loop-fixed.png" alt="A modified flowchart. Process Input &rarr; Update Game &rarr; Wait, then loop back to this step then &rarr; Render &rarr; Loop back to the beginning." />

Note that the time step here isn't the *visible* frame rate anymore.
`MS_PER_UPDATE` is just the *granularity* we use to update the game. The shorter
this step is, the more processing time it takes to catch up to real time. The
longer it is, the choppier the gameplay is. Ideally, you want it pretty short,
often faster than 60 FPS, so that the game simulates with high fidelity on fast
machines.

注意这里的时间间隔不是*可见的*帧率了。`MS_PER_UPDATE`只是我们更新游戏的*间隔*。这个间隔越短，需要更多的处理时间来追上真实时间。它越长，游戏抖动的越厉害。理想上，你想要它特别短，通常快过60FPS，所以游戏子啊快速机器上会有高效的表现。

But be careful not to make it *too* short. You need to make sure the time step
is greater than the time it takes to process an `update()`, even on the <span
name="bail">slowest</span> hardware. Otherwise, your game simply can't catch up.

但是小心不要把它整的*太*短了。你需要保证这个事件间隔长于需要处理一个`update()`，即使在最慢的硬件上。否则，你的游戏就跟不上了。

<aside name="bail">

I left it out here, but you can safeguard this by having the inner update loop
bail after a maximum number of iterations. The game will slow down then, but
that's better than locking up completely.

</aside>

Fortunately, we've bought ourselves some breathing room here. The trick is that
we've *yanked rendering out of the update loop*. That frees up a bunch of CPU
time. The end result is the game *simulates* at a constant rate using safe fixed
time steps across a range of hardware. It's just that the player's *visible
window* into the game gets choppier on a slower machine.

幸运的是，我们给自己弄出了一些呼吸的空间。技巧是我们将*渲染拉出了更新循环*。这释放了很大一部分CPU时间。结果是游戏以固定速率*模拟*，使用了对于很多硬件安全的固定时间间隔。这只是玩家的*视觉窗口*在慢速机器上会有抖动。

### Stuck in the middle

### 卡在中间

There's one issue we're left with, and that's residual lag. We update the game
at a fixed time step, but we render at arbitrary points in time. This means that
from the user's perspective, the game will often display at a point in time
between two updates.

我们还剩一个问题，就是剩下的延迟。我们以固定的时间间隔更新游戏，但是我们在任意时间渲染。这就意味着从玩家的角度看，游戏经常在两次更新之间的时候显示。

Here's a timeline:

这是时间线：

<img src="images/game-loop-timeline.png" alt="A timeline containing evenly spaced Updates and intermittent Renders." />

As you can see, we update at a nice tight, fixed interval. Meanwhile, we render
whenever we can. It's less frequent than updating, and it isn't steady either. Both
of those are OK. The lame part is that we don't always render right at the point
of updating. Look at the third render time. It's right between two updates:

就像你看到的那样，我们以一个紧凑固定的间隔更新。同时，我们在任何可以的时候渲染。它比更新发生的要少，二期它也不是稳定的。两者都是没问题的。糟糕的是，我们不总是能在更新的正确时间点渲染。看看第三次渲染时间。他在两次更新之间。

<img src="images/game-loop-timeline-close.png" alt="Close-up of the timeline showing Renders falling between Update steps." />

Imagine a bullet is flying across the screen. On the first update, it's on the
left side. The second update moves it to the right side. The game is rendered at
a point in time between those two updates, so the user expects to see that
bullet in the center of the screen. With our current implementation, it will
still be on the left side. This means motion looks jagged or stuttery.

想象一个子弹飞过屏幕。在第一次更新中，它在左边。第二次更新将它移到了右边。这个游戏在两次更新间的特定时间点渲染，所以玩家期待看到子弹在屏幕的中间。而我们现在的实现中，它还在左边。这就意味着移动看上去是锯齿状的。

Conveniently, we actually know *exactly* how far between update frames we are
when we render: it's stored in `lag`. We bail out of the update loop when it's
less than the update time step, not when it's *zero*. That leftover amount?
That's how far into the next frame we are.

方便的是，我们实际知道渲染时距离两帧的时间：它被存储在`lag`中。我们在它比更新时间间隔小的时候取出它，而不是它是*零*的时候。而剩余量？这就是到下一帧的时间。

When we go to render, we'll pass that in:

当我们要渲染时，我们会将其传入：

<span name="normal"></span>

^code 7

<aside name="normal">

We divide by `MS_PER_UPDATE` here to *normalize* the value. The value passed to
`render()` will vary from 0 (right at the previous frame) to just under 1.0
(right at the next frame), regardless of the update time step. This way, the
renderer doesn't have to worry about the frame rate. It just deals in values
from 0 to 1.

</aside>

The renderer knows each game object *and its current velocity*. Say that bullet
is 20 pixels from the left side of the screen and is moving right 400 pixels per
frame. If we are halfway between frames, then we'll end up passing 0.5 to
`render()`. So it draws the bullet half a frame ahead, at 220 pixels. Ta-da,
smooth motion.

渲染器知道每一个游戏对象*以及他现在的速度*。假设子弹在屏幕左边20像素的地方，正在以400像素每帧的速度向右移动。如果我们在两帧中间，我们会传0.5给`render()`。所以它绘制了半帧之前，在220像素，Ta-da，平滑移动。

Of course, it may turn out that that extrapolation is wrong. When we
calculate the next frame, we may discover the bullet hit an obstacle or slowed
down or something. We rendered its position interpolated between where it was on
the last frame and where we *think* it will be on the next frame. But we don't
know that until we've actually done the full update with physics and AI.

当让，也许这种推断是错误的。但我们计算下一帧的时候，我们也许会发现子弹碰撞到另一个障碍或者减速或者别的什么。我们在它上一帧和我们*认为*它在下一帧的位置之间插值。但是我们在完成完整的物理和AI更新后才能真的知道。

So the extrapolation is a bit of a guess and sometimes ends up wrong.
Fortunately, though, those kinds of corrections usually aren't noticeable. At
least, they're less noticeable than the stuttering you get if you don't
extrapolate at all.

所以推断有猜测的成分，有时候结果是错误的。但是，幸运的，这种修正通常不可感知。最起码，他们比你不使用推断导致的结巴更不明显。

## Design Decisions

## 设计决策

Despite the length of this chapter, I've left out more than I've included. Once
you throw in things like synchronizing with the display's refresh rate,
multithreading, and GPUs, a real game loop can get pretty hairy. At a high
level, though, here are a few questions you'll likely answer:

不管这一章的长度，我剩下的超过我包含的。一旦你将显示的刷新频率，多线程，多GPU，加到里面，一个真正的游戏循环就会变得毛绒绒的。在高层中，这里还有一些问题你需要回答：

### Do you own the game loop, or does the platform?

### 你拥有游戏循环，还是平台？

This is less a choice you make and more one that's made for you. If you're
making a game that runs in a web browser, you pretty much *can't* write your own
classic game loop. The browser's event-based nature precludes it. Likewise, if
you're using an existing game engine, you will probably rely on its game loop
instead of rolling your own.

与其说这是一个你的选择，不如说有人为你选了。如果你在做一个浏览器中的游戏，你很可能*不能*写你自己的经典游戏循环。浏览器本身的事件驱动阻碍了这一点。同样，如果你是用来一个现存的游戏引擎，你很可能依赖于它的游戏循环而不是自己写一个。

 *  **Use the platform's event loop:**

 *  **使用平台的事件循环：**

     *  *It's simple.* You don't have to worry about writing and optimizing the
        core loop of the game.

     *  *简单*你不必担心写和优化自己的游戏核心循环。

     *  *It plays nice with the platform.* You don't have to worry about
        explicitly giving the host time to process its own events, caching
        events, or otherwise managing the impedance mismatch between the
        platform's input model and yours.

     *  *平台友好。*你不必担心明确的给时间去处理它自己的事件，缓存事件，或者管理任何平台输入模型和你的不同之处。

     *  *You lose control over timing.* The platform will call your code as it
        sees fit. If that's not as frequently or as smoothly as you'd like, too
        bad. Worse, most application event loops weren't designed with games in
        mind and usually *are* slow and choppy.

     *  *你失去了对时间的控制。*平台会在它合适的时候调用你的代码。如果这不如你想要的平滑或者频繁，太糟了。更早的是，大多数应用的事件循环没有为游戏设计通常*是*又慢又抖动。

 *  **Use a game engine's loop:**

 *  **使用游戏引擎的循环：**

     *  *You don't have to write it.* Writing a game loop can get pretty tricky.
        Since that core code gets executed every frame, minor bugs or
        performance problems can have a large impact on your game. A tight game
        loop is one reason to consider using an existing engine.

     *  *你不必自己写。*写一个游戏循环可能会非常有技巧。友谊那是每帧都要执行的核心代码，小小的漏洞或者性能问题就可以对你的游戏有巨大的影响。一个紧密的游戏循环是一个使用现有引擎的原因。

     *  *You don't get to write it.* Of course, the flip side to that coin is
        the loss of control if you *do* have needs that aren't a perfect fit for
        the engine.

     *  *你不必自己写。*当然，硬币的另一面是逝去了控制，如果你*确实*需要的对引擎不是最好的。

 *  **Write it yourself:**

 *  **自己写：**

     *  *Total control.* You can do whatever you want with it. You can design it
        specifically for the needs of your game.

     *  *完全的控制。*你可以做任何你想做的事情。你可以做任何你想做的事情。你可以为你游戏的需求特定。

     *  *You have to interface with the platform.* Application frameworks and
        operating systems usually expect to have a slice of time to process
        events and do other work. If you own your app's core loop,
        it won't get any. You'll have to explicitly hand off control
        periodically to make sure the framework doesn't hang or get confused.

     *  *你需要与平台交互。*引用框架和操作系统通常有时间片去处理事件和做其他工作。如果你拥有你应用的核心循环，它不会有。你得明确地定期检查保证框架没有挂起或者糊涂。

### How do you manage power consumption?

### 你如何管理性能消耗？

This wasn't an issue five years ago. Games ran on things plugged into walls or on
dedicated handheld devices. But with the advent of smartphones, laptops, and
mobile gaming, the odds are good that you do care about this now. A game that runs
beautifully but turns players' phones into space heaters before running out of
juice thirty minutes later is not a game that makes people happy.

这在五年前还不是问题。游戏运行在插到插座上的或者专用的设备上。但是随着智能手机，笔记本以及移动游戏的发展，这些古怪的东西现在需要关注了。一个游戏完美运行但是会用完手机三十分钟前充的电，并变成空间加热器，这可不是一个能让人民开心的游戏。

Now, you may need to think not only about making your game look great, but also use as
little CPU as possible. There will likely be an *upper* bound to performance
where you let the CPU sleep if you've done all the work you need to do in a
frame.

现在，你需要不仅仅考虑让你的游戏看上去焊好，同时尽可能少的使用CPU。如果你让所有你需要的工作在一帧之内完成后CPU睡眠会对性能造成一个*上限*。

 *  **Run as fast as it can:**

 *  **尽可能快的运行：**

    This is what you're likely to do for PC games (though even those are
    increasingly being played on laptops). Your game loop will never explicitly
    tell the OS to sleep. Instead, any spare cycles will be spent cranking up
    the FPS or graphic fidelity.

    这就是你通常想让PC游戏做的事情（即使有不断增加数目实在笔记本上运行的）。你的游戏循环永远不会显式告诉系统休眠。相反，一个空闲的循环被划在提升FPS或者图像显示上了。

    This gives you the best possible gameplay experience but, it will use as much
    power as it can. If the player is on a laptop, they'll have a nice lap
    warmer.

    这会给你最好可能的游戏体验但是，他会尽可能多的使用电量。如果玩家在笔记本上玩，他们会有一个很好的笔记本加热器。

 *  **Clamp the frame rate:**

 *  **固定帧率**

    Mobile games are often more focused on the quality of gameplay than they are
    on maximizing the detail of the graphics. Many of these games will set an upper
    limit on the frame rate (usually 30 or 60 FPS). If the game loop is done
    processing before that slice of time is spent, it will just sleep for the
    rest.

    移动游戏更加注意游戏体验质量，而不是他们图形细节的最大化。很多这种游戏都会设置一个最大帧率（通常是30或60FPS）。如果游戏循环在分配的时间片消耗完之前完成了，他会在剩余的时间休眠。

    This gives the player a "good enough" experience and then goes easy on their
    battery beyond that.

    这给了玩家“足够好的”游戏体验，也让背后的电池放松了一点。

### How do you control gameplay speed?

### 你如何控制游玩速度？

A game loop has two key pieces: non-blocking user input and adapting to the
passage of time. Input is straightforward. The magic is in how you deal with
time. There are a <span name="platform">near-infinite</span> number of platforms
that games can run on, and any single game may run on quite a few. How it
accommodates that variation is key.

一个游戏循环有两个关键部分：不阻塞的用户输入和对通过时间的适应。输入很直观。魔力之处在于你如何处理事件。这里有数不尽的游戏可运行的平台，每一个游戏都能在其中一些上运行。对变化的适应性如何就是关键。

<aside name="platform">

Game-making seems to be part of human nature, because every time we've built a
machine that can do computing, one of the first things we've done is made games
on it. The PDP-1 was a 2 kHz machine with only 4,096 words of memory, yet Steve
Russell and friends managed to create Spacewar! on it.

</aside>

 *  **Fixed time step with no synchronization:**

 *  **固定的事件间隔没有同步：**

    This was our first sample code. You just run the game loop as fast as you
    can.

    这是我们第一个样例代码。你只需尽可能的快运行游戏。

     *  *It's simple*. This is its main (well, only) virtue.

     *  *简单*。这是主要的（好吧，唯一的）好处。

     *  *Game speed is directly affected by hardware and game complexity.* And
        its main vice is that if there's any variation, it will directly affect
        the game speed. It's the fixie of game loops.

     *  *游戏速度直接与硬件和游戏复杂度相关。*他主要的缺点是如果有变化，它会直接影响游戏速度。它与游戏循环咬死了。

 *  **Fixed time step with synchronization:**

 *  **有同步的固定时间间隔：**

    The next step up on the complexity ladder is running the game at a fixed
    time step but adding a delay or synchronization point at the end of the
    loop to keep the game from running too fast.

    下一步对复杂度控制是使用一个固定的时间间隔但是在循环的末尾增加一个同步点保证游戏不会运行的过快。

     *  *Still quite simple.* It's only one line of code more than the
        probably-too-simple-to-actually-work example. In most game loops, you
        will likely do synchronization *anyway*. You will probably [double
        buffer](double-buffer.html) your graphics and synchronize the buffer
        flip to the refresh rate of the display.

     *  *还是很简单。*这比太简单而不能运行的例子只多了一行代码。在多数游戏循环中，你可能*总*会做一些同步。你可能使用双缓冲你的图形并将你的缓存块与显示更新的频率同步。

     *  *It's power-friendly.* This is a surprisingly important consideration
        for mobile games. You don't want to kill the user's battery
        unnecessarily. By simply sleeping for a few milliseconds instead of
        trying to cram ever more processing into each tick, you save power.

     *  *电量友好的。*这是移动游戏惊人重要的一个关注点。你不想不必要的消耗电池电量。通过简单的休眠几个毫秒而不是试图把更多处理塞入每一个滴答，你节约了电量。

     *  *The game doesn't play too fast.* This fixes half of the speed concerns
        of a fixed loop.

     *  *游戏不会运行的太快。*这解决了固定循环的一半速度问题。

     *  *The game can play too slowly.* If it takes too long to update and render
        a game frame, playback will slow down. Because this style doesn't
        separate updating from rendering, it's likely to hit this sooner than
        more advanced options. Instead of just dropping *rendering* frames to
        catch up, gameplay will slow down.

     *  *游戏可能运行的太快。*如果花了太多是按去更新和渲染一帧，播放也会减缓。因为这个风格没有分离更新和渲染，它比高级更新系统会更容易碰到这一点。不是扔掉*渲染*的帧来追上，游戏本身会变慢。

 *  **Variable time step:**

 *  *动态时间间隔：**

    I'll put this in here as an option in the solution space with the caveat
    that most game developers I know recommend against it. It's good to remember
    *why* it's a bad idea, though.

    我把这个放在这里作为问题的解决办法之一，附加警告，大多数我认识的游戏开发者推荐反对它。不过记住*为什么*这是一个坏主意是很有用的。

     *  *It adapts to playing both too slowly and too fast.* If the game can't
        keep up with real time, it will just take larger and larger time steps
        until it does.

     *  *它适应运行太快或者太慢。*如果游戏不能追上真实时间，那就要消耗越来越多的时间间隔直到他跟上。

     *  *It makes gameplay non-deterministic and unstable.* And this is the real
        problem, of course. Physics and networking in particular become much
        harder with a variable time step.

     *  *它让游戏不确定而且不稳定。*这个是真正的问题，当然。特别是物理和网络部分在动态时间间隔会愈发的困难。

 *  **Fixed update time step, variable rendering:**

 *  **固定的更新时间间隔，动态的渲染：**

    The last option we covered in the sample code is the most complex, but also
    the most adaptable. It updates with a fixed time step, but it can drop
    *rendering* frames if it needs to to catch up to the player's clock.

    我们子啊示例代码中覆盖的最后一个选项是最复杂的，但是也是最有适应性的。它以固定时间间隔更新，但是如果他需要赶上玩家的时钟，它可以扔掉一些*渲染*帧。

     *  *It adapts to playing both too slowly and too fast.* As long as the game
        can *update* in real time, the game won't fall behind. If the player's
        machine is top-of-the-line, it will respond with a smoother gameplay
        experience.

     *  *它适应运行太快或者太慢。*只要有心可以在真实时间上*更新*，游戏就不会落后。如果玩家的机器是顶配，它会回应更平滑的游戏体验。

     *  *It's more complex.* The main downside is there is a bit more going on
        in the implementation. You have to tune the update time step to be both
        as small as possible for the high-end, while not being too slow on the
        low end.

     *  *更复杂。*这里的主要负面问题是需要在实现中写更多东西。你需要将更新时间间隔调整尽可能小来适应高端机，同时不至于在低端机上太慢。

## See Also

## 参见

 *  The classic article on game loops is Glenn Fiedler's "[Fix Your
    Timestep](http://gafferongames.com/game-physics/fix-your-timestep/)". This
    chapter wouldn't be the same without it.

 *  关于游戏循环的经典文章是Glenn Fiedler的"[Fix Your Timestep](http://gafferongames.com/game-physics/fix-your-timestep/)"。如果没有这一篇文章，这章就不会是这个样子。

 *  Witters' article on [game
    loops](http://www.koonsolo.com/news/dewitters-gameloop/) is a close
    runner-up.

 *  Witters关于[game loops](http://www.koonsolo.com/news/dewitters-gameloop/)的文章是一个很接近的亚军。

 *  The [Unity](http://unity3d.com/) framework has a complex game loop detailed
    in a wonderful illustration [here][unity].

 *  [Unity](http://unity3d.com/)框架有一个复杂的游戏循环，细节在[这里[unity]有完美的解释。

[unity]: http://docs.unity3d.com/Manual/ExecutionOrder.html
