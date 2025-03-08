#pragma once

#include <tuple>

namespace rfx
{


// member descriptor holding a member’s name and pointer.
template<typename T, typename MemberT>
struct member_descriptor
{
    const char* name;
    MemberT T::* pointer;
};

template<typename T, typename MemberT>
constexpr member_descriptor<T, MemberT> make_member(const char* name, MemberT T::* pointer)
{
    return { name, pointer };
}

// helper function that accepts an arbitrary number of field pairs and returns a tuple.
template<typename T, typename... Pairs>
constexpr auto reflect_fields(Pairs... pairs)
{
    return std::make_tuple(make_member<T>(pairs.first, pairs.second)...);
}

// SFINAE to detect if a type has a reflect() function.
template<typename T, typename = void>
struct is_reflectable : std::false_type {};

template<typename T>
struct is_reflectable<T, std::void_t<decltype(T::reflect())>> : std::true_type {};

template<typename T>
constexpr bool is_reflectable_v = is_reflectable<T>::value;

// iterate over each member using the reflect() function.
template<typename T, typename F>
void for_each_member(T& obj, F&& f)
{
    auto members = T::reflect();
    std::apply(
        [&](auto&& ... member_descs)
        {
            ((f(member_descs.name, obj.*(member_descs.pointer))), ...);
        }, members);
}


}

#define RFX_STRUCT(this_type, ...) \
    static auto reflect() { \
        using ThisType = this_type; \
        return rfx::reflect_fields<ThisType>(__VA_ARGS__); \
    }

#define RFX_MEMBER(x) std::pair<const char*, decltype(&ThisType::x)>{ #x, &ThisType::x }
