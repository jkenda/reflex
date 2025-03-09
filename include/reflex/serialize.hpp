#pragma once

#include "reflex.hpp"

#include <optional>
#include <cmath>
#include <sstream>

namespace rfx
{


template <typename T>
struct is_optional : std::false_type {};

template <typename T>
struct is_optional<std::optional<T>> : std::true_type {};

template <typename T>
constexpr bool is_optional_v = is_optional<T>::value;


template<typename T>
std::ostream& write_json(std::ostream& os, const T& value);

namespace __IMPL__
{


// for other arithmetic types.
template<typename T>
std::ostream& _write_json_impl(std::ostream& os, const T& value) requires std::is_arithmetic_v<T>
{
    os << value;
    if constexpr (std::is_same_v<T, float> || std::is_same_v<T, double>)
    {
        if (std::fmod(value, 1) == 0)
        {
            os << ".0";
        }
    }

    return os;
}

// trick for outputting bytes in decimal format
std::ostream& _write_json_impl(std::ostream& os, unsigned char value)
{
    write_json(os, (unsigned short)value);
    return os;
}

// for boolean
std::ostream& _write_json_impl(std::ostream& os, bool value)
{
    os << (value ? "true" : "false");
    return os;
}

// for std::string_view.
std::ostream& _write_json_impl(std::ostream& os, const std::string_view& s)
{
    // TODO: escaping
    os << '"';
    os << s;
    os << '"';
    return os;
}

// for std::string.
std::ostream& _write_json_impl(std::ostream& os, const std::string& s)
{
    write_json(os, std::string_view(s));
    return os;
}

// for std::optional.
template<typename T>
std::ostream& _write_json_impl(std::ostream& os, const std::optional<T>& opt)
{
    if (!opt)
        return os;

    return write_json(os, *opt);
}

// for std::vector
template<typename T>
std::ostream& _write_json_impl(std::ostream& os, const std::vector<T>& vec)
{
    os << '[';

    bool first = true;
    for (auto& mem : vec)
    {
        if (!first)
            os << ',';

        write_json(os, mem);
        first = false;
    }

    os << ']';
    return os;
}

// for reflectable structs.
template<typename T>
std::ostream& _write_json_impl(std::ostream& os, const T& obj) requires is_reflectable_struct_v<T>
{
    os << '{';
    bool first = true;
    for_each_member(const_cast<T&>(obj),
        [&](const char* name, const auto& member) {
            // if the member is an optional and not engaged, skip it.
            if constexpr(is_optional_v<std::decay_t<decltype(member)>>) {
                if (!member)
                    return; 
            }
            if (!first)
                os << ',';

            os << '"';
            os << name;
            os << "\":";

            write_json(os, member);
            first = false;
        });

    os << '}';
    return os;
}

// for reflectable enums.
template<typename T>
std::ostream& _write_json_impl(std::ostream& os, const T& obj) requires is_reflectable_enum_v<T>
{
    write_json(os, std::string_view(rfx::enum_value(obj)));
    return os;
}

// fallback overload for unsupported types.
template<typename T>
std::ostream& _write_json_impl(std::ostream& os, const T&)
{
    os << "<missing reflection>";
    return os;
}


}

template<typename T>
std::ostream& write_json(std::ostream& os, const T& value)
{
    return __IMPL__::_write_json_impl(os, value);
}

template<typename T>
std::string to_json(const T& value)
{
    std::stringstream ss;
    rfx::write_json(ss, value);
    return ss.str();
}


}
