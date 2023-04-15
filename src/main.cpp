#include <glog/logging.h>

int main(int argc, char** argv) 
{
  // 初始化 glog 库
  google::InitGoogleLogging(argv[0]);

  // 设置日志输出目录
  google::SetLogDestination(google::GLOG_INFO, "./log");

  // 设置日志输出最小级别为 INFO
  google::SetStderrLogging(google::GLOG_INFO);

  // 记录日志
  LOG(INFO) << "This is an informational message.";
  LOG(WARNING) << "This is a warning message.";
  LOG(ERROR) << "This is an error message.";

  // 关闭 glog 库
  google::ShutdownGoogleLogging();

  return 0;
}