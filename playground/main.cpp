#define BOOST_TEST_MODULE fakeit_playground
#include <boost/test/unit_test.hpp>
#include <fakeit.hpp>

using namespace fakeit;

class IView
{
public:
    virtual bool display(int) const = 0;
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
            throw std::runtime_error("could not display result");
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
