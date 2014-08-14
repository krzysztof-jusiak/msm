// Copyright 2010 Christophe Henry
// henry UNDERSCORE christophe AT hotmail DOT com
// This is an extended version of the state machine available in the boost::mpl library
// Distributed under the same license as the original.
// Copyright for the original version:
// Copyright 2005 David Abrahams and Aleksey Gurtovoy. Distributed
// under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
#include <boost/shared_ptr.hpp>
#include <boost/msm/back/state_machine.hpp>
#include <boost/msm/front/state_machine_def.hpp>
#include <boost/msm/front/euml/euml.hpp>
#include <boost/di.hpp>

#include <iostream>
#include <boost/units/detail/utility.hpp>
#include <boost/msm/test/expectations.hpp>

namespace front = boost::msm::front;
namespace euml  = boost::msm::front::euml;
namespace test  = boost::msm::test;
namespace msm   = boost::msm;
namespace mpl   = boost::mpl;
namespace di    = boost::di;

struct event1 : euml::euml_event<event1> { };
struct event2 : euml::euml_event<event2> { };

class guard : public euml::euml_action<guard> { };
class guard2 : public euml::euml_action<guard2> { };
class action : public euml::euml_action<action> { };
class action2 : public euml::euml_action<action2> { };

struct state1 : front::state<>, euml::euml_state<state1> { };
struct state2 : front::state<>, euml::euml_state<state2> { };
struct state3 : front::state<>, euml::euml_state<state3> { };

class sm : public front::state_machine_def<sm> {
public:
    typedef mpl::vector<state1> initial_state;

    BOOST_MSM_EUML_DECLARE_TRANSITION_TABLE((
    // +---------------------------------------------------------+
        state2() == state1() + event1() [guard()] / action()
      , state1() == state2() + event2() [guard() && guard2()] / (action(), action2())
    // +---------------------------------------------------------+
    ), transition_table)
};

typedef msm::back::state_machine<sm, msm::back::use_dependency_injection_for_test> sm_t;

int main() {
    sm_t sm;

    {
        test::expectations<sm_t> ex(sm);
        ex.call<guard>().will_return(false);
        ex.call<action>().times(0);
        ex.state<state1>();

        sm.process_event(event1());
    }

    {
        test::expectations<sm_t> ex(sm);
        ex.call<guard>().will_return(true);
        ex.call<action>().times(1);
        ex.state<state2>();

        sm.process_event(event1());
    }

    {
        test::expectations<sm_t> ex(sm);
        ex.call<guard>().will_return(false);
        ex.call<guard2>().will_return(false);
        ex.call<action>().times(0);
        ex.call<action2>().times(0);
        ex.state<state2>();

        sm.process_event(event2());
    }

    {
        test::expectations<sm_t> ex(sm);
        ex.call<guard>().will_return(true);
        ex.call<guard2>().will_return(false);
        ex.call<action>().times(0);
        ex.call<action2>().times(0);
        ex.state<state2>();

        sm.process_event(event2());
    }

    {
        test::expectations<sm_t> ex(sm);
        ex.call<guard>().will_return(false);
        ex.call<guard2>().will_return(true);
        ex.call<action>().times(0);
        ex.call<action2>().times(0);
        ex.state<state2>();

        sm.process_event(event2());
    }

    {
        test::expectations<sm_t> ex(sm);
        ex.call<guard>().will_return(true);
        ex.call<guard2>().will_return(true);
        ex.call<action>().times(1);
        ex.call<action2>().times(1);
        ex.state<state1>();

        sm.process_event(event2());
    }

    return 0;
}

