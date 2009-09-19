#ifndef GRAEHL_SHARED__SPLIT_HPP
#define GRAEHL_SHARED__SPLIT_HPP

#include <string>
#include <vector>
#include <graehl/shared/string_to.hpp>
#ifdef TEST
#include <graehl/shared/test.hpp>
#include <cstring>
#endif


namespace graehl {

template <class Cont>
struct split_push_back
{
    typedef typename Cont::value_type value_type;
    Cont *c;
    split_push_back(Cont &cr) : c(&cr) {}
    template <class Str>
    bool operator()(Str const& s)
    {
        c->push_back(string_to<value_type>(s));
        return true;
    }
};

template <class Cont>
struct split_string_push_back
{
    Cont *c;
    split_string_push_back(Cont &cr) : c(&cr) {}
    template <class Str>
    bool operator()(Str const& s)
    {
        c->push_back(s);
        return true;
    }
};

template <class Func>
inline void split_noquote(
    const std::string &csv,
    Func f,
    const std::string &delim=","
    )
{
    using namespace std;
    string::size_type pos=0,nextpos;
    string::size_type delim_len=delim.length();
    if (delim_len==0) delim_len=1;
    while((nextpos=csv.find(delim,pos)) != string::npos) {
        if (! f(string(csv,pos,nextpos-pos)) )
            return;
        pos=nextpos+delim_len;
    }
    if (csv.length()!=0)
        f(string(csv,pos,csv.length()-pos));
}

template <class Cont>
inline void split_into(
    std::string const& str,
    Cont &c,
    std::string const& delim=","
    )
{
    split_noquote(str,split_push_back<Cont>(c),delim);
}

template <class Cont>
inline Cont split_string(
    std::string const& str,
    std::string const& delim=","
    )
{
    Cont c;
    split_noquote(str,split_string_push_back<Cont>(c),delim);
    return c;
}

inline std::vector<std::string> split(
    std::string const& str,
    std::string const& delim=","
    )
{
    return split_string<std::vector<std::string> >(str,delim);
}


#ifdef TEST
char const* split_strs[]={"",",a","",0};
char const* seps[]={";",";;",",,","   ","=,",",=",0};

BOOST_AUTO_TEST_CASE( TEST_io )
{
    using namespace std;
    {
        split_noquote(";,a;",make_expect_visitor(split_strs),";");
        for (char const **p=seps;*p;++p) {
            string s;
            char const* sep=*p;
            for (char const **q=split_strs;*q;++q) {
                s.append(*q);
                if (q[1])
                    s.append(sep);
            }
            cout << sep << "\t"<< s << endl;
            split_noquote(s,make_expect_visitor(split_strs),sep);
        }
    }

}
#endif



}


#endif
