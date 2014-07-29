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

namespace front = boost::msm::front;
namespace euml  = boost::msm::front::euml;
namespace msm   = boost::msm;
namespace mpl   = boost::mpl;
namespace di    = boost::di;

struct event1 : euml::euml_event<event1> { };
struct event2 : euml::euml_event<event2> { };

class data {
public:
    data()
        : flag(false)
    { }

    void set_flag(bool flag) { this->flag = flag; }
    bool get_flag() const { return flag; }

private:
    bool flag;
};

struct status_fake {
    status_fake()
        : action_calls(0)
        , guard_calls(0)
    { }

    int action_calls;
    int guard_calls;
};

class guard : public euml::euml_action<guard> {
public:
    guard() { } // for euml

    guard(boost::shared_ptr<data> data, boost::shared_ptr<status_fake> status_fake)
        : data(data), status_fake(status_fake)
    { }

    template<typename TEvent>
    bool operator()(const TEvent&) const {
        ++status_fake->guard_calls;
        return data->get_flag();
    }

private:
    boost::shared_ptr<data> data;
    boost::shared_ptr<status_fake> status_fake;
};

typedef euml::bind_guard<euml_call(&data::get_flag), mpl::_1> bind_guard;

class action : public euml::euml_action<action>
{
public:
    action() { } // for euml

    action(boost::shared_ptr<data> data, boost::shared_ptr<status_fake> status_fake)
        : data(data), status_fake(status_fake)
    { }

    template<typename TEvent>
    void operator()(const TEvent&) const {
        ++status_fake->action_calls;
    }

private:
    boost::shared_ptr<data> data;
    boost::shared_ptr<status_fake> status_fake;
};

class sm : public front::state_machine_def<sm> {
    struct state1 : front::state<>, euml::euml_state<state1> { };
    struct state2 : front::state<>, euml::euml_state<state2> { };
    struct state3 : front::state<>, euml::euml_state<state3> { };

public:
    typedef mpl::vector<state1> initial_state;

    BOOST_MSM_EUML_DECLARE_TRANSITION_TABLE((
    // +---------------------------------------------------------+
        state2() == state1() + event1() [guard()] / action()
      , state1() == state2() + event2()
    // +---------------------------------------------------------+
    ), transition_table)
};

typedef msm::back::state_machine<sm, msm::back::use_dependency_injection> sm_t;

int main() {
    boost::shared_ptr<data> data_(new data());
    boost::shared_ptr<status_fake> status_fake_(new status_fake());

    BOOST_AUTO(injector, (
        di::make_injector(
            di::bind<data>::to(data_)
          , di::bind<status_fake>::to(status_fake_)
        )
    ));

    sm_t sm = injector.create<sm_t>();

    {
        data_->set_flag(false);
        sm.process_event(event1());
        assert(1 == status_fake_->guard_calls);
        assert(0 == status_fake_->action_calls);
    }

    {
        data_->set_flag(true);
        sm.process_event(event1());
        assert(2 == status_fake_->guard_calls);
        assert(1 == status_fake_->action_calls);
    }

    {
        data_->set_flag(false);
        sm.process_event(event2());
        assert(2 == status_fake_->guard_calls);
        assert(1 == status_fake_->action_calls);
    }

    return 0;
}

