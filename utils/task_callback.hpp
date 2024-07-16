#pragma once

namespace utils::task{
    template<typename ... Args>
    struct CallBack{
        using CB_t = void (*) (void*, Args ...);
        CallBack() = default;
        explicit CallBack(CB_t&& f)
            :cb_(f)
            ,context_()
        {}
        CallBack(void* context, CB_t&& f)
            :cb_(f)
            ,context_(context)
        {}
        void operator()(Args ...args) const {
            cb_(context_, std::forward<Args>(args)...);
        }
        [[nodiscard]] bool HasContext()const{ return context_; }
    private:
        void* context_;
        CB_t cb_;
    };
}