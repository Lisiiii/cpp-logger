## 一个小小的c++日志类，很好理解，很好用
> 最低C++ 11
### 如何使用
将```Logger.hpp```引入你的项目,

include 它,

初始化它
```
// 示例: 日志同时输出到终端和文件，日志等级为WARNING,输出到logfile.txt文件
    Lisii::Logger logger_name(
        Lisii::Logger::LogTarget::FILE_AND_TERMINAL, 
        Lisii::Logger::LogLevel::WARNING, 
        "logfile.txt");
```
> 你也可以删除namespace Lisii或者把他改成你想要的名字，小心不要冲突
开始使用！记得设置正确的日志文件路径！
```
logger_name.debug("Some message.");
```
