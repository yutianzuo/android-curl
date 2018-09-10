
/*
 * FileDes  : miscs.h
 * Abstract : util functions
 * Platform : linux
 * Author   : yutianzuo
 */
#ifndef __MISCS__H__TIMOTHY__
#define __MISCS__H__TIMOTHY__
//------------------------------------------------------------------------
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/time.h>
#include <sys/file.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <fcntl.h>
#include <sys/types.h>
#include <pwd.h>
#include <grp.h>

#include "string_x.h"


#ifndef FUNCTION_BEGIN
#define FUNCTION_BEGIN do{
#endif

#ifndef FUNCTION_LEAVE
#define FUNCTION_LEAVE break
#endif

#ifndef FUNCTION_END
#define FUNCTION_END }while(false)
#endif


inline
std::string get_uidname(uid_t uid)
{
    struct passwd *pw_ptr;

    if ((pw_ptr = ::getpwuid(uid)) == NULL)
    {
        char numstr[10] = {0};
        sprintf(numstr, "%d", uid);
        return stringxa(numstr);
    }
    else
    {
        return stringxa(pw_ptr->pw_name);
    }
}

inline
std::string get_gidname(gid_t gid)
{
    struct group *grp_ptr;

    if ((grp_ptr = ::getgrgid(gid)) == NULL)
    {
        char numstr[10] = {0};
        sprintf(numstr, "%d", gid);
        return stringxa(numstr);
    }
    else
    {
        return stringxa(grp_ptr->gr_name);
    }
}

/**
 * mod              | struname | strgname
 * 0777             | user     | group
 * 0-oct,4-R,2-W,1-X
 */
inline
bool get_fileprop(const char* path, std::string& mod, std::string& struname, std::string& strgname)
{
    if (!path)
    {
        return false;
    }
    struct stat file_stat;
    int ret = stat(path, &file_stat);
    if (ret == -1)
    {
        return false;
    }
    struname = get_uidname(file_stat.st_uid);
    strgname = get_gidname(file_stat.st_gid);

    int fmod = 0;
    if (file_stat.st_mode & S_IRUSR)
    {
         fmod += S_IRUSR;
    }

    if (file_stat.st_mode & S_IWUSR)
    {
        fmod += S_IWUSR;
    }

    if (file_stat.st_mode & S_IXUSR)
    {
        fmod += S_IXUSR;
    }

    if (file_stat.st_mode & S_IRGRP)
    {
        fmod += S_IRGRP;
    }

    if (file_stat.st_mode & S_IWGRP)
    {
        fmod += S_IWGRP;
    }

    if (file_stat.st_mode & S_IXGRP)
    {
        fmod += S_IXGRP;
    }

    if (file_stat.st_mode & S_IROTH)
    {
        fmod += S_IROTH;
    }

    if (file_stat.st_mode & S_IWOTH)
    {
        fmod += S_IWOTH;
    }

    if (file_stat.st_mode & S_IXOTH)
    {
        fmod += S_IXOTH;
    }

    char szmod[16] = {0};
    sprintf(szmod, "0%o", fmod);
    mod.assign(szmod);
    return true;
}


inline
void hex_printf(const unsigned char* sz_input, int len)
{
    if (!sz_input)
    {
        return;
    }
    for (int i = 0; i < len; ++i)
    {
        printf("%02X ", (unsigned char)sz_input[i]);
    }
    printf("\n");
}

inline
int get_cmdline(std::vector<std::string>& vec_cmds)
{

    const int n_lenth = 256;
    char sz_buff[n_lenth] = {0};

    int fd = open("/proc/self/cmdline", O_RDONLY);
    if (fd == -1)
    {
        return -1;
    }

    int n_ret = read(fd, sz_buff, n_lenth - 1);

    char* sz_tmp = sz_buff;
    vec_cmds.push_back(std::string(sz_tmp));
    for (int i = 0; i < n_ret; ++i)
    {
        if (sz_buff[i] == 0 && i + 1 < n_ret)
        {
            sz_tmp = &sz_buff[i+1];
            if (*sz_tmp != 0)
            {
            	vec_cmds.push_back(std::string(sz_tmp));
            }
        }
    }

    close(fd);

    return 0;
}

inline
void single_instance()
{
    int fd = open("sigle_instance_tmp", O_RDWR|O_CREAT, S_IRUSR|S_IWUSR);
    if (fd != -1)
    {
        struct flock lock_stru;
        lock_stru.l_len = 0;
        lock_stru.l_start = 0;
        lock_stru.l_type = F_WRLCK;
        lock_stru.l_whence = SEEK_SET;
        int n_ret = fcntl(fd, F_SETLK, &lock_stru);
        if (n_ret < 0)
        {
            if (EACCES == errno || EAGAIN == errno)
            {
                printf("sigle_instance hit!\n");
                _exit(0);
            }
            else
            {
                printf("fcntl err!\n");
            }
            close(fd);
        }
        else
        {
            printf("sigle_instance lauch!\n");
        }
    }
    else
    {
        printf("open err\n");
    }
}

inline
int dir_file_exist(const char* sz_file)
{
    stringxa str_file;
	if (!sz_file || (str_file.assign(sz_file)).size() == 0)
	{
		return 0;
	}

    str_file.trim();

	struct stat buf;
	int ret = lstat(str_file.c_str(), &buf);
	if (-1 == ret) //does not exist
	{
		return -1;
	}
	if (S_IFREG & buf.st_mode) //file
	{
		return 1;
	}
	if (S_IFDIR & buf.st_mode) //dir
	{
		return 2;
	}
	return 0; //other type
}

inline
bool mkdir_recursively(const char* sz_path_dir) //input should be a directory full path
{
	if (!sz_path_dir)
	{
		return false;
	}
	stringxa str_path(sz_path_dir);
	std::vector<stringxa> vec_part;
	if (0 != str_path.split_string("/", vec_part))
	{
        return false;
	}
	if (1  == vec_part.size())
	{
        stringxa str_folder = "/" + vec_part[0];
        return (mkdir(str_folder, S_IRWXU | S_IRWXG | S_IROTH | S_IXOTH) == 0);
	}

	stringxa str_head = "/" + vec_part[0];
	for (std::size_t i = 1; i < vec_part.size(); ++i)
	{
		str_head += "/";
		str_head += vec_part[i];
		if (2 != dir_file_exist(str_head.c_str()))
		{
			if (0 != mkdir(str_head, S_IRWXU | S_IRWXG | S_IROTH | S_IXOTH))
			{
				return false;
			}
		}
	}
	return true;
}

inline
bool get_app_path(std::string& str_app_path)
{
    const int max_path_len = 260;
	char sz_temp[max_path_len] = {0};
	int n_ret = readlink("/proc/self/exe", sz_temp, max_path_len);
	if (-1 == n_ret)
	{
        return false;
	}

	str_app_path = sz_temp;
	std::string::size_type pos = 0;
	pos = str_app_path.find_last_of('/');
	if (std::string::npos == pos)
	{
		return false;
	}
	str_app_path.erase(pos);
	str_app_path += "/";
	return true;
}

inline
bool get_current_formattime(stringxa& str_formattime)
{
    bool b_ret = false;
    FUNCTION_BEGIN;

    time_t now;
    struct tm *tm_now;
    if (-1 == time(&now))
    {
        FUNCTION_LEAVE;
    }
    if (!(tm_now = localtime(&now)))
    {
        FUNCTION_LEAVE;
    }

    str_formattime.format("%.4d-%.2d-%.2d %.2d:%.2d:%.2d",
    tm_now->tm_year + 1900, tm_now->tm_mon + 1, tm_now->tm_mday, tm_now->tm_hour,
    tm_now->tm_min, tm_now->tm_sec);

    b_ret = true;
    FUNCTION_END;
    return b_ret;
}

class LogUtil
{
public:
    LogUtil(const char* sz_log_path) : m_file_log(NULL)
    {
        if (sz_log_path)
        {
            m_file_log = fopen(sz_log_path, "a");
        }
        else
        {
            m_file_log = fopen("log_file.log", "a");
        }
    }
    LogUtil() : m_file_log(NULL)
    {
        m_file_log = fopen("log_file.log", "a");
    }
    ~LogUtil()
    {
        if (m_file_log)
        {
            fflush(m_file_log);
            fclose(m_file_log);
        }
    }

    void printf_to_log(const char* sz_log, ...)
    {
        if (!m_file_log)
            return;
        const int len = 512;
        char txt_buff[len] = {0};

        va_list argList2;
		va_start(argList2, sz_log);
		vsnprintf(txt_buff, len, sz_log, argList2);
        va_end(argList2);

        stringxa str_timeformat;
        get_current_formattime(str_timeformat);

        fwrite(str_timeformat.c_str(), str_timeformat.size(), 1, m_file_log);
        fwrite(": ", 2, 1, m_file_log);
        fwrite(txt_buff, strlen(txt_buff), 1, m_file_log);
    }
private:
    FILE* m_file_log;
};

class PrintExcuTime
{
public:
    PrintExcuTime() : m_sz_log_file(NULL)
    {
        gettimeofday(&m_timeval_begin, NULL);
    }

    PrintExcuTime(const char* log_file_path)
    {
        gettimeofday(&m_timeval_begin, NULL);
        m_sz_log_file = log_file_path;
    }
    ~PrintExcuTime()
    {
        gettimeofday(&m_timeval_end, NULL);
        if (!m_sz_log_file)
        {
            printf("execute time is : %ds, %dus\n", (int)(m_timeval_end.tv_sec - m_timeval_begin.tv_sec), (int)(m_timeval_end.tv_usec - m_timeval_begin.tv_usec));
        }
        else
        {
            LogUtil log_obj(m_sz_log_file);
            log_obj.printf_to_log("execute time is : %ds, %dus\n", (int)(m_timeval_end.tv_sec - m_timeval_begin.tv_sec), (int)(m_timeval_end.tv_usec - m_timeval_begin.tv_usec));
        }
    }
private:
    timeval m_timeval_begin;
    timeval m_timeval_end;
    const char* m_sz_log_file;
};

#ifndef PRINT_EXECUTE_TIME_BEGIN
#define PRINT_EXECUTE_TIME_BEGIN {PrintExcuTime _print_obj;
#endif

#ifndef PRINT_EXECUTE_TIME_TOLOG_BEGIN
#define PRINT_EXECUTE_TIME_TOLOG_BEGIN(file) {PrintExcuTime _print_obj(file);
#endif

#ifndef PRINT_EXECUTE_TIME_END
#define PRINT_EXECUTE_TIME_END }
#endif

#ifndef LOG_TO_FILE_BEING
#define LOG_TO_FILE_BEING {LogUtil _log_obj;
#endif

#ifndef LOG_TO_SPEC_FILE_BEING
#define LOG_TO_SPEC_FILE_BEING(file) {LogUtil _log_obj(file);
#endif

#ifndef LOG_TO_FILE_WRITE
#define LOG_TO_FILE_WRITE(...) _log_obj.printf_to_log(__VA_ARGS__);
#endif

#ifndef LOG_TO_FILE_END
#define LOG_TO_FILE_END }
#endif


//------------------------------------------------------------------------
#endif
