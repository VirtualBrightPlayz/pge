#ifndef PGE_RESOURCEREFERENCE_H_INCLUDED
#define PGE_RESOURCEREFERENCE_H_INCLUDED

#include <Exception/Exception.h>

namespace PGE {

template <class T>
class ResourceReference {
    private:
        T internalResource;
        bool holdsResource = false;
    public:
        ResourceReference() { }
        ResourceReference(T res) { internalResource = res; holdsResource = true; }

        operator const T& () const { __ASSERT(holdsResource, "Reference not filled"); return internalResource; }
        // Force cast.
        const T& operator()() const { __ASSERT(holdsResource, "Reference not filled"); return internalResource; }
        const T& operator->() const { __ASSERT(holdsResource, "Reference not filled"); return internalResource; }
        const T* operator&() const { __ASSERT(holdsResource, "Reference not filled"); return &internalResource; }

        bool isHoldingResource() const { return holdsResource; }
        void invalidate() { holdsResource = false; }
};

}

#endif // PGE_RESOURCEREFERENCE_H_INCLUDED
