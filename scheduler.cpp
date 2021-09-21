#include "scheduler.h"
 
int RoundRobin(const int& curTime, const vector<Process>& procList, const int& timeQuantum)
{

  static int timeToNextSched = 0;  //keeps track of when we should actually schedule a new process
  static deque<int> ready;  //keeps track of the processes that are ready to be scheduled
  static int selection = -1;
  
  for(int i = 0; i<static_cast<int>(procList.size()); i++)
  {
    if(procList[i].startTime == curTime)
      ready.push_back(i);
  }
// this is a bit of code you might find useful for debuging
//  for(auto& x : ready)
//  {
//    cout << x << " ";
//  }
//  cout << endl;

  // for if process selected is done
  if(selection != -1)
  {
    if(procList[selection].isDone == true)
    {
      selection = -1;
      timeToNextSched = timeQuantum;
      if(ready.size() != 0)
      {
        selection = ready[0];
        ready.pop_front();
      }
      else
      {
        timeToNextSched = 0;
      }
    }
  }

  // for if it is time to do a new schedule
  if(timeToNextSched == 0)
  {
    // if the selection is not -1 then we can just do this
    if(selection != -1)
    {
      ready.push_back(selection);
      selection = ready[0];
      ready.pop_front();
      timeToNextSched = timeQuantum;
    }
    // if the selection is -1 then we should check if there is anything that we could put there instead.
    else if(ready.size() > 0)
    {
      selection = ready[0];
      ready.pop_front();
      timeToNextSched = timeQuantum;
    }
  }
  // if timeToNextSchedule is a positive number then decrement it once per time step.
  if(timeToNextSched > 0)
  {
    timeToNextSched--;
  }
  // return selection could be a positive number within the size of the procList or -1.
  return selection;
}
