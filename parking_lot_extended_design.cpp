#include <iostream>
#include <vector>
#include <map>
#include <memory>
#include <ctime>
#include <iomanip>
using namespace std;


enum class VehicleType {
    BIKE,
    CAR,
    TRUCK
};

enum class ParkingSpotType {
    COMPACT,    // For bikes
    MEDIUM,     // For cars
    LARGE       // For trucks
};

enum class ParkingSpotStatus {
    AVAILABLE,
    OCCUPIED
};

class Vehicle;
class ParkingSpot;

class Vehicle {
protected:
    string licensePlate;
    VehicleType type;
    
public:
    Vehicle(const string& plate, VehicleType t) 
        : licensePlate(plate), type(t) {}
    
    virtual ~Vehicle() = default;
    
    string getLicensePlate() const { return licensePlate; }
    VehicleType getType() const { return type; }
    
    virtual ParkingSpotType getRequiredSpotType() const = 0;
};

class Bike : public Vehicle {
public:
    Bike(const string& plate) : Vehicle(plate, VehicleType::BIKE) {}
    
    ParkingSpotType getRequiredSpotType() const override {
        return ParkingSpotType::COMPACT;
    }
};

class Car : public Vehicle {
public:
    Car(const string& plate) : Vehicle(plate, VehicleType::CAR) {}
    
    ParkingSpotType getRequiredSpotType() const override {
        return ParkingSpotType::MEDIUM;
    }
};

class Truck : public Vehicle {
public:
    Truck(const string& plate) : Vehicle(plate, VehicleType::TRUCK) {}
    
    ParkingSpotType getRequiredSpotType() const override {
        return ParkingSpotType::LARGE;
    }
};

// ==================== Parking Spot ====================
class ParkingSpot {
private:
    int spotNumber;
    ParkingSpotType type;
    ParkingSpotStatus status;
    Vehicle* parkedVehicle;
    
public:
    ParkingSpot(int number, ParkingSpotType t) 
        : spotNumber(number), type(t), status(ParkingSpotStatus::AVAILABLE), 
          parkedVehicle(nullptr) {}
    
    bool isAvailable() const {
        return status == ParkingSpotStatus::AVAILABLE;
    }
    
    bool canFitVehicle(const Vehicle* vehicle) const {
        if (!isAvailable()) return false;
        
        ParkingSpotType requiredType = vehicle->getRequiredSpotType();
        
        // Bike can park in any spot
        if (requiredType == ParkingSpotType::COMPACT) return true;
        
        // Car can park in MEDIUM or LARGE
        if (requiredType == ParkingSpotType::MEDIUM) {
            return type == ParkingSpotType::MEDIUM || 
                   type == ParkingSpotType::LARGE;
        }
        
        // Truck needs LARGE spot
        return type == ParkingSpotType::LARGE;
    }
    
    void parkVehicle(Vehicle* vehicle) {
        parkedVehicle = vehicle;
        status = ParkingSpotStatus::OCCUPIED;
    }
    
    void removeVehicle() {
        parkedVehicle = nullptr;
        status = ParkingSpotStatus::AVAILABLE;
    }
    
    int getSpotNumber() const { return spotNumber; }
    ParkingSpotType getType() const { return type; }
    Vehicle* getParkedVehicle() const { return parkedVehicle; }
};

// ==================== Parking Ticket ====================
class ParkingTicket {
private:
    string ticketId;
    string licensePlate;
    int levelNumber;
    int spotNumber;
    time_t entryTime;
    
public:
    ParkingTicket(const string& id, const string& plate, 
                  int level, int spot)
        : ticketId(id), licensePlate(plate), 
          levelNumber(level), spotNumber(spot) {
        entryTime = time(nullptr);
    }
    
    string getTicketId() const { return ticketId; }
    string getLicensePlate() const { return licensePlate; }
    int getLevelNumber() const { return levelNumber; }
    int getSpotNumber() const { return spotNumber; }
    time_t getEntryTime() const { return entryTime; }
    
    void displayTicket() const {
        cout << "\n========== PARKING TICKET ==========" << endl;
        cout << "Ticket ID: " << ticketId << endl;
        cout << "License Plate: " << licensePlate << endl;
        cout << "Level: " << levelNumber << endl;
        cout << "Spot: " << spotNumber << endl;
        cout << "Entry Time: " << ctime(&entryTime);
        cout << "====================================" << endl;
    }
};

// ==================== Parking Level ====================
class ParkingLevel {
private:
    int levelNumber;
    vector<ParkingSpot*> spots;
    map<ParkingSpotType, int> availableSpots;
    
public:
    ParkingLevel(int level, int compactSpots, int mediumSpots, int largeSpots)
        : levelNumber(level) {
        
        int spotNumber = 1;
        
        // Create compact spots
        for (int i = 0; i < compactSpots; i++) {
            spots.push_back(new ParkingSpot(spotNumber++, ParkingSpotType::COMPACT));
        }
        
        // Create medium spots
        for (int i = 0; i < mediumSpots; i++) {
            spots.push_back(new ParkingSpot(spotNumber++, ParkingSpotType::MEDIUM));
        }
        
        // Create large spots
        for (int i = 0; i < largeSpots; i++) {
            spots.push_back(new ParkingSpot(spotNumber++, ParkingSpotType::LARGE));
        }
        
        // Initialize available spots count
        availableSpots[ParkingSpotType::COMPACT] = compactSpots;
        availableSpots[ParkingSpotType::MEDIUM] = mediumSpots;
        availableSpots[ParkingSpotType::LARGE] = largeSpots;
    }
    
    ~ParkingLevel() {
        for (auto spot : spots) {
            delete spot;
        }
    }
    
    ParkingSpot* findAvailableSpot(const Vehicle* vehicle) {
        for (auto spot : spots) {
            if (spot->canFitVehicle(vehicle)) {
                return spot;
            }
        }
        return nullptr;
    }
    
    bool parkVehicle(Vehicle* vehicle, ParkingSpot* spot) {
        if (spot && spot->canFitVehicle(vehicle)) {
            spot->parkVehicle(vehicle);
            availableSpots[spot->getType()]--;
            return true;
        }
        return false;
    }
    
    bool unparkVehicle(int spotNumber) {
        for (auto spot : spots) {
            if (spot->getSpotNumber() == spotNumber && !spot->isAvailable()) {
                availableSpots[spot->getType()]++;
                spot->removeVehicle();
                return true;
            }
        }
        return false;
    }
    
    int getLevelNumber() const { return levelNumber; }
    
    int getAvailableSpots(ParkingSpotType type) const {
        auto it = availableSpots.find(type);
        return (it != availableSpots.end()) ? it->second : 0;
    }
    
    void displayStatus() const {
        cout << "\nLevel " << levelNumber << " Status:" << endl;
        cout << "  Compact spots available: " 
             << getAvailableSpots(ParkingSpotType::COMPACT) << endl;
        cout << "  Medium spots available: " 
             << getAvailableSpots(ParkingSpotType::MEDIUM) << endl;
        cout << "  Large spots available: " 
             << getAvailableSpots(ParkingSpotType::LARGE) << endl;
    }
};

// ==================== Parking Lot (Singleton) ====================
class ParkingLot {
private:
    static ParkingLot* instance;
    vector<ParkingLevel*> levels;
    map<string, ParkingTicket*> activeTickets; // licensePlate -> ticket
    int ticketCounter;
    
    // Pricing per hour
    const double BIKE_RATE = 10.0;
    const double CAR_RATE = 20.0;
    const double TRUCK_RATE = 30.0;
    
    ParkingLot() : ticketCounter(0) {}
    
public:
    static ParkingLot* getInstance() {
        if (instance == nullptr) {
            instance = new ParkingLot();
        }
        return instance;
    }
    
    ~ParkingLot() {
        for (auto level : levels) {
            delete level;
        }
        for (auto& pair : activeTickets) {
            delete pair.second;
        }
    }
    
    void addLevel(int compactSpots, int mediumSpots, int largeSpots) {
        int levelNumber = levels.size() + 1;
        levels.push_back(new ParkingLevel(levelNumber, compactSpots, 
                                         mediumSpots, largeSpots));
    }
    
    ParkingTicket* parkVehicle(Vehicle* vehicle) {
        if (activeTickets.find(vehicle->getLicensePlate()) != activeTickets.end()) {
            cout << "Vehicle already parked!" << endl;
            return nullptr;
        }
        
        // Try to find spot in each level
        for (auto level : levels) {
            ParkingSpot* spot = level->findAvailableSpot(vehicle);
            if (spot != nullptr) {
                if (level->parkVehicle(vehicle, spot)) {
                    string ticketId = "T" + to_string(++ticketCounter);
                    ParkingTicket* ticket = new ParkingTicket(
                        ticketId, 
                        vehicle->getLicensePlate(),
                        level->getLevelNumber(),
                        spot->getSpotNumber()
                    );
                    
                    activeTickets[vehicle->getLicensePlate()] = ticket;
                    ticket->displayTicket();
                    return ticket;
                }
            }
        }
        
        cout << "No available spot for this vehicle!" << endl;
        return nullptr;
    }
    
    bool unparkVehicle(const string& licensePlate) {
        auto it = activeTickets.find(licensePlate);
        if (it == activeTickets.end()) {
            cout << "No active ticket found for this vehicle!" << endl;
            return false;
        }
        
        ParkingTicket* ticket = it->second;
        int levelNum = ticket->getLevelNumber();
        int spotNum = ticket->getSpotNumber();
        
        if (levelNum > 0 && levelNum <= levels.size()) {
            if (levels[levelNum - 1]->unparkVehicle(spotNum)) {
                // Calculate and display charges
                displayCharges(ticket);
                
                delete ticket;
                activeTickets.erase(it);
                return true;
            }
        }
        
        return false;
    }
    
    void displayCharges(const ParkingTicket* ticket) const {
        time_t exitTime = time(nullptr);
        double hours = difftime(exitTime, ticket->getEntryTime()) / 3600.0;
        if (hours < 0.1) hours = 0.1; 
        
        // For demo, we'll assume based on spot type
        double rate = CAR_RATE; // Default
        double charge = hours * rate;
        
        cout << "\n========== PARKING CHARGES ==========" << endl;
        cout << "Ticket ID: " << ticket->getTicketId() << endl;
        cout << "License Plate: " << ticket->getLicensePlate() << endl;
        cout << "Parking Duration: " << fixed << setprecision(2) 
             << hours << " hours" << endl;
        cout << "Rate: $" << rate << "/hour" << endl;
        cout << "Total Charge: $" << charge << endl;
        cout << "=====================================" << endl;
    }
    
    void displayAvailability() const {
        cout << "\n========== PARKING LOT STATUS ==========" << endl;
        for (const auto level : levels) {
            level->displayStatus();
        }
        cout << "========================================" << endl;
    }
};

// Initialize static member
ParkingLot* ParkingLot::instance = nullptr;

// ==================== Main (Demo) ====================
int main() {
    // Get parking lot instance
    ParkingLot* parkingLot = ParkingLot::getInstance();
    
    // Setup: 3 levels with different configurations
    parkingLot->addLevel(10, 20, 5);  // Level 1
    parkingLot->addLevel(15, 15, 8);  // Level 2
    parkingLot->addLevel(20, 10, 10); // Level 3
    
    cout << "Multi-Level Parking Lot System Initialized!" << endl;
    parkingLot->displayAvailability();
    
    // Create vehicles
    Vehicle* bike1 = new Bike("B-001");
    Vehicle* car1 = new Car("C-001");
    Vehicle* car2 = new Car("C-002");
    Vehicle* truck1 = new Truck("T-001");
    
    // Park vehicles
    cout << "\n--- Parking Vehicles ---" << endl;
    parkingLot->parkVehicle(bike1);
    parkingLot->parkVehicle(car1);
    parkingLot->parkVehicle(car2);
    parkingLot->parkVehicle(truck1);
    
    // Display current availability
    parkingLot->displayAvailability();
    
    // Unpark a vehicle
    cout << "\n--- Unparking Vehicle C-001 ---" << endl;
    parkingLot->unparkVehicle("C-001");
    
    // Display availability after unparking
    parkingLot->displayAvailability();
    
    // Try to park the same vehicle again
    cout << "\n--- Attempting to park C-001 again ---" << endl;
    parkingLot->parkVehicle(car1);
    
    // Final status
    parkingLot->displayAvailability();
    
    // Cleanup
    delete bike1;
    delete car1;
    delete car2;
    delete truck1;
    
    return 0;
}