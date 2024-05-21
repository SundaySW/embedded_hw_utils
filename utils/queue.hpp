#pragma once

#include <cstring>
#include <utility>
#include <array>

namespace utils{

template<typename T, std::size_t Size>
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

    void pop(){
        elems_--;
        SetHeadIdx();
    }

    T& front(){
        return storage_[head_];
    }

    bool full(){
        return elems_ == Size;
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
    std::array<T, Size> storage_;

    std::size_t SetTailIdx(){
        if(++tail_ == Size)
            tail_ = 0;
        return tail_;
    }
    std::size_t SetHeadIdx(){
        if(++head_ == Size)
            head_ = 0;
        return head_;
    }
};

}//namespace utils