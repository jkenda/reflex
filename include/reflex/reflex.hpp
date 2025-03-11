#pragma once

#include <tuple>
#include <vector>
#include <expected>


template <typename T>  requires std::is_scoped_enum_v<T>
constexpr auto _rfx_reflect();

namespace rfx
{


/*
    struct descriptor.
*/

enum flags_t
{
    none = 0,
    inside_parent = 1 << 0,
};

namespace __IMPL__
{
    template<typename T, typename MemberT>
    struct member_descriptor
    {
      const char* name;
      MemberT T::* pointer;
      flags_t flags = none;
    };

    template<typename T, typename MemberT>
    constexpr member_descriptor<T, MemberT> make_member(const char* name, MemberT T::* pointer, flags_t flags = none)
    {
      return { name, pointer, flags };
    }

    template<typename T, typename... Tuples>
    constexpr auto reflect_fields(Tuples... tuples)
    {
      return std::make_tuple(make_member<T>(std::get<0>(tuples), std::get<1>(tuples), std::get<2>(tuples))...);
    }
}

template<typename T, typename = void>
struct is_reflectable_struct : std::false_type {};

template<typename T>
struct is_reflectable_struct<T, std::void_t<decltype(T::reflect())>> : std::true_type {};

template<typename T>
constexpr bool is_reflectable_struct_v = is_reflectable_struct<T>::value;


/*
    enum descriptor
*/

namespace __IMPL__
{
    template<typename T, typename... Pairs>
    constexpr auto reflect_enum(Pairs... pairs)
    {
      return std::array{ pairs... };
    }
}

template<typename T, typename = void>
struct is_reflectable_enum : std::false_type {};

template<typename T>
struct is_reflectable_enum<T, std::void_t<decltype(_rfx_reflect<T>())>> : std::true_type {};

template<typename T>
constexpr bool is_reflectable_enum_v = is_reflectable_enum<T>::value;


// iterate over each member using the reflect() function.
template<typename T, typename F> requires is_reflectable_struct_v<T>
constexpr void for_each_member(const T& obj, F&& f)
{
    auto members = T::reflect();
    std::apply(
        [&](auto&& ... member_descs)
        {
            ((f(member_descs.name, obj.*(member_descs.pointer), member_descs.flags)), ...);
        }, members);
}

// get the string value of the enum
template<typename T> requires is_reflectable_enum_v<T>
constexpr std::expected<const char*, const char*> enum_name_from_value(const T& val)
{
    auto descript = _rfx_reflect<T>();
    auto it = std::ranges::find(descript, val, [](auto& r_pair) -> T { return r_pair.first; });
    return (it == descript.end())
      ? std::unexpected("Missing reflection")
      : std::expected<const char*, const char*>(it->second);
}

// get the string value of the enum
template<typename T> requires is_reflectable_enum_v<T>
constexpr std::expected<T, const char*> enum_value_from_name(const char* name)
{
    auto descript = _rfx_reflect<T>();
    auto it = std::ranges::find_if(descript,
      [name](auto& r_pair) -> bool { return std::strcmp(r_pair.second, name) == 0; });

    return (it == descript.end())
      ? std::unexpected("Missing reflection")
      : std::expected<T, const char*>(it->first);
}

// get the (value, name) pairs
template<typename T>
constexpr auto enum_pairs()
{
  return _rfx_reflect<T>();
}


}


#define RFX_STRUCT(this_type, ...) \
    static auto reflect() { \
        using ThisType = this_type; \
        return rfx::__IMPL__::reflect_fields<ThisType>(__VA_ARGS__); \
    }

#define RFX_MEMBER(x)       std::tuple<const char*, decltype(&ThisType::x), rfx::flags_t>{ #x, &ThisType::x, rfx::none }
#define RFX_MEMBER_EX(x, n) std::tuple<const char*, decltype(&ThisType::x), rfx::flags_t>{ n,  &ThisType::x, rfx::none }
#define RFX_MEMBER_FL(x, f) std::tuple<const char*, decltype(&ThisType::x), rfx::flags_t>{ #x, &ThisType::x, f }

#define RFX_ENUM(type, ...) \
    template<> \
    constexpr auto _rfx_reflect<type>() { \
        using ThisType = type; \
        return rfx::__IMPL__::reflect_enum<ThisType>(__VA_ARGS__); \
    }

#define RFX_E_MEMBER(x)       std::pair<ThisType, const char*>{ ThisType::x, #x }
#define RFX_E_MEMBER_EX(x, n) std::pair<ThisType, const char*>{ ThisType::x, n }
