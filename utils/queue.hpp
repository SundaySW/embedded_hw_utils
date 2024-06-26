#pragma once

#include <cstring>
#include <utility>
#include <array>

namespace utils{

template<typename T, std::size_t size>
class Queue{
public:
    Queue()=default;

    template<typename TT>
    bool push(TT&& new_elem){
        if(full())
            return false;
        elems_++;
        storage_[tail_] = std::forward<T>(new_elem);
        SetTailIdx();
        return true;
    }

    bool push(){
        if(full())
            return false;
        if(empty())
            return true;
        elems_++;
        SetTailIdx();
        return true;
    }

    T& tail(){
        return storage_[tail_];
    }

    void pop(){
        elems_--;
        SetHeadIdx();
    }

    T& front(){
        return storage_[head_];
    }

    bool full(){
        return elems_ == size;
    }

    bool empty(){
        return elems_ == 0;
    }
    void clear(){
        elems_ = 0;
        tail_ = 0;
        head_ = 0;
    }
private:
    std::size_t head_{0};
    std::size_t tail_{0};
    std::size_t elems_{0};
    std::array<T, size> storage_;

    std::size_t SetTailIdx(){
        if(++tail_ == size)
            tail_ = 0;
        return tail_;
    }
    std::size_t SetHeadIdx(){
        if(++head_ == size)
            head_ = 0;
        return head_;
    }
};

}//namespace utils