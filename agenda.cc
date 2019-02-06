#include <fstream>
#include <string>
#include <map>
#include <boost/serialization/map.hpp>
#include <time.h>

#include "agenda.h"

using namespace std;

const int GAP = 6;
const string SILENT = "-s";


// TODO:
// move some things to functions to clean up
// dynamic gap/priority


// Load in map from file (deserialize)
map<string, Date> load() {
    ifstream file {"agenda_data"};
    map <string, Date> agenda;
    boost::archive::text_iarchive iarch(file);
    iarch >> agenda;
    return agenda;
}

void save(map<std::string, Date>& agenda) {
    ofstream file {"agenda_data"};
    boost::archive::text_oarchive oarch(file);
    oarch << const_cast<map<std::string, Date> const &>(agenda);
}

// Returns length of the mth month where January is month 1
int monthLength(int m) {
    if (m == 11 || m == 9 || m == 6 || m == 4) {
        return 30;
    } else if (m == 2) {
        return 28;
    } else {
        return 31;
    }
}


// Check if a date is valid
bool valid(const int month, const int day) {
    return ((0 < month) && (month <= 12) &&
            (0 < day) && (day <= 31));
}


// Count number of days since January 1
int countDays(int month, int day) {
    month--;
    while (month > 0) {
        day += monthLength(month);
        month--;
    }
    return day;
}


int main(int argc, char * argv[]) {
    // Deserialize file to obtain map
    map<string, Date> agenda;
    try {
        agenda = load(); // fail if file doesn't exist
    } catch (...) {
        cout << "no existing agenda file" << endl;
    }

    // Check for "-s", which indicates silent mode
    // In silent mode we only check for impending deadline and print
    if ((argc > 1) && (argv[1] == SILENT)) {
        // Obtain days since January 1st
        time_t current = time(nullptr);
        tm * time_ptr = localtime(&current);
        int today = countDays(time_ptr->tm_mon + 1, time_ptr->tm_mday);
        // Iterate through events and see if date is in range
        // If it's in range then print to std::cout, erase old deadlines
        bool deadlines = 0;
        bool deleted = 0;
        for (auto x: agenda) {
            int distance = today - countDays(x.second.month, x.second.day);
            if (distance > 0) {
                agenda.erase(x.first);
                deleted = 1;
            } else if (distance > -GAP) {
                if (deadlines == 0) {
                    deadlines = 1;
                    cout << "Deadlines in the next " << GAP << " days:" << endl;
                }
                cout << '\t' << x.first << " " << x.second << endl;
            }
        }
        if (deleted) {
            save(agenda);
        }
        return 0;
    }
    // not silent mode so interact w user
    string word, date;
    int month, day;

    cout << "Welcome to your agenda" << endl;
    while (true) {
        cout << "What would you like to do? (add/view/wipe/remove/quit)" << endl;
        cout << "> ";
        getline(cin, word);
        if (word == "add") {
            // TODO add loop somewhere so we can keep adding
            cout << "Enter the name of the event" << endl << "> ";
            getline(cin, word);
            // TODO make date easier to work with
            cout << "Enter the date eg. 1 1 is January 1st" << endl;
            cout << "> ";
            getline(cin, date);
            stringstream s {date};
            s >> month >> day;
            if (valid(month, day)) {
                agenda[word] = Date{month, day};
            } else {
                cout << "Invalid date..." << endl;
                continue;
            }
        } else if (word == "view") {
            for (auto x: agenda) {
                cout << '\t' << x.first << " " << x.second << endl;
            }
        } else if (word == "quit" || word[0] == 'q') {
            break;
        } else if (word == "wipe") {
            agenda.clear();
        } else if (word == "remove") {
            cout << "What event would you like to remove?" << endl;
            for (auto x: agenda) {
                cout << '\t' << x.first << endl;
            }
            cout << "> ";
            getline(cin, word);
            if (agenda.count(word) == 0) {
                cout << "Event not found" << endl;
            } else {
                agenda.erase(word);
                cout << "Removed event successfully" << endl;
            }
        } else {
            cout << "Unrecognized command: " << word << endl;
        }
    }
    save(agenda);
    //ofstream file {"agenda_data"};
    //boost::archive::text_oarchive oarch(file);
    //oarch << const_cast<map<string, Date> const &>(agenda);
}
