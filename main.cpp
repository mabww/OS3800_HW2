#include <vector>
#include <iostream> // cout cin
#include <iomanip> // formating with setw
#include <chrono> // sleep times
#include <thread> // sleep_for
#include <math.h> // pow
#include <string> // strings stoi
#include "scheduler.h" // scheduler functions and data objects

using namespace std;

int main(int argc, char* argv[])
{
    // file name
    string file;
    // sleep time this will larger number will increase simulation time
    chrono::duration<int, std::milli> sleepTime = chrono::milliseconds(500);

    // list of processes
    vector<Process> procList;
    // input and schedChoice are practically the same thing.
    // timeQuantum is used for the roundRobin scheduler.
    int input, schedChoice, timeQuantum;
    // boolean use to end the scheduling when ever all proccess are done
    bool done;
    // used for printng the simulation
    string tempStr;

    // This is the default for when you run the code. 
    // It is to help you understand how to run the program.
    if(argc == 1)
    {
        cout << "Improper run command" << endl;
        cout << "./program filename simulationSpeed" << endl;
        cout << "EXAMPLE: ./program procList.txt 20" << endl;
        return 0;
    }
    // ./program filename
    else if(argc == 2)
    {
        file = argv[1];
    }
    // ./program filename simulationSpeed
    else if(argc == 3)
    {
        file = argv[1];
        sleepTime = chrono::milliseconds(stoul(argv[2]));
    }
    // too many arguments
    else
    {
        cout << "Invalid";
        return 1;
    }

    // Read in the processes to be done.
    readInProcList(file, procList);
    int numProc = procList.size();

    //Welcome message and tells you your filename and simulation speed
    cout << "\nWelcome to the scheduler simulator\n\n";
    cout << "File Name: " << file << endl;
    cout << "Simulation Speed: " << sleepTime.count() << " milliseconds"<< endl;

    schedChoice = -1;

    // select the scheduler you want to use.
    do
    {
        cout << "\nWhat scheduler would you like to test?\n\n"
            << "1) Round Robin\n"
            << "2) Shortest Process Next 2 at a time\n"
            << "3) Shortest Remaining Time\n"
            << "4) Highest Response Ratio Next\n";
        cout << "\n--> ";
        cin >> input;

        if(input > 0 && input <= 4)
        {
            schedChoice = input;
        }
        else
        {
            cout << "INVALID ENTRY\n\n";
        }
    } while(schedChoice == -1 || schedChoice > 4);
    
    if(schedChoice == 1)
    {
        cout << "Enter the time quantum you would like to use: ";
        cin >> timeQuantum;
    }


    // cout the header for the simulation
    cout << "\nStarting simulation\n"
        << "   O: Process scheduled\n"
        << "   X: Process completed\n"
        << "   !: Completed process scheduled more time than needed\n\n"
        << "Time ";
    tempStr = "-----";
    for(int i = 0; i < numProc; ++i)
    {
        cout << "| " << setw(4) << procList[i].id.substr(0,4) << " ";
        tempStr += "-------";
    }
    cout << "| IDLE |\n" << tempStr << "--------\n";

    done = false;
    int curTime = 0;
    int idx = -1; // idle state
    vector<int> procIdx = {}; // {-1, -1} and {some_number, -1} state

    while(!done)
    {
        switch(schedChoice)
        {
            case 1:
                // single threaded
                idx = RoundRobin(curTime, procList, timeQuantum);
                break;
            case 2:
                // multi threaded
                procIdx = ShortestProcessNext(curTime, procList, 2);
                break;
            case 3:
                // single threaded
                idx = ShortestRemainingTime(curTime, procList);
                break;
            case 4:
                // single threaded
                idx= HighestResponseRatioNext(curTime, procList);
                break;
        }

        // this is a multithreaded schedule so it will have to be output in this way for it output correctly.
        if(schedChoice == 2)
        {
            // sorting it to make sure it outputs correctly.
            if(procIdx.size() == 2)
                sort(procIdx.begin(), procIdx.end());
            while(procIdx.size() != 2)
                procIdx.push_back(-1);
                
            // this for loop handles increasing the schedule time and marking a process as done.
            for(int i = 0, procIdxSize = procIdx.size(); i < procIdxSize; i++)
            {
                int j = procIdx[i];
                if(j != -1 && j < static_cast<int>(procList.size()))
                {
                    procList[j].timeScheduled++;
                    if(procList[j].totalTimeNeeded == procList[j].timeScheduled)
                    {
                        procList[j].isDone = true;
                        procList[j].timeFinished = curTime;
                    }
                }
            }

            // current time step.
            cout << setw(4) << curTime;
            // true untill proved otherwise.
            done = true;
            // goes through the processes.
            long unsigned int k = 0;
            for(int i = 0; i < numProc; i++)
            {   
                // if the process is done then the simulation is not done.
                done = done && procList[i].isDone;
                if(i == procIdx[k])
                {   
                    // this is just a thing to prevent indexing outside of the range.
                    k++;
                    if(k == procIdx.size())
                    {
                        k--;
                    }

                    // if the processes is done then we have 2 options for what to print.
                    if(procList[i].isDone)
                    {
                        // if you schedule a process that is already done then it is being over scheduled.
                        if(procList[i].timeScheduled > procList[i].totalTimeNeeded)
                        {
                            cout << " |   ! ";
                        }
                        // like x mark the spot the process has reached the treasure chest of being done.
                        else
                        {
                            cout << " |   X ";
                        }
                    }
                    // if the process is scheduled then we use an O to mark it as processing.
                    else
                    {
                        cout << " |   O ";
                    }
                }
                // if the process is not scheduled then it will just be blank.
                else
                {
                    cout << " |     ";
                }
            }

            // gets how idol it is
            int numIdol = count(procIdx.begin(), procIdx.end(), -1);

            if(numIdol == 0)
            {
                cout << " |     ";
            }
            // 1 to 2 idol
            else
            {
                cout << " |   " << numIdol << " "; 
            }

            // end of the line
            cout << " |" << endl;
            
        }
        else
        {
            // update the process that is being processed
            if(idx >= 0 && idx < static_cast<int>(procList.size()))
            {
                //update the details for the scheduled process
                procList[idx].timeScheduled++;
                if(procList[idx].totalTimeNeeded == procList[idx].timeScheduled)
                {
                    procList[idx].isDone = true;
                    procList[idx].timeFinished = curTime;
                }
            }

            // this is mostly the same as the other one
            cout << setw(4) << curTime;
            done = true;
            
            for(int i = 0; i < numProc; i++)
            {
                done = done && procList[i].isDone;

                if(i == idx)
                {
                    if(procList[i].isDone)
                    {   
                        if(procList[i].timeScheduled > procList[i].totalTimeNeeded)
                        {
                            cout << " |   ! ";
                        }
                        else
                        {
                            cout << " |   X ";
                        }
                    }
                    else
                    {
                        cout << " |   O ";
                    }
                }
                else
                {
                    cout << " |     ";
                }
            }

            if(idx == -1)
            {
                cout << " |   1 ";
            }
            else
            {
                cout << " |     ";
            }
            cout << " |" << endl;
        }

        if(!done)
        {
            this_thread::sleep_for(sleepTime);
            curTime = curTime + 1;
        }
    }

      cout << "\n\nRun Statistics:\n";

    //TODO output the requested run stats
    //cout << "\n\nRun Statistics:\n";
    //cout << "Process | Finish Time | Turnaround Time | Normalized Turnaround Time |" << endl;
    //cout << "----------------------------------------------------------------------" << endl;


    //cout << "----------------------------------------------------------------------" << endl;
    //cout << setw(7) << "   Mean | ";
    //cout << setw(14) << " | ";
    return 0;

}
