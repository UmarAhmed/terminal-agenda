#include <fstream>
#include <string>
#include <map>
#include <boost/serialization/map.hpp>
#include <time.h>

#include "agenda.h"

using namespace std;

const int GAP = 5;
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


// Check if a date is valid
bool valid(int month, int day) {
    return ((0 < month) && (month <= 12) &&
            (0 < day) && (day <= 31));
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
        // Obtain the current month (0 - 11) and day (1 - 31)
        time_t current = time(nullptr);
        tm * time_ptr = localtime(&current);
        int month = time_ptr->tm_mon + 1;
        int day = time_ptr->tm_mday;
        // Iterate through events and see if date is in range
        // If it's in range then print to std::cout
        bool deadlines = 0;
        for (auto x: agenda) {
            /* auto-remove old events - should there be a delay between deletion? */
            if (x.second.month < month) {
                agenda.erase(x.first);
            }
            if (x.second.month == month) {
                if (x.second.day < day) {
                    agenda.erase(x.first);
                } else if ((x.second.day - day) < GAP) {
                    if (deadlines == 0) {
                        deadlines = 1;
                        cout << "Deadlines in the next " << GAP << " days:" << endl;
                    }
                    cout << '\t' << x.first << " " << x.second << endl;
                }
            }
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
            cout << "What event would you like to remove?" << endl << "> ";
            for (auto x: agenda) {
                cout << x.first << endl;
            }
            getline(cin, word);
            try {
                agenda.erase(word);
            } catch (...) {
                cout << "Event ''" << word << "'' not found" << endl;
            }
        } else {
            cout << "Unrecognized command: " << word << endl;
        }
    }
    ofstream file {"agenda_data"};
    boost::archive::text_oarchive oarch(file);
    oarch << const_cast<map<string, Date> const &>(agenda);

}
