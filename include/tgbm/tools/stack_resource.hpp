#pragma once

#include <kelcoro/memory_support.hpp>
#include <memory_resource>
#include <span>
#include <tgbm/tools/macro.hpp>

namespace tgbm{


    struct stack_resource final: std::pmr::memory_resource{
        using byte = unsigned char;

        TGBM_PIN;

        byte* begin{};
        byte* cur{};
        byte* end{};

        std::pmr::memory_resource* upstream_resource = std::pmr::new_delete_resource();
    public:
        stack_resource(std::span<unsigned char> bytes, std::pmr::memory_resource& upstream_resource = *std::pmr::new_delete_resource()) : 
        begin(bytes.data()), cur(bytes.data()), end(bytes.data() + bytes.size()), upstream_resource(&upstream_resource){

        }

        std::size_t bytes_len(std::size_t bytes){
            return bytes + dd::noexport::padding_len<dd::coroframe_align()>(bytes);
        }

        void * do_allocate(size_t bytes, size_t alligment) override final{
            assert(alligment <= dd::coroframe_align());
            std::size_t len = bytes_len(bytes);
            if (end - cur < len){
                return upstream_resource->allocate(bytes, alligment);
            }
            auto r = cur;
            cur += len;
            return r;
        }

        void do_deallocate(void* ptr, size_t bytes, size_t alligment) override final{
            assert(alligment <= dd::coroframe_align());
            std::size_t len = bytes_len(bytes);
            if ((byte*)ptr + len == cur){
                cur -= len;
            }
            else {
                assert(begin < ptr || ptr >= end);
                upstream_resource->deallocate(ptr, bytes, alligment);
            }
        }

        bool do_is_equal(const memory_resource &other) const noexcept override{
            return this == &other;
        }
    };
}