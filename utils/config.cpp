//
// Created by 20132 on 2022/11/23.
//
#include "config.h"

#include "log.h"
namespace xzmjx {

static auto g_logger = log::GetDefault();
///展平所有配置项。例如
/// a:
///     b:xx
///     c:yy
/// listAllNode后-->a.b a.c
void ListAllNodes(std::string prefix, YAML::Node node,
                  std::list<std::pair<std::string, YAML::Node>>& ouput) {
  if (prefix.find_first_not_of("abcdefghjiklmnopqrstuvwxyz._1234567890") !=
      std::string::npos) {
    SPDLOG_LOGGER_ERROR(g_logger, "Config invaild name: {}:{}", prefix,
                        node.Scalar());
  }
  ouput.push_back(std::make_pair(prefix, node));
  if (node.IsMap()) {
    for (auto it = node.begin(); it != node.end(); it++) {
      ListAllNodes(prefix == "" ? prefix = it->first.Scalar()
                                : prefix + "." + it->first.Scalar(),
                   it->second, ouput);
    }
  }
}

void Config::LoadFromYaml(const YAML::Node& rootNode) {
  std::list<std::pair<std::string, YAML::Node>> all_node;
  ListAllNodes("", rootNode, all_node);
  for (auto it : all_node) {
    std::string key = it.first;
    std::cout << key << std::endl;
    if (key.empty()) {
      continue;
    }
    ConfigBase::ptr var = LookupBase(key);
    if (var) {
      if (it.second.IsScalar()) {
        var->fromString(it.second.Scalar());
      } else {
        std::stringstream ss;
        ss << it.second;
        var->fromString(ss.str());
      }
    }
  }
}
// void Config::LoadFromConfDir(const std::string& path,bool force){
//    std::vector<std::string> files;
//    std::string abs_path = EnvMgr::GetInstance()->getAbsolutePath(path);
//    FSUtil::ListAllFile(files,abs_path,".yml");
//    for(auto&& file:files){
//        try{
//            YAML::Node n = YAML::LoadFile(file);
//            LoadFromYaml(n);
//            XZMJX_LOG_INFO(g_logger) << "LoadConfFile file="
//                                     << file << " ok";
//        }catch(...) {
//            XZMJX_LOG_ERROR(g_logger)<<"LoadConfFile file="
//                                     <<file<<" fali";
//        }
//    }
//}

ConfigBase::ptr Config::LookupBase(const std::string& name) {
  std::scoped_lock<RWMutexType::ReadView> lock(GetRwMutex().Reader());
  auto iter = GetConfigMap().find(name);
  return iter == GetConfigMap().end() ? nullptr : iter->second;
}

void Config::Visit(std::function<void(ConfigBase::ptr)> cb) {
  std::scoped_lock<RWMutexType::ReadView> lock(GetRwMutex().Reader());
  ConfigVarMap& map = GetConfigMap();
  for (auto m : map) {
    cb(m.second);
  }
}
}  // namespace xzmjx
