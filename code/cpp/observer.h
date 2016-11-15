#ifndef observer_h
#define observer_h

namespace ObserverPattern
{
  using namespace std;

  static const int SURFACE_BRIDGE = 0;

  class Entity {
  public:
    bool isHero() const { return false; }
    bool isStandingOn(int surface) const { return false; }
    bool isOnSurface() { return true; }
    void accelerate(int force) {}
    void update() {}
  };

  enum Event
  {
    EVENT_ENTITY_FELL
  };

  enum Achievement
  {
    ACHIEVEMENT_FELL_OFF_BRIDGE
  };

  static const int GRAVITY = 1;
  static const int EVENT_START_FALL = 1;

  namespace Motivation
  {
    class Physics
    {
    public:
      void updateEntity(Entity& entity);
      void notify(Entity& entity, int event) {}
    };

    //^physics-update
    void Physics::updateEntity(Entity& entity)
    {
      bool wasOnSurface = entity.isOnSurface();
      entity.accelerate(GRAVITY);
      entity.update();
      if (wasOnSurface && !entity.isOnSurface())
      {
        notify(entity, EVENT_START_FALL);
      }
    }
    //^physics-update
  }

  namespace Pattern
  {
    //^observer
    class Observer
    {
    public:
      virtual ~Observer() {}
      virtual void onNotify(const Entity& entity, Event event) = 0;
    };
    //^observer

    //^achievement-observer
    class Achievements : public Observer
    {
    public:
      virtual void onNotify(const Entity& entity, Event event)
      {
        switch (event)
        {
        case EVENT_ENTITY_FELL:
          if (entity.isHero() && heroIsOnBridge_)
          {
            unlock(ACHIEVEMENT_FELL_OFF_BRIDGE);
          }
          break;

          // 处理其他事件，更新heroIsOnBridge_变量……
        }
      }

    private:
      void unlock(Achievement achievement)
      {
        // 如果还没有解锁，那就解锁成就……
      }

      bool heroIsOnBridge_;
    };
    //^achievement-observer

    static const int MAX_OBSERVERS = 10;

    //^subject-list
    //^subject-register
    //^subject-notify
    class Subject
    {
      //^omit subject-list
      //^omit subject-notify
    public:
      void addObserver(Observer* observer)
      {
        // 添加到数组中……
        //^omit
        observers_[numObservers_++] = observer;
        //^omit
      }

      void removeObserver(Observer* observer)
      {
        // 从数组中移除……
        //^omit
        int index;
        for (index = 0; index < MAX_OBSERVERS; index++)
        {
          if (observers_[index] == observer) break;
        }

        if (index < numObservers_)
        {
          // 将下一个向前移动
          for (; index < numObservers_ - 1; index++)
          {
            observers_[index] = observers_[index + 1];
          }

          numObservers_--;
        }
        //^omit
      }

      // 其他代码……
      //^omit subject-notify
      //^omit subject-register
    protected:
      void notify(const Entity& entity, Event event)
      {
        for (int i = 0; i < numObservers_; i++)
        {
          observers_[i]->onNotify(entity, event);
        }
      }

      // 其他代码…………
      //^omit subject-notify

      //^omit subject-list
    private:
      Observer* observers_[MAX_OBSERVERS];
      int numObservers_;
      //^omit subject-register
      //^omit subject-notify
    };
    //^subject-list
    //^subject-register
    //^subject-notify

    //^physics-inherit
    class Physics : public Subject
    {
    public:
      void updateEntity(Entity& entity);
    };
    //^physics-inherit

    class PhysicsEvent : public Observer
    {
      Subject entityFell_;
      Subject& entityFell() { return entityFell_; }

      virtual void onNotify(const Entity& entity, Event event) {}

      void physicsEvent()
      {
        PhysicsEvent physics;

        //^physics-event
        physics.entityFell()
          .addObserver(this);
        //^physics-event
      }
    };
  }

  namespace LinkedObservers
  {
    //^linked-observer
    class Observer
    {
      friend class Subject;

    public:
      Observer()
      : next_(NULL)
      {}

      // 其他代码……
      //^omit
      virtual void onNotify(const Entity& entity, Event event) {}
      //^omit
    private:
      Observer* next_;
    };
    //^linked-observer

    //^linked-subject
    class Subject
    {
      Subject()
      : head_(NULL)
      {}

      // 方法……
      //^omit
      void addObserver(Observer* observer);
      void removeObserver(Observer* observer);
      void notify(const Entity& entity, Event event);
      //^omit
    private:
      Observer* head_;
    };
    //^linked-subject

    //^linked-add
    void Subject::addObserver(Observer* observer)
    {
      observer->next_ = head_;
      head_ = observer;
    }
    //^linked-add

    //^linked-remove
    void Subject::removeObserver(Observer* observer)
    {
      if (head_ == observer)
      {
        head_ = observer->next_;
        observer->next_ = NULL;
        return;
      }

      Observer* current = head_;
      while (current != NULL)
      {
        if (current->next_ == observer)
        {
          current->next_ = observer->next_;
          observer->next_ = NULL;
          return;
        }

        current = current->next_;
      }
    }
    //^linked-remove

    //^linked-notify
    void Subject::notify(const Entity& entity, Event event)
    {
      Observer* observer = head_;
      while (observer != NULL)
      {
        observer->onNotify(entity, event);
        observer = observer->next_;
      }
    }
    //^linked-notify
  }

  namespace One
  {
    class Observable;

    class Observer
    {
      friend class Observable;

    public:
      bool isObserving() const { return observable_ != NULL; }

      void observe(Observable& observable);
      void detach();

    protected:
      Observer()
      : prev_(this),
        next_(this)
      {}

      virtual ~Observer()
      {
        detach();
      }

      virtual void onNotify(Observable& observable) = 0;

    private:
      // 观察者正观察的对象
      Observable* observable_ = NULL;

      // 被观察这的环形链表的下一个和上一个节点。
      Observer* prev_;
      Observer* next_;
    };

    class Observable
    {
      friend class Observer;

    public:
      bool hasObserver() const { return observer_ != NULL; }

    protected:
      Observable()
      : observer_(NULL)
      {}

      virtual ~Observable()
      {
        // 分发给所有的观察者
        while (observer_ != NULL) observer_->detach();
      }

      void notify()
      {
        if (observer_ == NULL) return;

        Observer* observer = observer_;
        do
        {
          observer->onNotify(*this);
          observer = observer->next_;
        }
        while (observer != observer_);
      }

      // 观察者链表中的第一个
      Observer* observer_;
    };

    void Observer::observe(Observable& observable)
    {
      // 停止观察正在观察的事物
      detach();

      if (observable.observer_ == NULL)
      {
        // 第一个观察者
        observable.observer_ = this;
      }
      else
      {
        // 已然有了其他观察者，将它连接到链表的尾部。
        prev_ = observable.observer_->prev_;
        next_ = observable.observer_;

        observable.observer_->prev_->next_ = this;
        observable.observer_->prev_ = this;
      }

      observable_ = &observable;
    }

    void Observer::detach()
    {
      if (observable_ == NULL) return;

      // 保证被观察者的观察者指针没有指向这个节点。
      if (observable_->observer_ == this)
      {
        if (next_ == this)
        {
          // 这是唯一的观察者，清空它即可
          observable_->observer_ = NULL;
        }
        else
        {
          // 推进到下一个节点
          observable_->observer_ = next_;
        }
      }

      // 将这个观察者从列表中去除
      prev_->next_ = next_;
      next_->prev_ = prev_;

      prev_ = this;
      next_ = this;
      observable_ = NULL;
    }

    class Noise : public Observable
    {
    public:
      Noise(const char* name)
      : name_(name)
      {}

      void sound()
      {
        cout << name_ << "!" << endl;
        notify();
      }

    private:
      const char* name_;
    };

    class Ear : public Observer
    {
    public:
      Ear(const char* name)
      : name_(name)
      {}

      int numObserved = 0;

    protected:
      virtual void onNotify(Observable & observable)
      {
        numObserved++;
        cout << name_ << " heard it!" << endl;
      }

    private:
      const char* name_;
    };

    void destructSoloObserverTest()
    {
      Ear* ear = new Ear("ear");
      Noise noise("beep");
      ear->observe(noise);

      delete ear;
      ASSERT(!noise.hasObserver());

      noise.sound();
    }

    void destructMultipleObserverTest()
    {
      Ear* ear1 = new Ear("ear1");
      Ear* ear2 = new Ear("ear2");
      Noise noise("beep");
      ear1->observe(noise);
      ear2->observe(noise);

      delete ear2;
      ASSERT(noise.hasObserver());

      delete ear1;
      ASSERT(!noise.hasObserver());

      noise.sound();
    }

    void destructObservableTest()
    {
      Ear ear1("ear1");
      Ear ear2("ear2");
      Noise* noise = new Noise("beep");
      ear1.observe(*noise);
      ear2.observe(*noise);

      delete noise;
      ASSERT(!ear1.isObserving());
      ASSERT(!ear2.isObserving());
    }

    void notifyTest()
    {
      Noise noise1("beep");
      Ear ear1("one");
      Ear ear2("two");
      Ear ear3("three");

      noise1.sound();
      ASSERT(ear1.numObserved == 0);
      ASSERT(ear2.numObserved == 0);
      ASSERT(ear3.numObserved == 0);

      ear1.observe(noise1);
      noise1.sound();
      ASSERT(ear1.numObserved == 1);
      ASSERT(ear2.numObserved == 0);
      ASSERT(ear3.numObserved == 0);

      ear2.observe(noise1);
      noise1.sound();
      ASSERT(ear1.numObserved == 2);
      ASSERT(ear2.numObserved == 1);
      ASSERT(ear3.numObserved == 0);

      ear3.observe(noise1);
      noise1.sound();
      ASSERT(ear1.numObserved == 3);
      ASSERT(ear2.numObserved == 2);
      ASSERT(ear3.numObserved == 1);

      ear2.detach();
      noise1.sound();
      ASSERT(ear1.numObserved == 4);
      ASSERT(ear2.numObserved == 2);
      ASSERT(ear3.numObserved == 2);

      ear1.detach();
      noise1.sound();
      ASSERT(ear1.numObserved == 4);
      ASSERT(ear2.numObserved == 2);
      ASSERT(ear3.numObserved == 3);

      ear3.detach();
      noise1.sound();
      ASSERT(ear1.numObserved == 4);
      ASSERT(ear2.numObserved == 2);
      ASSERT(ear3.numObserved == 3);
    }

    void observeTest()
    {
      Ear ear("ear");
      Noise beep("beep");
      Noise boop("boop");

      ear.observe(beep);
      beep.sound();
      ASSERT(ear.numObserved == 1);
      boop.sound();
      ASSERT(ear.numObserved == 1);

      // 不应再听着beep
      ear.observe(boop);
      beep.sound();
      ASSERT(ear.numObserved == 1);
      boop.sound();
      ASSERT(ear.numObserved == 2);
    }

    void test()
    {
      destructSoloObserverTest();
      destructMultipleObserverTest();
      destructObservableTest();
      notifyTest();
      observeTest();
    }
  }

  namespace Pool
  {
    class Binding;
    class BindingPool;
    class Listener;

    class Event
    {
    public:
      Event(BindingPool& pool)
      : pool_(pool),
        binding_(NULL)
      {}

      void addListener(Listener& listener);
      void removeListener(Listener& listener);

      void send(const char* data);

    private:
      BindingPool& pool_;
      Binding* binding_;
    };

    class Listener
    {
    public:
      Listener(const char* name)
      : name_(name)
      {}

      int numEvents() const;

      void receive(const char* data)
      {
        cout << name_ << " received " << data << endl;
      }

    private:
      const char* name_;
    };

    class Binding
    {
      friend class BindingPool;
      friend class Event;

    public:
      Binding()
      : listener_(NULL),
        next_(NULL)
      {}

    private:
      Listener* listener_;

      // If the binding is in use, this will point to the binding for the
      // next listener. Otherwise, it will point to the next free binding.
      Binding* next_;
    };

    class BindingPool
    {
    public:
      BindingPool()
      {
        // Build the free list.
        free_ = &bindings_[0];
        for (int i = 0; i < POOL_SIZE - 1; i++)
        {
          bindings_[i].next_ = &bindings_[i + 1];
        }
      }

      Binding* newBinding()
      {
        // TODO(bob): Make sure there is a free one.
        // Remove the head of the free list.
        Binding* binding = free_;
        free_ = free_->next_;
        return binding;
      }

    private:
      static const int POOL_SIZE = 100;

      Binding bindings_[POOL_SIZE];

      // Pointer to the first binding in the list of free bindings.
      Binding* free_;
    };


    void Event::addListener(Listener& listener)
    {
      // TODO(bob): Add to end of list.
      Binding* binding = pool_.newBinding();
      binding->listener_ = &listener;
      binding->next_ = binding_;
      binding_ = binding;
    }

    void Event::removeListener(Listener& listener)
    {
      // TODO(bob): Implement me!
    }

    void Event::send(const char* data)
    {
      Binding* binding = binding_;
      while (binding != NULL)
      {
        binding->listener_->receive(data);
        binding = binding->next_;
      }
    }

    int Listener::numEvents() const
    {
      return 0; // ?
    }

    // TODO(bob): Destructors for all of these types.

    void destructEventTest()
    {
      BindingPool pool;

      Event* event1 = new Event(pool);
      Event* event2 = new Event(pool);

      Listener listener1("listener 1");
      Listener listener2("listener 2");

      event1->addListener(listener1);
      event1->addListener(listener2);
      event2->addListener(listener1);
      event2->addListener(listener2);

      ASSERT(listener1.numEvents() == 2);
      ASSERT(listener2.numEvents() == 2);

      delete event1;

      ASSERT(listener1.numEvents() == 1);
      ASSERT(listener2.numEvents() == 1);

      delete event2;

      ASSERT(listener1.numEvents() == 0);
      ASSERT(listener2.numEvents() == 0);
    }

    void test()
    {
      destructEventTest();

      BindingPool pool;

      Event event1(pool);
      Event event2(pool);

      Listener listener1("listener 1");
      Listener listener2("listener 2");
      Listener listener3("listener 3");

      event1.addListener(listener1);
      event1.addListener(listener2);
      event2.addListener(listener2);
      event2.addListener(listener3);

      event1.send("first");
      event2.send("second");
    }
  }

  void test()
  {
    //One::test();
    Pool::test();
  }
}

#endif
