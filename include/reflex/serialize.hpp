#pragma once

#include "reflex.hpp"

#include <format>
#include <optional>
#include <sstream>
#include <cmath>
#include <iomanip>

namespace rfx
{


template <typename T>
struct is_optional : std::false_type {};

template <typename T>
struct is_optional<std::optional<T>> : std::true_type {};

template <typename T>
constexpr bool is_optional_v = is_optional<T>::value;


template<typename T>
std::string to_json(const T& value);

namespace __IMPL__
{


// for other arithmetic types.
template<typename T>
std::string _to_json_impl(const T& value) requires std::is_arithmetic_v<T>
{
    std::stringstream ss;
    if constexpr (std::is_same_v<T, float> || std::is_same_v<T, double>)
    {
        if (std::fmod(value, 1) == 0)
            ss << std::fixed << std::setprecision(1);
    }
    ss << value;
    return ss.str();
}

// trick for outputting bytes in decimal format
std::string _to_json_impl(unsigned char value)
{
    return _to_json_impl((unsigned short)value);
}

// for boolean
std::string _to_json_impl(bool value)
{
    return (value) ? "true" : "false";
}

// for std::string.
std::string _to_json_impl(const std::string& s)
{
    // For simplicity, special characters are not escaped.
    return "\"" + s + "\"";
}

// for std::optional.
template<typename T>
std::string _to_json_impl(const std::optional<T>& opt)
{
    if (!opt)
        return ""; // Field will be omitted in reflectable types.

    return to_json(*opt);
}

// for std::vector
template<typename T>
std::string _to_json_impl(const std::vector<T>& vec)
{
    std::string json = "[";

    bool first = true;
    for (auto& mem : vec)
    {
        if (!first)
            json += ",";

        json += to_json(mem);
        first = false;
    }

    json += "]";
    return json;
}

// for reflectable structs.
template<typename T>
std::string _to_json_impl(const T& obj) requires is_reflectable_struct_v<T>
{
    std::string json = "{";
    bool first = true;
    for_each_member(const_cast<T&>(obj),
        [&](const char* name, const auto& member) {
            // if the member is an optional and not engaged, skip it.
            if constexpr(is_optional_v<std::decay_t<decltype(member)>>) {
                if (!member)
                    return; 
            }
            if (!first)
                json += ",";

            json += "\"";
            json += name;
            json += "\":";

            json += to_json(member);
            first = false;
        });
    json += "}";
    return json;
}

// for reflectable enums.
template<typename T>
std::string _to_json_impl(const T& obj) requires is_reflectable_enum_v<T>
{
    return std::format("\"{}\"", rfx::enum_value(obj));
}

// fallback overload for unsupported types.
template<typename T>
std::string _to_json_impl(const T&)
{
    return "<missing reflection>";
}


}

template<typename T>
std::string to_json(const T& value)
{
    return __IMPL__::_to_json_impl(value);
}


}
