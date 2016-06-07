^title Observer
^section Design Patterns Revisited

^title 观察者
^section Design Patterns Revisited

You can't throw a rock at a computer without hitting an application built using
the <span name="devised">[Model-View-Controller][MVC]</span> architecture, and
underlying that is the Observer pattern. Observer is so pervasive that Java put
it in its core library ([`java.util.Observer`][java]) and C# baked it right into
the *language* (the [`event`][event] keyword).

扔块石头到电脑世界中，
不可能砸不中一个不使用<span name="devised">[MVC架构][MVC]</span>的应用，
根本在于观察者模式。
观察者模式非常普遍，Java将其放到了核心库之中（[`java.util.Observer`][java]），而C#直接将其嵌入了*语言*（[`event`][event]关键字）。

[MVC]: http://en.wikipedia.org/wiki/Model%E2%80%93view%E2%80%93controller
[java]: http://docs.oracle.com/javase/7/docs/api/java/util/Observer.html
[event]: http://msdn.microsoft.com/en-us/library/8627sbea.aspx

<aside name="devised">

Like so many things in software, MVC was invented by Smalltalkers in the
seventies. Lispers probably claim they came up with it in the sixties but didn't
bother writing it down.

就像软件中的很多东西，MVC是Smalltalkers在七十年代创造的。Lisp程序员也许会说他们在六十年代就发明了但他们懒得记下来。

</aside>

Observer is one of the most widely used and widely known of the original Gang of
Four patterns, but the game development world can be strangely cloistered at
times, so maybe this is all news to you. In case you haven't left the abbey in a
while, let me walk you through a motivating example.

观察者模式是最广泛使用和最广为人知的GoF模式，
但是游戏开发世界与世隔绝，
所以这对你也许是全新的领域。
为了防止你还没有从修道院中走出来，
让我带你看看更加形象的例子。

## Achievement Unlocked

## 成就解锁

Say we're adding an <span name="weasel">achievements</span> system to our game.
It will feature dozens of different badges players can earn for completing
specific milestones like "Kill 100 Monkey Demons", "Fall off a Bridge", or
"Complete a Level Wielding Only a Dead Weasel".

假设我们向游戏中添加了<span name="weasel">成就系统</span>。
它存储了玩家可以完成的各种各样的成就，比如“杀死1000只猴子恶魔”，“从桥上掉下去”，或者“一命通关”。

<aside name="weasel">

<img src="images/observer-weasel-wielder.png" width="240" alt="Achievement: Weasel Wielder">

I swear I had no double meaning in mind when I drew this.

我发誓画的这个没有第二个意思。

</aside>


This is tricky to implement cleanly since we have such a wide range of
achievements that are unlocked by all sorts of different behaviors. If we aren't
careful, tendrils of our achievement system will twine their way through every
dark corner of our codebase. Sure, "Fall off a Bridge" is somehow tied to the
<span name="physics">physics engine</span>, but do we really want to see a call
to `unlockFallOffBridge()` right in the middle of the linear algebra in our
collision resolution algorithm?

有这么多行为可以解锁的成就系统，清晰地实现它是很有技巧的。
如果我们不够小心，成就系统会缠绕在代码库的每个黑暗角落。
当然，“从桥上掉落”被绑定到<span name="physics">物理引擎</span>上，
但我们并不想看到的在处理撞击代码的线性代数时，
有个对`unlockFallOffBridge()`的调用是不？

<aside name="physics">

This is a rhetorical question. No self-respecting physics programmer would ever
let us sully their beautiful mathematics with something as pedestrian as
*gameplay*.

这是个尊严问题。
任何有自尊的物理程序员都不会允许像*游戏玩法*这样的平凡之物玷污优美的算式。

</aside>

What we'd like, as always, is to have all the code concerned with one facet of
the game nicely lumped in one place. The challenge is that achievements are
triggered by a bunch of different aspects of gameplay. How can that work without
coupling the achievement code to all of them?

我们喜欢的是，照旧，让所有的关注游戏一部分的代码集成一块。
挑战在于成就在游戏的不同层面被触发。怎么解耦成就系统和其他部分呢？

That's what the observer pattern is for. It lets one piece of code announce that
something interesting happened *without actually caring who receives the
notification*.

这就是观察者模式出现的原因。
这让一块代码宣称有些有趣的事情发生了*而不必关心谁接受通知。*

For example, we've got some physics code that handles gravity and tracks which
bodies are relaxing on nice flat surfaces and which are plummeting toward sure
demise. To implement the "Fall off a Bridge" badge, we could just jam the
achievement code right in there, but that's a mess. Instead, we can just do:

举个例子，有物理代码处理重力，追踪哪些物体待在地表，哪些坠入深渊。
为了实现“桥上掉落”的徽章，我们可以直接把成就代码放在那里，但那就会一团糟。
相反，可以这样做：

^code physics-update

<span name="subtle">All</span> it does is say, "Uh, I don't know if anyone
cares, but this thing just fell. Do with that as you will."

<span name="subtle">它做的就是</span>，“额，我不知道有谁感兴趣，但是这个东西刚刚掉下去了。做你想做的事吧。”

<aside name="subtle">

The physics engine does have to decide what notifications to send, so it isn't
entirely decoupled. But in architecture, we're most often trying to make systems
*better*, not *perfect*.

物理引擎确实决定了发送什么通知，所以它没有完全解耦。但在架构中，通常只是让系统*更好*，而不是*完美*。

</aside>

The achievement system registers itself so that whenever the physics code sends
a notification, the achievement system receives it. It can then check to see if
the falling body is our less-than-graceful hero, and if his perch prior to this
new, unpleasant encounter with classical mechanics was a bridge. If so, it
unlocks the proper achievement with associated fireworks and fanfare, and it
does all of this with no involvement from the physics code.

成就系统注册它自己，这样无论何时物理代码发送通知，成就系统都能收到。
它可以检查掉落的物体是不是我们不那么伟大的英雄，
他之前有没有做过这种全新的，不愉快的与桥的经典力学遭遇。
如果满足条件，，就能伴着礼花和炫光解锁合适的成就，做完所有这些都不必牵扯到物理代码。

<span name="tear">In fact</span>, we can change the set of achievements or tear
out the entire achievement system without touching a line of the physics engine.
It will still send out its notifications, oblivious to the fact that nothing is
receiving them anymore.

<span name="tear">事实上</span>，我们可以改变成就的集合或者删除整个成就系统，而不必修改一行物理引擎。它仍然会发送它的通知，哪怕事实上没有东西在接收。

<aside name="tear">

Of course, if we *permanently* remove achievements and nothing else ever
listens to the physics engine's notifications, we may as well remove the
notification code too. But during the game's evolution, it's nice to have this
flexibility.

当然，如果我们*暂时*移除成就，没有任何东西监听物理引擎的通知，我们也同样可以移除通知代码。但是在游戏的演进中，最好保持这样的灵活性。

</aside>

## How it Works

## 它如何运作

If you don't already know how to implement the pattern, you could probably
guess from the previous description, but to keep things easy on you, I'll walk
through it quickly.

如果你还不知道如何实现这个模式，你可能可以从之前的描述中猜到，但是为了减轻你的负担，我会过一遍代码。

### The observer

### 观察者

We'll start with the nosy class that wants to know when another object does
something interesting. These inquisitive objects are defined by this interface:

我们从那个需要知道别的对象做了什么有趣事情的吵闹类开始。
这些好打听的对象用如下接口定义：

<span name="signature"></span>

<span name="signature"></span>

^code observer

<aside name="signature">

The parameters to `onNotify()` are up to you. That's why this is the Observer
*pattern* and not the Observer "ready-made code you can paste into your game".
Typical parameters are the object that sent the notification and a generic
"data" parameter you stuff other details into.

`onNotify()`的参数取决于你。这就是为什么是观察者*模式*，
而不是“可以粘贴到游戏中的真实代码”的观察者。
典型的参数是发送通知的对象和一个将其他细节装入的“数据”参数。

If you're coding in a language with generics or templates, you'll probably use
them here, but it's also fine to tailor them to your specific use case. Here,
I'm just hardcoding it to take a game entity and an enum that describes what
happened.

如果你用通用语言或者模板编码，你可能会在这里使用它们，但是将它们根据你的特殊用况裁剪也很好。
这里我将其硬编码为接受一个游戏实体和一个描述发生了什么的枚举。

</aside>

Any concrete class that implements this becomes an observer. In our example,
that's the achievement system, so we'd have something like so:

任何实现了这个的具体类就成为了观察者。
在我们的例子中，是成就系统，所以我们会做些像这样的事：

^code achievement-observer

### The subject

### 客体

The notification method is invoked by the object being observed. In Gang of Four
parlance, that object is called the "subject". It has two jobs. First, it holds
the list of observers that are waiting oh-so-patiently for a missive from it:

接受观察的对象拥有通知方法，以GoF的说法那些对象被称为“客体”。
它有两个任务。首先，它保存默默等待它的观察者列表：

<span name="stl"></span>

<span name="stl"></span>

^code subject-list

<aside name="stl">

In real code, you would use a dynamically-sized collection instead of a dumb
array. I'm sticking with the basics here for people coming from other languages
who don't know C++'s standard library.

在真实代码中，你会使用动态大小的集合而不是一个固定数组。
我在这里这种基本的形式是为了那些从其他语言过来而不懂C++的标准库的人们。

</aside>

The important bit is that the subject exposes a *public* API for modifying that
list:

重点是客体暴露了一个*公开*API来修改这个列表：

^code subject-register

That allows outside code to control who receives notifications. The subject
communicates with the observers, but it isn't *coupled* to them. In our example,
no line of physics code will mention achievements. Yet, it can still talk to the
achievements system. That's the clever part about this pattern.

这就允许了外界代码控制谁接收通知。
客体与观察者交流，但是不与它们*耦合*。
在我们的例子中，没有一行物理代码会提及成就。
现在，它仍然可以与成就系统交流。这就是这个模式的聪慧之处。

It's also important that the subject has a *list* of observers instead of a
single one. It makes sure that observers aren't implicitly coupled to *each
other*. For example, say the audio engine also observes the fall event so that
it can play an appropriate sound. If the subject only supported one observer,
when the audio engine registered itself, that would *un*-register the
achievements system.

客体有一*列表*观察者而不是一个也是很重要的。
这保证了观察者不会相互竞争。
举个例子，假设我们的音频引擎也需要观察坠落事件来播放合适的音乐。
如果客体只支持一个观察者，当音频引擎注册了它自己，这就会*取消*成就系统的注册。

That means those two systems would interfere with each other -- and in a
particularly nasty way, since the second would disable the first. Supporting a
list of observers ensures that each observer is treated independently from the
others. As far as they know, each is the only thing in the world with eyes on
the subject.

这意味着这两个系统需要相互交互——用一种极其糟糕的方式，
由于第二个会使第一个失效。
支持一列表的观察者保证了每个观察者都是与其他独立处理的。
它们知道的所有事情是，它是世界上唯一看着客体的东西。

The other job of the subject is sending notifications:

客体的剩余任务就是发送通知：

<span name="concurrent"></span>

<span name="concurrent"></span>

^code subject-notify

<aside name="concurrent">

Note that this code assumes observers don't modify the list in their `onNotify()`
methods. A more robust implementation would either prevent or gracefully handle
concurrent modification like that.

注意代码假设了观察者不在它们的`onNotify()`方法中修改列表。
一个更加强健的实现会阻止或优雅地处理像这样的并发修改。

</aside>

### Observable physics

### 可观察的物理

Now, we just need to hook all of this into the physics engine so that it can send
notifications and the achievement system can wire itself up to receive them.
We'll stay close to the original *Design Patterns* recipe and <span
name="event">inherit</span> `Subject`:

现在，我们只需要给物理引擎绑上钩子，这样它可以发送消息，
成就系统可以将它自己接上线来接受消息。
我们靠近之前的*设计模式*秘方，
然后<span name="event">继承</span>`Subject`：

^code physics-inherit

This lets us make `notify()` in `Subject` protected. That way the derived
physics engine class can call it to send notifications, but code outside of it
cannot. Meanwhile, `addObserver()` and `removeObserver()` are public, so
anything that can get to the physics system can observe it.

这让我们在`Subject`中保护地完成`notify()`。
这样推导的物理引擎类可以调用发送通知，但是外部的代码不行。
同时，`addObserver()`和`removeObserver()`是公开的，
所以任何可以接触物理引擎的东西都可以观察它。

<aside name="event">

In real code, I would avoid using inheritance here. Instead, I'd make `Physics`
*have* an instance of `Subject`. Instead of observing the physics engine itself,
the subject would be a separate "falling event" object. Observers could register
themselves using something like:

在真实代码中，我会避免使用这里的继承。
相反，我会让`Physics`*有*一个`Subject`的实例。
不是观察物理引擎本身，客体会是一个单独的“下落事件”对象。
观察者可以用像下面这样的东西注册它们自己：

^code physics-event

To me, this is the difference between "observer" systems and "event" systems.
With the former, you observe *the thing that did something interesting*. With
the latter, you observe an object that represents *the interesting thing that
happened*.

对于我，这是“观察者”系统与“事件”系统的不同之处。
使用前者，你观察*做了有趣事情的事物*。
使用后者，你观察代表*发生的有趣的事物*。

</aside>

Now, when the physics engine does something noteworthy, it calls `notify()`
like in the motivating example before. That walks the observer list and gives
them all the heads up.

现在，当物理引擎做了些值得关注的东西，它调用`notify()`，就像之前的例子。
它遍历了观察者列表然后通知所有观察者。

<img src="images/observer-list.png" alt="A Subject containing a list of Observer pointers. The first two point to Achievements and Audio." />

Pretty simple, right? Just one class that maintains a list of pointers to
instances of some interface. It's hard to believe that something so
straightforward is the communication backbone of countless programs and app
frameworks.

很简单，对吧？只要一个类管理一列表指向接口的实例的指针。
很难相信如此直观的东西是无数程序和应用框架交流的主心骨。

But the Observer pattern isn't without its detractors. When I've asked other
game programmers what
they think about this pattern, they bring up a few complaints. Let's see what we
can do to address them, if anything.

观察者模式不是没有诋毁者。当我问其他程序员怎么看，他们提出了一些抱怨。
让我们看看我们可以做些什么来掌控，如果能做的话。

## "It's Too Slow"

## 太慢了

I hear this a lot, often from programmers who don't actually know the details of
the pattern. They have a default assumption that anything that smells like a
"design pattern" must involve piles of classes and indirection and other
creative ways of squandering CPU cycles.

我经常听到这一点，通常是从那些不知道模式细节的程序员那里。
他们有一种假设，只要有东西听起来像是“设计模式”一定包含了一堆类，跳转和其他创造性的方式浪费CPU循环。

The Observer pattern gets a particularly bad rap here because it's been known to
hang around with some shady characters named "events", <span
name="names">"messages"</span>, and even "data binding". Some of those systems
*can* be slow (often deliberately, and for good reason). They involve things
like queuing or doing dynamic allocation for each notification.

观察者模式有特别坏的名声，因为它通常与一些坏名声的事物结伴出行，
比如“事件”，<span name="names">“消息”</span>，甚至“数据绑定”。
其中的一些系统*会*慢。（通常是故意的，基于很好的原因）。
他们包含了队列或者为每个通知做些动态分配。

<aside name="names">

This is why I think documenting patterns is important. When we get fuzzy about
terminology, we lose the ability to communicate clearly and succinctly. You say,
"Observer", and someone hears "Events" or "Messaging" because either no one
bothered to write down the difference or they didn't happen to read it.

这就是为什么我认为文档化模式很重要。
当我们为术语发狂，我们失去了简洁明确表达的能力。
你说“观察者”，别人听到了“事件”或“消息”，
因为没人花时间写下差异，也没人阅读它。

That's what I'm trying to do with this book. To cover my bases, I've got a
chapter on events and messages too: <a href="event-queue.html"
class="pattern">Event Queue</a>.

这就是我在这本书中要做的。
为了支撑我的说法，也有一章关于事件和消息：<a href="event-queue.html" class="pattern">事件队列</a>.

</aside>

But, now that you've seen how the pattern is actually implemented, you know that
isn't the case. Sending a notification is simply walking a list and calling some
virtual methods. Granted, it's a *bit* slower than a statically dispatched
call, but that cost is negligible in all but the most performance-critical
code.

现在你看到了模式是如何真正被实现的，
你知道了并不是这样。
发送一个通知只是简单的遍历列表，然后调用一些虚方法。
是的，这比静态调用慢*一点*，但是这点消耗在大多数性能攸关的代码都是微不足道的。

I find this pattern fits best outside of hot code paths anyway, so you can
usually afford the dynamic dispatch. Aside from that, there's virtually no
overhead. We aren't allocating objects for messages. There's no queueing. It's
just an indirection over a synchronous method call.

我发现这个模式在热点代码路径之外有很好的应用，
那些你可以付得起动态分配消耗的地方。
除了那点，这里几乎没有天花板。
我们不必为消息分配对象。这里没有队列。这里只有一个跳转用在同步方法调用上。

### It's too *fast?*

### 太*快*？

In fact, you have to be careful because the Observer pattern *is* synchronous.
The subject invokes its observers directly, which means it doesn't resume its
own work until all of the observers have returned from their notification
methods. A slow observer can block a subject.

事实上，你得小心观察者模式*是*同步的。
客体直接引入了观察者，这就意味着直到所有观察者从通知方法返回后，
它才会继续自己的工作。一个缓慢的观察者会阻塞客体。

This sounds scary, but in practice, it's not the end of the world. It's just
something you have to be aware of. UI programmers -- who've been doing
event-based programming like this for ages -- have a time-worn motto for this:
"stay off the UI thread".

这听起来很疯狂，但在实践中，这还不是世界末日。
这只是你得注意的事情。
UI程序员——那些基于事件的编程已经这么干了好几年了——有句经典名言：“滚出UI线程”。

If you're responding to an event synchronously, you need to finish and return
control as quickly as possible so that the UI doesn't lock up. When you have
slow work to do, push it onto another thread or a work queue.

如果要对事件同步响应，你需要完成然后尽可能快的返回控制权，这样UI就不会锁死。
当你有缓慢工作要做时，将其推到另一个线程或工作队列中去。

You do have to be careful mixing observers with threading and explicit locks,
though. If an observer tries to grab a lock that the subject has, you can
deadlock the game. In a highly threaded engine, you may be better off with
asynchronous communication using an <a href="event-queue.html"
class="pattern">Event Queue</a>.

你需要小心观察者混合线程和锁。
如果一个观察者试图获得客体拥有的锁，游戏就死锁了。
在更多线程的机器，你最好使用<a href="event-queue.html"
class="pattern">事件队列</a>来做异步通信。

## "It Does Too Much Dynamic Allocation"

## “它做了太多动态分配”

Whole tribes of the programmer clan -- including many game developers -- have
moved onto garbage collected languages, and dynamic allocation isn't the boogie
man that it used to be. But for performance-critical software like games, memory
allocation still matters, even in managed languages. <span
name="fragment">Dynamic</span> allocation takes time, as does reclaiming memory,
even if it happens automatically.

整个程序员部落——包括很多游戏开发者——迁移到了垃圾回收语言，
动态分配不再是以前的样子了。
但对于性能攸关的软件比如游戏，内存分配仍然重要，哪怕是在有垃圾回收的语言。
<span name="fragment">动态</span>分配需要时间，回收内存也需要时间，哪怕是自动运行的。

<aside name="fragment">

Many game developers are less worried about allocation and more worried about
*fragmentation.* When your game needs to run continuously for days without
crashing in order to get certified, an increasingly fragmented heap can prevent
you from shipping.

很多游戏开发者很少担忧分配而更多担忧*分页*。
当游戏需要不崩溃的连续运行多日来获得认证，不断增加的分页堆会阻止发布。

The <a href="object-pool.html" class="pattern">Object Pool</a> chapter goes into
more detail about this and a common technique for avoiding it.

<a href="object-pool.html" class="pattern">对象池</a>模式一章介绍了更多细节，以及避免它的常用技术。

</aside>

In the example code before, I used a fixed array because I'm trying to keep
things dead simple. In real implementations, the observer list is almost always
a dynamically allocated collection that grows and shrinks as observers are
added and removed. That memory churn spooks some people.

在上面的示例代码中，我使用了一个固长度的数组，因为我想尽可能保证简单。
在真实的实现中，观察者列表总是在随着观察者的添加和删除而动态地增长和消减。
这种内存搅拌吓坏了一些人。

Of course, the first thing to notice is that it only allocates memory when
observers are being wired up. *Sending* a notification requires no memory
allocation whatsoever -- it's just a method call. If you hook up your observers
at the start of the game and don't mess with them much, the amount of allocation
is minimal.

当然，第一件需要注意的事情是只在观察者连线时分配内存。
*发送*通知不需要内存分配——只是一个方法调用。
如果你在游戏一开始就挂上了观察者而不乱动它们，分配的总量是很小的。

If it's still a problem, though, I'll walk through a way to implement adding and
removing observers without any dynamic allocation at all.

但是，如果这还是问题，我会介绍一种方式增加和删除观察者，而无需任何动态分配。

### Linked observers

### 连接观察者

In the code we've seen so far, `Subject` owns a list of pointers to each
`Observer` watching it. The `Observer` class itself has no reference to this
list. It's just a pure virtual interface. Interfaces are preferred over
concrete, stateful classes, so that's generally a good thing.

我们现在看到的所有代码中，`Subject`拥有一列指针指向观察它的`Observer`。
`Observer`类本身没有对这个列表的引用。
它只是一个纯虚接口。接口比具体有状态的类更受欢迎，所以这大体上是一件好事。

But if we *are* willing to put a bit of state in `Observer`, we can solve our
allocation problem by threading the subject's list *through the observers
themselves*. Instead of the subject having a separate collection of pointers,
the observer objects become nodes in a linked list:

但是如果我们*确实*愿意在`Observer`中放一些状态，
我们可以通过将客体的列表包含*观察者自己*来解决分配问题。
不是客体有一集合分散的指针，观察者对象成为了链表中的一部分：

<img src="images/observer-linked.png" alt="A linked list of Observers. Each has a next_ field pointing to the next one. A Subject has a head_ pointing to the first Observer." />

To implement this, first we'll get rid of the array in `Subject` and replace it
with a pointer to the head of the list of observers:

为了实现这一点，我们首先要摆脱`Subject`中的数组，然后用链表头部的指针取而代之：

^code linked-subject

Then we'll extend `Observer` with a pointer to the next observer in the list:

然后，我们在`Observer`中添加指向列表中下一观察者的指针。

^code linked-observer

We're also making `Subject` a friend class here. The subject owns the API for adding
and removing observers, but the list it will be managing is now inside the
`Observer` class itself. The simplest way to give it the ability to poke at that
list is by making it a friend.

我们也可以让`Subject`成为一个友类。
客体拥有增加和删除观察者的API，但是现在列表在`Observer`内部管理。
接触这个列表最简单的办法就是让它成为友类。

Registering a new observer is just wiring it into the list. We'll take the easy
option and insert it at the front:

注册一个新观察者就是将其连到链表中。我们使用更简单的选项，将其插到前头：

^code linked-add

The other option is to add it to the end of the linked list. Doing that adds a
bit more complexity. `Subject` has to either walk the list to find the end or
keep a separate `tail_` pointer that always points to the last node.

另一个选项是将其添加到链表的末尾。这么做增加了一定的复杂性。
`Subject`要么遍历整个链表来找到尾部，要么保留一个分离的`tail_`指针指向最后一个节点。

Adding it to the front of the list is simpler, but does have one side effect.
When we walk the list to send a notification to every observer, the most
*recently* registered observer gets notified *first*. So if you register
observers A, B, and C, in that order, they will receive notifications in C, B, A
order.

将它增加在列表的头很简单，但也有另一副作用。
当我们遍历列表给每个观察者发送一个通知，
最*近*注册的观察者最*先*接到通知。
所以如果以A，B，C的顺序来注册观察者，他们会以C，B，A的顺序接到通知。

In theory, this doesn't matter one way or the other. It's a tenet of good
observer discipline that two observers observing the same subject should have no
ordering dependencies relative to each other. If the ordering *does* matter, it
means those two observers have some subtle coupling that could end up biting
you.

理论上，这种方式和那种没什么区别。
一种好的观察者设计原则是观察同一客体的两个观察者互相之间不应该有任何顺序相关。
如果顺序*确实*有影响，这意味着这两个观察者有一些微妙的耦合，最终会伤到你。

Let's get removal working:

让我们把删除完成：

<span name="remove"></span>

<span name="remove"></span>

^code linked-remove

<aside name="remove">

Removing a node from a linked list usually requires a bit of ugly special case
handling for removing the very first node, like you see here. There's a more
elegant solution using a pointer to a pointer.

从链表移除一个节点通常需要一点丑陋的特殊情况来应对头节点，就像你在这里看到的。
这里还有一个更优雅的，用指向指针的指针的解决方案。

I didn't do that here because it confuses at least half the people I show it to.
It's a worthwhile exercise for you to do, though: It helps you really think in
terms of pointers.

我在这里没有那么做，是因为看到展示的半数人都迷糊了。
但这对你是一个很值得做的练习：它帮助你深入思考指针。

</aside>

Because we have a singly linked list, we have to walk it to find the observer
we're removing. We'd have to do the same thing if we were using a regular array
for that matter. If we use a *doubly* linked list, where each observer has a
pointer to both the observer after it and before it, we can remove an observer
in constant time. If this were real code, I'd do that.

因为我们有一个链表，得遍历它找到我们要删除的观察者。
如果我们使用一个普通的数组也要做相同的事。
如果我们使用*双向*链表，每个观察者都有指向前面和后面的各一个指针，
我们可以用常量时间移除一个观察者。如果这是真实代码，我会那么做。

The <span name="chain">only</span> thing left to do is send a notification.
That's as simple as walking the list:

<span name="chain">唯一</span>需要做的事情是发送通知，这和遍历列表同样简单；

^code linked-notify

<aside name="chain">

Here, we walk the entire list and notify every single observer in it. This
ensures that all of the observers get equal priority and are independent of each
other.

这里，我们遍历了整个链表，通知了其中每一个观察者。
这保证了所有的观察者有同样的优先级并相互独立。

We could tweak this such that when an observer is notified, it can return a flag
indicating whether the subject should keep walking the list or stop. If you do
that, you're pretty close to having the <a href="http://en.wikipedia.org/wiki
/Chain-of-responsibility_pattern" class="gof-pattern">Chain of
Responsibility</a> pattern.

我们可以这样实现，当观察者被通知时，它返回了一个标识表明客体是否应该继续遍历列表还是停止。如果你那么做，你就接近了<a href="http://en.wikipedia.org/wiki/Chain-of-responsibility_pattern" class="gof-pattern">职责链</a>模式。

</aside>

Not too bad, right? A subject can have as many observers as it wants, without a
single whiff of dynamic memory. Registering and unregistering is as fast as it
was with a simple array. We have sacrificed one small feature, though.

不差嘛，对吧？一个客体现在想有多少观察者就有多少观察者，不必使用动态内存。
注册和取消注册就像使用简单数组一样快。
但是，我们牺牲了一些小小的特性。

Since we are using the observer object itself as a list node, that implies it
can only be part of one subject's observer list. In other words, an observer can
only observe a single subject at a time. In a more traditional implementation
where each subject has its own independent list, an observer can be in more than
one of them simultaneously.

由于我们使用观察者对象作为链表节点，
这暗示了它只能在一个对象的观察者链表中。
换言之，一个观察者一次只能观察一个客体。
在传统的实现中，每一个客体有它独立的列表，
一个观察者同时可以在多个列表中。

You may be able to live with that limitation. I find it more common for a
*subject* to have multiple *observers* than vice versa. If it *is* a problem for
you, there is another more complex solution you can use that still doesn't
require dynamic allocation. It's too long to cram into this chapter, but I'll
sketch it out and let you fill in the blanks...

你也许可以接受这一限制。
我发现*客体*有多个*观察者*要比相反更常见。
如果这*真是*一个问题，这里还有一种不必使用动态分配的解决方案。
要将这个塞到这章就太长了，但我会大致描述，让你填补空白……

### A pool of list nodes

### 一池链表节点

Like before, each subject will have a linked list of observers. However, those
list nodes won't be the observer objects themselves. Instead, they'll be
separate little "list <span name="intrusive">node</span>" objects that contain a
pointer to the observer and then a pointer to the next node in the list.

就像以前，每个客体有一个观察者的链表。
但是，这些链表节点不是观察者本身。
相反，它们是分散的小“链表<span name="intrusive">节点</span>”对象，
包含了一个指向观察者的指针和另一个指向链表下一节点的指针。

<img src="images/observer-nodes.png" alt="A linked list of nodes. Each node has an observer_ field pointing to an Observer, and a next_ field pointing to the next node in the list. A Subject's head_ field points to the first node." />

Since multiple nodes can all point to the same observer, that means an observer
can be in more than one subject's list at the same time. We're back to being
able to observe multiple subjects simultaneously.

由于多个节点可以指向同一观察者，这就意味着观察者可以同时在超过一个客体列中。
我们可以同时观察多个对象了。

<aside name="intrusive">

Linked lists come in two flavors. In the one you learned in school, you have a
node object that contains the data. In our previous linked observer example,
that was flipped around: the *data* (in this case the observer) contained the
*node* (i.e. the `next_` pointer).

链表有两种风味。在你学校学习的那一种，你的节点对象包含数据。
在我们之前的链接观察者例子中，就反过来了：
*数据*（在这种例子中的观察者）包含了*节点*（`next_`指针）。

The latter style is called an "intrusive" linked list because using an object in
a list intrudes into the definition of that object itself. That makes intrusive
lists less flexible but, as we've seen, also more efficient. They're popular in
places like the Linux kernel where that trade-off makes sense.

后者的风格被称为“侵入式”链表，因为在对象内部使用链表侵入了对象本身的定义。
这让侵入式链表有更少的灵活性，但如我们所见，也更有效率。
它们在Linux核心这样的地方很流行。

</aside>

The way you avoid dynamic allocation is simple: since all of those nodes are the
same size and type, you pre-allocate an <a href="object-pool.html"
class="pattern">Object Pool</a> of them. That gives you a fixed-size pile of
list nodes to work with, and you can use and reuse them as you need without
having to hit an actual memory allocator.

避免动态分配的方法很简单：由于这些节点都是同样大小和类型，
可以预先在<a href="object-pool.html" class="pattern">对象池</a>中分配他们。
这给了你固定大小的列表节点处理，你可以随你所需使用并重用他们，
无需使用一个真正的内存分配器。

## Remaining Problems

## 剩余的问题

I think we've banished the three boogie men used to scare people off this
pattern. As we've seen, it's simple, fast, and can be made to play nice with
memory management. But does that mean you should use observers all the time?

我认为我们已经搞定了三个该模式将人们吓阻的主要问题。
我们看到，它简单，快速，对内存管理友好。
但是这意味着你总是应该使用观察者吗？

Now, that's a different question. Like all design patterns, the Observer pattern
isn't a cure-all. Even when implemented correctly and efficiently, it may not be
the right solution. The reason design patterns get a bad rap is because people
apply good patterns to the wrong problem and end up making things worse.

现在，这是另一个的问题。
就像所有的设计模式，观察者模式不是万能药。
哪怕可以实现的正确高效，它也不一定是好的解决方案。
设计模式获得坏名声的原因之一就是人们将好模式运用在错问题上，最终结果更糟。

Two challenges remain, one technical and one at something more like the
maintainability level. We'll do the technical one first because those are always
easiest.

还剩两个挑战，一个是技术性的，另一个更像是可维护性层次。
我们先处理技术性的，因为那总是更好处理。

### Destroying subjects and observers

### 销毁客体和观察者

The sample code we walked through is solid, but it
<span name="destruct">side-steps</span> an important issue: what happens when
you delete a subject or an
observer? If you carelessly call `delete` on some observer, a subject may still
have a pointer to it. That's now a dangling pointer into deallocated memory.
When that subject tries to send a notification, well... let's just say you're
not going to have a good time.

我们看到的样例代码很牢靠，但有一个严重的<span name="destruct">副作用</span>：
当删除一个客体或观察者时会发生什么？
如果你不小心的在某些观察者上面调用了`delete`，客体也许仍然持有指向它的指针。
这就是指向一片释放了的区域的悬挂指针。
当客体试图发送一个通知，好吧……就说不会有好时光吧。

<aside name="destruct">

Not to point fingers, but I'll note that *Design Patterns* doesn't mention this
issue at all.

不是针对谁，但我注意到*设计模式*完全没有提到这个问题。

</aside>

Destroying the subject is easier since in most implementations, the observer
doesn't have any references to it. But even then, sending the subject's bits to
the memory manager's recycle bin may cause some problems. Those observers may
still be expecting to receive notifications in the future, and they don't know
that that will never happen now. They aren't observers at all, really, they just
think they are.

删除客体更加容易，因为在大多数实现中，观察者没有对它的引用。
但是即使这样，将客体所占的位发送到内存管理系统的回收站也许会造成一些问题。
这些观察者也许仍然期待在以后收到通知，而他们不知道这再也不会发生了。
他们不再是观察者了，真的，他们只是认为他们是。

You can deal with this in a couple of different ways. The simplest is to do what
I did and just punt on it. It's an observer's job to unregister itself from any
subjects when it gets deleted. More often than not, the observer *does* know
which subjects it's observing, so it's usually just a matter of <span
name="destructor">adding</span> a `removeObserver()` call to its destructor.

你可以用好几种方式处理这点。
最简单的就是像我做的，然后一脚踩空。
在被删除时取消注册是观察者的职责。
多数情况下，观察者*确实*知道它在观察哪个实体，
所以这通常是给它的析构器<span name="destructor">添加</span>一个`removeObserver()`调用的工作量。

<aside name="destructor">

As is often the case, the hard part isn't doing it, it's *remembering* to do it.

通常在这种情况下，难点不在于如何做，而是*记得*做。

</aside>

If you don't want to leave observers hanging when a subject gives up the ghost,
that's easy to fix. Just have the subject send one final "dying breath"
notification right before it gets destroyed. That way, any observer can receive
that and take <span name="mourn">whatever action</span> it thinks is
appropriate.

如果在客体放弃存在，而你不想让观察者挂着，这也很好解决。
只需要让客体在它被摧毁前发送一个最终的“死亡叹息”通知。
通过这种方式，任何观察者都可以接收到，然后做那些它认为合适的<span name="mourn">行为</span>。

<aside name="mourn">

Mourn, send flowers, compose elegy, etc.

默哀，献花，挽歌……

</aside>

People -- even those of us who've spent enough time in the company of machines
to have some of their precise nature rub off on us -- are reliably terrible at
being reliable. That's why we invented computers: they don't make the mistakes
we so often do.

人们——哪怕是那些花费在大量时间在机器前，拥有让我们黯然失色才能的人——也是可靠地不可靠。
这就是为什么我们发明了电脑：它们不像我们那样经常犯错误。

A safer answer is to make observers automatically unregister themselves from
every subject when they get destroyed. If you implement the logic for that once
in your base observer class, everyone using it doesn't have to remember to do it
themselves. This does add some complexity, though. It means each *observer* will
need a list of the *subjects* it's observing. You end up with pointers going in
both directions.

更安全的回答是在每个客体销毁时，让观察者自动取消注册。
如果你在观察者基类中实现了这个逻辑，每个人不必记住就可以使用它。
这确实增加了一定的复杂度。
这意味着每个*观察者*都需要一个它在观察的*客体*列表。
你最终拥有了双向指针。

### Don't worry, I've got a GC

### 别担心，我有垃圾回收器

All you cool kids with your hip modern languages with garbage collectors are
feeling pretty smug right now. Think you don't have to worry about this because
you never explicitly delete anything? Think again!

你们那些装备有垃圾回收系统的孩子现在一定很洋洋自得。
觉得你不必担心这个，因为你从来不必显式删除任何东西？再想一想！

Imagine this: you've got some UI screen that shows a bunch of stats about the
player's character like their health and stuff. When the player brings up the
screen, you instantiate a new object for it. When they close it, you just forget
about the object and let the GC clean it up.

想象一下这个：你有UI显示玩家角色情况的状态，比如健康和道具。
当玩家在屏幕上时，你为其初始化了一个对象。
当他们退出时，你直接忘掉那个对象让GC清理。

Every time the character takes a punch to the face (or elsewhere, I suppose), it
sends a notification. The UI screen observes that and updates the little health
bar. Great. Now what happens when the player dismisses the screen, but you don't
unregister the observer?

每当角色脸上（或者其他什么地方）挨了一拳，它发送一个通知。
UI屏幕观察到了，然后更新健康槽。很好。
当玩家离开了屏幕，但你没有取消观察者的注册，会发生什么？

The UI isn't visible anymore, but it won't get garbage collected since the
character's observer list still has a reference to it. Every time the screen is
loaded, we add a new instance of it to that increasingly long list.

UI不再可见，但也不会进入垃圾回收系统，因为角色的观察者列表还保存着对它的引用。
每一次屏幕加载后，我们给那个不断增长的长列表添加一个新实例。

The entire time the player is playing the game, running around, and getting in
fights, the character is sending notifications that get received by *all* of
those screens. They aren't on screen, but they receive notifications and waste
CPU cycles updating invisible UI elements. If they do other things like play
sounds, you'll get noticeably wrong behavior.

玩家玩游戏的整个时间，来回跑，打架，角色都会发送通知给*所有*这些屏幕。
它们不在屏幕上，但它们接受通知，浪费CPU循环在不可见的UI元素上。
如果它们做了一些像播放声音的事情，就得到了可见的错误行为。

This is such a common issue in notification systems that it has a name: the
<span name="lapsed">*lapsed listener problem*</span>. Since subjects retain
references to their listeners, you can end up with zombie UI objects lingering
in memory. The lesson here is to be disciplined about unregistration.

这在通知系统中非常常见，甚至有个名字：<span name="lapsed">*失效监听者问题*</span>。
由于客体保留了对监听者的引用，你最终有僵尸UI对象徘徊在内存中。
这里上的一课是关于取消注册的纪律。

<aside name="lapsed">

An even surer sign of its significance: it has [a Wikipedia
article](http://en.wikipedia.org/wiki/Lapsed_listener_problem).

还有更加突出的标志：它有[维基条目](http://en.wikipedia.org/wiki/Lapsed_listener_problem)。

</aside>

### What's going on?

### 发生了什么？

The other, deeper issue with the Observer pattern is a direct consequence of its
intended purpose. We use it because it helps us loosen the coupling between two
pieces of code. It lets a subject indirectly communicate with some observer
without being statically bound to it.

另一个观察者的深层次问题是它意图的直接结果。
我们使用它是因为它帮助我们放松了两块代码之间的耦合。
它让客体间接与没有静态绑定的观察者交流。

This is a real win when you're trying to reason about the subject's behavior,
and any hangers-on would be an annoying distraction. If you're poking at the
physics engine, you really don't want your editor -- or your mind -- cluttered
up with a bunch of stuff about achievements.

当你要让客体行为有意义时，这是很有价值的，任何悬挂件都讨厌地分散注意力。
如果你在接触物理引擎，你根本不想要你的编辑器——或者你的脑子——为一堆成就系统的东西而繁忙。

On the other hand, if your program isn't working and the bug spans some chain of
observers, reasoning about that communication flow is much more difficult. With
an explicit coupling, it's as easy as looking up the method being called. This
is child's play for your average IDE since the coupling is static.

另一方面，如果你的程序没有工作，漏洞跨越了多个观察者，理清交流流程更加困难。
通过显式耦合，更容易查看哪一个方法被调用了。
这是由于耦合是静态的，分析它对于IDE是小孩子的把戏。

But if that coupling happens through an observer list, the only way to tell who
will get notified is by seeing which observers happen to be in that list *at
runtime*. Instead of being able to *statically* reason about the communication
structure of the program, you have to reason about its *imperative, dynamic*
behavior.

但是如果耦合发生在观察者列表中，唯一能告诉你谁被通知的方法是看看哪个观察者恰巧在列表中而且*处于运行中*。
不再是理清程序的*静态*交流结构，你得理清它的*命令式，动态*行为。

My guideline for how to cope with this is pretty simple. If you often need to
think about *both* sides of some communication in order to understand a part of
the program, don't use the Observer pattern to express that linkage. Prefer
something more explicit.

我对如何处理这个的指导原则很简单。
如果为了理解程序的一部分，沟通的两边*都*需要考虑，
那就不要使用观察者模式，使用其他更加显式的东西。

When you're hacking on some big program, you tend to have lumps of it that you
work on all together. We have lots of terminology for this like "separation of
concerns" and "coherence and cohesion" and "modularity", but it boils down to
"this stuff goes together and doesn't go with this other stuff".

当你在某些大型程序上用黑魔法时，你会感觉这样处理很笨拙。
我们有很多术语给它，比如“角落分离”，“一致性和内聚性”和“模块化”，
但是总归就是“这些东西待在一起而不与其他东西待在一起。”

The observer pattern is a great way to let those mostly unrelated lumps talk to
each other without them merging into one big lump. It's less useful *within* a
single lump of code dedicated to one feature or aspect.

观察者模式是一个让这些大量不相关的代码块互相交流，而不必打包成更大块的好方法。
这在专注于一个特性或层面的单一代码块*内*不会太有用。

That's why it fits our example well: achievements and physics are almost entirely
unrelated domains, likely implemented by different people. We want the bare
minimum of communication between them so that working on either one doesn't
require much knowledge of the other.

这就是为什么它能很好地适应我们的例子：
成就和物理是几乎完全不相干的领域，通常被不同人实现。
我们想要它们之间的交流最小化，
这样无论在哪一个上工作都不需要另一个的太多信息。

## Observers Today

## 今日观察者

*Design Patterns* came out in <span name="90s">1994</span>. Back then,
object-oriented programming was *the* hot paradigm. Every programmer on Earth
wanted to "Learn OOP in 30 Days," and middle managers paid them based on the
number of classes they created. Engineers judged their mettle by the depth of
their inheritance hierarchies.

*设计模式*源于<span name="90s">1994</span>。
那时候，面向对象语言是*那个*热门范式。
每个程序员都想要“30天学会OOP”，
中层管理员根据他们创建类的数量为他们支付工资。
工程师通过继承层次的深度评价质量。

<aside name="90s">

That same year, Ace of Base had not one but *three* hit singles, so that may
tell you something about our taste and discernment back then.

同一年，Ace of Base发行了不是一首而是*三首*畅销单曲，那也许能告诉你一些我们那时的品味和洞察力。

</aside>

The Observer pattern got popular during that zeitgeist, so it's no surprise that
it's class-heavy. But mainstream coders now are more comfortable with functional
programming. Having to implement an entire interface just to receive a
notification doesn't fit today's aesthetic.

观察者模式在那个时代中很流行，所以它有很多类就不奇怪了。
但是现代的主流程序员更加适应函数式语言。
实现一整套接口只是为了接受一个通知不再适合今日的美学了。

It feels <span name="different">heavyweight</span> and rigid. It *is*
heavyweight and rigid. For example, you can't have a single class that uses
different notification methods for different subjects.

它感觉上去是又<span name="different">沉重</span>又死板。它*确实*又沉重又死板。
举个例子，你不能有为不同的对象使用不同通知方法的类。

<aside name="different">

This is why the subject usually passes itself to the observer. Since an observer
only has a single `onNotify()` method, if it's observing multiple subjects, it
needs to be able to tell which one called it.

这就是为什么客体经常将它自己传给观察者。
一个观察者只有单一的`onNotify()`方法，如果它观察多个客体，它需要知道哪个在调用它。

</aside>

A more modern approach is for an "observer" to be only a reference to a method
or function. In languages with first-class functions, and especially ones with
<span name="closures">closures</span>, this is a much more common way to do
observers.

现代的解决办法是让“观察者”只是对方法或者函数的引用。
在函数作为第一公民的语言中，特别是那些有<span name="closures">闭包</span>的，
这是一个实现观察者更加普遍的方式。

<aside name="closures">

These days, practically *every* language has closures. C++ overcame the challenge
of closures in a language without garbage collection, and even Java finally got
its act together and introduced them in JDK 8.

今日，几乎*每种*语言都有闭包。C++克服了在没有垃圾回收的语言中构建闭包的挑战，
甚至是Java都一起行动起来在JDK8中引入了它们。

</aside>

For example, C# has "events" baked into the language. With those, the observer
you register is a "delegate", which is that language's term for a reference to a
method. In JavaScript's event system, observers *can* be objects supporting a
special `EventListener` protocol, but they can also just be functions. The
latter is almost always what people use.

举个例子，C#有“事件”嵌在语言中。
通过这些，你注册的观察者是一个“委托”，
这是那种语言的描述方法的引用的术语。
在JavaScript事件系统中，观察者*可以*是支持了特定`EventListener`协议的类，
但是他们也可以是函数。
后者是人们常用的方式。

If I were designing an observer system today, I'd make it <span
name="function">function-based</span> instead of class-based. Even in C++, I
would tend toward a system that let you register member function pointers as
observers instead of instances of some `Observer` interface.

如果现在设计观察者模式，我会让它<span name="function">基于函数</span>而不是基于类。
哪怕是在C++，我倾向于让你注册一个成员函数指针作为观察者，而不是`Observer`接口的实例。

<aside name="function">

[Here's][delegate] an interesting blog post on one way to implement this in C++.

[这里][delegate]的一篇有趣博文在C++上以某种方式实现了这一点。

[delegate]: http://molecularmusings.wordpress.com/2011/09/19/generic-type-safe-delegates-and-events-in-c/

</aside>

## Observers Tomorrow

## 明日观察者

Event systems and other observer-like patterns are incredibly common these days.
They're a well-worn path. But if you write a few large apps using them, you
start to notice something. A lot of the code in your observers ends up looking
the same. It's usually something like:

  1. Get notified that some state has changed.
  2. Imperatively modify some chunk of UI to reflect the new state.

事件系统和其他类观察者模式在今日令人惊奇的多。
它们都是经典方法。
但是如果你用它们写一个稍微大一些的应用，你会发现一件事情。
在观察者中很多代码最后都长得一样。通常是像这样：

  1. 获知有状态改变了。
  2. 命令式的改变一些UI来反映新的状态。

It's all, "Oh, the hero health is 7 now? Let me set the width of the health bar
to 70 pixels." After a while, that gets pretty tedious. Computer science
academics and software engineers have been trying to eliminate that tedium for a
*long* time. Their attempts have gone under a number of different names:
"dataflow programming", "functional reactive programming", etc.

这就是全部了，“哦，英雄的健康现在是7了？让我们把血条的宽度设为70像素。
过上一段时间，这会变得很沉闷。
计算机科学学术界和软件工程师已经尝试结束这种沉闷*很长*时间了。
他们用不同的方式做了很多次：“数据流编程”，“函数反射编程”等等。

While there have been some successes, usually in limited domains like audio
processing or chip design, the Holy Grail still hasn't been found. In the
meantime, a less ambitious approach has started gaining traction. Many recent
application frameworks now use "data binding".

如果有突破，一般局限在特定的领域中，比如音频处理或芯片设计，圣杯还没有被找到。
与此同时，一个更小野心的方式开始获得成效。很多现在的应用框架使用“数据绑定”。

Unlike more radical models, data binding doesn't try to entirely eliminate
imperative code and doesn't try to architect your entire application around a
giant declarative dataflow graph. What it does do is automate the busywork where
you're tweaking a UI element or calculated property to reflect a change to some
value.

不像激进的模型，数据绑定不再指望完全终结命令式代码，
也不尝试在巨大的宣言式数据图表中架构你的整个应用。
它做的只是自动改变UI元素或计算数值来反映一些值的改变。

Like other declarative systems, data binding is probably a bit too slow and
complex to fit inside the core of a game engine. But I would be surprised if I
didn't see it start making inroads into less critical areas of the game like
UI.

就像其他宣言式系统，数据绑定也许适应游戏引擎的核心太慢太复杂。
但是如果说没看到它开始侵入游戏不那么性能攸关的部分，比如UI，那我会很惊讶。

In the meantime, the good old Observer pattern will still be here waiting for
us. Sure, it's not as exciting as some hot technique that manages to cram both
"functional" and "reactive" in its name, but it's dead simple and it works. To
me, those are often the two most important criteria for a solution.


与此同时，经典观察者模式仍然在那里等着我们。
是的，它不像其他的新热门技术一样在名字中填满了“函数”“反射”，
但是它超简单而且能工作。对于我来说，这通常是一个解决方案最重要的条件。
