#include <bits/stdc++.h>
using namespace std;


enum SpotType { COMPACT, REGULAR, LARGE };
enum VehicleType { BIKE, CAR, TRUCK };

class Vehicle {
public:
    string plate;
    VehicleType type;

    Vehicle(string plate, VehicleType t) {
        this->plate = plate;
        this->type = t;
    }
};


class ParkingSpot {
public:
    int spotID;
    SpotType spotType;
    bool isAvailable;
    Vehicle* vehicle;

    ParkingSpot(int id, SpotType type) {
        spotID = id;
        spotType = type;
        isAvailable = true;
        vehicle = NULL;
    }

    bool canFit(VehicleType vtype) {
        if (spotType == COMPACT)
            return vtype == BIKE;

        if (spotType == REGULAR)
            return vtype == BIKE || vtype == CAR;

        return true;
    }

    void parkVehicle(Vehicle* v) {
        this->vehicle = v;
        isAvailable = false;
    }

    void removeVehicle() {
        this->vehicle = NULL;
        isAvailable = true;
    }
};

class Ticket {
public:
    int ticketID;
    int spotID;
    string plate;
    time_t entry_time;

    Ticket(int id, int spot, string plate) {
        ticketID = id;
        spotID = spot;
        this->plate = plate;
        entry_time = time(0);
    }
};


class ParkingLot {
private:
    vector<ParkingSpot*> spots;
    unordered_map<int, Ticket*> ticketMap;   
    unordered_map<string, int> plateToTicket;   
    int nextSpotID = 1;
    int nextTicketID = 1;

public:
    ParkingLot(int compact, int regular, int large) {
        for (int i = 0; i < compact; i++)
            spots.push_back(new ParkingSpot(nextSpotID++, COMPACT));

        for (int i = 0; i < regular; i++)
            spots.push_back(new ParkingSpot(nextSpotID++, REGULAR));

        for (int i = 0; i < large; i++)
            spots.push_back(new ParkingSpot(nextSpotID++, LARGE));
    }

    Ticket* parkVehicle(Vehicle* vehicle) {
        if (plateToTicket.count(vehicle->plate)) {
            cout << "Vehicle already parked.\n";
            return NULL;
        }

        ParkingSpot* freeSpot = NULL;
        for (auto spot : spots) {
            if (spot->isAvailable && spot->canFit(vehicle->type)) {
                freeSpot = spot;
                break;
            }
        }

        if (!freeSpot) {
            cout << "No suitable spot available!\n";
            return NULL;
        }

        freeSpot->parkVehicle(vehicle);

        Ticket* ticket = new Ticket(nextTicketID, freeSpot->spotID, vehicle->plate);
        ticketMap[nextTicketID] = ticket;
        plateToTicket[vehicle->plate] = nextTicketID;

        nextTicketID++;
        return ticket;
    }

    void removeVehicle(Ticket* ticket) {
        if (!ticketMap.count(ticket->ticketID)) {
            cout << "Ticket already removed or invalid.\n";
            return;
        }

        int spotID = ticket->spotID;

        for (auto spot : spots) {
            if (spot->spotID == spotID) {
                spot->removeVehicle();
                break;
            }
        }

        plateToTicket.erase(ticket->plate);
        ticketMap.erase(ticket->ticketID);

        cout << "Vehicle " << ticket->plate << " removed successfully.\n";
    }
};

int main() {
    ParkingLot lot(2, 2, 1);

    Vehicle* bike1 = new Vehicle("KA01AB1234", BIKE);
    Vehicle* car1  = new Vehicle("KA05XY7890", CAR);
    Vehicle* truck1 = new Vehicle("KA09TR0001", TRUCK);

    Ticket* t1 = lot.parkVehicle(bike1);
    if (t1) cout << "Ticket issued: " << t1->ticketID << "\n";

    Ticket* t2 = lot.parkVehicle(car1);
    if (t2) cout << "Ticket issued: " << t2->ticketID << "\n";

    Ticket* t3 = lot.parkVehicle(truck1);
    if (t3) cout << "Ticket issued: " << t3->ticketID << "\n";

    lot.removeVehicle(t2);

    return 0;
}

