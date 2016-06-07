^title 行为模式

Once you've built your game's set and festooned it with actors and props, all
that remains is to start the scene. For this, you need behavior -- the
screenplay that tells each entity in your game what to do.

一旦做好游戏设定，挂满了角色和道具，剩下的就是启动场景。
为了完成这点，你需要行为——告诉游戏中每个实体做什么的剧本。

Of course all code is "behavior", and all software is defining behavior, but
what's different about games is often the *breadth* of it that you have to
implement. While your word processor may have a long list of features, it pales
in comparison with the number of inhabitants, items, and quests in your average
role-playing game.

当然，所有代码都是“行为”，并且所有软件都是定义行为，
但在游戏中有所不同的是，行为通常很*多*。
文字处理器也许有很长的特性清单，
但与角色扮演游戏中的居民，物品和任务数量相比，那就相形见绌了。

The patterns in this chapter help to quickly define and refine a large quantity of
maintainable behavior. [Type Objects](type-object.html) create
categories of behavior without the rigidity of defining an actual class. A
[Subclass Sandbox](subclass-sandbox.html) gives you a safe set of primitives
you can use to define a variety of behaviors. The most advanced option is
[Bytecode](bytecode.html), which moves behavior out of code entirely and into
data.

本章的模式帮助快速定义和完善大量的行为。
[类型对象](type-object.html)定义行为的类别而无需完成真正的类。
[子类沙盒](subclass-sandbox.html)定义各种行为的安全原语。
最先进的是[字节码](bytecode.html)，将行为从代码中拖出，放入数据。

## The Patterns
## 模式

* [Bytecode](bytecode.html)
* [Subclass Sandbox](subclass-sandbox.html)
* [Type Object](type-object.html)
* [字节码](bytecode.html)
* [子类沙箱](subclass-sandbox.html)
* [类型对象](type-object.html)
