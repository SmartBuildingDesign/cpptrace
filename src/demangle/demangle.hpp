#ifndef DEMANGLE_HPP
#define DEMANGLE_HPP

#include <string>

namespace cpptrace {
namespace detail {
    std::string demangle(const std::string& name, bool check_prefix);
}
}

#endif
