**Boost Meta State Machine with Dependency Injection** allows to injection actions/guards into the state machine using dependency injection framework.

**Hello World**
```cpp
#include <memory>
#include <boost/msm/back/state_machine.hpp>
#include <boost/msm/front/state_machine_def.hpp>
#include <boost/msm/front/euml/euml.hpp>
#include <boost/di.hpp>

using namespace boost::msm;;
using namespace boost::msm::front;;
using namespace boost::msm::front::euml;;
using namespace std;

struct event : euml_event<event> { };

struct data {
    bool flag = false;
};

class guard : public euml_action<guard> {
public:
    explicit guard(shared_ptr<data> data = nullptr) // default for eUML
        : data(data)
    { }

    bool operator()(const event&) const {
        return data->flag;
    }

private:
    shared_ptr<data> data;
};

class action : public euml_action<action> {
public:
    explicit action(shared_ptr<data> data = nullptr) // default for eUML
        : data(data)
    { }

    void operator()(const event&) {
        data->flag = true;
    }

private:
    shared_ptr<data> data;
};

struct state1 : state<>, euml_state<state1> { };
struct state2 : state<>, euml_state<state2> { };

struct state_machine_ : public state_machine_def<state_machine_> {
    using initial_state = state1;

    BOOST_MSM_EUML_DECLARE_TRANSITION_TABLE((
        state1() + event() / action()
      , state1() + event() [guard()] == state2()
    ), transition_table)
};

using state_machine = back::state_machine<
    state_machine_
  , back::use_dependency_injection // dependency injection policy
>;

int main() {
    // create state machine and inject dependencies
    auto sm = boost::di::make_injector().create<state_machine>();

    // or create state machine manualy
    // auto d = make_shared<data>();
    // state_machine sm(back::pool(guard(d), action(d)));

    sm.process_event(event());
    assert(0 == *sm.current_state());

    sm.process_event(event());
    assert(1 == *sm.current_state());

    return 0;
}
```

**State Machine Testing**
```cpp
struct state_machine_ : public state_machine_def<state_machine_> {
    using initial_state = state1;

    BOOST_MSM_EUML_DECLARE_TRANSITION_TABLE((
        state2() == state1() + event1() [guard()] / action()
      , state1() == state2() + event2() [guard() && guard2()] / (action(), action2())
    ), transition_table)
};

using state_machine = back::state_machine<
    state_machine_
  , back::use_dependency_injection_for_test // dependency injection policy for test
>;

int main() {
    //create state machine
    //fake actions/guards have no dependencies so default constructor is enough
    state_machine sm;

    // test transition to state2 after event1 when
    // guard returns true with action
    {
        test::expectations<sm_t> ex(sm);
        ex.call<guard>().will_return(true);
        ex.call<action>().times(1);
        ex.state<state2>();

        sm.process_event(event1());
    }

    // test transition back to state1 after event2 when
    // guard and guard2 return true with action and action2
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
```

### License
Distributed under the [Boost Software License, Version 1.0](http://www.boost.org/LICENSE_1_0.txt).

