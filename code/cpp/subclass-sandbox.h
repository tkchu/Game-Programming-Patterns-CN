typedef int SoundId;
typedef int ParticleType;

const SoundId SOUND_SPROING = 1;
const SoundId SOUND_SWOOP = 1;
const SoundId SOUND_DIVE = 1;
const ParticleType PARTICLE_DUST = 1;
const ParticleType PARTICLE_SPARKLES = 1;

namespace SimpleExample
{
  //^1
  class Superpower
  {
  public:
    virtual ~Superpower() {}

  protected:
    virtual void activate() = 0;

    void move(double x, double y, double z)
    {
      // 实现代码……
    }

    void playSound(SoundId sound, double volume)
    {
      // 实现代码……
    }

    void spawnParticles(ParticleType type, int count)
    {
      // 实现代码……
    }
  };
  //^1

  //^2
  class SkyLaunch : public Superpower
  {
  protected:
    virtual void activate()
    {
      // 空中滑行
      playSound(SOUND_SPROING, 1.0f);
      spawnParticles(PARTICLE_DUST, 10);
      move(0, 0, 20);
    }
  };
  //^2
}

namespace Elaborated
{
  //^3
  class Superpower
  {
  protected:
    //^omit
    virtual void activate() = 0;
    void move(double x, double y, double z) {}
    void playSound(SoundId sound, double volume) {}
    void spawnParticles(ParticleType type, int count) {}
    //^omit
    double getHeroX()
    {
      // 实现代码……
      //^omit
      return 0;
      //^omit
    }

    double getHeroY()
    {
      // 实现代码……
      //^omit
      return 0;
      //^omit
    }

    double getHeroZ()
    {
      // 实现代码……
      //^omit
      return 0;
      //^omit
    }

    // 退出之类的……
  };
  //^3

  //^4
  class SkyLaunch : public Superpower
  {
  protected:
    virtual void activate()
    {
      if (getHeroZ() == 0)
      {
        // 在地面上，冲向空中
        playSound(SOUND_SPROING, 1.0f);
        spawnParticles(PARTICLE_DUST, 10);
        move(0, 0, 20);
      }
      else if (getHeroZ() < 10.0f)
      {
        // 接近地面，再跳一次
        playSound(SOUND_SWOOP, 1.0f);
        move(0, 0, getHeroZ() + 20);
      }
      else
      {
        // 正在空中，跳劈攻击
        playSound(SOUND_DIVE, 0.7f);
        spawnParticles(PARTICLE_SPARKLES, 1);
        move(0, 0, -getHeroZ());
      }
    }
  };
  //^4
}

namespace Forwarding
{
  struct SoundEngine
  {
    void play(SoundId sound, double volume) {}
  };

  SoundEngine soundEngine_;

  //^5
  void playSound(SoundId sound, double volume)
  {
    soundEngine_.play(sound, volume);
  }
  //^5
}

namespace HelperClassBefore
{
  //^6
  class Superpower
  {
  protected:
    void playSound(SoundId sound, double volume)
    {
      // 实现代码……
    }

    void stopSound(SoundId sound)
    {
      // 实现代码……
    }

    void setVolume(SoundId sound)
    {
      // 实现代码……
    }

    // 沙盒方法和其他操作……
  };
  //^6
};

namespace HelperClassAfter
{
  //^7
  class SoundPlayer
  {
    void playSound(SoundId sound, double volume)
    {
      // 实现代码……
    }

    void stopSound(SoundId sound)
    {
      // 实现代码……
    }

    void setVolume(SoundId sound)
    {
      // 实现代码……
    }
  };
  //^7

  //^8
  class Superpower
  {
  protected:
    SoundPlayer& getSoundPlayer()
    {
      return soundPlayer_;
    }

    // 沙箱方法和其他操作……

  private:
    SoundPlayer soundPlayer_;
  };
  //^8
}

namespace PassToConstructor
{
  class ParticleSystem {};

  //^pass-to-ctor-base
  class Superpower
  {
  public:
    Superpower(ParticleSystem* particles)
    : particles_(particles)
    {}

    // 沙箱方法和其他操作……

  private:
    ParticleSystem* particles_;
  };
  //^pass-to-ctor-base

  //^pass-to-ctor-sub
  class SkyLaunch : public Superpower
  {
  public:
    SkyLaunch(ParticleSystem* particles)
    : Superpower(particles)
    {}
  };
  //^pass-to-ctor-sub
}

namespace TwoStageInit
{
  class ParticleSystem {};

  class Superpower
  {
  public:
    void init(ParticleSystem* particles) {}
  };

  ParticleSystem* particles;

  class SkyLaunch : public Superpower {};

  void foo()
  {
    //^9
    Superpower* power = new SkyLaunch();
    power->init(particles);
    //^9
  }
}

namespace TwoStageInitEncapsulated
{
  class ParticleSystem {};

  class Superpower
  {
  public:
    void init(ParticleSystem* audio) {}
  };

  class SkyLaunch : public Superpower {};

  //^10
  Superpower* createSkyLaunch(ParticleSystem* particles)
  {
    Superpower* power = new SkyLaunch();
    power->init(particles);
    return power;
  }
  //^10
}

namespace StaticState
{
  class ParticleSystem {};

  //^11
  class Superpower
  {
  public:
    static void init(ParticleSystem* particles)
    {
      particles_ = particles;
    }

    // 沙箱方法和其他操作……

  private:
    static ParticleSystem* particles_;
  };
  //^11
}

namespace UseServiceLocator
{
  struct ParticleSystem
  {
    void spawn(ParticleType type, int count);
  };

  ParticleSystem particles;

  class Locator
  {
  public:
    static ParticleSystem& getParticles() { return particles; }
  };

  //^12
  class Superpower
  {
  protected:
    void spawnParticles(ParticleType type, int count)
    {
      ParticleSystem& particles = Locator::getParticles();
      particles.spawn(type, count);
    }

    // 沙箱方法和其他操作……
  };
  //^12
}
