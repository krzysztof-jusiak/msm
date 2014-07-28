**Boost Meta State Machine with Dependency Injection** allows to injection actions/guards into the state machine using dependency injection framework.

**Hello World**
```cpp
#include <boost/msm/back/state_machine.hpp>
#include <boost/msm/front/state_machine_def.hpp>
#include <boost/msm/front/euml/euml.hpp>
#include <boost/di.hpp>

namespace msm   = boost::msm;
namespace front = boost::msm::front;
namespace euml  = boost::msm::front::euml;
namespace di    = boost::di;

struct event : euml_event<event> { };

struct data {
    int value = 0;
};

class guard : public euml_action<guard> {
public:
    guard() { } // for euml

    explicit guard(shared_ptr<data> data_)
        : data_(data_)
    { }

    bool operator()(const event&) const {
        return dep->value != 0;
    }

private:
    shared_ptr<data> data_;
};

class action : public euml_action<action> {
public:
    action() { } // for euml

    explicit action(shared_ptr<data> data_)
        : data_(data_)
    { }

    void operator()(const event&) {
        data_->value++;
    }

private:
    shared_ptr<data> data_;
};

class state_machine_ : state_machine_def<state_machine_> {
    struct state1 : state<>, euml_state<state1> { };
    struct state2 : state<>, euml_state<state2> { };

public:
    using initial_state = state1;

    BOOST_MSM_EUML_DECLARE_TRANSITION_TABLE((
        state1() + event() [guard] / action == state2()
    ), transition_table)
};

using state_machine = msm::back::state_machine<
    state_machine_
  , msm::back::use_dependency_injection
>;

int main() {
    auto sm = di::make_injector().create<state_machine>();
    sm.process_event(event());

    return 0;
}
```

### License
Distributed under the [Boost Software License, Version 1.0](http://www.boost.org/LICENSE_1_0.txt).

