#include <gtest/gtest.h>

#include "reflex/reflex.hpp"
#include "reflex/serialize.hpp"

GTEST_API_ int main(int argc, char **argv)
{
    testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}


struct IP_Address
{
    enum class Type
    {
        IPV4, IPV6,
    } type;
    std::vector<uint8_t> bits;

    IP_Address(uint8_t a, uint8_t b, uint8_t c, uint8_t d)
        : type(Type::IPV4), bits({ a, b, c, d })
    {}

    RFX_STRUCT(IP_Address,
        RFX_MEMBER(type),
        RFX_MEMBER(bits))
};

RFX_ENUM(IP_Address::Type,
    RFX_E_MEMBER(IPV4),
    RFX_E_MEMBER(IPV6))

struct Person {
    std::string name;
    int age;
    bool is_of_age;
    float height_m;
    std::optional<IP_Address> address;

    RFX_STRUCT(Person,
        RFX_MEMBER(name),
        RFX_MEMBER(age),
        RFX_MEMBER(is_of_age),
        RFX_MEMBER(height_m),
        RFX_MEMBER(address))
};

TEST(reflex, serialization)
{
    Person p1{ "Alice", 17, false, 1.67, IP_Address{ 127, 0, 0, 1 } };
    Person p2{ "Bob", 25, true, 2.0, std::nullopt };

    EXPECT_EQ(rfx::to_json(p1), R"({"name":"Alice","age":17,"is_of_age":false,"height_m":1.67,"address":{"type":"IPV4","bits":[127,0,0,1]}})");
    EXPECT_EQ(rfx::to_json(p2), R"({"name":"Bob","age":25,"is_of_age":true,"height_m":2.0})");
}
