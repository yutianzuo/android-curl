#ifndef TESTPROJ_TIMEUTILS_H
#define TESTPROJ_TIMEUTILS_H

#include <ctime>
#include <string>
#include <iomanip>
#include <iostream>
#include <thread>

class TimeUtils
{
public:
    static std::string get_zhCN_time()
    {
        const int buff_len = 256;
        std::time_t timeNow = std::time(nullptr);
        std::tm *tm = std::localtime(&timeNow); //assume local pc use zh_CN
        char buff[buff_len] = {0};
        std::strftime(buff, buff_len, "%Y年%m月%d日 %T", tm);
        return std::string(buff);
    }

    void time_click_begin()
    {
        m_time_pt_start = std::chrono::system_clock::now();
    }

    void time_click_end()
    {
        m_time_pt_end = std::chrono::system_clock::now();
    }

    template <typename TIME = std::chrono::milliseconds>
    typename TIME::rep sum_time()
    {
        auto elapsed = m_time_pt_end - m_time_pt_start;
        return std::chrono::duration_cast<TIME>(elapsed).count();
    }

    using elapsed_sec = std::chrono::seconds;
    using elapsed_micro = std::chrono::microseconds;
    using elapsed_milli = std::chrono::milliseconds;
    using elapsed_hour = std::chrono::hours;
    using elapsed_minute = std::chrono::minutes;

    template <typename TIME = std::chrono::milliseconds>
    static typename TIME::rep get_current_time()
    {
        return std::chrono::duration_cast<TIME>(std::chrono::system_clock::now().time_since_epoch()).count();
    }

    static void sleep_for_secs(std::size_t secs)
    {
        std::this_thread::sleep_for(std::chrono::seconds(secs));
    }

    static void sleep_for_millis(std::size_t millis)
    {
        std::this_thread::sleep_for(std::chrono::milliseconds(millis));
    }
private:
    std::chrono::system_clock::time_point m_time_pt_start;
    std::chrono::system_clock::time_point m_time_pt_end;

};

class TestTimeUtils
{
public:
    static void execute()
    {
        TimeUtils test;
        std::cout << TimeUtils::get_zhCN_time() << std::endl;

        test.time_click_begin();

        TimeUtils::sleep_for_millis(100);

        test.time_click_end();

        auto elapsed_micro = test.sum_time<TimeUtils::elapsed_micro>();
        auto elapsed_mill = test.sum_time();
        auto elapsed_sec = test.sum_time<TimeUtils::elapsed_sec>();

        std::cout<<"elapsed time mill is :" << elapsed_mill << std::endl;
        std::cout<<"elapsed time micro is :" << elapsed_micro << std::endl;
        std::cout<<"elapsed time secs is :" << elapsed_sec << std::endl;


        std::cout<<"now time stamp is:" <<TimeUtils::get_current_time()<<std::endl;
        std::cout<<"now time stamp is:" <<TimeUtils::get_current_time<TimeUtils::elapsed_hour >()<<std::endl;
    }
};

#endif //TESTPROJ_TIMEUTILS_H
