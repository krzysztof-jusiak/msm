#if !BOOST_PP_IS_ITERATING

    #ifndef POOL_HPP
    #define POOL_HPP

    #include <boost/preprocessor/iteration/iterate.hpp>
    #include <boost/preprocessor/iteration/local.hpp>
    #include <boost/preprocessor/repetition/repeat.hpp>
    #include <boost/preprocessor/punctuation/comma_if.hpp>
    #include <boost/preprocessor/repetition/enum_params.hpp>
    #include <boost/preprocessor/repetition/enum_binary_params.hpp>
    #include <boost/di/ctor.hpp>
    #include <boost/utility/enable_if.hpp>
    #include <boost/ref.hpp>
    #include <boost/mpl/vector.hpp>
    #include <boost/mpl/fold.hpp>
    #include <boost/mpl/copy.hpp>
    #include <boost/mpl/back_inserter.hpp>
    #include <boost/mpl/at.hpp>
    #include <boost/mpl/size.hpp>

    #define BOOST_PP_ITERATION_PARAMS_1 (                   \
        3, (1, BOOST_MPL_LIMIT_VECTOR_SIZE, "boost/msm/back/pool.hpp")     \
    )

    #if defined(__GNUC__) && (__GNUC__ >= 4)
    #    pragma GCC diagnostic ignored "-Wreorder"
    #endif

    namespace boost {
    namespace msm {

    template<
        typename = mpl::vector0<>
      , typename = void
    >
    class pool
    {
    public:
        typedef pool type;

        pool() { }

        template<typename T>
        T& get() {
            return static_cast<T&>(*this);
        }
    };

    #include BOOST_PP_ITERATE()

    } // namespace msm
    } // namespace boost

    #endif

#else
    #define n BOOST_PP_ITERATION()

    #define BOOST_DI_DERIVES_IMPL(_, n, TSeq)                          \
        BOOST_PP_COMMA_IF(n) public mpl::at_c<TSeq, n>::type

    template<typename TSeq>
    class pool<
        TSeq
      , typename enable_if_c<mpl::size<TSeq>::value == n>::type
    >
        : BOOST_PP_REPEAT(
              n
            , BOOST_DI_DERIVES_IMPL
            , TSeq
          )
    {
    public:
        typedef pool type;

        pool() { }

        #define BOOST_DI_CTOR_INITLIST_IMPL(_, n, na)                           \
            BOOST_PP_COMMA_IF(n) Args##n::element_type(args##n)

        #define BOOST_DI_CTOR_TRAITS_IMPL(_, n, TSeq)                           \
            BOOST_PP_COMMA_IF(n) typename mpl::at_c<TSeq, n>::type p##n

        BOOST_DI_CTOR_TRAITS(BOOST_PP_REPEAT(n, BOOST_DI_CTOR_TRAITS_IMPL, TSeq));

        #define BOOST_PP_LOCAL_MACRO(n)                                         \
            template<BOOST_PP_ENUM_PARAMS(n , typename Args)>                   \
            explicit pool(BOOST_PP_ENUM_BINARY_PARAMS(n, const Args, &args))    \
                : BOOST_PP_REPEAT(n, BOOST_DI_CTOR_INITLIST_IMPL, ~)            \
            { }                                                                 \

        #define BOOST_PP_LOCAL_LIMITS (1, n)
        #include BOOST_PP_LOCAL_ITERATE()
        #undef BOOST_DI_CTOR_INITLIST_IMPL

        template<typename T>
        T& get() {
            return static_cast<T&>(*this);
        }
    };

    #undef BOOST_DI_DERIVES_IMPL
    #undef n

#endif

