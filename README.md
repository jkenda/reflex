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
    Person p1{ "Alice", 30, Address{ "Wonderland", 12345, { 172, 0, 0, 1 } }, false };
    Person p2{ "Bob", 25, std::nullopt, true };

    std::cout << rfx::to_json(p1);
    // {"name":"Alice","age":30,"address":{"city":"Wonderland","zip":12345,"IP":[172,0,0,1]},"is_of_age":false}

    std::cout << rfx::to_json(p2);
    // {"name":"Bob","age":25,"is_of_age":true}
}
```
