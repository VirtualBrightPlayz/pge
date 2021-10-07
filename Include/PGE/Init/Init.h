#ifndef PGE_INIT_H_INCLUDED
#define PGE_INIT_H_INCLUDED

#include <vector>

#include <PGE/String/String.h>

namespace PGE {

namespace Init {
    void init();
    void quit();

#ifdef WIN32
#undef main
#endif
    int main(const std::vector<String>& args);
}

}

#endif // PGE_INIT_H_INCLUDED
