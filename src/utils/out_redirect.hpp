#ifndef _RIVEEXTENSION_OUT_REDIRECT_
#define _RIVEEXTENSION_OUT_REDIRECT_

#include <iostream>
#include <sstream>
#include <string>

struct CoutRedirect {
    CoutRedirect(std::streambuf *new_buffer)
        : old(std::cout.rdbuf(new_buffer)) {}

    ~CoutRedirect() {
        std::cout.rdbuf(old);
    }

   private:
    std::streambuf *old;
};

struct CerrRedirect {
    CerrRedirect() : old(std::cerr.rdbuf(output.rdbuf())) {}

    ~CerrRedirect() {
        std::cerr.rdbuf(old);
    }

   public:
    std::ostringstream output;

    std::string str() {
        return output.str();
    }

   private:
    std::streambuf *old;
};

#endif