# Reflex

A simple reflection library for C++.
_Note_: in development.

Currenty implemented: reflection, JSON serialization.
Not implemented: JSON deserialization, other kinds of serdes.

## Example usage
```C++
#include "reflex/reflex.hpp"
#include "reflex/serialize.hpp"

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

int main()
{
    std::cout << rfx::to_json(Person{ "Alice", 17, IP_Address{ 127, 0, 0, 1 }, false });
    // {"name":"Alice","age":17,"address":{"type":"IPV4","bits":[127,0,0,1]},"is_of_age":false}

    std::cout << rfx::to_json(Person{ "Bob", 25, std::nullopt, true });
    // {"name":"Bob","age":25,"is_of_age":true}
}
```
