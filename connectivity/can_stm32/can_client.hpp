#pragma once

#include "impl/can_pack.hpp"

namespace connectivity::can{
    struct Client{
        using callable = void(*)(void*, Pack& pack);

        Client() = default;

        explicit Client(void* context, callable cb)
            :context_(context)
            ,call_(cb)
        {}

        bool HasContext(){
            return context_;
        }

        void operator()(Pack& pack) const{
            if(context_)
                call_(context_, pack);
        }
    private:
        void* context_;
        callable call_;
    };
}
