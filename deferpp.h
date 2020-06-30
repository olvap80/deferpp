/** @file deferpp.h
    @brief Deferpp (Defer++) is Go-like DEFER construction for C++11
    
    Single header. No installation required, no build needed.
    Copy this whole file to any location you like :)
    Created by Pavlo M, https://github.com/olvap80
    
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
    
    This is quick alternative for RAII: to ensure cleanup code is called
    when scope is exited, so that one does not need to write RAII wrapper
    class for every kind of resource.
    See also sample test fragment (demo) at the bottom of this file.


Copyright (c) 2015-2018, Pavlo M, https://github.com/olvap80
All rights reserved.

Redistribution and use in source and binary forms, with or without
modification, are permitted provided that the following conditions are met:

* Redistributions of source code must retain the above copyright notice, this
  list of conditions and the following disclaimer.

* Redistributions in binary form must reproduce the above copyright notice,
  this list of conditions and the following disclaimer in the documentation
  and/or other materials provided with the distribution.

* Neither the name of deferpp nor the names of its
  contributors may be used to endorse or promote products derived from
  this software without specific prior written permission.

THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE
FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER
CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,
OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*/
#ifndef DEFERPP_H_ONCE
#define DEFERPP_H_ONCE

#include <utility>


///Defer following code until enclosing scope is exited
/** Usage: DEFER{ some_code_to_be_deferred }; 
    Remember that some_code_to_be_deferred shall not allow
    exceptions to be propagated out of curly braces */
#define DEFER\
        auto DEFER_CAT_ID(callOnScopeExit,__LINE__) \
          = (Defer_SupportNamespace::tagClassForLambda) ->* [&]()


//==============================================================================
//Implementation details follow
          

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


//==============================================================================
//Demo (sample test fragment)

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
#endif /*DEFERPP_H_ONCE*/
