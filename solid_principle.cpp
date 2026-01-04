//1.  Single Responsibilty Principle : Every class should have a single reason to change , meaning that a class should only have on job or responsibility . 

// Bad Example : 
class User {
    string name ; 
    string email ; 
    public : 
    void saveToDatabase(){
        //code save user to database
    }
    void sendEmailVerification(){
        //code to send email verification
    }
    void generateReport(){
        //code to generate user report
    }
}

//Good Example : 
class User {
    string name ; 
    string email ; 
    public : 
    string getName(){
        return name;
    }
    string getEmail(){
        return email ; 
    }
}

class UserRepository {
    public : 
    void saveToDatabase(User user){
        //code save user to database
    }
}

class EmailService {
    public : 
    void sendEmailVerification(User user){
        //code to send email verification
    }
}


//2 . Open/Closed Principle : Class should be open for extension but closed for modification . 

// Bad Example : 

class Shape {
    public : 
    string type ; 

} ; 
class AreaCalculator {
    public : 
    double calculateArea(Shape shape){
        if(shape.type=="circle"){
            //return area of circle 
        }
        if (shape.type=="rectangle"){
            //return are of rectangle 
        }
        //Need to mofiy the function to add new shapes .
    }
}

//Good Example : 

class Shape {
    public : 
    virtual double area()=0 ; 
    virtual ~Shape() = default; //Virtual destructor 
}

class Circle : public Shape {
    public : 
    double radius ; 
    Circle (double r){
        radius = r ;
    }
    double area() override {
        return 3.14 *radius *radius ; 
    }
} ; 

class Rectrangle : public Shape {
    public : 
    double length , width ; 
    Rectangle (double l , double w){
        length =l ; 
        width = w ; 
    }
    double area() override {
        return length*width ;
    }
}

//3. Liskov Substitution Principle : Objects of a superclass should be replaceable with objects of its subclasses without affecting the correctness of the program. 

//Bad Example : 

class Bird {
    public : 
    virtual void fly(){
        //code to fly 
    }
}
class Sparrow : public Bird {
    public :
    void fly() override {
        //code to fly 
    }
}

class Penguin : public Bird {
    public :
    void fly() override {
        throw "Penguins can't fly!" ; 
    }
}

//Good Example : 

class Bird {
    public : 
    virtual void eat(){
        //code to eat ;
    }
}
class FlyingBird : public Bird {
    public : 
    virtual void fly() {
        //code to fly
    }
}
class Sparrow : public FlyingBird {
    public : 
    void fly() override {
        //code to fly
    }
}
class Penguin : public Bird {
    public : 
    void eat() override{
        //code to eat
    }
}

//4. Interface Segregation Principle : Clients should not be forced to depend on interfaces they do not use . 

// Bad Example : 
class Worker {
    public : 
    virtual void work() = 0 ; 
    virtual void eat() = 0 ; 
    virtual void sleep() =0 ; 
}

class Robot : public Worker {
    public : 
    void work() override {
        //code to work
    }
    void eat() override {
        //Robot don't eat
    }
    void sleep() override {
        //Robot don't sleep
    }
}

//Good Example : 
class Workable {
public:
    virtual void work() = 0;
    virtual ~Workable() = default;
};

class Eatable {
public:
    virtual void eat() = 0;
    virtual ~Eatable() = default;
};

class Sleepable {
public:
    virtual void sleep() = 0;
    virtual ~Sleepable() = default;
};

class Human : public Workable, public Eatable, public Sleepable {
public:
    void work() override { /* working */ }
    void eat() override { /* eating */ }
    void sleep() override { /* sleeping */ }
};

class Robot : public Workable {
public:
    void work() override { /* working */ }
};

// 5. Dependency Inversion Principle : High level modules should not depend on low level modules . Bot should depend on abstractions . 

// Bad Example : 
class MySQLDatabase {
public:
    void connect() { /* MySQL connection */ }
};

class UserService {
    MySQLDatabase db; // Tightly coupled to MySQL
public:
    void getUser() {
        db.connect();
        // get user logic
    }
};

// Good Example : 

class Database {
public:
    virtual void connect() = 0;
    virtual ~Database() = default;
};

class MySQLDatabase : public Database {
public:
    void connect() override { /* MySQL connection */ }
};

class PostgreSQLDatabase : public Database {
public:
    void connect() override { /* PostgreSQL connection */ }
};

class UserService {
    Database* db; // Depends on abstraction
public:
    UserService(Database* database) : db(database) {}
    
    void getUser() {
        db->connect();
        // get user logic
    }
};

// Usage
MySQLDatabase mysql;
UserService service(&mysql); // Easy to swap databases