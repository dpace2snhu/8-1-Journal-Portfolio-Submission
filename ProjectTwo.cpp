//============================================================================
// Name        : ProjectTwo.cpp
// Author      : Darcy Pace
// Version     : 1.0
// Copyright   : Copyright © 2023 SNHU
// Description : Project Two
//============================================================================

#include <algorithm>
#include <cctype>
#include <fstream>
#include <iostream>
#include <sstream>
#include <string>
#include <vector>

using namespace std;

// Course object stores the course number, title, and prerequisite course numbers.
struct Course {
    string courseNumber;
    string courseTitle;
    vector<string> prerequisites;
};

// Node object used by the binary search tree.
struct Node {
    Course course;
    Node* left;
    Node* right;

    Node(Course aCourse) {
        course = aCourse;
        left = nullptr;
        right = nullptr;
    }
};

class BinarySearchTree {
private:
    Node* root;

    void addNode(Node* node, Course course) {
        if (course.courseNumber < node->course.courseNumber) {
            if (node->left == nullptr) {
                node->left = new Node(course);
            }
            else {
                addNode(node->left, course);
            }
        }
        else {
            if (node->right == nullptr) {
                node->right = new Node(course);
            }
            else {
                addNode(node->right, course);
            }
        }
    }

    void printInOrder(Node* node) const {
        if (node != nullptr) {
            printInOrder(node->left);
            cout << node->course.courseNumber << ", " << node->course.courseTitle << endl;
            printInOrder(node->right);
        }
    }

    void deleteTree(Node* node) {
        if (node != nullptr) {
            deleteTree(node->left);
            deleteTree(node->right);
            delete node;
        }
    }

public:
    BinarySearchTree() {
        root = nullptr;
    }

    ~BinarySearchTree() {
        clear();
    }

    void clear() {
        deleteTree(root);
        root = nullptr;
    }

    void insert(Course course) {
        if (root == nullptr) {
            root = new Node(course);
        }
        else {
            addNode(root, course);
        }
    }

    Course search(string courseNumber) const {
        Node* current = root;

        while (current != nullptr) {
            if (current->course.courseNumber == courseNumber) {
                return current->course;
            }
            else if (courseNumber < current->course.courseNumber) {
                current = current->left;
            }
            else {
                current = current->right;
            }
        }

        Course emptyCourse;
        return emptyCourse;
    }

    void printCourseList() const {
        printInOrder(root);
    }
};

string trim(string value) {
    size_t start = value.find_first_not_of(" \t\r\n");
    size_t end = value.find_last_not_of(" \t\r\n");

    if (start == string::npos || end == string::npos) {
        return "";
    }

    return value.substr(start, end - start + 1);
}

string toUpperCase(string value) {
    transform(value.begin(), value.end(), value.begin(), [](unsigned char c) {
        return toupper(c);
    });

    return value;
}

vector<string> splitLine(string line) {
    vector<string> tokens;
    string token;
    stringstream ss(line);

    while (getline(ss, token, ',')) {
        tokens.push_back(trim(token));
    }

    return tokens;
}

bool validatePrerequisites(const vector<Course>& courses) {
    for (Course course : courses) {
        for (string prerequisite : course.prerequisites) {
            bool found = false;

            for (Course checkCourse : courses) {
                if (checkCourse.courseNumber == prerequisite) {
                    found = true;
                    break;
                }
            }

            if (!found) {
                cout << "Error: prerequisite " << prerequisite << " listed for "
                     << course.courseNumber << " was not found in the file." << endl;
                return false;
            }
        }
    }

    return true;
}

bool loadCourses(string fileName, BinarySearchTree& courseTree) {
    ifstream file(fileName);
    string line;
    vector<Course> courses;

    if (!file.is_open()) {
        cout << "Error: Could not open file " << fileName << "." << endl;
        return false;
    }

    while (getline(file, line)) {
        if (trim(line).empty()) {
            continue;
        }

        vector<string> tokens = splitLine(line);

        // Each line must include at least a course number and course title.
        if (tokens.size() < 2 || tokens.at(0).empty() || tokens.at(1).empty()) {
            cout << "Error: Invalid course format found in file." << endl;
            return false;
        }

        Course course;
        course.courseNumber = toUpperCase(tokens.at(0));
        course.courseTitle = tokens.at(1);

        for (size_t i = 2; i < tokens.size(); ++i) {
            if (!tokens.at(i).empty()) {
                course.prerequisites.push_back(toUpperCase(tokens.at(i)));
            }
        }

        courses.push_back(course);
    }

    file.close();

    if (courses.empty()) {
        cout << "Error: The file did not contain any course data." << endl;
        return false;
    }

    if (!validatePrerequisites(courses)) {
        return false;
    }

    courseTree.clear();

    for (Course course : courses) {
        courseTree.insert(course);
    }

    cout << "Course data loaded successfully." << endl;
    return true;
}

void printCourseInformation(const BinarySearchTree& courseTree, string courseNumber) {
    Course course = courseTree.search(toUpperCase(courseNumber));

    if (course.courseNumber.empty()) {
        cout << "Course not found." << endl;
        return;
    }

    cout << course.courseNumber << ", " << course.courseTitle << endl;

    if (course.prerequisites.empty()) {
        cout << "Prerequisites: None" << endl;
    }
    else {
        cout << "Prerequisites: ";

        for (size_t i = 0; i < course.prerequisites.size(); ++i) {
            Course prerequisiteCourse = courseTree.search(course.prerequisites.at(i));

            cout << course.prerequisites.at(i);

            // Print the prerequisite title when it is available in the data structure.
            if (!prerequisiteCourse.courseTitle.empty()) {
                cout << " (" << prerequisiteCourse.courseTitle << ")";
            }

            if (i < course.prerequisites.size() - 1) {
                cout << ", ";
            }
        }

        cout << endl;
    }
}

void displayMenu() {
    cout << "1. Load Data Structure." << endl;
    cout << "2. Print Course List." << endl;
    cout << "3. Print Course." << endl;
    cout << "9. Exit" << endl;
    cout << "What would you like to do? ";
}

int main() {
    BinarySearchTree courseTree;
    bool dataLoaded = false;
    int choice = 0;
    string fileName;
    string courseNumber;

    cout << "Welcome to the course planner." << endl;

    while (choice != 9) {
        displayMenu();

        if (!(cin >> choice)) {
            cin.clear();
            cin.ignore(10000, '\n');
            cout << "Invalid input. Please enter a number from the menu." << endl;
            continue;
        }

        switch (choice) {
        case 1:
            cout << "Enter the file name: ";
            cin.ignore(10000, '\n');
            getline(cin, fileName);
            dataLoaded = loadCourses(fileName, courseTree);
            break;

        case 2:
            if (!dataLoaded) {
                cout << "Please load the data first." << endl;
            }
            else {
                cout << "Here is a sample schedule:" << endl;
                courseTree.printCourseList();
            }
            break;

        case 3:
            if (!dataLoaded) {
                cout << "Please load the data first." << endl;
            }
            else {
                cout << "What course do you want to know about? ";
                cin >> courseNumber;
                printCourseInformation(courseTree, courseNumber);
            }
            break;

        case 9:
            cout << "Thank you for using the course planner!" << endl;
            break;

        default:
            cout << choice << " is not a valid option." << endl;
            break;
        }
    }

    return 0;
}
