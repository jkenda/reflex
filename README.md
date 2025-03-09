# Reflex

A simple reflection library for C++.
_Note_: in development.

Currenty implemented: reflection, JSON serialization.
Not implemented: JSON deserialization, other kinds of serdes.

## Example usage
```C++
struct Address {
    std::string city;
    int zip;
    std::vector<int> IP;
    
    RFX_STRUCT(Address,
        RFX_MEMBER(city),
        RFX_MEMBER(zip),
        RFX_MEMBER(IP))
};

RFX_ENUM(IP_Address::Type,
    RFX_E_MEMBER(IPV4),
    RFX_E_MEMBER(IPV6))

struct Person {
    std::string name;
    int age;
    std::optional<Address> address;
    bool is_of_age;
    
    RFX_STRUCT(Person,
        RFX_MEMBER(name),
        RFX_MEMBER(age),
        RFX_MEMBER(address),
        RFX_MEMBER(is_of_age))
};

int main()
{
    std::cout << rfx::to_json(Person{ "Alice", 17, IP_Address{ 127, 0, 0, 1 }, false });
    // {"name":"Alice","age":17,"address":{"type":"IPV4","bits":[127,0,0,1]},"is_of_age":false}

    std::cout << rfx::to_json(Person{ "Bob", 25, std::nullopt, true });
    // {"name":"Bob","age":25,"is_of_age":true}
}
```
