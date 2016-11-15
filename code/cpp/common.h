#pragma once

// 让变量表现得像是在使用中
// 这样我们不会因为未使用变量而获得警告。
// 把警告用于捕获真正疏忽没有使用的变量。
template <class T>
void use(const T& obj) {
  // 啥也不做
}

#define ASSERT(condition) \
    if (!(condition)) \
    { \
      std::cout << "FAIL: " #condition << "\n" << __FILE__ \
                << ":" << __LINE__ << std::endl; \
      abort(); \
    }

void assert(bool condition) {
  if (!condition) {
    printf("WTF\n");
    exit(1);
  }
}
