//Runtime Polymorphism in c++ using Method overriding . 
// If a subclass provides a specific implemntation of a method that is already provided by its parent class , it is known as method overriding . 

#include <iostream>
using namespace std ; 

class Vehicle {
    public : 
    virtual void func1(){
        cout<<"Method is getting called from Vehicle class"<<endl ;  
    }
} ;

class Car : public Vehicle {
    public : 
    void func1() override {
        cout<<"Method is getting called from Car class"<<endl ; 
    }

} ; 

int main(){
    Vehicle *vehicle  ;
    Car car ; 
    vehicle = &car ; 
    vehicle->func1() ; 

    return 0 ; 

}