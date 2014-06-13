
#pragma once

#include <string>
#include <iostream>
#include <fstream>

inline void log_impl(unsigned long type, unsigned long level, const std::string& text)
{
	std::cout << text << std::endl;
}

inline void file_impl(unsigned long type, unsigned long level, const std::string& text)
{
	std::ofstream ofs;
	ofs.open("out.log", std::ios::app);
	ofs << text << std::endl;
}
