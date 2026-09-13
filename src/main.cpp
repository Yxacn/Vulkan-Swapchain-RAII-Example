// main.cpp
// 程序入口：解析命令行参数并组装 Application，驱动主循环。
// 进程级职责集中在此：用法/版本输出、参数校验、异常收敛与退出码；
// 窗口与渲染资源的生命周期完全交给 Application 及其成员管理。

#include <charconv>
#include <cstdlib>
#include <exception>
#include <iostream>
#include <stdexcept>
#include <string>
#include <string_view>
#include <system_error>

#include "Application.hpp"
#include "Version.hpp"

namespace
{
    void printUsage(std::ostream& out)
    {
        out << "Usage: " << vkp::PROJECT_NAME << " [options]\n"
            << "  -h, --help         Show this help and exit\n"
            << "  -v, --version      Show version information and exit\n"
            << "  --width <pixels>   Window width (default " << vkp::DEFAULT_WINDOW_WIDTH << ")\n"
            << "  --height <pixels>  Window height (default " << vkp::DEFAULT_WINDOW_HEIGHT << ")\n"
            << "  --title <text>     Window title (default " << vkp::DEFAULT_WINDOW_TITLE << ")\n";
    }

    // 尺寸参数必须是正整数：含多余字符、溢出或非正数一律视为用法错误
    [[nodiscard]] int parseDimension(std::string_view text, std::string_view option)
    {
        int value = 0;
        const char* const begin = text.data();
        const char* const end = begin + text.size();
        const auto [parsedEnd, error] = std::from_chars(begin, end, value);
        if (error != std::errc{} || parsedEnd != end || value <= 0)
        {
            throw std::invalid_argument(std::string(option) + " expects a positive integer, got \"" +
                                        std::string(text) + "\"");
        }
        return value;
    }

    // 取下一个参数作为选项值；缺值时立即报错，避免把后续选项误当值消费
    [[nodiscard]] std::string_view requireValue(int argc, char* argv[], int& index, std::string_view option)
    {
        if (index + 1 >= argc)
        {
            throw std::invalid_argument(std::string(option) + " requires a value");
        }
        return argv[++index];
    }
} // namespace

int main(int argc, char* argv[])
{
    try
    {
        vkp::WindowInfo windowInfo;
        for (int i = 1; i < argc; ++i)
        {
            const std::string_view argument = argv[i];
            if (argument == "-h" || argument == "--help")
            {
                printUsage(std::cout);
                return EXIT_SUCCESS;
            }
            if (argument == "-v" || argument == "--version")
            {
                std::cout << vkp::PROJECT_NAME << ' ' << vkp::VERSION_STRING << '\n';
                return EXIT_SUCCESS;
            }
            if (argument == "--width")
            {
                windowInfo.width = parseDimension(requireValue(argc, argv, i, argument), argument);
                continue;
            }
            if (argument == "--height")
            {
                windowInfo.height = parseDimension(requireValue(argc, argv, i, argument), argument);
                continue;
            }
            if (argument == "--title")
            {
                windowInfo.title = requireValue(argc, argv, i, argument);
                continue;
            }
            throw std::invalid_argument("unknown option \"" + std::string(argument) + "\"");
        }

        vkp::Application app(windowInfo);
        app.run();
    }
    catch (const std::invalid_argument& error)
    {
        // 用法错误给出原因并附上帮助，避免用户只看到一行报错
        std::cerr << "Argument error: " << error.what() << "\n\n";
        printUsage(std::cerr);
        return EXIT_FAILURE;
    }
    catch (const std::exception& error)
    {
        std::cerr << "Fatal error: " << error.what() << '\n';
        return EXIT_FAILURE;
    }
    catch (...)
    {
        // 第三方库可能抛出非 std::exception 派生的异常，同样按失败退出
        std::cerr << "Fatal error: unknown exception!\n";
        return EXIT_FAILURE;
    }
    return EXIT_SUCCESS;
}
