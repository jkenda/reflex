#include <gtest/gtest.h>

#include "reflex.hpp"
#include "serialize.hpp"

GTEST_API_ int main(int argc, char **argv)
{
    testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}


struct Address {
    std::string city;
    int zip;
    
    RFX_STRUCT(Address,
        RFX_MEMBER(city),
        RFX_MEMBER(zip))
};

struct Person {
    std::string name;
    int age;
    std::optional<Address> address;
    
    RFX_STRUCT(Person,
        RFX_MEMBER(name),
        RFX_MEMBER(age),
        RFX_MEMBER(address))
};

TEST(reflex, serialization)
{
    Person p1{"Alice", 30, Address{"Wonderland", 12345}};
    Person p2{"Bob", 25, std::nullopt};
    
    std::cout << "Person 1 JSON:\n" << rfx::to_json(p1) << "\n\n";
    std::cout << "Person 2 JSON:\n" << rfx::to_json(p2) << "\n";
}
