#pragma once

namespace task{
    template<typename ... Args>
    struct CallBack{
        using CB_t = void (*) (void*, Args ...);
        CallBack() = default;
        CallBack(CB_t&& f)
            :cb_(f)
        {}
        CallBack(void* context, CB_t&& f)
            :cb_(f)
            ,context_(context)
        {}
        void operator()(Args ...args) const {
            cb_(context_, std::forward<Args>(args)...);
        }
    private:
        void* context_;
        CB_t cb_;
    };
}
