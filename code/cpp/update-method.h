//
//  update-method.h
//  cpp
//
//  Created by Bob Nystrom on 8/6/13.
//  Copyright (c) 2013 Bob Nystrom. All rights reserved.
//

#ifndef cpp_update_method_h
#define cpp_update_method_h

namespace UpdateMethod
{
  namespace Motivation
  {
    struct Entity
    {
      void shootLightning() {}
      void setX(double x) {}
    };

    void refreshGame() {}

    void justPatrol()
    {
      Entity skeleton;

      //^just-patrol
      while (true)
      {
        // 向右巡逻
        for (double x = 0; x < 100; x++)
        {
          skeleton.setX(x);
        }

        // 向左巡逻
        for (double x = 100; x > 0; x--)
        {
          skeleton.setX(x);
        }
      }
      //^just-patrol
    }

    void patrolInLoop()
    {
      //^patrol-in-loop
      Entity skeleton;
      bool patrollingLeft = false;
      double x = 0;

      // 游戏主循环
      while (true)
      {
        if (patrollingLeft)
        {
          x--;
          if (x == 0) patrollingLeft = false;
        }
        else
        {
          x++;
          if (x == 100) patrollingLeft = true;
        }

        skeleton.setX(x);

        // 处理用户输入并渲染游戏……
      }
      //^patrol-in-loop
    }

    void statues()
    {
      //^statues
      // 骷髅的变量……
      Entity leftStatue;
      Entity rightStatue;
      int leftStatueFrames = 0;
      int rightStatueFrames = 0;

      // 游戏主循环：
      while (true)
      {
        // 骷髅的代码……

        if (++leftStatueFrames == 90)
        {
          leftStatueFrames = 0;
          leftStatue.shootLightning();
        }

        if (++rightStatueFrames == 80)
        {
          rightStatueFrames = 0;
          rightStatue.shootLightning();
        }

        // 处理用户输入，渲染游戏
      }
      //^statues
    }
  }

  namespace KeepInMind
  {
    struct Entity
    {
      void setPosition(int x, int y) {}
      void shootLightning() {}
      void update() {}
    };

    static const int MAX_ENTITIES = 10;

    void refreshGame() {}

    void skipAdded()
    {
      int numObjects_ = 0;
      Entity* objects_[MAX_ENTITIES];
      //^skip-added
      int numObjectsThisTurn = numObjects_;
      for (int i = 0; i < numObjectsThisTurn; i++)
      {
        objects_[i]->update();
      }
      //^skip-added
    }

    void skipRemoved()
    {
      int numObjects_ = 0;
      Entity* objects_[MAX_ENTITIES];

      //^skip-removed
      for (int i = 0; i < numObjects_; i++)
      {
        objects_[i]->update();
      }
      //^skip-removed
    }
  }

  namespace SampleCode
  {
    static const int MAX_ENTITIES = 10;

    //^entity-class
    class Entity
    {
    public:
      Entity()
      : x_(0), y_(0)
      {}

      virtual ~Entity() {}
      virtual void update() = 0;

      double x() const { return x_; }
      double y() const { return y_; }

      void setX(double x) { x_ = x; }
      void setY(double y) { y_ = y; }

    private:
      double x_;
      double y_;
    };
    //^entity-class

    //^game-world
    class World
    {
    public:
      World()
      : numEntities_(0)
      {}

      void gameLoop();

    private:
      Entity* entities_[MAX_ENTITIES];
      int numEntities_;
    };
    //^game-world

    //^game-loop
    void World::gameLoop()
    {
      while (true)
      {
        // 处理用户输入……

        // 更新每个实体
        //^update-component-entities
        for (int i = 0; i < numEntities_; i++)
        {
          entities_[i]->update();
        }
        //^update-component-entities

        // 物理和渲染……
      }
    }
    //^game-loop

    //^skeleton
    class Skeleton : public Entity
    {
    public:
      Skeleton()
      : patrollingLeft_(false)
      {}

      virtual void update()
      {
        if (patrollingLeft_)
        {
          setX(x() - 1);
          if (x() == 0) patrollingLeft_ = false;
        }
        else
        {
          setX(x() + 1);
          if (x() == 100) patrollingLeft_ = true;
        }
      }

    private:
      bool patrollingLeft_;
    };
    //^skeleton

    //^statue
    class Statue : public Entity
    {
    public:
      Statue(int delay)
      : frames_(0),
        delay_(delay)
      {}

      virtual void update()
      {
        if (++frames_ == delay_)
        {
          shootLightning();

          // 重置计时器
          frames_ = 0;
        }
      }

    private:
      int frames_;
      int delay_;

      void shootLightning()
      {
        // 火光效果……
      }
    };
    //^statue
  }

  namespace ForwardToDelegate
  {
    class Entity;

    class Entity
    {
    public:
      Entity* state_;
      void update();
    };

    //^forward
    void Entity::update()
    {
      // 转发给状态对象
      state_->update();
    }
    //^forward
  }

  namespace VariableTimeStep
  {
    class Skeleton
    {
    public:
      double x;
      bool patrollingLeft_;
      void update(double elapsed);
    };

    //^variable
    void Skeleton::update(double elapsed)
    {
      if (patrollingLeft_)
      {
        x -= elapsed;
        if (x <= 0)
        {
          patrollingLeft_ = false;
          x = -x;
        }
      }
      else
      {
        x += elapsed;
        if (x >= 100)
        {
          patrollingLeft_ = true;
          x = 100 - (x - 100);
        }
      }
    }
    //^variable
  }

}

#endif
