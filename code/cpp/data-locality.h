#include <time.h>

#ifndef cpp_data_locality_h
#define cpp_data_locality_h

// TODO(bob):
//
// cache effects are magnified by:
// - turning up number of actors
// - adding padding to the actor class
//   (both because it spaces the actors out more in memory)
//   in examples below, creating much bigger shuffled array just to spread
//   them out more.
// - adding padding to component magnifies it too, but also punishes best case

namespace DataLocality
{
  void sleepFor500Cycles() {}

  struct Thing
  {
    void doStuff() {}
  };

  static const int NUM_THINGS = 3;

  void callDoNothing()
  {
    Thing things[NUM_THINGS];

    //^do-nothing
    for (int i = 0; i < NUM_THINGS; i++)
    {
      sleepFor500Cycles();
      things[i].doStuff();
    }
    //^do-nothing
  }

  //^components
  class AIComponent
  {
  public:
    void update() { /* 处理并修改状态…… */ }

  private:
    // 目标，情绪，等等……
  };

  class PhysicsComponent
  {
  public:
    void update() { /* 处理并修改状态…… */ }

  private:
    // 刚体，速度，质量，等等……
  };

  class RenderComponent
  {
  public:
    void render() { /* 处理并修改状态…… */ }

  private:
    // 网格，纹理，着色器，等等……
  };
  //^components

  //^game-entity
  class GameEntity
  {
  public:
    GameEntity(AIComponent* ai,
               PhysicsComponent* physics,
               RenderComponent* render)
    : ai_(ai), physics_(physics), render_(render)
    {}

    AIComponent* ai() { return ai_; }
    PhysicsComponent* physics() { return physics_; }
    RenderComponent* render() { return render_; }

  private:
    AIComponent* ai_;
    PhysicsComponent* physics_;
    RenderComponent* render_;
  };
  //^game-entity

  void gameLoop()
  {
    int numEntities = 123;
    GameEntity* entities[123];
    bool gameOver = false;

    //^game-loop
    while (!gameOver)
    {
      // 处理AI
      for (int i = 0; i < numEntities; i++)
      {
        entities[i]->ai()->update();
      }

      // 更新物理
      for (int i = 0; i < numEntities; i++)
      {
        entities[i]->physics()->update();
      }

      // 绘制屏幕
      for (int i = 0; i < numEntities; i++)
      {
        entities[i]->render()->render();
      }

      // 其他和时间有关的游戏循环机制……
    }
    //^game-loop
  }

  static const int MAX_ENTITIES = 100;

  void componentArrays()
  {
    //^component-arrays
    AIComponent* aiComponents =
        new AIComponent[MAX_ENTITIES];
    PhysicsComponent* physicsComponents =
        new PhysicsComponent[MAX_ENTITIES];
    RenderComponent* renderComponents =
        new RenderComponent[MAX_ENTITIES];
    //^component-arrays

    int numEntities = 123;
    bool gameOver = false;

    //^game-loop-arrays
    while (!gameOver)
    {
      // 处理AI
      for (int i = 0; i < numEntities; i++)
      {
        aiComponents[i].update();
      }

      // 更新物理
      for (int i = 0; i < numEntities; i++)
      {
        physicsComponents[i].update();
      }

      // 绘制屏幕
      for (int i = 0; i < numEntities; i++)
      {
        renderComponents[i].render();
      }

      // 其他和时间有关的游戏循环机制……
    }
    //^game-loop-arrays

    delete [] aiComponents;
    delete [] physicsComponents;
    delete [] renderComponents;
  }

  //^particle-system
  class Particle
  {
  public:
    //^omit particle-system
    bool isActive() { return false; }
    //^omit particle-system
    void update() { /* 重力，等等…… */ }
    // 位置，速度，等等……
  };

  class ParticleSystem
  {
  public:
    ParticleSystem()
    : numParticles_(0)
    {}

    void update();
    //^omit particle-system
    void activateParticle(int index);
    void deactivateParticle(int index);
    //^omit particle-system
  private:
    static const int MAX_PARTICLES = 100000;

    int numParticles_;
    Particle particles_[MAX_PARTICLES];
  };
  //^particle-system

  //^update-particle-system
  void ParticleSystem::update()
  {
    for (int i = 0; i < numParticles_; i++)
    {
      particles_[i].update();
    }
  }
  //^update-particle-system

  void updateParticlesSlow()
  {
    Particle particles_[100];
    int numParticles_ = 0;
    //^particles-is-active
    for (int i = 0; i < numParticles_; i++)
    {
      if (particles_[i].isActive())
      {
        particles_[i].update();
      }
    }
    //^particles-is-active
  }

  Particle particles[100];
  int numActive_ = 0;
  void updateParticles()
  {
    //^update-particles
    for (int i = 0; i < numActive_; i++)
    {
      particles[i].update();
    }
    //^update-particles
  }

  //^activate-particle
  void ParticleSystem::activateParticle(int index)
  {
    // 不应该已被激活！
    assert(index >= numActive_);

    // 将它和第一个未激活的粒子交换
    Particle temp = particles_[numActive_];
    particles_[numActive_] = particles_[index];
    particles_[index] = temp;

    // 现在多了一个激活粒子
    numActive_++;
  }
  //^activate-particle

  //^deactivate-particle
  void ParticleSystem::deactivateParticle(int index)
  {
    // 不应该已被激活！
    assert(index < numActive_);

    // 现在少了一个激活粒子
    numActive_--;

    // 将它和最后一个激活粒子交换
    Particle temp = particles_[numActive_];
    particles_[numActive_] = particles_[index];
    particles_[index] = temp;
  }
  //^deactivate-particle

  enum Mood {
    MOOD_WISTFUL
  };

  class Animation {};
  class Vector {};
  class LootType {};

  namespace HotColdMixed
  {
    //^ai-component
    class AIComponent
    {
    public:
      void update() { /* ... */ }

    private:
      Animation* animation_;
      double energy_;
      Vector goalPos_;
    };
    //^ai-component
  }

  namespace HotColdMixedLoot
  {
    //^loot-drop
    class AIComponent
    {
    public:
      void update() { /* ... */ }

    private:
      // 之前的字段……
      LootType drop_;
      int minDrops_;
      int maxDrops_;
      double chanceOfDrop_;
    };
    //^loot-drop
  }

  namespace HotCold
  {
    class LootDrop;

    //^hot-cold
    class AIComponent
    {
    public:
      // 方法……
    private:
      Animation* animation_;
      double energy_;
      Vector goalPos_;

      LootDrop* loot_;
    };

    class LootDrop
    {
      friend class AIComponent;
      LootType drop_;
      int minDrops_;
      int maxDrops_;
      double chanceOfDrop_;
    };
    //^hot-cold
  }
}

#endif
