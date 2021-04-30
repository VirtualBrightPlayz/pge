#ifndef PGE_EXCEPTION_H_INCLUDED
#define PGE_EXCEPTION_H_INCLUDED

#include <Misc/String.h>

namespace PGE {

class Exception {
    public:
        // Invalid exception.
        Exception();

        // Never use this directly.
        Exception(const char* file, int line, const String& extra);

        const String& what() const;

    private:
        String info;
};

}

#define __CREATE_EX(INFO) PGE::Exception(__FILE__, __LINE__, INFO)

#define __ASSERT(COND, INFO) if (!(COND)) throw __CREATE_EX(INFO)

#endif // PGE_EXCEPTION_H_INCLUDED
