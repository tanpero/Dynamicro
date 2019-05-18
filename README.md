# Dynamicro

---

一个基于 C++ 11 的唯头文件库，提供了一组简单易用的方法，来根据名称调用指定动态链接库中的接口。

以下是简单示例。

在 `library.h` 中：

```c++
#ifndef _LIBRARY_H_
#define _LIBRARY_H_

int add(int a, int b);

#endif // !_LIBRARY_H_
```

在 `library.cpp` 中：

```c++
#include "library.h"

int add(int a, int b)
{
    return a + b;
}
```

假设我们已经根据 `library.cpp` 构建出了 `library.dll` ，那么我们可以通过 `Dynamicro` 来使用它。

在 `main.cpp` 中：

```c++
#include "library.h"
#include "dynamicro.h"
#include <iostream>

int main(int argc, char* argv[])
{
    Dynamicro d;
    d.load("library.dll");
    int result = d.exec("add", 10, 20);
    std::cout << "result is " << result << std::endl;
    return 0;
}
```

在编译时，请确保编译器使用了 C++ 14。一切正常，我们会得到这样的输出：

```
result is 30
```

就是这样简单。玩得愉快~



