#ifndef _INTORPOINTER_HPP
#define _INTORPOINTER_HPP

#include "myassert.h"

#ifdef TEST
#include "test.hpp"
#endif

template <class Pointed>
struct IntOrPointer {
    typedef Pointed *value_type;
    typedef IntOrPointer<Pointed> Self;
    explicit IntOrPointer(unsigned j) { *this=j; }
    IntOrPointer(value_type v) { *this=v; }
    union {
        value_type p; // must be even (guaranteed unless you're pointing at packed chars)
        unsigned i; // stored as 2*data+1, so only ranges [0,INT_MAX]
    };
    bool is_integer() const { return p & 1; }
    bool is_pointer() const { return !is_integer(); }
    value_type & pointer() { return p; }
    const value_type & pointer() const { Assert(is_pointer()); return p; }
    unsigned integer() const { Assert(is_integer()); return i >> 1; }
    void operator=(unsigned j) { i = 2*j+1; }
    void operator=(value_type v) { p=v; }

    IntOrPointer(const Self &s) : p(s.p) {}
    void operator=(const Self &s) { p=s.p; }
    template <class C>
    bool operator ==(const C* v) { return p==v; }
    template <class C>
    bool operator ==(C j) { return integer() == j; }
    bool operator ==(Self s) { return p==s.p; }
    bool operator !=(Self s) { return p!=s.p; }
};

#ifdef TEST
BOOST_AUTO_UNIT_TEST( TEST_INTORPOINTER )
{
    int i=3;
    IntOrPointer<int> p(5);
    BOOST_CHECK(p.is_integer());
    BOOST_CHECK(!p.is_pointer());
    BOOST_CHECK(p.integer() == 5);
    p.set(0);
    BOOST_CHECK(p.is_integer());
    BOOST_CHECK(p.integer() == 0);
    BOOST_CHECK(p == 0);
    BOOST_CHECK(p == NULL);
    p.set(&i);
    BOOST_CHECK(p.is_pointer());
    BOOST_CHECK(p.pointer() == &i);
    BOOST_CHECK(p == &i);
}
#endif

#endif
