^title Introduction

^title 序

In fifth grade, my friends and I were given access to a little unused
classroom housing a couple of very beat-up TRS-80s. Hoping to inspire us, a
teacher found a printout of some simple BASIC programs for us to tinker with.

在五年级，我和我的朋友被准许进入一间有非常破旧的TRS-80s的房间。
为了激励我们，一位老师为我们准备了一些简单的BASIC程序去摆治。

The audio cassette drives on the computers were broken, so any time we wanted to
run some code, we'd have to carefully type it in from scratch. This led us
to prefer programs that were only a few lines long:

驱动电脑的磁带已经坏掉了，所以任何时候我们想要去允许写代码，我们就必须得小心地从头输入。
这样我们更喜欢那些只有几行长的程序：

<span name="radical"></span>

    10 PRINT "BOBBY IS RADICAL!!!"
    20 GOTO 10

<aside name="radical">

Maybe if the computer prints it enough times, it will magically become true.

也许如果计算机输出足够多次，它就会魔法般成真。

</aside>

Even so, the process was fraught with peril. We didn't know *how* to program,
so a tiny syntax error was impenetrable to us. If the program didn't work,
which was often, we started over from the beginning.

哪怕这样，过程也充满了危险。我们不知道如何编程，所以一个小小的语法错误对我们也是不可逾越的。
如果程序没有工作，通常如此，我们得从头开始。

At the back of the stack of pages was a real monster: a program that took up
several dense pages of code. It took a while before we worked up the courage
to even try it, but it was irresistible -- the title above the listing was
"Tunnels and Trolls". We had no idea what it did, but it sounded like a game,
and what could be cooler than a computer game that you programmed yourself?

书的最后几页是一个真正的怪物：一个占据了几页代码的程序。
在我们鼓起勇气尝试它之前花费了一些时间，但它实在诱人——列表的标题是“地道与巨魔”。
我们不知道它会做什么，但听起来像是一个游戏，还有什么比你自己编一个电脑游戏更酷的吗？

We never did get it running, and after a year, we moved out of that classroom.
(Much later when I actually knew a bit of BASIC, I realized that it was just a
character generator for the table-top game and not a game in itself.) But the
die was cast -- from there on out, I wanted to be a game programmer.

我们从来没能让它运行起来，过了一年，我们离开了那件教室。
（在很久以后我真正学会了一点BASIC，我意识到那只是个桌面游戏角色生成器而非游戏。）
但是结局已经注定——从那开始，我想要成为游戏程序员。

When I was in my teens, my family got a Macintosh with QuickBASIC and later
THINK C. I spent almost all of my <span name="snakes">summer vacations</span>
hacking together games. Learning on my own was slow and painful. I'd get
something up and running easily -- maybe a map screen or a little puzzle --
but as the program grew, it got harder and harder.

当我还是青少年，我家有了一台Macintosh，能运行QuickBASIC，之后能运行THINK C。
我花费了几乎整个<span name="snakes">暑假</span>编游戏。
自学缓慢而痛苦。
我需要让东西跑起来——也许是地图或者解密游戏——但随着程序代码增长，这越来越难。

<aside name="snakes">

Many of my summers were also spent catching snakes and turtles in the swamps
of southern Louisiana. If it wasn't so blisteringly hot outside, there's a
good chance this would be a herpetology book instead of a programming one.

暑假的很多时候我也在路易斯安那州的沼泽里逮蛇和乌龟。
如果外面不是那么酷热，很有可能这是一本爬虫书而不是编程书了。

</aside>

At first, the challenge was just getting something working. Then, it became
figuring out how to write programs bigger than what would fit in my head. Instead
of just reading about "How to Program in C++", I started trying to find books
about how to *organize* programs.

早先，挑战是让所有东西运行起来。然后，是搞明白怎样写出我脑袋装不下的代码。
不是只阅读“如何用C++编程”，我开始尝试找那些如何*组织*程序的书。

Fast-forward several years, and a <span name="friend">friend</span> hands me a
book: *Design Patterns: Elements of Reusable Object-Oriented Software*.
Finally! The book I'd been looking for since I was a teenager. I read it cover
to cover in one sitting. I still struggled with my own programs, but it was
such a relief to see that other people struggled too and came up with
solutions. I felt like I finally had a couple of *tools* to use instead of
just my bare hands.

在早先的几年里，一位<span name="friend">朋友</span>给我一本书：《设计模式：可复用面向对象软件的基础》。
终于！在从青年时期就在找的书找到了。
我一口气从头读到尾。我仍然在我的程序中挣扎，但看到别人也在挣扎并尝试找到解决方法是一种解脱。
我意识到我终于有些除双手外的*工具*了。

<aside name="friend">

This was the first time we'd met, and five minutes after being introduced, I
sat down on his couch and spent the next few hours completely ignoring him and
reading. I'd like to think my social skills have improved at least a little
since then.

那是我们首次见面，在五分钟介绍后，我坐在他的沙发上，在接下来几个小时，忽视他并阅读。
我想自那以后我的社交技能有所提高。

</aside>

In 2001, I landed my dream job: software engineer at Electronic Arts. I
couldn't wait to get a look at some real games and see how the pros put them
together. What was the architecture like for an enormous game like Madden
Football? How did the different systems interact? How did they get a single
codebase to run on multiple platforms?

在2001年，我获得了梦想的工作：EA的软件工程师。
我等不及要看看真实的游戏是如何的，那些专业人士是如何将他们组织到一切的。
像实况足球这样的巨大游戏使用了什么样的架构？不同的系统是如何交互的？他们是如何使用单一的代码库在多个平台上运行的？

Cracking open the source code was a humbling and surprising experience. There
was brilliant code in graphics, AI, animation, and visual effects. We had
people who knew how to squeeze every last cycle out of a CPU and put it to
good use. Stuff I didn't even know was *possible*, these people did before
lunch.

打开源代码是恭敬而震惊的体验。那有图形，AI，动画，视觉效果的杰出代码。
我们有直到如何榨干CPU的最后一个循环并好好使用的专家。
那些我*想都没想过*的事情，这些人已经完成了。

But the *architecture* this brilliant code hung from was often an
afterthought. They were so focused on *features* that organization went overlooked. Coupling was rife between modules.
New features were often bolted onto the codebase wherever they could be made
to fit. To my disillusioned eyes, it looked like many programmers, if they ever
cracked open *Design Patterns* at all, never got past <a class="pattern"
href="singleton.html">Singleton</a>.

但是这些杰出代码依赖的*架构*就是之后考虑的了。
他们太注重*特性*而忽视了架构。耦合充斥在模块间。
新的功能通常被塞到任何能塞进去的地方。
在我梦想破灭的眼中，这和其他程序员没什么不同，
如果他们阅读过《设计模式》，最多也就用用 <a class="pattern" href="singleton.html">单例</a>。

Of course, it wasn't really that bad. I'd imagined game programmers sitting in
some ivory tower covered in whiteboards, calmly discussing architectural
minutiae for weeks on end. The reality was that the code I was looking at was
written by people working to meet intense deadlines. They did the best they
could, and, as I gradually realized, their best was often very good. The more
time I spent working on game code, the more bits of brilliance I found hiding
under the surface.

当然，没那么糟。我设想游戏程序员坐在白板包围的象牙塔里，为架构冷静地讨论上几周。
现实是我看到的代码是由努力应对紧张截止期限的人完成的。
他们尽他们所能，而且，就像我逐渐意识到的那样，他们通常完成的很好。
我花在游戏代码上的时间越多，我就越能发现藏在表面下的杰作。

Unfortunately, "hiding" was often a good description. There were gems buried
in the code, but many people walked right over them. I watched coworkers
struggle to reinvent good solutions when examples of exactly what they needed
were nestled in the same codebase they were standing on.

不幸的是，“藏”通常是个很好的描述。
通常有宝石埋在代码中，但人们直接从它们上面越过。
我看到合作伙伴挣扎着重复发明解决方案，而事实上他们需要的就埋在他们所用的代码库中。

That problem is what this book aims to solve. I dug up and polished the best
patterns I've found in games, and presented them here so that we can spend our
time inventing new things instead of *re*-inventing them.

这个问题是这本书要解决的。
我挖出了我在游戏中找到最好的模式，然后在这里展示它们，这样我们可以花时间在发明新事物上，而非“重新”发明它们。

## What's in Store

## 这有什么

There are already dozens of game programming books out there. Why write
another?

市面上已经有很多游戏编程书籍了。为什么要再写一本呢？

Most game programming books I've seen fall into one of two categories:

我看到的很多编程书籍可以归为这两类：

* **Domain-specific books.** These narrowly-focused books give you a deep dive
  on some specific aspect of game development. They'll teach you about 3D
  graphics, real-time rendering, physics simulation, artificial intelligence,
  or audio. These are the areas that many game programmers specialize in as
  their careers progress.

* **特定领域的书籍。**这些狭窄领域的书籍给你关于游戏开发某一特定层面的深入研究。他们会教授你3D图形，实时渲染，物理模拟，人工智能，或者音频播放。那些事很多程序员究其一生研究的特定领域。

* **Whole-engine books.** In contrast, these try to span all of the different
  parts of an entire game engine. They are oriented towards building a
  complete engine suited to some specific genre of game, usually a 3D first-person shooter.

* **完整引擎的书籍。**相反，还有试图探索引擎的所有不同部分的书。他们倾向于构建一个特定种类游戏的引擎，通常是第三人称射击游戏。

I like both of these kinds of books, but I think they leave some gaps. Books
specific to a domain rarely tell you how that chunk of code interacts with the
rest of the game. You may be a wizard at physics and rendering, but do you
know how to tie them together gracefully?

我喜欢这些书，但我认为他们之间有些缝隙。特定领域的书籍很少告诉你这些代码如何与游戏的其他部分打交道。
你也许是物理或者渲染上的专家，但是你直到这么将它们两者绑在一起吗？

The second category covers that, but I often find whole-engine books to be too monolithic and too
genre-specific. Especially with the rise of mobile and casual gaming, we're in
a period where lots of different genres of games are being created. We aren't
all just cloning Quake anymore. Books that walk you through a single engine
aren't helpful when *your* game doesn't fit that mold.

第二类书包含里这一点，但是我通常发现完整引擎的书籍通常过于整体，而且过于特定类型了。
特别随着手游和碎片时间游戏兴起，我们处于众多游戏类型创建的时刻。
我们不只是复制Quake了。如果*你的*游戏与它不同，那些带你遍历单一引擎的书就不那么有用了。

Instead, what I'm trying to do here is more <span name="carte">*à la
carte*</span>. Each of the chapters in this book is an independent idea that
you can apply to your code. This way, you can mix and match them in a way that
works best for the game *you* want to make.

相反，我在这里做的更<span name="carte">*à la carte* </span>。
每一章都是一个独立的，你可以应用到代码上的意图。
这样，你可以以*你*认为对游戏最好的方式混合并组合它们。

<aside name="carte">

Another example of this *à la carte* style is the widely beloved [*Game
Programming Gems*](http://www.satori.org/game-programming-gems/) series.

另一个这种*à la carte*风格广泛应用的例子是[*Game Programming Gems*](http://www.satori.org/game-programming-gems/)系列。

</aside>

## How it Relates to Design Patterns

## 它和设计模式有什么关联

Any programming book with <span name="alexander">"Patterns"</span> in its name
clearly bears a relationship to the classic *Design Patterns: Elements of
Reusable Object-Oriented Software* by Erich Gamma, Richard Helm, Ralph Johnson,
and John Vlissides (ominously called the "Gang of Four").

任何有<span name="alexander">“模式”</span>在它名字中的编程书都与Erich Gamma，Richard Helm，Ralph Johnson，和John Vlissides（通常被称为GoF）合著的经典《设计模式：可复用面向对象软件要素》相关。

<aside name="alexander">

*Design Patterns* itself was in turn inspired by a previous book. The idea of
crafting a language of patterns to describe open-ended solutions to problems
comes from [*A Pattern
Language*](http://en.wikipedia.org/wiki/A_Pattern_Language), by Christopher
Alexander (along with Sarah Ishikawa and Murray Silverstein).

《设计模式》本书受到之前的一本书的启发。创建一种关于描述问题的开放式解法的模式的语言来自 [*A Pattern Language*](http://en.wikipedia.org/wiki/A_Pattern_Language)，作者是Christopher Alexander (还有Sarah Ishikawa和Murray Silverstein).

Their book was about architecture (like *real* architecture with buildings and
walls and stuff), but they hoped others would use the same structure to
describe solutions in other fields. *Design Patterns* is the Gang of Four's
attempt to do that for software.

他们的书关于架构（就像建筑和墙那样的*真实*架构），但他们希望其他人使用相同的结构去描述其他领域的解法。
《设计模式》是GoF在软件业做出的努力。

</aside>

By calling this book "Game Programming Patterns", I'm not trying to imply that
the Gang of Four's book is <span name="nongames">inapplicable</span> to games.
On the contrary: the [Design Patterns Revisited](design-patterns-revisited.html)
section of this book covers many of the patterns from *Design
Patterns*, but with an emphasis on how they can be applied to game
programming.

称这本书为“游戏编程模式”，我不是暗示GoF的书<span name="nongames">不适用</span>于游戏。
相反：本书[重返设计模式](design-patterns-revisited.html)一节包含了《设计模式》中的很多模式，但是增加了对如何应用于游戏编程的强调。

Conversely, I think this book is applicable to non-game software too. I could
just as well have called this book *More Design Patterns*, but I think games
make for more engaging examples. Do you really want to read yet another book
about employee records and bank accounts?

同样的，我认为本书也适用于非游戏软件。
我可以同样称本书为“更多设计模式”，但是我认为游戏制作是更契合的例子。
你真的像要另一本书介绍员工记录和银行账户吗？

That being said, while the patterns introduced here are useful in other
software, I think they're particularly well-suited to engineering challenges
commonly encountered in games:

虽然这里介绍的模式在其他软件上也很有用，但我认为它们对于处理游戏中常见的工程挑战更加合适：

*   Time and sequencing are often a core part of a game's architecture. Things
    must happen in the right order and at the right time.

*   时间和顺序通常是游戏架构的核心部分。事物必须以正确的顺序发生在正确的时间。

*   Development cycles are highly compressed, and a number of programmers need
    to be able to rapidly build and iterate on a rich set of different
    behavior without stepping on each other's toes or leaving footprints all
    over the codebase.

*   开发周期高度压缩，很多程序员需要能快速构建和迭代一系列不同的行为，而不会踩到别人的脚趾，或者在代码库中留下一堆痕迹。

*   After all of this behavior is defined, it starts interacting. Monsters
    bite the hero, potions are mixed together, and bombs blast enemies and
    friends alike. Those interactions must happen without the codebase turning
    into an intertwined hairball.

*   在所有的行为都定义后，它开始交互。怪物咬英雄，药物混合，炸弹炸飞敌人或者友军。实现这些交互不能把代码库搞成一团乱麻。

*   And, finally, performance is critical in games. Game developers are in a
    constant race to see who can squeeze the most out of their platform.
    Tricks for shaving off cycles can mean the difference between an A-rated
    game and millions of sales or dropped frames and angry reviewers.

*   最终，性能在游戏中很关键。游戏开发者在一场谁能榨干平台性能的竞赛中。能节约CPU循环的技巧是A级百万销量游戏和掉帧差评游戏间的区分之处。

## How to Read the Book

## 如何阅读这本书

*Game Programming Patterns* is divided into three broad sections. The first
introduces and frames the book. It's the chapter you're reading now along with
the [next one](architecture-performance-and-games.html).

《游戏设计模式》本分为三大块。
第一部分介绍书的框架，是你现在阅读的这章和[下一章](architecture-performance-and-games.html)。

The second section, [Design Patterns Revisited](design-patterns-revisited.html),
goes through a handful of patterns from the Gang of Four book. With each chapter,
I give my spin on a pattern and how I think it relates to game programming.

第二部分，[重放设计模式](design-patterns-revisited.html)，复习了GoF书籍里的很多模式。
在每一章中，我给你了我对这个模式的看法，以及我认为它和游戏编程有什么关系。

The last section is the real meat of the book. It presents thirteen
design patterns that I've found useful. They're grouped into four categories:
[Sequencing Patterns](sequencing-patterns.html), [Behavioral Patterns](behavioral-patterns.html), [Decoupling Patterns](decoupling-patterns.html),
and [Optimization Patterns](optimization-patterns.html).

最后一部分是这本书最肥美的部分。
它展示了十三种我发现有用的模式。他们被分为四类：
[序列模式](sequencing-patterns.html), [行为模式](behavioral-patterns.html), [解耦模式](decoupling-patterns.html),和[优化模式](optimization-patterns.html)。

Each of these patterns is described using a consistent structure so that you
can use this book as a reference and quickly find what you need:

每种模式都使用固定的格式，这样你可以将这本书视为引用，快速找到你需要的：

* The **Intent** section provides a snapshot description of the pattern in
  terms of the problem it intends to solve. This is first so that you can hunt
  through the book quickly to find a pattern that will help you with your
  current struggle.

* **意图**一节提供这个模式想要解决什么问题的简短介绍。将它放在首位，这样可以很快翻一遍然后找到你现在需要的模式。

* The **Motivation** section describes an example problem that we will be
  applying the pattern to. Unlike concrete algorithms, a pattern is usually
  formless unless applied to some specific problem. Teaching a pattern without
  an example is like teaching baking without mentioning dough. This section
  provides the dough that the later sections will bake.

* **动机**一节描述了我们需要应用的问题。不像具体的算法，模式通常不针对某一个问题。不用示例教模式就像不用面团教烘烤。这一届提供了面团而下一节会烘烤它。

* The **Pattern** section distills the essence of the pattern out of the
  previous example. If you want a dry textbook description of the pattern,
  this is it. It's also a good refresher if you're familiar with a pattern
  already and want to make sure you don't forget an ingredient.

* **模式**一节将模式从例子中剥离出来。如果你想要一段教科书式的对模式的简短介绍，那就是这部分了。如果你已经熟悉了这一种模式，想要确保你没有忘掉什么，它也是很好的提示。

* So far, the pattern has only been explained in terms of a single example.
  But how do you know if the pattern will be good for *your* problem?
  The **When to Use It** section provides some guidelines on when the pattern
  is useful and when it's best avoided. The **Keep in Mind** section points
  out consequences and risks when using the pattern.

* 到目前为止，模式只是用一两个例子解释。但是你如何知道模式对*你的*问题有没有用呢？**何时使用**一节提供了这个模式在何时有用何时最好回避。**记住**一节指出了使用模式的结果和危险。

* If, like me, you need concrete examples to really *get* something,
  then **Sample Code** is your section. It walks step by step through a full
  implementation of the pattern so you can see exactly how it works.

* 如果，像我，你需要具体的例子来*理解*某物，那么**示例代码**如你所愿。它通过模式的一步步具体实现来展示模式是如何工作的。

* Patterns differ from single algorithms because they are open-ended. Each
  time you use a pattern, you'll likely implement it differently. The next section,
  **Design Decisions**, explores that space and shows you different options to
  consider when applying a pattern.

* 模式与算法不同的是它们是开放的。每一次你使用模式，你会以不同的方式实现。下一节**设计决策**，探索了这个空间，给了你不同的选项去考虑如何应用模式。

* To wrap it up, there's a short **See Also** section that shows how this
  pattern relates to others and points you to real-world open source code that
  uses it.

* 作为结尾，这里有**参见**一节展示了这一模式与其他模式的关联，以及那些使用它的真实代码。

## About the Sample Code

## 关于示例代码

Code samples in this book are in C++, but that isn't to imply that these
patterns are only useful in that language or that C++ is a better language
for them than others. Almost any language will work fine, though some patterns
do tend to presume your language has objects and classes.

这本书的代码是用C++写就的，但是并不意味着这些模式只在C++中有用，或者C++对他们比其他语言更适用。
几乎每种语言都能好好工作，虽然该有的模式假设你的语言有对象和类。

I chose C++ for a couple of reasons. First, it's the most popular language for
commercially shipped games. It is the *lingua franca* of the industry. Moreso,
the C syntax that C++ is based on is also the basis for Java, C#, JavaScript,
and many other languages. Even if you don't know C++, the odds are good you
can understand the code samples here with a little bit of effort.

我选择C++有几个原因。首先，这是在发布游戏中最流行的语言。
这是业界的*通用语*。
通常，C++基于的C语法也是Java，C#，JavaScript和其他很多语言的基础。
哪怕你不懂C++，你也只需一点点努力就能理解这里的示例代码。

The goal of this book is *not* to teach you C++. The samples are kept as
simple as possible and don't represent good C++ style or usage. Read the code
samples for the idea being expressed, not the code expressing it.

这本书的目标*不是*教会你C++。
示例代码尽可能的简单，不真正代表好的C++风格或规范。
阅读示例代码展示的是意图，而不是代码。

In particular, the code is not written in "modern" -- C++11 or newer -- style.
It does not use the standard library and rarely uses templates. This makes for
"bad" C++ code, but I hope that by keeping it stripped down, it will be more
approachable to people coming from C, Objective-C, Java, and other languages.

特别的，代码不是用“现代”——C++11或者更新的——标准。
没有使用标准库，很少使用模板。
这是“坏的”C++代码，但我希望尽可能简明易懂，对那些从C，Objective-C，Java和其他语言的人更容易理解。

To avoid wasting space on code you've already seen or that isn't relevant to
the pattern, code will sometimes be omitted in examples. When this occurs, an
ellipsis will be placed in the sample to show where the missing code goes.

为了避免花费时间在你已经看过或者是与模式无关的代码上，代码有时在例子中省略了。
如果是那样，一串省略号会在示例代码中表明隐藏了一些代码。

Consider a function that will do some work and then return a value. The
pattern being explained is only concerned with the return value, and not the
work being done. In that case, the sample code will look like:

想想那些做了些工作然后返回值的代码。
展示的模式只关心返回的值，而不是完成了什么工作。那样的话，示例代码长的像这样：

^code update

## Where to Go From Here

## 接下来呢

Patterns are a constantly changing and expanding part of software development.
This book continues the process started by the Gang of Four of documenting and
sharing the software patterns they saw, and that process will continue after
the ink dries on these pages.

模式在软件开发过程中不断的改变和扩展。
这本书继续了GoF记录分享所见模式的旅程，而这旅程也会在笔墨已干后继续。

You are a core part of that process. As you develop your own patterns and
refine (or refute!) the patterns in this book, you contribute to the software
community. If you have suggestions, corrections, or other feedback about
what's in here, please get in touch!


你是这段旅程的核心部分。在你改良（或者否决）了这本书中的模式，你回馈给软件开发团体。
如果你有任何建议，更正，或者有关这里的任何反馈，请联系我！
