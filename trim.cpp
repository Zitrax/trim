#include <iostream>

#include <chrono>
#include <utility>

typedef std::chrono::high_resolution_clock::time_point TimeVar;

#define duration(a) std::chrono::duration_cast<std::chrono::microseconds>(a).count()
#define timeNow() std::chrono::high_resolution_clock::now()

template<typename F, typename... Args>
double funcTime(F func, Args&& ... args){
    const int count = 10000;
    TimeVar t1=timeNow();
    for(int i=0; i<count; ++i) {
	std::string s("  a  ");
	func(s, std::forward<Args>(args)...);
	s = "uuuuuuuuuuuuuuuuuuuuuu";
	func(s, std::forward<Args>(args)...);
	s = "";
	func(s, std::forward<Args>(args)...);
	s = "       u";
	func(s, std::forward<Args>(args)...);
	s = "jjjjjjjjjjjj  ";
	func(s, std::forward<Args>(args)...);
    }
    return duration(timeNow()-t1);
}

#include <algorithm> 
#include <functional> 
#include <cctype>
#include <locale>

// trim from start (in place)
static inline void ltrimA(std::string &s) {
    s.erase(s.begin(), std::find_if(s.begin(), s.end(),
            std::not1(std::ptr_fun<int, int>(std::isspace))));
}
// trim from end (in place)
static inline void rtrimA(std::string &s) {
    s.erase(std::find_if(s.rbegin(), s.rend(),
            std::not1(std::ptr_fun<int, int>(std::isspace))).base(), s.end());
}
// trim from both ends (in place)
static inline void trimA(std::string &s) {
    ltrimA(s);
    rtrimA(s);
}
// trim from start (copying)
static inline std::string ltrimmedA(std::string s) {
    ltrimA(s);
    return s;
}
// trim from end (copying)
static inline std::string rtrimmedA(std::string s) {
    rtrimA(s);
    return s;
}
// trim from both ends (copying)
static inline std::string trimmedA(std::string s) {
    trimA(s);
    return s;
}

#include <boost/algorithm/string.hpp>

inline std::string trimC(const std::string &s)
{
   auto wsfront=std::find_if_not(s.begin(),s.end(),[](int c){return std::isspace(c);});
   auto wsback=std::find_if_not(s.rbegin(),s.rend(),[](int c){return std::isspace(c);}).base();
   return (wsback<=wsfront ? std::string() : std::string(wsfront,wsback));
}

inline std::string trimC2(const std::string &s)
{
   auto  wsfront=std::find_if_not(s.begin(),s.end(),[](int c){return std::isspace(c);});
   return std::string(wsfront,std::find_if_not(s.rbegin(),std::string::const_reverse_iterator(wsfront),[](int c){return std::isspace(c);}).base());
}

#include <sstream>

inline std::string trimD(std::string &str)
{
    std::stringstream trimmer;
    trimmer << str;
    str.clear();
    trimmer >> str;
    return str;
}

inline std::string trimE(std::string& str)
{
    str.erase(0, str.find_first_not_of(' '));       //prefixing spaces
    str.erase(str.find_last_not_of(' ')+1);         //surfixing spaces
    return str;
}

const char* ws = " \t\n\r\f\v";
// trim from end of string (right)
inline std::string& rtrimF(std::string& s, const char* t = ws)
{
    s.erase(s.find_last_not_of(t) + 1);
    return s;
}
// trim from beginning of string (left)
inline std::string& ltrimF(std::string& s, const char* t = ws)
{
    s.erase(0, s.find_first_not_of(t));
    return s;
}
// trim from both ends of string (left & right)
inline std::string& trimF(std::string& s, const char* t = ws)
{
    return ltrimF(rtrimF(s, t), t);
}

std::string trimG(const std::string &s)
{
    std::string::const_iterator it = s.begin();
    while (it != s.end() && isspace(*it))
        it++;

    std::string::const_reverse_iterator rit = s.rbegin();
    while (rit.base() != it && isspace(*rit))
        rit++;

    return std::string(it, rit.base());
}

int main() {
    std::cout << "A (inpl): " << funcTime(trimA) << std::endl;
    std::cout << "A (copy): " << funcTime(trimmedA) << std::endl;
    // FIXME: cost of lambda ?
    std::cout << "B (inpl): " << funcTime([](std::string& s){ return boost::trim(s); }) << std::endl;
    std::cout << "B (copy): " << funcTime([](std::string& s){ return boost::trim_copy(s); }) << std::endl;

    std::cout << "C (copy): " << funcTime(trimC) << std::endl;
    std::cout << "C (cop2): " << funcTime(trimC2) << std::endl;

    std::cout << "D (inpl): " << funcTime(trimD) << std::endl;
    std::cout << "E (inpl): " << funcTime(trimE) << std::endl;
    std::cout << "F (inpl): " << funcTime(trimF, ws) << std::endl;
    std::cout << "G (copy): " << funcTime(trimG) << std::endl;
    return 0;
}
