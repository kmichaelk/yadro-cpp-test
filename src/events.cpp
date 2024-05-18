#include "events.h"
#include <regex>

const std::regex name_regex(R"(^[A-Za-z0-9\-_]+$)");
bool validate_client_name(const std::string& name) {
    return std::regex_match(name, name_regex);
}