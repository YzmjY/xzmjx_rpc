//
// Created by 20132 on 2022/11/23.
//
#ifndef XZMJX_CONFIG_H
#define XZMJX_CONFIG_H
#include <algorithm>
#include <string>
#include <memory>
#include <unordered_map>
#include <unordered_set>
#include <map>
#include "libgo.h"
#include "yaml-cpp/yaml.h"
#include "log.h"
#include "boost/lexical_cast.hpp"

namespace xzmjx{
/**
 * @brief 配置系统，底层采用YAML作为配置文件，提供读取文件，以及string和各种类型的相互转换。
 */

/**
 * @brief 类型转换仿函数，不同的偏特化实现对应不同类型的转化，同时可以相互叠加（装饰模式）
 * @tparam From
 * @tparam To
 */
template<class From,class To>
class LexicalCast{
public:
    To operator()(const From& f){
        ///简单类型的默认转换string->scalar scalar->string
        return boost::lexical_cast<To>(f);
    }
};

/**
 * @brief 支持vector
 * @tparam T
 */
template<class T>
class LexicalCast<std::string,std::vector<T>>{
public:
    std::vector<T> operator()(const std::string&f){
        YAML::Node node = YAML::Load(f);
        typename std::vector<T> ans;
        std::stringstream ss;
        for(size_t i = 0;i<node.size();i++){
            ss.str("");
            ss<<node[i];
            ans.push_back(LexicalCast<std::string,T>()(ss.str()));
        }
        return ans;
    }
};
template<class T>
class LexicalCast<std::vector<T>,std::string>{
public:
    std::string operator()(const std::vector<T>&f){
        YAML::Node node(YAML::NodeType::Sequence);
        for(size_t i = 0;i<f.size();i++){
            node.push_back(YAML::Load(LexicalCast<T,std::string>()(f[i])));
        }
        std::stringstream ss;
        ss<<node;
        return ss.str();
    }
};

/**
 * @brief list的支持
 * @tparam T
 */
template<class T>
class LexicalCast<std::string,std::list<T>>{
public:
    std::list<T> operator()(const std::string&f){
        YAML::Node node = YAML::Load(f);
        typename std::list<T> ans;
        std::stringstream ss;
        for(size_t i = 0; i<node.size();i++){
            ss.str("");
            ss<<node[i];
            ans.push_back(LexicalCast<std::string,T>()(ss.str()));
        }
        return ans;
    }
};
template<class T>
class LexicalCast<std::list<T>,std::string>{
public:
    std::string operator()(const std::list<T>&f){
        YAML::Node node(YAML::NodeType::Sequence);
        for(size_t i = 0;i<f.size();i++){
            node.push_back(LexicalCast<T,std::string>()(f[i]));
        }
        std::stringstream ss;
        ss<<node;
        return ss.str();
    }
};

/**
 * @brief set的支持
 * @tparam T
 */
template<class T>
class LexicalCast<std::string,std::set<T>>{
public:
    std::set<T> operator()(const std::string&f){
        YAML::Node node = YAML::Load(f);
        typename std::set<T> ans;
        std::stringstream ss;
        for(size_t i = 0;i<node.size();i++){
            ss.str("");
            ss<<node[i];
            ans.insert(LexicalCast<std::string,T>()(ss.str()));
        }
        return ans;
    }
};
template<class T>
class LexicalCast<std::set<T>,std::string>{
public:
    std::string operator()(const std::set<T>&f){
        YAML::Node node(YAML::NodeType::Sequence);
        for(auto it:f){
            node.push_back(LexicalCast<T,std::string>()(it));
        }
        std::stringstream ss;
        ss<<node;
        return ss.str();
    }
};

/**
 * @brief unordered_set的支持
 * @tparam T
 */
template<class T>
class LexicalCast<std::string,std::unordered_set<T>>{
public:
    std::unordered_set<T> operator()(const std::string&f){
        YAML::Node node = YAML::Load(f);
        typename std::unordered_set<T> ans;
        std::stringstream ss;
        for(size_t i = 0;i<node.size();i++){
            ss.str("");
            ss<<node[i];
            ans.insert(LexicalCast<std::string,T>()(ss.str()));
        }
        return ans;
    }
};
template<class T>
class LexicalCast<std::unordered_set<T>,std::string>{
public:
    std::string operator()(const std::unordered_set<T>&f){
        YAML::Node node(YAML::NodeType::Sequence);
        for(auto it:f){
            node.push_back(LexicalCast<T,std::string>()(it));
        }
        std::stringstream ss;
        ss<<node;
        return ss.str();
    }
};

/**
 * @brief map的支持
 * @tparam T
 */
template<class T>
class LexicalCast<std::string,std::map<std::string,T>>{
public:
    std::map<std::string,T> operator()(const std::string&f){
        YAML::Node node = YAML::Load(f);
        typename std::map<std::string,T> ans;
        std::stringstream ss;
        for(auto iter = node.begin();iter != node.end();iter++){
            ss.str("");
            ss<<iter->second;
            ans.insert(std::make_pair(iter->first.Scalar(),LexicalCast<std::string,T>()(ss.str())));
        }
        return ans;
    }
};
template<class T>
class LexicalCast<std::map<std::string,T>,std::string>{
public:
    std::string operator()(const std::map<std::string,T>&f){
        YAML::Node node(YAML::NodeType::Map);
        for(auto it:f){
            node[it.first] = YAML::Load(LexicalCast<T,std::string>()(it.second));
        }
        std::stringstream ss;
        ss<<node;
        return ss.str();
    }
};

/**
 * @brief unordered_map的支持
 * @tparam T
 */
template<class T>
class LexicalCast<std::string,std::unordered_map<std::string,T>>{
public:
    std::unordered_map<std::string,T> operator()(const std::string&f){
        YAML::Node node = YAML::Load(f);
        typename std::unordered_map<std::string,T> ans;
        std::stringstream ss;
        for(auto iter = node.begin();iter != node.end();iter++){
            ss.str("");
            ss<<iter->second;
            ans.insert(std::make_pair(iter->first.Scalar(),LexicalCast<std::string,T>()(ss.str())));
        }
        return ans;
    }
};
template<class T>
class LexicalCast<std::unordered_map<std::string,T>,std::string>{
public:
    std::string operator()(const std::unordered_map<std::string,T>&f){
        YAML::Node node(YAML::NodeType::Map);
        for(auto it:f){
            node[it.first] = YAML::Load(LexicalCast<T,std::string>()(it.second));
        }
        std::stringstream ss;
        ss<<node;
        return ss.str();
    }
};



 class ConfigBase{
 public:
     typedef std::shared_ptr<ConfigBase> ptr;
     ConfigBase(const std::string& name,const std::string& description )
                :m_name(name),m_description(description){
         std::transform(m_name.begin(),m_name.end(),m_name.begin(),::tolower);
     }
     virtual ~ConfigBase(){}

     /**
      * @brief 配置项名称
      * @return
      */
     const std::string& getName() const {return m_name;}

     /**
      * @brief 配置项描述
      * @return
      */
     const std::string& getDescription() const {return m_description;}

     /**
      * @brief 转化为字符串
      * @return
      */
     virtual std::string toString() = 0;

     /**
      * @brief 从字符串中加载
      */
     virtual void fromString(const std::string& f) = 0;

     /**
      * @brief 返回配置参数值的类型描述
      * @return
      */
     virtual std::string getTypeName() = 0;

 protected:
     std::string m_name;
     std::string m_description;
 };

 template <typename T,typename toStr = LexicalCast<T,std::string>,typename fromStr = LexicalCast<std::string,T> >
 class ConfigVar:public ConfigBase{
 public:
     typedef std::shared_ptr<ConfigVar> ptr;
     typedef co::CoRWMutex RWMutexType;
     typedef std::function<void (const T& old_val,const T& new_val)> on_change_cb;

     ConfigVar(const std::string& name,
               const T& default_value,
               const std::string& description = "")
               : ConfigBase(name,description),m_val(default_value){

     }
     std::string toString() override{
        try{
            std::scoped_lock<RWMutexType::ReadView> lock(m_rwMutex.Reader());
            return toStr()(m_val);
        }catch (std::exception& ex){
            SPDLOG_ERROR("ConfigVar::toString() exception {} ,convert: {} to string name =  {}",ex.what(),TypeToName<T>(),m_name);
        }
        return "";
     }

     void fromString(const std::string& f) override{
        try{
            setValue(fromStr()(f));
        } catch (std::exception &ex) {
            SPDLOG_ERROR("ConfigVar::toString() exception {} ,convert: {} to string name =  {} - {}",ex.what(),TypeToName<T>(),m_name,f);
        }
     }

     void setValue(const T& val){
         {
             std::scoped_lock<RWMutexType::ReadView> lock(m_rwMutex.Reader());
             if(val == m_val){
                 return;
             }
             for(auto i =m_listeners.begin();i!=m_listeners.end();i++){
                 i->second(m_val,val);
             }
         }

        std::scoped_lock<RWMutexType::WriteView> lock(m_rwMutex.Writer());
        m_val = val;
     }

     const T& getValue(){
         std::scoped_lock<RWMutexType::ReadView> lock(m_rwMutex.Reader());
         return m_val;
     }

     std::string getTypeName() override{
        return TypeToName<T>();
     }

     uint64_t addListener(on_change_cb cb){
         static uint64_t s_listener_id = 0;
         std::scoped_lock<RWMutexType::WriteView> lock(m_rwMutex.Writer());
         ++s_listener_id;
         m_listeners[s_listener_id] = cb;
         return s_listener_id;
     }

     void delListener(uint64_t id){
         std::scoped_lock<RWMutexType::WriteView> lock(m_rwMutex.Writer());
         m_listeners.erase(id);
     }

     on_change_cb getListener(uint64_t id){
         std::scoped_lock<RWMutexType::ReadView> lock(m_rwMutex.Reader());
         auto iter = m_listeners.find(id);
         if(iter == m_listeners.end()){
             return nullptr;
         }else{
             return iter->second;
         }
     }

     void clearListener(){
         std::scoped_lock<RWMutexType::WriteView> lock(m_rwMutex.Writer());
         m_listeners.clear();
     }

 private:
     RWMutexType m_rwMutex;
     T m_val;
     std::map<uint64_t,on_change_cb> m_listeners;
 };

 class Config{
 public:
     typedef std::unordered_map<std::string,ConfigBase::ptr> ConfigVarMap;
     typedef co::CoRWMutex RWMutexType;

     template<class T>
     static typename ConfigVar<T>::ptr Lookup(const std::string& name,
                                              const T& defaultVal,
                                              const std::string& description){
         {
             std::scoped_lock<RWMutexType::ReadView> lock(GetRwMutex().Reader());
             auto iter = GetConfigMap().find(name);
             if(iter != GetConfigMap().end()){
                 auto p = std::dynamic_pointer_cast<ConfigVar<T>>(iter->second);
                if(p){
                    SPDLOG_ERROR("Lookup name = {} exists",name);
                    return p;
                }else{
                    SPDLOG_ERROR("Lookup name({}) exists but type not {},real type = {} {}",name,TypeToName<T>(),iter->second->getTypeName(),iter->second->toString());
                    return nullptr;
                }
             }
         }
         if(name.find_first_not_of("abcdefghijklmnopqrstuvwxyz._1234567890") !=
            std::string::npos){
             SPDLOG_ERROR("Lookup name({}) invalid",name);
             throw std::invalid_argument(name);
         }
         std::scoped_lock<RWMutexType::WriteView> lock(GetRwMutex().Writer());
         auto iter = GetConfigMap().find(name);
         if(iter != GetConfigMap().end()){
             return std::dynamic_pointer_cast<ConfigVar<T>>(iter->second);
         }
         typename ConfigVar<T>::ptr val = std::make_shared<ConfigVar<T>>(name,defaultVal,description);
         GetConfigMap()[name] = val;
         return val;
     }

     template<class T>
     static typename ConfigVar<T>::ptr Lookup(const std::string& name){
         std::scoped_lock<RWMutexType::ReadView> lock(GetRwMutex().Reader());
         auto iter = GetConfigMap().find(name);
         if(iter == GetConfigMap().end()){
             return nullptr;
         }
         auto p = std::dynamic_pointer_cast<ConfigVar<T>>(iter->second);
         if(p){
             SPDLOG_ERROR("Lookup name({}) exists",name);
             return p;
         }else{
             SPDLOG_ERROR("Lookup name({}) exists but type not {},real type = {} {}",name,TypeToName<T>(),iter->second->getTypeName(),iter->second->toString());
             return nullptr;
         }
     }

     static void LoadFromYaml(const YAML::Node& rootNode);
     //static void LoadFromConfDir(const std::string& path,bool force = false);

     static ConfigBase::ptr LookupBase(const std::string&name);

     static void Visit(std::function<void(ConfigBase::ptr)>cb);


 private:
     static ConfigVarMap &GetConfigMap(){
         static ConfigVarMap s_data;
         return s_data;
     }

     static RWMutexType &GetRwMutex(){
         static RWMutexType rwMutex;
         return rwMutex;
     }
 };
}

#endif //XZMJX_CONFIG_H
