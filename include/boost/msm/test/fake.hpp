// Copyright 2008 Christophe Henry
// henry UNDERSCORE christophe AT hotmail DOT com
// This is an extended version of the state machine available in the boost::mpl library
// Distributed under the same license as the original.
// Copyright for the original version:
// Copyright 2005 David Abrahams and Aleksey Gurtovoy. Distributed
// under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)

#ifndef BOOST_MSM_TEST_FAKE_HPP
#define BOOST_MSM_TEST_FAKE_HPP

namespace boost { namespace msm { namespace test {

template<typename T>
class fake {
public:
    typedef T type;

    fake()
        : calls(0)
        , return_(false)
        , times_(-1)
    { }

    template<typename TEvent>
    bool operator()(const TEvent&) {
        ++calls;
        return return_;
    }

    fake& will_return(bool value) {
        return_ = value;
        return *this;
    }

    void times(int value) {
        times_ = value;
    }

    void reset() {
        calls = 0;
        return_ = false;
        times_ = -1;
    }

    void verify() {
        if (times_ > -1) {
            assert(times_ == calls);
        }
    }

    ////

    int calls;
    bool return_;
    int times_;
};

}}} // boost::msm::test

#endif

