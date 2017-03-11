#define BOOST_TEST_MODULE fakeit_playground
#include <boost/test/unit_test.hpp>
#include <fakeit.hpp>

using namespace fakeit;

class IView
{
public:
    virtual bool display(int) const = 0;
    virtual bool error(std::string) const = 0;
};

class Calculator
{
public:
    Calculator(const IView& view)
        : m_view(view)
    {}

    void add(int a, int b)
    {
        if(!m_view.display(a + b))
        {
            static_cast<void>(m_view.error("could not display result"));
        }
    }
private:
    const IView& m_view;
};

class fakeit_env
{
public:
    fakeit_env()
    {}

    ~fakeit_env()
    {}

    Mock<IView> mockView{};
    const IView& view{mockView.get()};
    Calculator calc{view};

};

class true_on_pos_false_on_neg : public fakeit_env
{
public:
    true_on_pos_false_on_neg()
    {
        Fake(Method(mockView, error));
        When(Method(mockView, display).Using(Ge(0))).AlwaysReturn(true);
        When(Method(mockView, display).Using(Lt(0))).AlwaysReturn(false);
    }

    ~true_on_pos_false_on_neg()
    {}
};

BOOST_FIXTURE_TEST_CASE(stub_display_methods_return_value, fakeit_env)
{
    {
        When(Method(mockView, display)).Return(true).Return(true);

        BOOST_REQUIRE(view.display(0));
        BOOST_REQUIRE(view.display(0));
    }
    {
        When(Method(mockView, display)).Return(true, true);

        BOOST_REQUIRE(view.display(0));
        BOOST_REQUIRE(view.display(0));
    }
    {
        When(Method(mockView, display)).Return(2_Times(true));

        BOOST_REQUIRE(view.display(0));
        BOOST_REQUIRE(view.display(0));
    }
    {
        When(Method(mockView, display)).AlwaysReturn(true);

        BOOST_REQUIRE(view.display(0));
        BOOST_REQUIRE(view.display(0));
        BOOST_REQUIRE(view.display(0));
    }
    {
        Method(mockView, display) = false;

        BOOST_REQUIRE(!view.display(0));
        BOOST_REQUIRE(!view.display(0));
        BOOST_REQUIRE(!view.display(0));
    }
}

BOOST_FIXTURE_TEST_CASE(fake_view_methods, fakeit_env)
{
    {
        Fake(Method(mockView, display));
        Fake(Method(mockView, error));

        calc.add(0, 0);
    }
    {
        When(Method(mockView, display)).AlwaysReturn();
        When(Method(mockView, error)).AlwaysDo([](...){ return false; });

        calc.add(0, 0);
    }
    {
        Fake(Method(mockView, display), Method(mockView, error));

        calc.add(0, 0);
    }
}

BOOST_FIXTURE_TEST_CASE(argument_matching_params_to_view_methods, true_on_pos_false_on_neg)
{
    BOOST_REQUIRE(view.display(0));
    BOOST_REQUIRE(view.display(1));
    BOOST_REQUIRE(view.display(2));

    BOOST_REQUIRE(!view.display(-1));
    BOOST_REQUIRE(!view.display(-2));
    BOOST_REQUIRE(!view.display(-3));
}

BOOST_FIXTURE_TEST_CASE(verifying_calls_to_view_methods, true_on_pos_false_on_neg)
{
    calc.add(0, 0);
    calc.add(-1, -2);

    Verify(Method(mockView, display)).Exactly(2_Times);
    Verify(Method(mockView, error)).Once();
}

BOOST_FIXTURE_TEST_CASE(verifying_with_matcher, true_on_pos_false_on_neg)
{
    calc.add(0, 0);
    calc.add(-1, -2);

    Verify(Method(mockView, display).Matching([](auto a){ return a >= 0; })).Once();
    Verify(Method(mockView, display).Matching([](auto a){ return a <  0; })).Once();
    Verify(Method(mockView, error).Using("could not display result")).Once();
}

BOOST_FIXTURE_TEST_CASE(verifying_in_sequence, true_on_pos_false_on_neg)
{
    calc.add(0, 0);
    calc.add(-1, -2);

    calc.add(0, 0);
    calc.add(-1, -2);

    Verify((Method(mockView, display) * 2 + Method(mockView, error)) * 2);
}
