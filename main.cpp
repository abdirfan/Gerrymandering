//
//  Project 1 Starter Code - Gerrymandering
//  TODO:  Replace this comment with your header info.
//
//

#include "ourvector.h"
#include <iostream>
#include <fstream>
#include <sstream>

using namespace std;

struct District
{
    int distrNo;
    int demVotes;
    int repVotes;
};

struct State
{
    string name;
    int eligibleVoters;
    ourvector<District> districts;
};

/**
 * reads in the data from the file and stores it in the vector of states
 * @param states the vector of states
 * @param filename the name of the file to read from
 */
bool caseInsensitiveStringCompare(string &s1, string &s2)
{
    // if the strings are not the same length, they are not equal
    if (s1.length() != s2.length())
    {
        return false;
    }

    // compare each character in the string
    for (size_t i = 0; i < s1.length(); i++)
    {
        // case insensitive comparison
        // make sure the characters are the same, regardless of case
        if (tolower(s1[i]) != tolower(s2[i]))
        {
            return false;
        }
    }
    return true;
}

/**
 * finds the index of a state in the vector of states
 * @param states the vector of states
 * @param name the name of the state to find
 * @return the index of the state in the vector, or -1 if not found
 */
int findState(ourvector<State> &states, string name)
{
    //cout << "findState: " << name << endl;
    for (int i = 0; i < states.size(); i++)
    {
        if (caseInsensitiveStringCompare(states[i].name, name))
        {
            // the state was found
            return i;
        }
        else
        {
            //cout << "not found: |" << states[i].name << "| !=|" << name << "|" << endl;
        }
    }
    return -1;
}

/**
 * @brief loads the data from the files into the vector of states
 *
 * @param states the vector of states, which will be filled with the data from the files
 * @param districts the name of the file containing the districts data
 * @param voters  the name of the file containing the voters data
 * @return true if the files were opened successfully and the data was loaded into the vector
 */
bool load(ourvector<State> &states, string districts, string voters)
{
    // first load the districts information
    ifstream dis(districts);
    ifstream vot(voters);
    // check if the files were opened successfully
    if (!dis.is_open())
    {
        cout << "Invalid first file, try again.\n";
        return false;
    }

    states.clear();

    cout << "Reading: " << districts << endl;
    string line;
    string tmp;
    // while there are still lines to read in the districts file
    while (getline(dis, line))
    {
        // create a string stream from the line
        // this allows us to read from the line as if it were a file
        stringstream ss(line);

        // create a new state
        State s;
        // get the state name
        getline(ss, s.name, ','); // read up to the comma

        while (ss)
        {
            // create a new district
            District d;

            // find first district ID
            //(it can be an integer or AL)
            d.distrNo = 1;
            getline(ss, tmp, ',');

            if (!ss) // if we are at the end of the line,
                break;

            if (tmp != "AL")
                d.distrNo = stoi(tmp);


            // find first district dem votes
            getline(ss, tmp, ',');
            d.demVotes = stoi(tmp);

            // find first district rep votes
            getline(ss, tmp, ',');
            d.repVotes = stoi(tmp);

            // add the district to the state
            s.districts.push_back(d);
        }

        states.push_back(s);
        cout << "..." << s.name << "..." << s.districts.size() << " districts total\n";
    }

    if (!vot.is_open())
    {
        cout << "\nInvalid second file, try again.\n";
        return false;
    }
    cout << "\nReading: " << voters << endl;

    // while there are still lines to read in the voters file
    while (getline(vot, line))
    {
        // create a string stream from the line
        // this allows us to read from the line as if it were a file
        stringstream ss(line);

        // get the state name
        getline(ss, tmp, ','); // read up to the comma

        // find the state in the vector
        int stateIndex = findState(states, tmp);

        // if the state was not found, skip this line
        if (stateIndex == -1)
        {
            continue;
        }

        // get the eligible voters
        getline(ss, tmp, ',');
        states[stateIndex].eligibleVoters = stoi(tmp);
        cout << "..." << states[stateIndex].name << "..." << states[stateIndex].eligibleVoters << " eligible voters\n";
    }

    dis.close();
    vot.close();
    return true;
}

void stats(ourvector<State> &states, int stateIndex)
{
    int total=0;
    int wastedDemVotes = 0;
    int wastedRepVotes = 0;
    State &s = states[stateIndex];

    for (int i = 0; i < s.districts.size(); i++)
    {
        District d = s.districts[i];
        int totalVotesInDistrict = d.demVotes + d.repVotes;
        int overHalf = totalVotesInDistrict / 2 + 1;
        // find the winner of the district
        total += totalVotesInDistrict;
        if (d.demVotes >= overHalf)
        {
            wastedRepVotes += d.repVotes;
            wastedDemVotes += d.demVotes - overHalf;
        }
        else
        {
            wastedDemVotes += d.demVotes;
            wastedRepVotes += abs(d.repVotes - overHalf);
        }
    }

    //cout<<"total votes: "<<total<<endl;
    double efficiencyGap = (double)(wastedDemVotes - wastedRepVotes) /total ;
    if (efficiencyGap < 0)
        efficiencyGap *= -1;

    cout << "Gerrymandered: ";
    if (s.districts.size()>=3 && efficiencyGap >= 0.07){
        cout << "Yes\n";
        cout<<"Gerrymandered against: ";
        if(wastedDemVotes > wastedRepVotes){
            cout<<"Democrats\n";
        }
        else{
            cout<<"Republicans\n";
        }

       printf("Efficiency Factor: %.4f%%\n", efficiencyGap*100);
    }

    else
        cout << "No\n";

    cout << "Wasted Democratic Votes: " << wastedDemVotes << endl;
    cout << "Wasted Republican Votes: " << wastedRepVotes << endl;
    cout << "Eligible Voters: " << s.eligibleVoters << endl;
}

void plot(ourvector<State> &states, int stateIndex)
{
    for (int i = 0; i < states[stateIndex].districts.size(); i++)
    {
        District d = states[stateIndex].districts[i];

        double demPercent = ((double)d.demVotes / (d.demVotes + d.repVotes)) * 100;
        int demBar = demPercent;
        int repBar = 100 - demBar;

        cout << "District: " << d.distrNo << "\n";
        for (int j = 0; j < demBar; j++)
        {
            cout << "D";
        }
        for (int j = 0; j < repBar; j++)
        {
            cout << "R";
        }

        cout << endl;
    }
}

int main()
{
    bool dataLoaded = false;
    int stateIndex = -1;
    ourvector<State> states;

    cout << "Welcome to the Gerrymandering App!\n";
    while (true)
    {
        cout << "\nData loaded? " << (dataLoaded ? "Yes" : "No") << endl;
        cout << "State: ";
        if (stateIndex != -1)
        {
            cout << states[stateIndex].name << endl;
        }
        else
        {
            cout << "N/A" << endl;
        }

        cout << "\nEnter command: ";
        string command;
        cin >> command;

        cout << "\n-----------------------------\n";

        if (command == "load")
        {
            string districts;
            string voters;
            cin >> districts >> voters;
            dataLoaded = load(states, districts, voters);
            continue;
        }

        // if data is not loaded, do not allow any other commands
        if (!dataLoaded)
        {
            cout << "\nData not loaded, please load data first.\n";
            continue;
        }

        if (command == "stats")
        {
            cout<<"\n";
            stats(states, stateIndex);
        }
        else if (command == "search")
        {
            string stateName;
            getline(cin, stateName);

            // remove the space at the beginning
            while (stateName[0] == ' ')
            {
                stateName.erase(0, 1);
            }

            stateIndex = findState(states, stateName);

            if (stateIndex == -1)
            {
                cout << "State does not exist, search again.\n";
            }
        }
        else if (command == "plot")
        {
            cout<<"\n";
            plot(states, stateIndex);
        }
        else if (command == "exit")
        {
            break;
        }
        else
        {
            cout<<"\n";
            cout << "Invalid command, try again.\n";
        }
    }
    return 0;
}