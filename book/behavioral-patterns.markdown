^title Behavioral Patterns

行为模式

Once you've built your game's set and festooned it with actors and props, all
that remains is to start the scene. For this, you need behavior -- the
screenplay that tells each entity in your game what to do.

一旦你做好你的游戏设定，挂满了角色和道具，剩下的就是启动场景。对于这点，你需要行为——告诉每一个你游戏中的实体做什么的剧本。

Of course all code is "behavior", and all software is defining behavior, but
what's different about games is often the *breadth* of it that you have to
implement. While your word processor may have a long list of features, it pales
in comparison with the number of inhabitants, items, and quests in your average
role-playing game.

当然，所有的代码都是“行为”，并且所有的软件都是定义行为，但在游戏中有所不同的是，行为通常很*多*。你的文字处理器也许有很长的特性清单，但是与通常玩的角色扮演游戏中的居民，物品和任务数量相比，那就相形见绌了。

The patterns in this chapter help to quickly define and refine a large quantity of
maintainable behavior. [Type Objects](type-object.html) create
categories of behavior without the rigidity of defining an actual class. A
[Subclass Sandbox](subclass-sandbox.html) gives you a safe set of primitives
you can use to define a variety of behaviors. The most advanced option is
[Bytecode](bytecode.html), which moves behavior out of code entirely and into
data.

本章的模式帮助快速定义和完善大量的维护行为。[类型对象]定义行为的类别而不需要完成一个真正的类。[子类沙盒]给你定义各种行为的安全原语。最先进的方法是[字节码]，将行为从代码中拖出，放入数据。

## The Patterns

* [Bytecode](bytecode.html)
* [Subclass Sandbox](subclass-sandbox.html)
* [Type Object](type-object.html)
