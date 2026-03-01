//===========================================================================
// Name        : ProjectTwo.cpp
// Author      : Steven Halliwell
// Version     : 1.0
// Description : CS 300 Project Two
//===========================================================================

#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>
#include <string>
#include <algorithm>

using namespace std;

// structure stores info about each course including prerequisites

struct Course {
    string courseNumber;
    string courseTitle;
    vector<string> prerequisites;
};

// BST node structure
// each node in the tree holds a course and pointers to left and right child
struct Node {
    Course course;
    Node* left;
    Node* right;

    Node() {               // default constructor
        left = nullptr;
        right = nullptr;
    }

    Node(Course aCourse) : Node() {  // constructor sets course
        course = aCourse;
    }
};

// handles inserting courses and traversing or searching through the tree
class BST {
private:
    Node* root;   // root of the tree

    // function which adds the node recursively
    void addNode(Node* node, Course course) {
        if (course.courseNumber < node->course.courseNumber) {  // left if smaller just as BST
            if (node->left == nullptr)
                node->left = new Node(course);                  // insert here if left is empty
            else
                addNode(node->left, course);                  // else go deeper left
        } else {                                              // go right if equal or bigger
            if (node->right == nullptr)
                node->right = new Node(course);               // insert here if right is empty
            else
                addNode(node->right, course);                // else go deeper right
        }
    }

    // recursive in-order traversal
    // prints courses in order by course number
    void inOrder(Node* node) {
        if (node == nullptr) return;        // base case, nothing to do
        inOrder(node->left);                // visit left subtree
        cout << node->course.courseNumber << ", " << node->course.courseTitle << endl; // prints the current course
        inOrder(node->right);               // visit right subtree
    }

    // recursive search for a specific course
    void searchCourse(Node* node, string searchNumber) {
        if (node == nullptr) return;        // base case, not found

        string nodeNumber = node->course.courseNumber;
        string searchUpper = searchNumber;
        transform(searchUpper.begin(), searchUpper.end(), searchUpper.begin(), ::toupper); // make uppercase for comparison

        if (searchUpper == nodeNumber) {    // found the course
            cout << node->course.courseNumber << ", " << node->course.courseTitle << endl;
            if (node->course.prerequisites.empty())
                cout << "Prerequisites: None" << endl;  // where no prereqs
            else {             // lists prereqs
                cout << "Prerequisites: ";
                for (size_t i = 0; i < node->course.prerequisites.size(); i++) {
                    cout << node->course.prerequisites[i];
                    if (i != node->course.prerequisites.size() - 1)
                        cout << ", ";
                }
                cout << endl;
            }
        } else if (searchUpper < nodeNumber) {   // goes left if search is smaller
            searchCourse(node->left, searchNumber);
        } else {     // goes right if the search is bigger
            searchCourse(node->right, searchNumber);
        }
    }

    // helper to delete all nodes recursively
    void deleteAll(Node* node) {
        if (node == nullptr) return;    // base case, nothing to delete
        deleteAll(node->left);          // delete left subtree
        deleteAll(node->right);         // delete right subtree
        delete node;        // delete current node
    }
public:
    BST() { root = nullptr; }    // initialize empty tree

    ~BST() { deleteAll(root); }    // clean up tree when done

    // insert a course into the tree
    void insert(Course course) {
        if (root == nullptr)        // if tree empty, new root
            root = new Node(course);
        else
            addNode(root, course);       // otherwise call helper
    }

    // print all of the courses in order
    void printAllCourses() {
        inOrder(root);
    }

    // print one course by number with prereqs
    void printCourse(string courseNumber) {
        searchCourse(root, courseNumber);
    }
};

// CSV parsing and course loading
// reads CSV file and converts each line into a Course
vector<Course> loadCourses(string filename) {
    vector<Course> courses;
    ifstream file(filename);

    if (!file.is_open()) {   // checks whether the file exists
        cout << "Error: file not found" << endl;
        return courses;
    }

    string line;
    int lineNumber = 0;
    while (getline(file, line)) {   // read each line
        lineNumber++;
        if (line.empty()) continue;

        stringstream ss(line);
        vector<string> tokens;
        string temp;

        while (getline(ss, temp, ',')) {      // split line by comma
            temp.erase(0, temp.find_first_not_of(" \t\r\n"));   // trim left spaces
            temp.erase(temp.find_last_not_of(" \t\r\n") + 1);   // trim right spaces
            tokens.push_back(temp);
        }

        if (tokens.size() < 2) {
            cout << "error: invalid format on line " << lineNumber << endl;
            continue;
        }

        Course c;
        c.courseNumber = tokens[0];        // first column
        c.courseTitle = tokens[1];         // second column

        for (size_t i = 2; i < tokens.size(); i++) {
            if (!tokens[i].empty())
                c.prerequisites.push_back(tokens[i]);
        }

        courses.push_back(c);  // adds to vector with push back
    }

    file.close();
    return courses;
}

// main menu and program
int main() {
    BST courseTree;
    vector<Course> loadedCourses;
    bool loaded = false;
    int choice = 0;

    cout << "Welcome to the course planner." << endl;

    while (choice != 9) {                          // menu loop
        cout << "1. Load Data Structure." << endl;
        cout << "2. Print Course List." << endl;
        cout << "3. Print Course." << endl;
        cout << "9. Exit" << endl;
        cout << "What would you like to do? ";
        cin >> choice;

        if (cin.fail()) {      // checks for bad input
            cin.clear();
            cin.ignore(1000, '\n');
            cout << choice << " Sorry that's not a valid option." << endl;
            continue;
        }

        switch (choice) {       // menu options
            case 1: {           // loads csv
                string filename;
                cout << "Enter file name: ";
                cin >> filename;
                loadedCourses = loadCourses(filename);
                if (!loadedCourses.empty()) {       // inserts courses if loaded
                    courseTree = BST();
                    for (auto c : loadedCourses)
                        courseTree.insert(c);
                    loaded = true;
                }
                break;
            }
            case 2:                     // prints all of the courses when chosen
                if (!loaded)
                    cout << "please load the data structure first." << endl;
                else
                    courseTree.printAllCourses();
                break;
            case 3:                    // prints single course
                if (!loaded)
                    cout << "please load the data structure first." << endl;
                else {
                    string searchNum;
                    cout << "What course do you want to know about? ";
                    cin >> searchNum;
                    transform(searchNum.begin(), searchNum.end(), searchNum.begin(), ::toupper);
                    courseTree.printCourse(searchNum);
                }
                break;
            case 9:                  // exits the program
                cout << "Thank you for using the course planner!" << endl;
                break;
            default:
                cout << choice << " is not a valid option." << endl;
                break;
        }
    }

    return 0;
}
