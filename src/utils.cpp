#pragma once

#include <string>
#include <string_view>

std::string_view ltrim(std::string_view str, std::string const &whitespace = " \r\n\t\v\f")
{
    const auto pos(str.find_first_not_of(whitespace));
    str.remove_prefix(std::min(pos, str.length()));
    return str;
}

std::string_view rtrim(std::string_view str, std::string const &whitespace = " \r\n\t\v\f")
{
    const auto pos(str.find_last_not_of(whitespace));
    str.remove_suffix(std::min(str.length() - pos - 1, str.length()));
    return str;
}

std::string_view trim(std::string_view str, std::string const &whitespace = " \r\n\t\v\f")
{
    return ltrim(rtrim(str, whitespace), whitespace);
}
