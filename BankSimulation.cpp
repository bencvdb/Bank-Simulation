// BankSimulation.cpp
// by Ben Van de Brooke
// On 10/31/16
// Editor: X-Code
// Compiler: GCC
// Description: A simulation of interactions at a bank

#include <iostream>
#include <vector>
#include <list>
#include <queue>
#include <string>
#include <sstream>
#include <fstream>
#include <cstdlib>
#include <cmath>
#include <ctime>


using namespace std;

struct Customer
{
  char name;
  int arrival, served, end;
  Customer():name('?'),arrival(0),served(0),end(0){};
  
  bool operator <(const Customer &otherCustomer) const
  {
    return name > otherCustomer.name;
  }

};




struct Message
{
  int serverNum, endTime;
  
  Message():serverNum(0),endTime(0){};
  
  //places the lowest time at the top of the priorityqueue
  bool operator <(const Message &otherMessage) const
  {
    return endTime > otherMessage.endTime;
  }
};




struct serverElements
{
  bool inUse;
  Customer aCustomer;
  
  serverElements(): inUse(false){};
  
};




int numEmptyWindows(const vector<serverElements> &windowBank)
{
  int boredTellers = 0;
  for(serverElements x : windowBank)
  {
    if(!x.inUse)
    {
      boredTellers++;
    }
  }
  return boredTellers;
}




//taken from canvas
int getRandomNumberOfServiceRequests(double averageRequestsPerTimeStep)
{
  int requests = 0;
  double probOfnRequests = exp(-averageRequestsPerTimeStep);
 
   for (double randomValue = (double)rand() / RAND_MAX;
        (randomValue -= probOfnRequests) > 0.0;
    probOfnRequests *= averageRequestsPerTimeStep / (double)++requests);
  return requests;
}




//getNextName
// returns the next letter of the alphabet, wraps back to A from Z
//taken from canvas
char getNextName(int &totalCustomersCreated)
{
  return (char)('A' + (totalCustomersCreated++ % 26));
}




void updateBank(const int & time, vector<serverElements>& windowBank, deque<Customer>& theLine, vector<Customer>& completedCustomers, int &numRejects, priority_queue<Message> &timer, const int &arrivalRate, const int &queueCap, int & totalCustomersCreated, const int &numServers,const int &minServiceTime,const int &maxServiceTime)
{
  //check to see if the person at the front of the windowQueue is ready to pop
    // if so pop them
    // check again
  
  int newCustomers = getRandomNumberOfServiceRequests(arrivalRate);
  bool customersLeaving = true;
  bool freeWindow = false;
  priority_queue<int> freeWindows;
  priority_queue<Customer> customersToAdd;    //used to add new customers to appropriate place
  Customer newCustomer;
  
  //creating list of free windows
  for(int x = 0; x < windowBank.size();x++)
  {
    if(!windowBank.at(x).inUse)
    {
      freeWindows.push(x);
    }
  }
  
  
  newCustomer.arrival = time;
  //makes a priorityQueue full of the customers to be added
  for(int x = 0; x < newCustomers; x++)
  {
    newCustomer.name = getNextName(totalCustomersCreated);
    
    customersToAdd.push(newCustomer);
  }
  
  
    //MOVING PEOPLE FROM THE WINDOWS TO COMPLETED CUSTOMER LIST
    //if there is a customer ready to move from the window
    while(customersLeaving)
    {
      if(!timer.empty() && timer.top().endTime == time)
      {
        int customerLocation = timer.top().serverNum;
      
        //adds them to the completed customers list
        windowBank.at(customerLocation).aCustomer.end = time;
        
        completedCustomers.push_back(windowBank.at(customerLocation).aCustomer);
        timer.pop();
      
        //clears customer from window
        windowBank.at(customerLocation).inUse = false;
        windowBank.at(customerLocation).aCustomer.name = '~';
        freeWindows.push(customerLocation);
        //freeWindow = true;
      
      }
      else
      {
        customersLeaving = false;
      }
    }
  
  
    //MOVING PEOPLE FROM THE LINE TO THE WINDOWS
    //if there is someone at the  front of the line, move them to an open window until there are no more empty windows.
    while(theLine.size() != 0 && freeWindows.size() > 0)
    {
    
    //find first free windown
      int firstFreeWindow = freeWindows.top();
      freeWindows.pop();
    
      // assign that elements aCustomer to theLine.front()
      theLine.front().served = time;
      windowBank.at(firstFreeWindow).aCustomer = theLine.front();
      windowBank.at(firstFreeWindow).inUse = true;
      
      
      Message newMessage;
      newMessage.serverNum = firstFreeWindow;
      newMessage.endTime = time + (rand() % (maxServiceTime-minServiceTime + 1) + minServiceTime);
      timer.push(newMessage);
    
      //takes away first in line
      theLine.pop_front();
  

      if(freeWindows.empty())
      {
        freeWindow = false;
      }
    }

  
    // LET PEOPLE INTO THE BANK
    
    while(customersToAdd.size() > 0)
    {
      if(freeWindows.size() > 0) //if free window available
      {
        int firstFreeWindow = freeWindows.top();
        freeWindows.pop();
        
        windowBank.at(firstFreeWindow).aCustomer = customersToAdd.top();
        windowBank.at(firstFreeWindow).inUse = true;
      
      
        Message newMessage;
        newMessage.serverNum = firstFreeWindow;
        newMessage.endTime = time + (rand() % (maxServiceTime-minServiceTime + 1) + minServiceTime);
        timer.push(newMessage);
      
        customersToAdd.pop();
      }
      else if (theLine.size() <  queueCap) // no room at window, add to line until its full
      {
        theLine.push_back(customersToAdd.top());
        customersToAdd.pop();
        
      }
      else // no room in line, add to rejects
      {
        numRejects += customersToAdd.size();
        
        while (!customersToAdd.empty())
        {
          customersToAdd.pop();
        }
      }
    }
 }




void printCurrentState(const int & time, const vector<serverElements>& windowBank, const deque<Customer>& theLine, const vector<Customer>& completedCustomers, const int &numRejects, const int &totalCustomersCreated, const priority_queue<Message> & timer, const int &numServers)
{
  bool queuePrinted = false;
  
  cout << endl << endl;
  cout << "Time: " << time << endl;
  cout << "----------------------------------" << endl;
  cout << " Window    Now Serving    Waiting " << endl;
  cout << "----------------------------------" << endl;
  
  
  for(int x = 0,y = numServers -1; x < windowBank.size(); x++,y--)
  {
    cout << "    "<< x << "            ";
    
    if(windowBank.at(y).inUse)
    {
      cout  << windowBank.at(y).aCustomer.name;
    }
    
    
    if(!queuePrinted)
    {
      queuePrinted = true;
      cout << "        ";
      for(int y = 0; y < theLine.size(); y++)
      {
        
          cout << theLine.at(y).name;
        
        
      }
    }
    
    cout << endl;
    
  }
  
  
  if(time != 0)
  {
    int timeWaited = 0;
    
    //to calculate average time from arrival to end
    for(int x = 0; x < completedCustomers.size(); x++)
    {
      timeWaited += (completedCustomers[x].end - completedCustomers[x].arrival);
    }
    
    cout << "----------------------------------" << endl;
    if(completedCustomers.empty())
    {
      cout << "Average time: 0" << endl;
    }
    else
    {
      cout << "Average time: " << (timeWaited / completedCustomers.size()) << endl;
    }
    
    cout << "Turned away per minuet " << (numRejects / time) << endl;
  
  }

  cout << endl << endl;
}






int main()
{
  //Universal Program Requirements
  cout << "BankSimulation.cpp " << endl;
  cout << "by Ben Van de Brooke" << endl;
  cout << "On 10/31/16" << endl;
  cout << "Editor: X-Code" << endl;
  cout << "Compiler: GCC" << endl;
  cout << "Description: A simulation of server/client interactions" << endl<< endl;

  //seeding the rand function (and "priming" it?)
  srand((unsigned int)clock());
  rand();
  
  
  int numServers = 0, minServiceTime = 0, maxServiceTime = 0, arrivalEnd = 0, queueCap = 0, numRejects = 0, time = -1, totalCustomersCreated = 0;
  double arrivalRate = 0, x = 0;
  string input = "";
  vector<double> fileData;
  
  ifstream inFile;
  inFile.open("simulation.txt");

//
//set up
//
  if(inFile.good())
  {
    //fill a vector with data from the file
    while(inFile)
    {
      getline(inFile,input);
      stringstream(input) >> x;
      fileData.push_back(x);
    }
    
    //fills up the data into their variables
    for(int x = 0; x < 6; x++)
    {
      switch (x)
      {
        case 0: numServers = fileData.at(x);
          break;
        case 1: arrivalRate = fileData.at(x);
          break;
        case 2 : queueCap = fileData.at(x);
          break;
        case 3 : minServiceTime = fileData.at(x);
          break;
        case 4 : maxServiceTime = fileData.at(x);
          break;
        case 5 : arrivalEnd = fileData.at(x);
          break;
        default: cout << "file read error(switch)!" << endl;
          break;
      }
    }
    
    //creates containers to hold data
    deque<Customer> theLine;
    vector<serverElements> windowBank(numServers);
   
    
    priority_queue<Message> timer;
    vector<Customer> completedCustomers;
    

    //output the read values
    cout << "number of servers: " << numServers << endl;
    cout << "arrival rate: " << arrivalRate << " per minuet for " << arrivalEnd << " minuets." << endl;
    cout << "max queue length: " << queueCap << endl;
    cout << "minimum service time: " << minServiceTime << endl;
    cout << "max service time: " << maxServiceTime << endl << endl << endl;
  
    
//
// the meat of the program
//
    while(input != "X" && input != "x")
    {
      time++;
      
      if(time < arrivalEnd)
      {
        if(time == 0)
         {
          updateBank(time,windowBank,theLine,completedCustomers,numRejects,timer,arrivalRate, queueCap, totalCustomersCreated,numServers, minServiceTime,maxServiceTime);
         }
        
      
        //display regular info
        printCurrentState(time,windowBank, theLine,completedCustomers, numRejects, totalCustomersCreated, timer, numServers);
      
        //get user input
        getline(cin, input);
      
        //if ok update
        if(input != "X" && input != "x")
        {
          updateBank(time,windowBank,theLine,completedCustomers,numRejects,timer,arrivalRate, queueCap, totalCustomersCreated,numServers, minServiceTime,maxServiceTime);
          
        }
      }
      else
      {
        printCurrentState(time,windowBank, theLine,completedCustomers, numRejects, totalCustomersCreated, timer,numServers);
        getline(cin, input);
      }
      
    }

  }
  else
  {
    cout << "File error" << endl;
  }

    return 0;
}