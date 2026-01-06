#include <iostream>
#include <string>
#include <vector>
#include <unordered_map>
#include <ctime>
#include <algorithm>

// R1: The system should store information about books and members, and maintain a complete log of all book borrowing, return, reservation, and renewal transactions.

// R2: Every book must have a unique identification number and detailed information, including rack/location in the library.

// R3: Each book should include the ISBN, title, author name, subject, and publication date.

// R4: A book can have multiple copies; each physical copy is a distinct book item with a unique ID.

// R5: The system must support two types of users: librarian and member, each with a library card and a unique card number.

// R6: Every user must have a library card with a unique card number.

// R7: Members can borrow a maximum of 10 books at a time.

// R8: A member can borrow a book for 15 days.

// R9: Only one member can reserve each book item at a time.

// R10: The system must record who issued, reserved, or renewed a book item and on which date.

// R11: According to policy limits, the system must allow members to renew borrowed books.

// R12: The system should notify members if a book is not returned by the due date or when a reserved book becomes available.

// R13: If a book is unavailable, a member should be able to reserve it for when it becomes available.

// R14: The system should allow users to search for books by title, author, subject, or publication date.

using namespace std;
  
enum class BookStatus { AVAILABLE, ISSUED, RESERVED, LOST };
enum class ReservationStatus { WAITING, PENDING, COMPLETED, CANCELED };
enum class AccountStatus { ACTIVE, CLOSED, SUSPENDED };

class BookItem;
class Member;

class Date {
public:
    time_t timestamp;
    
    Date() {
        timestamp = time(nullptr);
    }
    
    Date(time_t t) {
        timestamp = t;
    }
    
    Date addDays(int days) const {
        return Date(timestamp + days * 24 * 60 * 60);
    }
    
    bool isPast(const Date& other) const {
        return timestamp > other.timestamp;
    }
    
    string toString() const {
        char buffer[26];
        ctime_r(&timestamp, buffer);
        return string(buffer);
    }
};

// Book class (R3: ISBN, title, author, subject, publication date)
class Book {
private:
    string isbn;
    string title;
    string author;
    string subject;
    Date publicationDate;
    vector<BookItem*> bookItems; // R4: Multiple copies
    
public:
    Book(const string& isbn, const string& title, const string& author, 
         const string& subject, const Date& pubDate) {
        this->isbn = isbn;
        this->title = title;
        this->author = author;
        this->subject = subject;
        this->publicationDate = pubDate;
    }
    
    ~Book() {
        // Note: BookItems are owned by Library, so we don't delete them here
        bookItems.clear();
    }
    
    void addBookItem(BookItem* item) {
        bookItems.push_back(item);
    }
    
    const vector<BookItem*>& getBookItems() const { 
        return bookItems; 
    }
    
    string getISBN() const { return isbn; }
    string getTitle() const { return title; }
    string getAuthor() const { return author; }
    string getSubject() const { return subject; }
    Date getPublicationDate() const { return publicationDate; }
};

// BookItem class (R2: unique ID, R4: physical copy)
class BookItem {
private:
    string barcode; // Unique ID
    Date borrowedDate;
    Date dueDate;
    double price;
    BookStatus status;
    Date dateOfPurchase;
    string rackLocation; // R2: rack/location
    Book* book;
    
public:
    BookItem(const string& barcode, const string& rackLocation, 
             double price, Book* book) {
        this->barcode = barcode;
        this->rackLocation = rackLocation;
        this->price = price;
        this->book = book;
        this->status = BookStatus::AVAILABLE;
        this->dateOfPurchase = Date();
    }
    
    string getBarcode() const { return barcode; }
    BookStatus getStatus() const { return status; }
    void setStatus(BookStatus s) { status = s; }
    string getRackLocation() const { return rackLocation; }
    Book* getBook() const { return book; }
    Date getDueDate() const { return dueDate; }
    
    bool checkout(const Date& borrowDate, const Date& due) {
        if (status != BookStatus::AVAILABLE) return false;
        status = BookStatus::ISSUED;
        borrowedDate = borrowDate;
        dueDate = due;
        return true;
    }
    
    void returnBook() {
        status = BookStatus::AVAILABLE;
    }
    
    bool isOverdue() const {
        if (status != BookStatus::ISSUED) return false;
        Date today;
        return today.isPast(dueDate);
    }
};

// R5, R6: User with library card
class LibraryCard {
private:
    string cardNumber; // Unique card number
    Date issuedAt;
    bool isActive;
    
public:
    LibraryCard(const string& cardNum) {
        this->cardNumber = cardNum;
        this->issuedAt = Date();
        this->isActive = true;
    }
    
    string getCardNumber() const { return cardNumber; }
    bool getIsActive() const { return isActive; }
    void setIsActive(bool active) { isActive = active; }
};

// Base User class
class User {
protected:
    string name;
    string email;
    string phone;
    LibraryCard* card;
    AccountStatus status;
    
public:
    User(const string& name, const string& email, const string& phone,
         LibraryCard* card) {
        this->name = name;
        this->email = email;
        this->phone = phone;
        this->card = card;
        this->status = AccountStatus::ACTIVE;
    }
    
    virtual ~User() {
        // Card is owned by this user
        delete card;
    }
    
    string getName() const { return name; }
    string getCardNumber() const { return card->getCardNumber(); }
    AccountStatus getStatus() const { return status; }
    void setStatus(AccountStatus s) { status = s; }
};

// Transaction classes (R1, R10: Log all transactions)
class Transaction {
protected:
    Date creationDate;
    string performedBy; // Card number of who performed it
    
public:
    Transaction(const string& performer) {
        this->creationDate = Date();
        this->performedBy = performer;
    }
    
    virtual ~Transaction() {}
    
    Date getDate() const { return creationDate; }
    string getPerformedBy() const { return performedBy; }
};

class BookIssue : public Transaction {
private:
    string bookItemBarcode;
    string memberCardNumber;
    Date dueDate;
    
public:
    BookIssue(const string& barcode, const string& memberCard,
              const string& issuedBy, const Date& due) : Transaction(issuedBy) {
        this->bookItemBarcode = barcode;
        this->memberCardNumber = memberCard;
        this->dueDate = due;
    }
    
    string getBookItemBarcode() const { return bookItemBarcode; }
    string getMemberCardNumber() const { return memberCardNumber; }
};

class BookReturn : public Transaction {
private:
    string bookItemBarcode;
    Date returnDate;
    double fine;
    
public:
    BookReturn(const string& barcode, const string& returnedBy, double fine = 0.0) 
        : Transaction(returnedBy) {
        this->bookItemBarcode = barcode;
        this->returnDate = Date();
        this->fine = fine;
    }
};

class BookReservation : public Transaction {
private:
    string bookItemBarcode;
    string memberCardNumber;
    ReservationStatus status;
    
public:
    BookReservation(const string& barcode, const string& memberCard)
        : Transaction(memberCard) {
        this->bookItemBarcode = barcode;
        this->memberCardNumber = memberCard;
        this->status = ReservationStatus::WAITING;
    }
    
    ReservationStatus getStatus() const { return status; }
    void setStatus(ReservationStatus s) { status = s; }
    string getBookItemBarcode() const { return bookItemBarcode; }
    string getMemberCardNumber() const { return memberCardNumber; }
};

class BookRenewal : public Transaction {
private:
    string bookItemBarcode;
    Date newDueDate;
    
public:
    BookRenewal(const string& barcode, const string& renewedBy, const Date& newDue)
        : Transaction(renewedBy) {
        this->bookItemBarcode = barcode;
        this->newDueDate = newDue;
    }
};

// Member class (R7: max 10 books, R8: 15 days borrow period)
class Member : public User {
private:
    static const int MAX_BOOKS_LIMIT = 10;
    static const int BORROW_DAYS = 15;
    
    vector<BookItem*> checkedOutBooks;
    vector<BookReservation*> reservations;
    
public:
    Member(const string& name, const string& email, const string& phone,
           LibraryCard* card) : User(name, email, phone, card) {
        // All initialization done in base class
    }
    
    ~Member() {
        // BookItems are owned by Library, so we just clear the vector
        checkedOutBooks.clear();
        
        // Reservations are owned by Library's transaction log
        reservations.clear();
    }
    
    int getTotalBooksCheckedOut() const {
        return checkedOutBooks.size();
    }
    
    bool canBorrowBook() const {
        return checkedOutBooks.size() < MAX_BOOKS_LIMIT;
    }
    
    bool checkoutBook(BookItem* bookItem) {
        if (!canBorrowBook()) return false;
        
        Date today;
        Date dueDate = today.addDays(BORROW_DAYS);
        
        if (bookItem->checkout(today, dueDate)) {
            checkedOutBooks.push_back(bookItem);
            return true;
        }
        return false;
    }
    
    bool returnBook(BookItem* bookItem) {
        auto it = find(checkedOutBooks.begin(), checkedOutBooks.end(), bookItem);
        if (it != checkedOutBooks.end()) {
            bookItem->returnBook();
            checkedOutBooks.erase(it);
            return true;
        }
        return false;
    }
    
    bool renewBook(BookItem* bookItem) {
        auto it = find(checkedOutBooks.begin(), checkedOutBooks.end(), bookItem);
        if (it != checkedOutBooks.end() && !bookItem->isOverdue()) {
            Date today;
            Date newDueDate = today.addDays(BORROW_DAYS);
            bookItem->checkout(today, newDueDate); // Renew
            return true;
        }
        return false;
    }
    
    void addReservation(BookReservation* reservation) {
        reservations.push_back(reservation);
    }
    
    const vector<BookItem*>& getCheckedOutBooks() const {
        return checkedOutBooks;
    }
};

// Librarian class
class Librarian : public User {
public:
    Librarian(const string& name, const string& email, const string& phone,
              LibraryCard* card) : User(name, email, phone, card) {
        // All initialization done in base class
    }
};

// Notification system (R12: Observer Pattern)
class Notification {
public:
    virtual void send(const string& recipientEmail, const string& message) = 0;
    virtual ~Notification() {}
};

class EmailNotification : public Notification {
public:
    void send(const string& recipientEmail, const string& message) {
        cout << "Email sent to " << recipientEmail << ": " << message << endl;
    }
};

class NotificationService {
private:
    Notification* notification;
    
public:
    NotificationService() {
        notification = new EmailNotification();
    }
    
    ~NotificationService() {
        delete notification;
    }
    
    void notifyOverdue(Member* member, BookItem* bookItem) {
        string message = "Book '" + bookItem->getBook()->getTitle() + 
                        "' is overdue. Please return it.";
        notification->send(member->getName(), message);
    }
    
    void notifyBookAvailable(Member* member, Book* book) {
        string message = "Reserved book '" + book->getTitle() + 
                        "' is now available.";
        notification->send(member->getName(), message);
    }
};

// Search interface (R14: Strategy Pattern)
class BookSearch {
public:
    virtual vector<Book*> search(
        const unordered_map<string, Book*>& books,
        const string& query) = 0;
    virtual ~BookSearch() {}
};

class SearchByTitle : public BookSearch {
public:
    vector<Book*> search(
        const unordered_map<string, Book*>& books,
        const string& query) {
        vector<Book*> result;
        for (const auto& pair : books) {
            if (pair.second->getTitle().find(query) != string::npos) {
                result.push_back(pair.second);
            }
        }
        return result;
    }
};

class SearchByAuthor : public BookSearch {
public:
    vector<Book*> search(
        const unordered_map<string, Book*>& books,
        const string& query) {
        vector<Book*> result;
        for (const auto& pair : books) {
            if (pair.second->getAuthor().find(query) != string::npos) {
                result.push_back(pair.second);
            }
        }
        return result;
    }
};

class SearchBySubject : public BookSearch {
public:
    vector<Book*> search(
        const unordered_map<string, Book*>& books,
        const string& query) {
        vector<Book*> result;
        for (const auto& pair : books) {
            if (pair.second->getSubject().find(query) != string::npos) {
                result.push_back(pair.second);
            }
        }
        return result;
    }
};

// Catalog for searching
class Catalog {
private:
    unordered_map<string, Book*> books; // ISBN -> Book
    
public:
    ~Catalog() {
        // Books are owned by Library, not by Catalog
        books.clear();
    }
    
    void addBook(Book* book) {
        books[book->getISBN()] = book;
    }
    
    vector<Book*> search(BookSearch* strategy, const string& query) {
        return strategy->search(books, query);
    }
    
    const unordered_map<string, Book*>& getAllBooks() const {
        return books;
    }
};

// Library System - Singleton Pattern
class Library {
private:
    static Library* instance;
    string name;
    string address;
    
    unordered_map<string, Member*> members;
    unordered_map<string, Librarian*> librarians;
    unordered_map<string, BookItem*> bookItems;
    unordered_map<string, Book*> books; // ISBN -> Book
    
    Catalog catalog;
    NotificationService notificationService;
    
    vector<Transaction*> transactions; // R1: Complete log
    
    Library(const string& name, const string& address) {
        this->name = name;
        this->address = address;
    }
    
public:
    static Library* getInstance(const string& name = "City Library",
                               const string& address = "123 Main St") {
        if (!instance) {
            instance = new Library(name, address);
        }
        return instance;
    }
    
    ~Library() {
        // Clean up all owned objects
        
        // Delete members
        for (auto& pair : members) {
            delete pair.second;
        }
        members.clear();
        
        // Delete librarians
        for (auto& pair : librarians) {
            delete pair.second;
        }
        librarians.clear();
        
        // Delete book items
        for (auto& pair : bookItems) {
            delete pair.second;
        }
        bookItems.clear();
        
        // Delete books
        for (auto& pair : books) {
            delete pair.second;
        }
        books.clear();
        
        // Delete transactions
        for (auto tx : transactions) {
            delete tx;
        }
        transactions.clear();
    }
    
    // Member management
    Member* addMember(const string& name, const string& email,
                      const string& phone, const string& cardNum) {
        LibraryCard* card = new LibraryCard(cardNum);
        Member* member = new Member(name, email, phone, card);
        members[cardNum] = member;
        return member;
    }
    
    // Book management
    Book* addBook(const string& isbn, const string& title,
                  const string& author, const string& subject,
                  const Date& pubDate) {
        Book* book = new Book(isbn, title, author, subject, pubDate);
        books[isbn] = book;
        catalog.addBook(book);
        return book;
    }
    
    BookItem* addBookItem(const string& barcode, 
                          const string& rackLocation,
                          double price, Book* book) {
        BookItem* bookItem = new BookItem(barcode, rackLocation, price, book);
        bookItems[barcode] = bookItem;
        book->addBookItem(bookItem);
        return bookItem;
    }
    
    // Issue book (R8: 15 days)
    bool issueBook(const string& memberCardNum, const string& barcode,
                   const string& librarianCardNum) {
        Member* member = members[memberCardNum];
        BookItem* bookItem = bookItems[barcode];
        
        if (!member || !bookItem) return false;
        if (!member->canBorrowBook()) return false;
        
        if (member->checkoutBook(bookItem)) {
            BookIssue* issue = new BookIssue(barcode, memberCardNum,
                                            librarianCardNum, bookItem->getDueDate());
            transactions.push_back(issue);
            return true;
        }
        return false;
    }
    
    // Return book
    bool returnBook(const string& memberCardNum, const string& barcode) {
        Member* member = members[memberCardNum];
        BookItem* bookItem = bookItems[barcode];
        
        if (!member || !bookItem) return false;
        
        double fine = 0.0;
        if (bookItem->isOverdue()) {
            fine = 5.0; // $5 fine per overdue book
        }
        
        if (member->returnBook(bookItem)) {
            BookReturn* returnTx = new BookReturn(barcode, memberCardNum, fine);
            transactions.push_back(returnTx);
            return true;
        }
        return false;
    }
    
    // R11: Renew book
    bool renewBook(const string& memberCardNum, const string& barcode) {
        Member* member = members[memberCardNum];
        BookItem* bookItem = bookItems[barcode];
        
        if (!member || !bookItem) return false;
        
        if (member->renewBook(bookItem)) {
            BookRenewal* renewal = new BookRenewal(barcode, memberCardNum,
                                                   bookItem->getDueDate());
            transactions.push_back(renewal);
            return true;
        }
        return false;
    }
    
    // R13: Reserve book (R9: Only one member can reserve)
    bool reserveBook(const string& memberCardNum, const string& barcode) {
        Member* member = members[memberCardNum];
        BookItem* bookItem = bookItems[barcode];
        
        if (!member || !bookItem) return false;
        if (bookItem->getStatus() != BookStatus::ISSUED) return false;
        
        BookReservation* reservation = new BookReservation(barcode, memberCardNum);
        member->addReservation(reservation);
        transactions.push_back(reservation);
        bookItem->setStatus(BookStatus::RESERVED);
        return true;
    }
    
    // R14: Search books
    vector<Book*> searchBooksByTitle(const string& title) {
        SearchByTitle* strategy = new SearchByTitle();
        vector<Book*> result = catalog.search(strategy, title);
        delete strategy;
        return result;
    }
    
    vector<Book*> searchBooksByAuthor(const string& author) {
        SearchByAuthor* strategy = new SearchByAuthor();
        vector<Book*> result = catalog.search(strategy, author);
        delete strategy;
        return result;
    }
    
    vector<Book*> searchBooksBySubject(const string& subject) {
        SearchBySubject* strategy = new SearchBySubject();
        vector<Book*> result = catalog.search(strategy, subject);
        delete strategy;
        return result;
    }
    
    // R12: Check and notify overdue books
    void checkAndNotifyOverdueBooks() {
        for (auto& memberPair : members) {
            Member* member = memberPair.second;
            for (auto bookItem : member->getCheckedOutBooks()) {
                if (bookItem->isOverdue()) {
                    notificationService.notifyOverdue(member, bookItem);
                }
            }
        }
    }
    
    void displayTransactionLog() {
        cout << "\n=== Transaction Log ===" << endl;
        for (const auto tx : transactions) {
            cout << "Date: " << tx->getDate().toString()
                 << " | Performed by: " << tx->getPerformedBy() << endl;
        }
    }
};

Library* Library::instance = nullptr;

// Demo usage
int main() {
    Library* library = Library::getInstance("Central Library", "456 Book St");
    
    // Add books
    Book* book1 = library->addBook("978-0134685991", "Effective C++",
                                   "Scott Meyers", "Programming", Date());
    BookItem* bookItem1 = library->addBookItem("BOOK001", "A-12", 49.99, book1);
    
    Book* book2 = library->addBook("978-0132350884", "Clean Code",
                                   "Robert Martin", "Programming", Date());
    BookItem* bookItem2 = library->addBookItem("BOOK002", "A-13", 39.99, book2);
    
    // Add member
    Member* member1 = library->addMember("John Doe", "john@email.com",
                                        "555-1234", "CARD001");
    
    // Issue book
    cout << "Issuing book..." << endl;
    library->issueBook("CARD001", "BOOK001", "LIB001");
    
    // Search books
    cout << "\nSearching books by author 'Scott'..." << endl;
    vector<Book*> results = library->searchBooksByAuthor("Scott");
    for (auto book : results) {
        cout << "Found: " << book->getTitle() << " by " << book->getAuthor() << endl;
    }
    
    // Renew book
    cout << "\nRenewing book..." << endl;
    library->renewBook("CARD001", "BOOK001");
    
    // Display transaction log
    library->displayTransactionLog();
    
    // Cleanup
    delete library;
    
    return 0;
}