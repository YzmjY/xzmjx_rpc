//
// Created by 20132 on 2022/11/29.
//
#include <tuple>
#include <iostream>
#include <string>

using namespace std;

template<typename T>
class AddSpace {
private:
    const T& m_data;
    size_t m_idx;
public:
    AddSpace(const T& v,size_t i) :m_data(v),m_idx(i) {}

    friend ostream& operator<< (ostream& os,AddSpace<T> s) {
        os<<s.m_idx<<":"<<s.m_data<<" ";
        return os;
    }
};

template <typename ... Types>
void output(const tuple<Types...>& tp) {
    auto f =[]<typename Tp,std::size_t ... Index> (const Tp& t,index_sequence<Index...> idx) {
        (cout<< ... <<AddSpace(get<Index>(t),Index));
    };
    f(tp,std::index_sequence_for<Types...>{});
}

int main() {
    tuple<int,string,double,float,char> tp = make_tuple(1,"xzmjx",0.99,0.4,'x');

    output(tp);
    return 0;
}