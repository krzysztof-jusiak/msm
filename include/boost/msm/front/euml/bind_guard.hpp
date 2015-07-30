#ifndef BIND_GUARD_HTQROD28
#define BIND_GUARD_HTQROD28

#include <boost/shared_ptr.hpp>
#include <boost/type_traits/remove_reference.hpp>
#include <boost/function_types/components.hpp>
#include <boost/mpl/at.hpp>
#include <boost/mpl/placeholders.hpp>
#include <boost/msm/front/euml/common.hpp>
#include <boost/di.hpp>

#define euml_call(func) BOOST_TYPEOF(func), func

namespace boost { namespace msm { namespace front { namespace euml {

template<typename X, X func, typename = void>
class bind_guard;

template<typename X, X func>
class bind_guard<X, func> : public euml_action<bind_guard<X, func> >
{
    typedef typename function_types::components<X>::type t;
    typedef typename boost::remove_reference<typename mpl::at_c<t, 1>::type>::type B;

public:
    bind_guard() { }

    BOOST_DI_INJECT(bind_guard, shared_ptr<B> b)
        : b(b)
    { }

    template<typename T>
    bool operator()(const T&) const {
        return (*b.*func)();
    }

private:
    shared_ptr<B> b;
};

template<typename X, X func>
class bind_guard<X, func, mpl::_1> : public euml_action<bind_guard<X, func, mpl::_1> >
{
    typedef typename function_types::components<X>::type t;
    typedef typename boost::remove_reference<typename mpl::at_c<t, 1>::type>::type B;

public:
    bind_guard() { }

    BOOST_DI_INJECT(bind_guard, shared_ptr<B> b)
        : b(b)
    { }

    template<typename TEvent>
    bool operator()(const TEvent& event) const {
        return (*b.*func)(event);
    }

private:
    shared_ptr<B> b;
};

}}}} // boost::msm::front::euml

#endif

