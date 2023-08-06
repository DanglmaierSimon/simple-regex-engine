#pragma once

#include <functional>
#include <string>

std::function<bool(const std::string &)>
create_matcher(const std::string &regexp);