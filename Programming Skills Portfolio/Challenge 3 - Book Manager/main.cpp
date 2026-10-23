#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>
#include <string>
#include <iomanip>

class Book {
private:
    std::string title;
    std::string author;
    int pages;
    std::string id;
    bool isBorrowed;

public:
    Book(std::string t, std::string a, int p, std::string idNum, bool borrowed)
        : title(t), author(a), pages(p), id(idNum), isBorrowed(borrowed) {}

    std::string getId() const { return id; }
    std::string getTitle() const { return title; }
    std::string getAuthor() const { return author; }
    int getPages() const { return pages; }
    bool getBorrowedStatus() const { return isBorrowed; }

    void borrowBook() {
        if (!isBorrowed) {
            isBorrowed = true;
            std::cout << "Successfully borrowed \"" << title << "\".\n";
        } else {
            std::cout << "Sorry, \"" << title << "\" is already on loan.\n";
        }
    }

    void returnBook() {
        if (isBorrowed) {
            isBorrowed = false;
            std::cout << "Successfully returned \"" << title << "\".\n";
        } else {
            std::cout << "\"" << title << "\" was not marked as borrowed.\n";
        }
    }

    void displayDetails() const {
        std::cout << "\n----------------------------------------\n";
        std::cout << "ID:       " << id << "\n";
        std::cout << "Title:    " << title << "\n";
        std::cout << "Author:   " << author << "\n";
        std::cout << "Pages:    " << pages << "\n";
        std::cout << "Status:   " << (isBorrowed ? "Borrowed" : "Available") << "\n";
        std::cout << "----------------------------------------\n";
    }
};

std::vector<Book> loadBooks(const std::string& filename) {
    std::vector<Book> books;
    std::ifstream file(filename);

    if (!file.is_open()) {
        std::cerr << "Error: Could not open " << filename << "\n";
        return books;
    }

    std::string line;
    while (std::getline(file, line)) {
        if (line.empty()) continue;

        std::stringstream ss(line);
        std::string title, author, pagesStr, id, statusStr;

        if (std::getline(ss, title, ',') &&
            std::getline(ss, author, ',') &&
            std::getline(ss, pagesStr, ',') &&
            std::getline(ss, id, ',') &&
            std::getline(ss, statusStr, ',')) {

            int pages = std::stoi(pagesStr);
            bool isBorrowed = (statusStr == "true" || statusStr == "true\r");
            books.emplace_back(title, author, pages, id, isBorrowed);
        }
    }
    return books;
}

Book* findBookById(std::vector<Book>& books, const std::string& id) {
    for (auto& book : books) {
        if (book.getId() == id) {
            return &book;
        }
    }
    return nullptr;
}

void displayAllBooks(const std::vector<Book>& books) {
    std::cout << "\n" << std::left
              << std::setw(6)  << "ID"
              << std::setw(30) << "Title"
              << std::setw(24) << "Author"
              << std::setw(8)  << "Pages"
              << std::setw(12) << "Status" << "\n";
    std::cout << std::string(80, '-') << "\n";

    for (const auto& b : books) {
        std::cout << std::left
                  << std::setw(6)  << b.getId()
                  << std::setw(30) << b.getTitle()
                  << std::setw(24) << b.getAuthor()
                  << std::setw(8)  << b.getPages()
                  << std::setw(12) << (b.getBorrowedStatus() ? "Borrowed" : "Available")
                  << "\n";
    }
}

int main() {
    std::vector<Book> library = loadBooks("bookData.txt");
    if (library.empty()) {
        std::cerr << "Library database empty or file not found.\n";
        return 1;
    }

    int choice = 0;
    while (choice != 5) {
        std::cout << "\n========== Library Management System ==========\n";
        std::cout << "1. View All Books\n";
        std::cout << "2. View Book Details by ID\n";
        std::cout << "3. Borrow a Book\n";
        std::cout << "4. Return a Book\n";
        std::cout << "5. Exit\n";
        std::cout << "Enter your choice (1-5): ";

        if (!(std::cin >> choice)) {
            std::cin.clear();
            std::cin.ignore(10000, '\n');
            std::cout << "Invalid input. Please enter a number.\n";
            continue;
        }

        if (choice == 1) {
            displayAllBooks(library);
        } else if (choice >= 2 && choice <= 4) {
            std::string id;
            std::cout << "Enter Book ID (e.g., 001): ";
            std::cin >> id;

            Book* book = findBookById(library, id);
            if (!book) {
                std::cout << "Book with ID " << id << " not found.\n";
            } else if (choice == 2) {
                book->displayDetails();
            } else if (choice == 3) {
                book->borrowBook();
            } else if (choice == 4) {
                book->returnBook();
            }
        } else if (choice != 5) {
            std::cout << "Invalid option. Please choose between 1 and 5.\n";
        }
    }

    std::cout << "Exiting system. Goodbye!\n";
    return 0;
}