#define BOOST_TEST_MODULE fakeit_playground
#include <boost/test/unit_test.hpp>
#include <fakeit.hpp>

struct SomeInterface
{
    virtual int foo(int) = 0;
    virtual int bar(int, int) = 0;
};

BOOST_AUTO_TEST_CASE(test_1)
{
    using namespace fakeit;

    Mock<SomeInterface> mock;
    // Stub a method to return a value once
    When(Method(mock, foo)).Return(1);

    // Stub multiple return values (The next two lines do exactly the same)
    When(Method(mock, foo)).Return(1, 2, 3);
    When(Method(mock, foo)).Return(1).Return(2).Return(3);

    // Return the same value many times (56 in this example)
    When(Method(mock, foo)).Return(56_Times(1));

    // Return many values many times (First 100 calls will return 1, next 200 calls will return 2)
    When(Method(mock, foo)).Return(100_Times(1), 200_Times(2));

    // Always return a value (The next two lines do exactly the same)
    When(Method(mock, foo)).AlwaysReturn(1);
    Method(mock, foo) = 1;

    BOOST_CHECK(true);
}
