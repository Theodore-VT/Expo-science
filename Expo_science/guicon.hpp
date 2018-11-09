#ifndef __GUICON_HPP__
#define __GUICON_HPP__
#ifdef _DEBUG

#include <string>

#ifndef UNICODE  
typedef std::string String;
#else
typedef std::wstring String;
#endif

void RedirectIOToConsole();

void print(const char* msg);
void print(String msg);
void print(std::string msg);

#endif
#endif

// End of File