#include <iostream>

class AudioSystem
{
public:
  static void playSound(int id) {}
  static AudioSystem* instance() { return NULL; }
};

void example()
{
  int VERY_LOUD_BANG = 0;

  //^15
  // 使用静态类？
  AudioSystem::playSound(VERY_LOUD_BANG);

  // 还是使用单例？
  AudioSystem::instance()->playSound(VERY_LOUD_BANG);
  //^15
}

//^9
class Audio
{
public:
  virtual ~Audio() {}
  virtual void playSound(int soundID) = 0;
  virtual void stopSound(int soundID) = 0;
  virtual void stopAllSounds() = 0;
};
//^9

//^10
class ConsoleAudio : public Audio
{
public:
  virtual void playSound(int soundID)
  {
    // 使用主机音频API播放声音……
  }

  virtual void stopSound(int soundID)
  {
    // 使用主机音频API停止声音……
  }

  virtual void stopAllSounds()
  {
    // 使用主机音频API停止所有声音……
  }
};
//^10

//^12
class LoggedAudio : public Audio
{
public:
  LoggedAudio(Audio &wrapped)
  : wrapped_(wrapped)
  {}

  virtual void playSound(int soundID)
  {
    log("play sound");
    wrapped_.playSound(soundID);
  }

  virtual void stopSound(int soundID)
  {
    log("stop sound");
    wrapped_.stopSound(soundID);
  }

  virtual void stopAllSounds()
  {
    log("stop all sounds");
    wrapped_.stopAllSounds();
  }

private:
  void log(const char* message)
  {
    // 记录日志的代码……
  }

  Audio &wrapped_;
};
//^12

// design decisions / di
namespace Version1
{
  //^1
  class Locator
  {
  public:
    static Audio* getAudio() { return service_; }

    static void provide(Audio* service)
    {
      service_ = service;
    }

  private:
    static Audio* service_;
  };
  //^1

  Audio *Locator::service_;

  void initGame()
  {
    //^11
    ConsoleAudio *audio = new ConsoleAudio();
    Locator::provide(audio);
    //^11
  }

  void someGameCode()
  {
    int VERY_LOUD_BANG = 0;
    //^5
    Audio *audio = Locator::getAudio();
    audio->playSound(VERY_LOUD_BANG);
    //^5
  }
}

// design decisions / compile time
namespace Version2
{
  class DebugAudio: public Audio
  {
  public:
    virtual void playSound(int soundID) { /* 什么也不做 */ }
    virtual void stopSound(int soundID) { /* 什么也不做 */ }
    virtual void stopAllSounds()        { /* 什么也不做 */ }
  };
  class ReleaseAudio: public DebugAudio {};

  //^2
  class Locator
  {
  public:
    static Audio& getAudio() { return service_; }

  private:
    #if DEBUG
      static DebugAudio service_;
    #else
      static ReleaseAudio service_;
    #endif
  };
  //^2
}

// design decisions / scope
namespace Version3
{
  //^3
  class Base
  {
    // 定位和设置服务的代码……

  protected:
    // 派生类可以使用服务
    static Audio& getAudio() { return *service_; }

  private:
    static Audio* service_;
  };
  //^3
}

namespace Version4
{
  //^4
  class Locator
  {
  public:
    static Audio& getAudio()
    {
      Audio* service = NULL;

      // Code here to locate service...

      assert(service != NULL);
      return *service;
    }
  };
  //^4
}

namespace Version5
{
  //^7
  class NullAudio: public Audio
  {
  public:
    virtual void playSound(int soundID) { /* 什么也不做 */ }
    virtual void stopSound(int soundID) { /* 什么也不做 */ }
    virtual void stopAllSounds()        { /* 什么也不做 */ }
  };
  //^7

  //^8
  class Locator
  {
  public:
    static void initialize() { service_ = &nullService_; }

    static Audio& getAudio() { return *service_; }

    static void provide(Audio* service)
    {
      if (service == NULL)
      {
        // 退回空服务
        service_ = &nullService_;
      }
      else
      {
        service_ = service;
      }
    }

  private:
    static Audio* service_;
    static NullAudio nullService_;
  };
  //^8

  Audio *Locator::service_ = NULL;
  NullAudio Locator::nullService_;

  //^13
  void enableAudioLogging()
  {
    // 装饰现有的服务
    Audio *service = new LoggedAudio(Locator::getAudio());

    // 将它换进来
    Locator::provide(service);
  }
  //^13
}
