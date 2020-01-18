#ifndef GSTRING_H
#define GSTRING_H
#include  "foo.h"
using std::string;
class GString : public string
{
    private:
    public:
        //constructor
        GString();
        template <typename T>
        GString(const T&s){
            this->assign(s);
        }
        GString(const char* s){
            this->assign(s);
        }
        template <typename T>
        GString& operator=(T& s){
            this->assign(s);
            return *this;
        } ;
        ~GString();
    public:
        //maniulator
        operator const char* () const{
            return c_str();
        }
        void format(const char* fmt, ...);
        void out(const char* fmt, ...);
        void out();
        //accessor
};
#endif

