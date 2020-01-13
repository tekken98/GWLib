//#include <cstdarg>
#include <stdarg.h>
#include "GString.h"

//GSTRING BEGIN
void GString::out()
{
    string s = *this;
    std::cout << s  << std::endl;
}
void GString::out(const char* fmt, ...)
{
    char buff[1024];
    va_list va;
    va_start(va,fmt);
    vsprintf(buff,fmt,va);
    va_end(va);
    assign(buff);
    out();
    //std::cout << buff  << std::endl;
}
void GString::format(const char* fmt, ...)
{
    char buff[1024];
    va_list va;
    va_start(va,fmt);
    vsprintf(buff,fmt,va);
    va_end(va);
    assign(buff);
}
GString::~GString()
{
}
GString::GString()
{
}
//GSTRING END
