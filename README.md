# [Deferpp](https://github.com/olvap80/deferpp) (Defer++) is Go-like DEFER construction for C++ (execute code on scope exit)

Simple, lightweight, header-only, quick and portable Go-like DEFER construction for C++11 and above to do automatic resource cleanup once scope is exited. The header
[deferpp.h](https://github.com/olvap80/deferpp/blob/master/deferpp.h) is the only file needed to add DEFER macro for automatic resource cleanup.
DEFER is implemented in standard C++11 (but without involving any "heavyweight" stuff like std::function and without any dependencies at all!).

Sample sage:
```cpp
{
    auto resource = AcquireSomeResource(parameters_here)
    DEFER{ FreeThatResource(resource); };
    
    ... //work with that resource
}
//Note: code after DEFER is called when leaving scope due to any reason
//      (one can reach scope end, issue return/break/continue or throw
//       some exception, there is a guarantee such deferred code is called)
```

Intended to be a quick alternative for resource management and serves as a fast scope guard instead of coding RAII classes by hand. Use DEFER construction to ensure **cleanup code is called automatically
once {} scope is exited**.
Those ones being familiar with **defer** keyword in golang can now use DEFER macro in C++ and one does not need to write RAII wrapper class manually for every unique kind of resource usage.
Header-only library (single header [deferpp.h](https://github.com/olvap80/deferpp/blob/master/deferpp.h)). No installation required, no build needed,
just copy [deferpp.h](https://github.com/olvap80/deferpp/blob/master/deferpp.h) header file to any location in your project you like :)

More complex demo:
```cpp
#include "deferpp.h"
#include <iostream>

int main() {
    int i = 0;
    std::cout << "Enter application, i = " << i << std::endl;
    {
        std::cout << "After enter scope, i = " << i << std::endl;

        DEFER {
            ++i;
            std::cout << "Lambda called (apply side effect), i = " << i << std::endl;
        };

        std::cout << "Before exit scope, i = " << i << std::endl;
    }
    std::cout << "Exit application, i = " << i << std::endl;
    return 0;
}
```
Expected output is:

        Enter application, i = 0
        After enter scope, i = 0
        Before exit scope, i = 0
        Lambda called (apply side effect), i = 1
        Exit application, i = 1

Using DEFER macro allows easy debugging of your cleanup code in any IDE, just place debug break inside {} ))
