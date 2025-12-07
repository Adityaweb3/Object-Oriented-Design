#include <iostream>
#include <string>
#include <queue>
#include <unordered_map>
#include <ctime>
using namespace std;

class RateLimiter {
private:
    int maxRequests;
    int timeWindowSeconds;
    unordered_map<string, queue<time_t>> userRequests; 
    
public:
    RateLimiter(int maxReq, int windowSec) {
        maxRequests = maxReq;
        timeWindowSeconds = windowSec;
    }
    
    bool allowRequest(string userID) {
        time_t currentTime = time(0);
        queue<time_t>& requests = userRequests[userID];

        while (!requests.empty() && 
               currentTime - requests.front() >= timeWindowSeconds) {
            requests.pop();
        }

        if (requests.size() >= maxRequests) {
            return false;  
        }

        requests.push(currentTime);
        return true; 
    }
    
    int getRemainingRequests(string userID) {
        time_t currentTime = time(0);
        queue<time_t>& requests = userRequests[userID];
        while (!requests.empty() && 
               currentTime - requests.front() >= timeWindowSeconds) {
            requests.pop();
        }
        
        return maxRequests - requests.size();
    }
};
```
