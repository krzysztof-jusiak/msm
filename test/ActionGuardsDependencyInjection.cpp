#include <iostream>
#include <boost/mpl/vector.hpp>
#include <boost/di.hpp>
#include <boost/shared_ptr.hpp>
#include <boost/msm/back/state_machine.hpp>
#include <boost/msm/front/state_machine_def.hpp>
#include <boost/msm/front/euml/euml.hpp>
#include <boost/units/detail/utility.hpp>

namespace euml = boost::msm::front::euml;
namespace msm  = boost::msm;
namespace mpl  = boost::mpl;
namespace di   = boost::di;

struct play : msm::front::euml::euml_event<play> {};

struct Empty : msm::front::state<>, msm::front::euml::euml_state<Empty>
{ };

struct Stopped : msm::front::state<>, msm::front::euml::euml_state<Stopped>
{ };

template<int V>
struct action : public euml::euml_action<action<V>>
{
public:
    BOOST_DI_CTOR(action, int i = 0)
        : i(i)
    { }

    template <class EVT>
    void operator()(EVT const&)
    {
        std::cout << "action: " << i << std::endl;
    }

    int i = 0;
};

template<int V>
struct guard : public euml::euml_action<guard<V>>
{
public:
    BOOST_DI_CTOR(guard, int i = 0)
        : i(i)
    { }

    template <class EVT>
    bool operator()(EVT const&)
    {
        std::cout << "guard: " << i << std::endl;
        return true;
    }

    int i = 0;
};

struct player_ : public msm::front::state_machine_def<player_>
{
    typedef Empty initial_state;

    BOOST_MSM_EUML_DECLARE_TRANSITION_TABLE((
        Empty() + play() [guard<1>()] / action<1>() == Stopped()
      , Empty() + play() [guard<1>() and  guard<2>()] / (action<1>(), action<2>(), action<3>()) == Stopped()
    ), transition_table)
};

typedef msm::back::state_machine<player_> player;

int main(int argc, const char *argv[])
{
    std::cout << boost::units::detail::demangle(typeid(player::actions_t::type).name()) << std::endl;

    auto sm = di::injector<
        di::generic_module<
            di::bind<int, mpl::int_<11>>
        >
    >().create<boost::shared_ptr<player>>();
    sm->process_event(play());

    return 0;
}

