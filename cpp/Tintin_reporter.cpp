#include "Tintin_reporter.hpp"

Tintin_reporter::Tintin_reporter(void)
{
    this->file.open ("matt_daemon.log", std::fstream::app | std::fstream::out);
    
}

Tintin_reporter::Tintin_reporter(Tintin_reporter const &src)
{
    *this = src;
}

Tintin_reporter::~Tintin_reporter(void) {
    this->file.close();
}

Tintin_reporter &Tintin_reporter::operator=(Tintin_reporter const &rhs)
{
    return *this;
}

void Tintin_reporter::logtime(void)
{
    this->time = std::time(nullptr);
    if (std::strftime(this->timeFormat, sizeof(this->timeFormat), "[%d/%m/%Y-%H:%M:%S]", std::localtime(&this->time)) == 0)
    {
        this->timeFormat[0] = '\0';
    }
}

void Tintin_reporter::log(std::string msg)
{
    this->logtime();
    this->file << this->timeFormat << " [ LOG ] - " << msg;
    this->file.flush();
    
}

void Tintin_reporter::info(std::string msg)
{
    this->logtime();
    this->file << this->timeFormat << " [ INFO ] - " << msg;
    this->file.flush();
}

void Tintin_reporter::error(std::string msg)
{
    this->logtime();
    this->file << this->timeFormat << " [ ERROR ] - " << msg;
    this->file.flush();
}