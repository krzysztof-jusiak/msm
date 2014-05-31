#include <iostream>
#include <boost/mpl/vector.hpp>
#include <boost/shared_ptr.hpp>
#include <boost/msm/back/state_machine.hpp>
#include <boost/msm/front/state_machine_def.hpp>
#include <boost/msm/front/euml/euml.hpp>
#include <boost/units/detail/utility.hpp>
#include <boost/none_t.hpp>
#include <boost/di.hpp>

namespace euml = boost::msm::front::euml;
namespace front = boost::msm::front;
namespace msm  = boost::msm;
namespace mpl  = boost::mpl;
namespace di   = boost::di;

const int SDLK_ESCAPE = 0;

class flag_game_over { };

typedef euml::True_ anonymous;
struct button_clicked : euml::euml_event<button_clicked>
{
    operator int() const {
        return 42;
    }
};
struct key_pressed : euml::euml_event<key_pressed> { };
struct window_close : euml::euml_event<window_close> { };
struct time_tick : euml::euml_event<time_tick> { };

class board
{
public:
    bool check() {
        std::cout << "check" << std::endl;
        return false;
    }

    bool check1(int) const {
        return false;
    }
};

template<typename T>
class action : public euml::euml_action<T>
{
public:
    action() { }

    action(boost::shared_ptr<board> b, boost::none_t)
        : board_(b)
    {
        std::cout << typeid(T).name() << ": "  << (b.get()) << std::endl;
    }

protected:
    boost::shared_ptr<board> board_;
};

class init_board : public euml::euml_action<init_board>
{
public:
    template<typename Event>
    void operator()(const Event&) const {
        std::cout << "init board" << std::endl;
    }
};

class select_item : public euml::euml_action<select_item>
{
public:
    void operator()(const button_clicked& button) const {
    }
};

class unselect_all : public euml::euml_action<unselect_all>
{
public:
    template<typename Event>
    void operator()(const Event&) const {
    }
};

class swap_items : public euml::euml_action<swap_items>
{
public:
    template<typename Event>
    void operator()(const Event&) const {
    }
};

class revert_swap_items : public euml::euml_action<revert_swap_items>
{
public:
    template<typename Event>
    void operator()(const Event&) const {
    }
};

class show_matches : public euml::euml_action<show_matches>
{
public:
    template<typename Event>
    void operator()(const Event&) const {
    }
};

class scroll_board : public euml::euml_action<scroll_board>
{
public:
    template<typename Event>
    void operator()(const Event&) const {
    }
};

class show_time : public euml::euml_action<show_time>
{
public:
    template<typename Event>
    void operator()(const Event&) const {
    }
};

template<int Value>
class add_points : public euml::euml_action<add_points<Value>>
{
public:
    template<typename Event>
    void operator()(const Event&) const {
    }
};

template<int Value>
class sub_points : public euml::euml_action<sub_points<Value>>
{
public:
    template<typename Event>
    void operator()(const Event&) const {
    }
};

class show_points : public euml::euml_action<show_points>
{
public:
    template<typename Event>
    void operator()(const Event&) const {
    }
};

class show_results : public euml::euml_action<show_results>
{
public:
    template<typename Event>
    void operator()(const Event&) const {
    }
};

class finish_game : public euml::euml_action<finish_game>
{
public:
    template<typename Event>
    void operator()(const Event&) const {
    }
};

using is_within_board = euml::bind_guard<euml_call(&board::check1), mpl::_1>;

class is_neighbor : public action<is_neighbor>
{
public:
    using action::action;

    bool operator()(const button_clicked&) const {
        return true;
    }
};

struct is_same_item : action<is_same_item>
{
    using action::action;

    bool operator()(const button_clicked&) const {
        return true;
    }
};

class is_same_color : public euml::euml_action<is_same_color>
{
public:
    bool operator()(const button_clicked& button) const {
        return true;
    }
};

class is_swap_items_winning : public euml::euml_action<is_swap_items_winning>
{
public:
    template<typename Event>
    bool operator()(const Event&) const {
        return true;
    }
};

class is_game_timeout : public euml::euml_action<is_game_timeout>
{
public:
    bool operator()(const time_tick&) const {
        return true;
    }
};

template<int Key>
class is_key : public euml::euml_action<is_key<Key>>
{
public:
    bool operator()(const key_pressed&) const {
        return true;
    }
};

class controller : public front::state_machine_def<controller>
{
    struct idle                 : front::state<>, euml::euml_state<idle> { };
    struct wait_for_first_item  : front::state<>, euml::euml_state<wait_for_first_item> { };
    struct wait_for_second_item : front::state<>, euml::euml_state<wait_for_second_item> { };
    struct wait_for_client      : front::state<>, euml::euml_state<wait_for_client> { };
    struct let_swap_items       : front::state<>, euml::euml_state<let_swap_items> { };
    struct try_swap_items       : front::state<>, euml::euml_state<try_swap_items> { };
    struct board_scrolling      : front::state<>, euml::euml_state<board_scrolling> { };
    struct game_over            : front::state<>, euml::euml_state<game_over>
    {
        typedef mpl::vector1<flag_game_over> flag_list;
    };
    struct wait_for_any_key     : front::interrupt_state<key_pressed>, euml::euml_state<wait_for_any_key> { };

public:
    typedef mpl::vector<idle, wait_for_client> initial_state;

    BOOST_MSM_EUML_DECLARE_TRANSITION_TABLE((
   // +------------------------------------------------------------------------------------------------------------------------------------------------------------------+
        wait_for_first_item()  == idle()                   [anonymous()] / (init_board(), show_time(), show_points())
      , wait_for_second_item() == wait_for_first_item()  + button_clicked() [is_within_board()] / select_item()
      , wait_for_first_item()  == wait_for_second_item() + button_clicked() [is_same_item()] / unselect_all()
      , try_swap_items()       == wait_for_second_item() + button_clicked() [is_within_board() and not is_same_color() and is_neighbor()] / (select_item(), swap_items())
      , wait_for_first_item()  == try_swap_items()         [is_swap_items_winning()] / (show_matches(), add_points<10>(), show_points(), unselect_all(), scroll_board())
      , wait_for_first_item()  == try_swap_items()         [not is_swap_items_winning()] / (revert_swap_items(), sub_points<5>(), show_points(), unselect_all())
   // +------------------------------------------------------------------------------------------------------------------------------------------------------------------+
      , wait_for_any_key()     == wait_for_client()      + time_tick() [is_game_timeout()] / show_results()
      ,                           wait_for_client()      + time_tick() [not is_game_timeout()] / show_time()
      , wait_for_any_key()     == wait_for_client()      + key_pressed() [is_key<SDLK_ESCAPE>()] / show_results()
      , wait_for_any_key()     == wait_for_client()      + window_close() / show_results()
      , game_over()            == wait_for_any_key()     + key_pressed() / finish_game()
   // +------------------------------------------------------------------------------------------------------------------------------------------------------------------+
    ), transition_table)
};

typedef msm::back::state_machine<controller> controller_t;

int main()
{
    auto sm = di::make_injector().create<boost::shared_ptr<controller_t>>();
    sm->start();
    sm->process_event(button_clicked());

    return 0;
}

