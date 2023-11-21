#ifndef SAFE_DL_HPP
#define SAFE_DL_HPP

#include "../utils/common.hpp"
#include "../utils/utils.hpp"
#include "../utils/program_name.hpp"

#include <string>
#include <vector>
#include <mutex>
#include <unordered_map>
#include <cstring>
#include <iostream>

#if CPPTRACE_HAS_DL_FIND_OBJECT
#if IS_LINUX || IS_APPLE
 #include <unistd.h>
 #include <dlfcn.h>
 #include <link.h>
#endif

namespace cpptrace {
namespace detail {
    inline void get_safe_object_frame(frame_ptr address, safe_object_frame* out) {
        dl_find_object result;
        if(_dl_find_object(reinterpret_cast<void*>(address), &result) == 0) {
            out->raw_address = address;
            out->address_relative_to_object_start = address - reinterpret_cast<frame_ptr>(result.dlfo_map_start);
            if(result.dlfo_link_map->l_name != nullptr && result.dlfo_link_map->l_name[0] != 0) {
                std::size_t path_length = std::strlen(result.dlfo_link_map->l_name);
                std::memcpy(
                    out->object_path,
                    result.dlfo_link_map->l_name,
                    std::min(path_length + 1, std::size_t(CPPTRACE_PATH_MAX + 1))
                );
            } else {
                // empty l_name, this means it's the currently running executable
                memset(out->object_path, 0, PATH_MAX + 1);
                auto res = readlink("/proc/self/exe", out->object_path, PATH_MAX);
                if(res == -1) {
                    // error handling?
                }
                // TODO: Special handling for /proc/pid/exe unlink edge case
            }
        } else {
            // std::cout<<"error"<<std::endl;
            out->raw_address = address;
            out->address_relative_to_object_start = 0;
            out->object_path[0] = 0;
        }
        // TODO: Handle this part of the documentation?
        // The address can be a code address or data address. On architectures using function descriptors, no attempt is
        // made to decode the function descriptor. Depending on how these descriptors are implemented, _dl_find_object
        // may return the object that defines the function descriptor (and not the object that contains the code
        // implementing the function), or fail to find any object at all.
    }
}
}
#else
namespace cpptrace {
namespace detail {
    inline void get_safe_object_frame(frame_ptr address, safe_object_frame* out) {
        out->raw_address = address;
        out->address_relative_to_object_start = 0;
        out->object_path[0] = 0;
    }
}
}
#endif

#endif
