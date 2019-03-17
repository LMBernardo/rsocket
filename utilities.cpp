//--------------------------
// General Utilities
//--------------------------
// Description:
// Provides general utilities useful for multiple projects
// E.g., timestamped console logging
//--------------------------
// Author: Layne Bernardo
// Email: lmbernar@uark.edu
//
// Created August 11th, 2018
// Modified: August 11th, 2018
// Version 0.1
//--------------------------

#include "utilities.h"

void console_log(std::string message, std::string caller){
        time_t rawtime;
        struct tm * timeinfo;

        time ( &rawtime );
        timeinfo = localtime ( &rawtime );
        std::string timestamp = asctime( timeinfo );

        std::cout << "[" << timestamp << "] " << caller << ": ";
        std::cout << message << std::endl;
}
