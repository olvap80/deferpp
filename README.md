# Deferpp (Defer++) Go-like DEFER construction for C++ 
Simple, header only, quick and portable Go-like DEFER construction for C++11 and above,
implemented in standard C++11 but without involving std::function.

Intended to be quick alternative for RAII: to ensure cleanup code is called
when scope is exited, so that one does not need to write RAII wrapper
class for every kind of resource.
Single header (deferpp.h). No installation required, no build needed,
just copy deferpp.h header file to any location you like :)
  
Usage:

    {
        auto resource = AcquireSomeResource(parameters_here)
        DEFER{ FreeeThatResource(resource); };
        
        ... //work with that resource
    }
    //Note: code after DEFER is called when leaving scope due to any reason
    //      (one can reach scope end, issue return/break/continue or throw
    //       some exception, and there is guarantee deferred code is called)

Demo:

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

Expected output is:

        Enter application, i = 0
        After enter scope, i = 0
        Before exit scope, i = 0
        Lambda called (apply side effect), i = 1
        Exit application, i = 1
