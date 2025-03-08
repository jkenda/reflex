#pragma once

#include "reflex.hpp"

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


// For arithmetic types.
template<typename T>
std::enable_if_t<std::is_arithmetic_v<T>, std::string> _to_json_impl(const T& value)
{
    return std::to_string(value);
}

// For std::string.
std::string _to_json_impl(const std::string& s)
{
    // For simplicity, special characters are not escaped.
    return "\"" + s + "\"";
}

// For std::optional.
template<typename T>
std::string _to_json_impl(const std::optional<T>& opt)
{
    if (!opt)
        return ""; // Field will be omitted in reflectable types.

    return to_json(*opt);
}

// For std::vector
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

// For reflectable types.
template<typename T>
std::enable_if_t<is_reflectable_v<T>, std::string> _to_json_impl(const T& obj)
{
    std::string json = "{";
    bool first = true;
    for_each_member(const_cast<T&>(obj),
        [&](const char* name, const auto& member) {
            // If the member is an optional and not engaged, skip it.
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

// Fallback overload for unsupported types.
template<typename T>
std::enable_if_t<!std::is_arithmetic_v<T> && !is_reflectable_v<T> && !std::is_same_v<T, std::string>, std::string> _to_json_impl(const T&)
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
