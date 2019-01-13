/*
This module is for recording important dates, and sending reminders
when those dates approach. This all happens through your terminal
*/

#include <iostream>
#include <boost/archive/text_oarchive.hpp>
#include <boost/archive/text_iarchive.hpp>

const char * months[12] = {"January", "February", "March", "April", "May", 
    "June", "July", "August", "September", "October", "November", "December"};

struct Date { 
    int month, day;
    template <class Archive>
    void serialize(Archive & ar, const unsigned int version) {
        ar & month;
        ar & day;
    }
};

std::ostream& operator<<(std::ostream & out, Date d) {
    return out << months[d.month - 1] << " " << d.day;
}


