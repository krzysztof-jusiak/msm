// Copyright 2008 Christophe Henry
// henry UNDERSCORE christophe AT hotmail DOT com
// This is an extended version of the state machine available in the boost::mpl library
// Distributed under the same license as the original.
// Copyright for the original version:
// Copyright 2005 David Abrahams and Aleksey Gurtovoy. Distributed
// under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)

#ifndef BOOST_MSM_BACK_DEFAULT_DI_POLICY_HPP
#define BOOST_MSM_BACK_DEFAULT_DI_POLICY_HPP

#include <boost/mpl/bool.hpp>

namespace boost { namespace msm { namespace back
{
struct use_dependency_injection
{
    typedef int di_policy;
};

struct use_dependency_injection_for_test
{
    typedef int di_policy;
};

}}}// boost::msm::back
#endif

