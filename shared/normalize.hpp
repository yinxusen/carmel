#ifndef _NORMALIZE_HPP
#define _NORMALIZE_HPP

#include "dynarray.h"

#include "pointeroffset.hpp"

#include "container.hpp"
#include "byref.hpp"
#include "genio.h"

//FIXME: leave rules that don't occur in normalization groups alone (use some original/default value)
template <class W>
struct NormalizeGroups {
    typedef W weight_type;
    typedef PointerOffset<W> value_type; // pointer offsets
    typedef FixedArray<value_type> Inner;
    typedef FixedArray<Inner> Outer;
    Outer norm_groups;
//    value_type max_offset;
    DynamicArray<bool> seen_index;
//    GENIO_get_from {
//        IndirectReader<IndexToOffsetReader<W> > reader;
//        norm_groups.get_from(in,reader);
//    }
    struct seen_offset_p {
        DynamicArray<bool> *seen_index;
        void operator()(value_type v) {
            (*seen_index)(v.get_index())=true; // default init of false when expanded
        }
        seen_offset_p(DynamicArray<bool> *a) : seen_index(a) {}
        seen_offset_p(const seen_offset_p &o) : seen_index(o.seen_index) {}
    };
    struct max_p {
        value_type max; // default init = 0
        void operator()(value_type v) {
            if (max < v)
                max = v;
        }
    };
    size_t max_index() {
        nested_enumerate(norm_groups,seen_offset_p(&seen_index));
        return seen_index.size();
//        return max_offset.get_index();
/*        max_p m;
        nested_enumerate(norm_groups,ref(m));
        return m.max.get_index();*/
    }
    size_t required_size() {
        return max_index()+1;
    }
    W *base;
    void operator ()(Inner &i) {
        typedef typename Inner::iterator It;
        It end=i.end(), beg=i.begin();
        weight_type sum=0;
        for (It j=beg;j!=end;++j) {
            weight_type &w=*(j->add_base(base));
            sum+=w;
        }
        if (sum > 0)
            for (It j=beg;j!=end;++j) {
                weight_type &w=*(j->add_base(base));
                w /= sum;
            }
    }
    void copy_unseen(W *src, W *to) {
        for (unsigned i=0,e=seen_index.size();i!=e;++i) {
            if (!seen_index[i])
                to[i]=src[i];
        }
    }
    void set_unseen_to(W src, W *to) {
        for (unsigned i=0,e=seen_index.size();i!=e;++i) {
            if (!seen_index[i])
                to[i]=src;
        }
    }
    /*
    template <class F>
    void enumerate_seen(F f) {
        for (unsigned i=0,e=seen_index.size();i!=e;++i) {
            if (seen_index[i])
                f(i);
    }
    template <class F>
    void enumerate_unseen(F f) {
        for (unsigned i=0,e=seen_index.size();i!=e;++i) {
            if (!seen_index[i])
                f(i);
    }
    */
    // array must have values for all max_index()+1 rules
    void normalize(W *array_base) {
        base=array_base;
        enumerate(norm_groups,ref(*this));
    }
};

template <class charT, class Traits,class C>
std::basic_istream<charT,Traits>&
operator >>
  (std::basic_istream<charT,Traits>& is, NormalizeGroups<C> &arg)
{
    return is >> arg.norm_groups;
//    return gen_extractor(is,arg);
}


template <class charT, class Traits,class C>
std::basic_ostream<charT,Traits>&
operator <<
  (std::basic_ostream<charT,Traits>& os, const NormalizeGroups<C> &arg)
{
    return os << arg.norm_groups;
}

#ifdef TEST
#include "test.hpp"
#endif

#ifdef TEST
BOOST_AUTO_UNIT_TEST( TEST_NORMALIZE )
{
    typedef Weight W;
    FixedArray<W> w(4u);
    w[0]=1;
    w[1]=2;
    w[2]=3;
    w[3]=4;
    NormalizeGroups<W> ng;
    string s="((1) (2 3))";
    istringstream is(s);
    BOOST_CHECK(is >> ng);
    BOOST_CHECK(ng.max_index() == 3);
//    cerr << Weight::out_always_real;
//    cout << Weight::out_variable;
//    DBP(w);
//    DBP(ng);
    ng.normalize(w.begin());
//    BOOST_CHECK_CLOSE(w[2].getReal()+w[3].getReal(),1,1e-6);
//    BOOST_CHECK_CLOSE(w[2].getReal()*4,w[3].getReal()*3,1e-6);

//    DBP(w);
}
#endif

#endif
