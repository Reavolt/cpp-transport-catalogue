#pragma once

#include "transport_catalogue.h"

#include <string>
#include <utility>

namespace transport_catalogue
{
    struct Stop;

    namespace detail
    {
        const int SIMPLE_NUMBER = 37;

        struct StopPtrHasher
        {
            size_t operator()(const std::pair<const Stop*, const Stop*>& p) const
            {
                return pointer_hasher(p.first) * SIMPLE_NUMBER +
                       pointer_hasher(p.second) * SIMPLE_NUMBER * SIMPLE_NUMBER;
            }
            size_t operator()(const Stop* p) const
            {
                return pointer_hasher(p) * SIMPLE_NUMBER;
            }

        private:
            std::hash<const void*> pointer_hasher{};
        };

        // trim from left
        inline std::string& ltrim(std::string& s, const char* t = " \t\n\r\f\v")
        {
            s.erase(0, s.find_first_not_of(t));
            return s;
        }

        // trim from right
        inline std::string& rtrim(std::string& s, const char* t = " \t\n\r\f\v")
        {
            s.erase(s.find_last_not_of(t) + 1);
            return s;
        }

        // trim from left & right
        inline std::string& trim(std::string& s, const char* t = " \t\n\r\f\v")
        {
            return ltrim(rtrim(s, t), t);
        }

        // copying versions
        inline std::string ltrim_copy(std::string s, const char* t = " \t\n\r\f\v")
        {
            return ltrim(s, t);
        }

        inline std::string rtrim_copy(std::string s, const char* t = " \t\n\r\f\v")
        {
            return rtrim(s, t);
        }

        inline std::string trim_copy(std::string s, const char* t = " \t\n\r\f\v")
        {
            return trim(s, t);
        }
    }    // namespace detail
}    // namespace transport_catalogue