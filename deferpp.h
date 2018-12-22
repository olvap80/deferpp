/** @file deferpp.h
    @brief Deferpp (Defer++) is Go-like DEFER construction for C++11
    
    No installation/build needed. Copy in any location you like :)
    Created by Pavlo M
    
    Usage:
@code
    {
        auto resource = AcquireSomeResource(parameters_here)
        DEFER{ FreeeThatResource(resource); };
        
        ... //work with resource
    }
    //Note: code after DEFER is called when leaving scope due to any reason
    //      (one can reach scope end, issue return/break/continue or throw
    //       some exception, and there is guarantee deferred code is called)
@endcode
    
    See also sample test fragment at the bottom of this file
*/

#include <utility>


///Defer following code until enclosing scope is exited
/** Usage: DEFER{ some_code_to_be_deferred }; 
    Remember that some_code_to_be_deferred shall not allow
    exceptions to be propagated out of figure braces */
#define DEFER\
        auto DEFER_CAT_ID(callOnScopeExit,__LINE__) \
          = (Defer_SupportNamespace::tagClassForLambda) ->* [&]()



//Helper macro to expand and concatenate macro arguments into combined identifier
#define DEFER_CAT_ID(a,b) DEFER_CAT_ID_EXPANDED_HELPER(a,b)
//helper macro to concatenate expanded macro arguments
#define DEFER_CAT_ID_EXPANDED_HELPER(a,b) a##b

namespace Defer_SupportNamespace{
    ///Helper type to trigger operator ->*
    struct TagClassForLambda{ constexpr TagClassForLambda(){} };
    ///Use this "instance" to trigger overloaded operator ->*
    constexpr TagClassForLambda tagClassForLambda;

    ///RAII for implementing DEFER behavior
    template<class Lambda>
    class CallOnScopeExit{
    public:
        constexpr CallOnScopeExit(Lambda&& initialLambda)
            : lambda(std::move(initialLambda)), isOwner(true)
        {}
    
        constexpr CallOnScopeExit(CallOnScopeExit&& other)
            : lambda(std::move(other.lambda) ), isOwner(true)
        {
            other.isOwner = false;
        }

        
        CallOnScopeExit(const CallOnScopeExit& other) = delete;
        CallOnScopeExit& operator=(const CallOnScopeExit& other) = delete;

        
        ~CallOnScopeExit(){
            if( isOwner ){
                lambda();
            }
        }

    private:
        Lambda lambda; ///< Hold lambda, avoid slow std::function here
        bool isOwner; ///< Ensure 100% lambda is called only one time
    };

    ///Helper operator to easy catch lambda for DEFER macro
    /** Use template to avoid slow std::function
        (raw lambda is copied/stored here) */
    template<class Lambda>
    inline CallOnScopeExit<Lambda> operator ->* (const TagClassForLambda&, Lambda&& lambda){
        return CallOnScopeExit<Lambda>(std::move(lambda));
    }
}

//some basic testing (demo)
#if 0

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

    /* Expected output is:
        Enter application, i = 0
        After enter scope, i = 0
        Before exit scope, i = 0
        Lambda called (apply side effect), i = 1
        Exit application, i = 1
    */
#endif
