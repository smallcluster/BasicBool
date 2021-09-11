// A very basic console logger with some template magic.

#pragma once

#include "defines.hpp"
#include <vector>
#include <iostream>
#include <sstream>

// Enable or disable warning & info level logging.
#define LOG_WARN_ENABLED 1
#define LOG_INFO_ENABLED 1

// Release mode should exclude debug info.
#if defined(BUILD_DEBUG)
#define LOG_DEBUG_ENABLED 1
#else
#define LOG_DEBUG_ENABLED 0
#endif

// Static class, DO NOT USE DIRECTLY ! USE THE DEFINED MACROS AT THE END OF THIS FILE.
class logger
{
private:
    // Split string at each "{}" patern.
    static const std::vector<string> splitMessage(const string &message)
    {
        std::vector<string> chunks;
        enum BraceMatching
        {
            OPENED,
            CLOSED,
            NONE
        };
        BraceMatching state = BraceMatching::NONE; // Current reading state
        std::stringstream ss;
        for (char const &c : message)
        {
            if (c == '{' && state != BraceMatching::OPENED)
                state = BraceMatching::OPENED;
            else if (c == '}' && state == BraceMatching::OPENED) // Match found !
            {
                state = BraceMatching::CLOSED;
                chunks.push_back(ss.str());
                ss = std::stringstream(); // Clear the string stream
            }
            else
                ss << c;
        }
        return chunks;
    }

    // Recursively show variadic template args.
    template <typename T>
    static void argsToStringStream(std::stringstream &ss, T v)
    {
        ss << v;
    }
    template <typename T, typename... Args>
    static void argsToStringStream(std::stringstream &ss, T first, Args... args)
    {
        ss << first << ", ";
        argsToStringStream(ss, args...);
    }

    // Recursively format the message with variadic template args.
    template <typename T>
    static void format(std::stringstream &ss, const std::vector<string> &chunks, int n, T v)
    {
        ss << chunks[n] << v;
    }

    template <typename T, typename... Args>
    static void format(std::stringstream &ss, const std::vector<string> &chunks, int n, T first, Args... args)
    {
        ss << chunks[n] << first;
        format(ss, chunks, n + 1, args...);
    }

public:
    enum LogLevel
    {
        LOG_FATAL,
        LOG_ERROR,
        LOG_WARN,
        LOG_INFO,
        LOG_DEBUG
    };

    // Simple text without formating.
    static void log(const LogLevel &level, const string &message)
    {
        std::stringstream ss;
        switch (level)
        {
        case logger::LOG_FATAL:
            ss << "[FATAL] : ";
            break;

        case logger::LOG_ERROR:
            ss << "[ERROR] : ";
            break;

        case logger::LOG_WARN:
            ss << "[WARN] : ";
            break;

        case logger::LOG_INFO:
            ss << "[INFO] : ";
            break;

        case logger::LOG_DEBUG:
            ss << "[DEBUG] : ";
            break;
        }
        ss << message;
        std::cout << ss.str() << std::endl;
    }

    // Formating is required with args.
    template <typename... Args>
    static void log(const LogLevel &level, const string &message, Args... args)
    {
        std::stringstream ss;

        // Message level type.
        switch (level)
        {
        case logger::LOG_FATAL:
            ss << "[FATAL] : ";
            break;

        case logger::LOG_ERROR:
            ss << "[ERROR] : ";
            break;

        case logger::LOG_WARN:
            ss << "[WARN] : ";
            break;

        case logger::LOG_INFO:
            ss << "[INFO] : ";
            break;

        case logger::LOG_DEBUG:
            ss << "[DEBUG] : ";
            break;
        }

        const std::vector<string> chunks = splitMessage(message);

        // Check if the formatting matchs with the number of args.
        constexpr auto size = sizeof...(Args);

        if (size != chunks.size())
        {
            std::stringstream listArgs;
            argsToStringStream(listArgs, args...);

            // Formating error !
            std::string text = "**Logger args number Error**\nMSG = " + ss.str() + message + "\nARGS : " + listArgs.str() + '\n';
            std::cout << text;
        }
        else
        {
            // Format and show the message.
            format(ss, chunks, 0, args...);
            std::cout << ss.str() << std::endl;
        }

        if (level == LOG_FATAL)
            abort();
    }
};

// Macros for ease of use and compile time optimization.

#if defined(__clang__) || defined(__GNUC__)
    #define LOGFATAL(message, ...) logger::log(logger::LOG_FATAL, message __VA_OPT__(,) __VA_ARGS__)
    #define LOGERROR(message, ...) logger::log(logger::LOG_ERROR, message __VA_OPT__(,) __VA_ARGS__)
    #if LOG_WARN_ENABLED
        #define LOGWARN(message, ...) logger::log(logger::LOG_WARN, message __VA_OPT__(,) __VA_ARGS__)
    #else
        #define LOGWARN(message, ...)
    #endif
    #if LOG_INFO_ENABLED
        #define LOGINFO(message, ...) logger::log(logger::LOG_INFO, message __VA_OPT__(,) __VA_ARGS__)
    #else
        #define LOGINFO(message, ...)
    #endif
    #if LOG_DEBUG_ENABLED
        #define LOGDEBUG(message, ...) logger::log(logger::LOG_DEBUG, message __VA_OPT__(,) __VA_ARGS__)
    #else
        #define LOGDEBUG(message, ...)
    #endif
#elif defined(_MSC_VER)
    #define LOGFATAL(message, ...) logger::log(logger::LOG_FATAL, message, __VA_ARGS__)
    #define LOGERROR(message, ...) logger::log(logger::LOG_ERROR, message, __VA_ARGS__)
    #if LOG_WARN_ENABLED
        #define LOGWARN(message, ...) logger::log(logger::LOG_WARN, message, __VA_ARGS__)
    #else
        #define LOGWARN(message, ...)
    #endif
    #if LOG_INFO_ENABLED
        #define LOGINFO(message, ...) logger::log(logger::LOG_INFO, message, __VA_ARGS__)
    #else
        #define LOGINFO(message, ...)
    #endif
    #if LOG_DEBUG_ENABLED
        #define LOGDEBUG(message, ...) logger::log(logger::LOG_DEBUG, message, __VA_ARGS__)
    #else
        #define LOGDEBUG(message, ...)
    #endif
#else
#error Unsupported compiler
#endif

// __VA_OPT__(,) __VA_ARGS__