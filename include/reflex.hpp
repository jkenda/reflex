#pragma once

#include <tuple>

namespace rfx
{


/*
    struct member descriptor.
*/

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

template<typename T, typename... Pairs>
constexpr auto reflect_fields(Pairs... pairs)
{
    return std::make_tuple(make_member<T>(pairs.first, pairs.second)...);
}

template<typename T, typename = void>
struct is_reflectable_struct : std::false_type {};

template<typename T>
struct is_reflectable_struct<T, std::void_t<decltype(T::reflect())>> : std::true_type {};

template<typename T>
constexpr bool is_reflectable_struct_v = is_reflectable_struct<T>::value;


/*
    enum member descriptor.
*/

template<typename T, typename... Pairs>
constexpr auto reflect_enum(Pairs... pairs)
{
    return std::unordered_map({ pairs... });
}

template <typename T>
constexpr auto reflect();

template<typename T, typename = void>
struct is_reflectable_enum : std::false_type {};

template<typename T>
struct is_reflectable_enum<T, std::void_t<decltype(reflect<T>())>> : std::true_type {};

template<typename T>
constexpr bool is_reflectable_enum_v = is_reflectable_enum<T>::value;


// iterate over each member using the reflect() function.
template<typename T, typename F> requires is_reflectable_struct_v<T>
void for_each_member(T& obj, F&& f)
{
    auto members = T::reflect();
    std::apply(
        [&](auto&& ... member_descs)
        {
            ((f(member_descs.name, obj.*(member_descs.pointer))), ...);
        }, members);
}

// get the string value of the enum
template<typename T> requires is_reflectable_enum_v<T>
const char* enum_value(const T& val)
{
    auto descript = reflect<T>();
    return descript[val];
}


}

#define RFX_STRUCT(this_type, ...) \
    static auto reflect() { \
        using ThisType = this_type; \
        return rfx::reflect_fields<ThisType>(__VA_ARGS__); \
    }

#define RFX_MEMBER(x) std::pair<const char*, decltype(&ThisType::x)>{ #x, &ThisType::x }

#define RFX_ENUM(type, ...) \
    template<> \
    constexpr auto rfx::reflect<type>() { \
        using ThisType = type; \
        return rfx::reflect_enum<ThisType>(__VA_ARGS__); \
    }

#define RFX_E_MEMBER(x) std::pair<ThisType, const char*>{ ThisType::x, #x }
