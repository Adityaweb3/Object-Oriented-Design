// Method Overloading  : Compile Time polymorphism (Static) in c++ using Method Overloading
//If a class has multiple methods with the same name but different number of arguments or types of arguments is different . 
#include <iostream>
using namespace std ; 

class Sum {
    public : 
    int addition (int a , int b){
        return a+b ; 
    }
    int addition (int a , int b , int c){
        return a+b+c ; 
    }
} ; 

int main(void){
    Sum sum ; 
    cout<<sum.addition(10+12)<<endl ; 
    cout<<sum.addition(10+12+15)<<endl ; 
    return 0 ; 
}

//Operator Overloading  : 

class ComplexNumber {
private:
	int real, imaginary;
public:
	ComplexNumber(int r = 0, int i = 0) {real = r; imaginary = i;}

	ComplexNumber operator + (ComplexNumber const &c) {
		ComplexNumber result;
		result.real = real + c.real;
		result.imaginary = imaginary + c.imaginary;
		return result;
	}

	void display() { 
		cout << "( " << real << " + " << imaginary << " i )" << '\n'; 
	}
};

int main() {
	ComplexNumber c1(11, 5), c2(2, 6);
	ComplexNumber c3 = c1 + c2;
	c3.display();
}