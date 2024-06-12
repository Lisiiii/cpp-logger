#pragma once

#include <chrono>
#include <ctime>
#include <fstream>
#include <iomanip>
#include <iostream>
#include <mutex>
#include <stdexcept>
#include <string>

namespace Lisii {

// Logger类实现日志记录功能
class Logger {
public:
    // 日志等级
    enum class LogLevel {
        DEBUG,
        INFO,
        WARNING,
        ERROR
    };

    // 日志输出目标
    enum class LogTarget {
        FILE,
        TERMINAL,
        FILE_AND_TERMINAL
    };

    // 默认构造函数，初始化为输出到终端，日志等级为DEBUG
    Logger();

    // 带参数的构造函数，初始化输出目标、日志等级和文件路径
    Logger(LogTarget target, LogLevel level, const std::string& path);

    // 析构函数，负责关闭文件输出流
    ~Logger();

    // 各种日志记录函数
    void debug(const std::string& text);
    void info(const std::string& text);
    void warning(const std::string& text);
    void error(const std::string& text);

private:
    std::ofstream m_outfile; // 文件输出流对象
    LogTarget m_target; // 日志输出目标
    std::string m_path; // 日志文件路径
    LogLevel m_level; // 日志等级
    std::mutex m_mutex; // 互斥锁，确保线程安全

    // 实际的日志输出函数，根据日志等级和输出目标输出日志信息
    void output(const std::string& text, LogLevel act_level);

    // 获取当前时间，并返回格式化后的时间字符串
    std::string currTime();

    // 初始化文件输出流
    void init_file_stream();
};

// 获取当前时间，并返回格式化后的时间字符串
inline std::string Logger::currTime()
{
    auto now = std::chrono::system_clock::now();
    auto in_time_t = std::chrono::system_clock::to_time_t(now);
    auto ms = std::chrono::duration_cast<std::chrono::milliseconds>(now.time_since_epoch()) % 1000;

    std::ostringstream oss;
    oss << std::put_time(std::localtime(&in_time_t), "%Y-%m-%d %H:%M:%S");
    oss << '.' << std::setw(3) << std::setfill('0') << ms.count();

    return oss.str();
}

// 默认构造函数，初始化为输出到终端，日志等级为DEBUG
inline Logger::Logger()
    : m_target(LogTarget::TERMINAL)
    , m_level(LogLevel::DEBUG)
{
}

// 带参数的构造函数，初始化输出目标、日志等级和文件路径
inline Logger::Logger(LogTarget target, LogLevel level, const std::string& path)
    : m_target(target)
    , m_level(level)
    , m_path(path)
{
    if (target != LogTarget::TERMINAL) {
        init_file_stream(); // 初始化文件输出流
    }
    std::string strContent = "[" + currTime() + "]" + "<开始日志>\n";
    if (target != LogTarget::TERMINAL) {
        m_outfile << strContent; // 写入日志开始信息
    }
    if (target != LogTarget::FILE) {
        std::cout << strContent; // 在终端输出日志开始信息
    }
}

// 析构函数
inline Logger::~Logger()
{
    std::string strContent = "[" + currTime() + "]" + "<结束日志>\n\n";
    if (m_outfile.is_open()) {
        m_outfile << strContent; // 写入日志结束信息
        m_outfile.flush(); // 刷新输出流缓冲区
        m_outfile.close(); // 关闭文件输出流
    }
}

// 初始化文件输出流
inline void Logger::init_file_stream()
{
    m_outfile.open(m_path, std::ios::out | std::ios::app); // 打开文件输出流
    if (!m_outfile.is_open()) { // 检查文件是否成功打开
        throw std::runtime_error("\n[ERROR] Failed to open log file: " + m_path);
    }
}

// 调用output函数输出DEBUG级别日志
inline void Logger::debug(const std::string& text)
{
    output(text, LogLevel::DEBUG);
}

// 调用output函数输出INFO级别日志
inline void Logger::info(const std::string& text)
{
    output(text, LogLevel::INFO);
}

// 调用output函数输出WARNING级别日志
inline void Logger::warning(const std::string& text)
{
    output(text, LogLevel::WARNING);
}

// 调用output函数输出ERROR级别日志
inline void Logger::error(const std::string& text)
{
    output(text, LogLevel::ERROR);
}

// 实际的日志输出函数，根据日志等级和输出目标输出日志信息
inline void Logger::output(const std::string& text, LogLevel act_level)
{
    std::lock_guard<std::mutex> guard(m_mutex); // 加锁，确保线程安全

    // 如果当前日志等级高于实际日志等级，不输出日志
    if (m_level > act_level) {
        return;
    }

    std::string prefix;
    // 根据日志等级设置日志前缀
    switch (act_level) {
    case LogLevel::DEBUG:
        prefix = "[DEBUG] ";
        break;
    case LogLevel::INFO:
        prefix = "[INFO] ";
        break;
    case LogLevel::WARNING:
        prefix = "[WARNING] ";
        break;
    case LogLevel::ERROR:
        prefix = "[ERROR] ";
        break;
    }

    // 生成完整的日志输出内容
    std::string outputContent = "[" + currTime() + "]" + prefix + text + "\n";

    // 根据日志输出目标输出日志
    if (m_target != LogTarget::FILE) {
        std::cout << outputContent; // 输出到终端
    }
    if (m_target != LogTarget::TERMINAL && m_outfile.is_open()) {
        m_outfile << outputContent; // 输出到文件
        m_outfile.flush(); // 刷新输出流缓冲区
    }
}

} // namespace Lisii
