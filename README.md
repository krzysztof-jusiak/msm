**Boost Meta State Machine fork**

### Features
 + actions/guards injection using ctor
 + dispatcher for non typed events

### Injection: basic example
```cpp

struct event : euml_event<event> { };

class guard : public euml_action<guard>
{
    explicit guard(shared_ptr<int> dep)
        : dep(dep)
    { }

    bool operator()(const event&) const {
        return *dep != 0;
    }

private:
    shared_ptr<int> dep;
};

class action : public euml_action<action>
{
    explicit action(shared_ptr<int> dep)
        : dep(dep)
    { }

    void operator()(const event&) {
        (*dep)++;
    }

private:
    shared_ptr<int> dep;
};

class state_machine_ : state_machine_def<state_machine_>
{
    struct state1 : state<>, euml_state<state1> { };
    struct state2 : state<>, euml_state<state2> { };

public:
    typedef state1 initial_state;

    BOOST_MSM_EUML_DECLARE_TRANSITION_TABLE((
        state1() + event() [guard] / action == state2()
    ), transition_table)
};

int main() {
    auto dep_ = make_shared<int>(0);

    {
        state_machine<state_machine_> sm_(pool(action(dep_), guard(dep_)));
        sm_.process_event(event());
    }

    {
        state_machine<state_machine_> sm_(pool(guard(dep_), action(dep_)));
        sm_.process_event(event());
    }

    return 0;
}

```

### Injection: integration with dependency injection framework example
```cpp

struct event : euml_event<event> { };

class guard : public euml_action<guard>
{
    BOOST_DI_CTOR(explicit guard, shared_ptr<int> dep)
        : dep(dep)
    { }

    bool operator()(const event&) const {
        return *dep != 0;
    }

private:
    shared_ptr<int> dep;
};

class action : public euml_action<action>
{
    BOOST_DI_CTOR(explicit action, shared_ptr<int> dep)
        : dep(dep)
    { }

    void operator()(const event&) {
        (*dep)++;
    }

private:
    shared_ptr<int> dep;
};

class state_machine_ : state_machine_def<state_machine_>
{
    struct state1 : state<>, euml_state<state1> { };
    struct state2 : state<>, euml_state<state2> { };

public:
    typedef state1 initial_state;

    BOOST_MSM_EUML_DECLARE_TRANSITION_TABLE((
        state1() + event() [guard] / action == state2()
    ), transition_table)
};

int main() {
    auto sm_ = injector<>().create<state_machine<state_machine_>>();
    sm_.process_event(event());

    return 0;
}
```
