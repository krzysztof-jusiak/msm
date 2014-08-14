// Copyright 2008 Christophe Henry
// henry UNDERSCORE christophe AT hotmail DOT com
// This is an extended version of the state machine available in the boost::mpl library
// Distributed under the same license as the original.
// Copyright for the original version:
// Copyright 2005 David Abrahams and Aleksey Gurtovoy. Distributed
// under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)

#ifndef BOOST_MSM_TEST_EXPECTATIONS_HPP
#define BOOST_MSM_TEST_EXPECTATIONS_HPP

#include <boost/mpl/for_each.hpp>

namespace boost { namespace msm { namespace test {

template<typename FSM>
class expectations
{
    struct verify_and_reset {
        verify_and_reset(FSM& fsm)
            : fsm(fsm)
        { }

        template<typename T>
        void operator()(const T&) {
            fsm.template get<typename T::type>().verify();
            fsm.template get<typename T::type>().reset();
        }

    private:
        FSM& fsm;
    };

public:
    explicit expectations(FSM& fsm)
        : fsm(fsm)
        , state_(0)
    { }

    ~expectations() {
        assert(state_ == fsm.template get_current_state());
        mpl::for_each<typename FSM::actions::types>(verify_and_reset(fsm));
    }

    template<typename T>
    typename FSM::template get_impl<T, typename FSM::DIPolicy>::result_type& call() {
        return fsm.template get<T>();
    }

    template<typename T>
    void state() {
        state_ = fsm.template get_state<T>();
    }

private:
    int state_;
    FSM& fsm;
};

}}} // boost::msm::test

#endif

