//
// Created by 20132 on 2022/11/30.
//
#include "gtest/gtest.h"
#include "rpc/serializer.h"
using namespace xzmjx::rpc;

TEST(Serializer,bool) {
    Serializer s;
    s<<true;
    bool b;
    s.reset();
    s>>b;
    EXPECT_EQ(b,true);
}

TEST(Serializer,uint8_t) {
    Serializer s;
    s<<(uint8_t)200;
    uint8_t b;
    s.reset();
    s>>b;
    EXPECT_EQ(b,(uint8_t)200);
}

TEST(Serializer,int8_t) {
    Serializer s;
    s<<(int8_t)20;
    int8_t b;
    s.reset();
    s>>b;
    EXPECT_EQ(b,(int8_t)20);
}

TEST(Serializer,uint16_t) {
    Serializer s;
    s<<(uint16_t)200;
    uint16_t b;
    s.reset();
    s>>b;
    EXPECT_EQ(b,(uint16_t)200);
}

TEST(Serializer,int16_t) {
    Serializer s;
    s<<(int16_t)200;
    int16_t b;
    s.reset();
    s>>b;
    EXPECT_EQ(b,(int16_t)200);
}

TEST(Serializer,uint32_t) {
    Serializer s;
    s<<(uint32_t)200;
    uint32_t b;
    s.reset();
    s>>b;
    EXPECT_EQ(b,(uint32_t)200);
}

TEST(Serializer,int32_t) {
    Serializer s;
    s<<(int32_t)200;
    int32_t b;
    s.reset();
    s>>b;
    EXPECT_EQ(b,(int32_t)200);
}

TEST(Serializer,uint64_t) {
    Serializer s;
    s<<(uint64_t)200;
    uint64_t b;
    s.reset();
    s>>b;
    EXPECT_EQ(b,(uint64_t)200);
}

TEST(Serializer,int64_t) {
    Serializer s;
    s<<(int64_t)200;
    int64_t b;
    s.reset();
    s>>b;
    EXPECT_EQ(b,(int64_t)200);
}

TEST(Serializer,string) {
    Serializer s;
    std::string str("xzmjx");
    s<<str;
    std::string b;
    s.reset();
    s>>b;
    int ret = b.compare(str);
    EXPECT_EQ(ret,0);
}

TEST(Serializer,const_char_ptr) {
    Serializer s;
    const char* str = "xzmjx";
    s<<str;
    std::string b;
    s.reset();
    s>>b;
    EXPECT_STREQ(b.c_str(),str);
}

TEST(Serializer,tuple) {
    Serializer s;
    std::tuple<int,double,std::string> tp{1,2.0,"xzmjx"};
    s<<tp;
    std::tuple<int,double,std::string> b;
    s.reset();
    s>>b;
    EXPECT_EQ(std::get<0>(b),std::get<0>(tp));
    EXPECT_EQ(std::get<1>(b),std::get<1>(tp));
    EXPECT_EQ(std::get<2>(b),std::get<2>(tp));
}

TEST(Serializer,vector) {
    Serializer s;
    std::vector<int> vec{1,2,3,4,5};
    s<<vec;
    std::vector<int> b;
    s.reset();
    s>>b;
    EXPECT_EQ(vec[0],b[0]);
    EXPECT_EQ(vec[1],b[1]);
    EXPECT_EQ(vec[2],b[2]);
}

