#ifndef __GUICON_HPP__
#define __GUICON_HPP__
#ifdef _DEBUG

#include <string>

void RedirectIOToConsole();

void print(const char* msg);
void print(std::string msg);

#endif
#endif

// End of File