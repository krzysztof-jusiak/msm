**Boost Meta State Machine** 

### Features
 + actions/guards injection using ctor
 + dispatcher for non typed events

### Example
```cpp

class dependency { ... };

BOOST_MSM_EUML_ACTION(guard)
{
    BOOST_DI_CTOR(guard, boost::shared_ptr<dependency> d)
        : dependency_(d)
    { }

    template<typename Event>
    bool operator()(const Event&) {
        return dependency_.check();
    }

private:
    boost::shared_ptr<dependency> dependency_;
}; 

BOOST_MSM_EUML_ACTION(action)
{
    BOOST_DI_CTOR(guard, boost::shared_ptr<dependency> d)
        : dependency_(d)
    { }

    template<typename Event>
    void operator()(const Event&) {
        dependency_.set();
    }

private:
    boost::shared_ptr<dependency> dependency_;
}; 

struct player_ : msm::front::state_machine_def<player_>
{
    typedef Empty initial_state;

    BOOST_MSM_EUML_DECLARE_TRANSITION_TABLE((
        Empty() + play() [guard] / action == Stopped()
    ), transition_table)
};

typedef msm::back::state_machine<player_> player;

int main() {
    auto sm = di::injector<>().create<boost::shared_ptr<player>>();
    sm->process_event(play());
}

```

