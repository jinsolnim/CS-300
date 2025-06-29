
/*
    ABCU Advising Assistance Program
    Author: Jinsol Cantrall
    Course: CS-300

    One-file implementation that:
    - loads course data from a comma-separated file
    - stores courses in a binary search tree
    - lets advisers print a sorted list or details for a single course
    - handles user input robustly and case-insensitively
*/

#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <vector>
#include <cctype>
#include <algorithm>

using namespace std;

/* -------------------- Course object -------------------- */
struct Course {
    string number;
    string title;
    vector<string> prerequisites;
};

/* -------------------- Binary Search Tree -------------------- */
struct Node {
    Course data;
    Node* left;
    Node* right;
    explicit Node(const Course& c) : data(c), left(nullptr), right(nullptr) {}
};

class CourseBST {
private:
    Node* root = nullptr;

    Node* insert(Node* node, const Course& c) {
        if (!node) return new Node(c);
        if (c.number < node->data.number)
            node->left = insert(node->left, c);
        else
            node->right = insert(node->right, c);
        return node;
    }

    void inOrder(Node* node) const {
        if (!node) return;
        inOrder(node->left);
        cout << node->data.number << ", " << node->data.title << endl;
        inOrder(node->right);
    }

    Node* search(Node* node, const string& num) const {
        if (!node || node->data.number == num) return node;
        if (num < node->data.number)
            return search(node->left, num);
        return search(node->right, num);
    }

    void destroy(Node* node) {
        if (!node) return;
        destroy(node->left);
        destroy(node->right);
        delete node;
    }
public:
    ~CourseBST() { destroy(root); }
    void insert(const Course& c)          { root = insert(root, c); }
    void printCourseList() const          { inOrder(root); }
    const Course* find(const string& num) const {
        Node* n = search(root, num);
        return n ? &n->data : nullptr;
    }
    bool isEmpty() const { return root == nullptr; }
};

/* -------------------- Utility helpers -------------------- */
static inline string trim(const string& s) {
    size_t start = s.find_first_not_of(" 	
");
    size_t end   = s.find_last_not_of(" 	
");
    return (start == string::npos) ? "" : s.substr(start, end - start + 1);
}

vector<string> splitCsv(const string& line) {
    vector<string> tokens;
    string token;
    stringstream ss(line);
    while (getline(ss, token, ',')) tokens.push_back(trim(token));
    return tokens;
}

string toUpper(const string& str) {
    string result = str;
    transform(result.begin(), result.end(), result.begin(), ::toupper);
    return result;
}

/* -------------------- Load courses from file -------------------- */
bool loadCourses(const string& filename, CourseBST& bst) {
    ifstream fin(filename);
    if (!fin.is_open()) {
        cout << "Error: could not open file '" << filename << "'." << endl;
        return false;
    }

    vector<Course> tempCourses;
    string line;
    while (getline(fin, line)) {
        if (trim(line).empty()) continue;
        vector<string> tokens = splitCsv(line);
        if (tokens.size() < 2) {
            cout << "Format error: each line needs course number and title." << endl;
            return false;
        }
        Course c;
        c.number = toUpper(tokens[0]);
        c.title  = tokens[1];
        for (size_t i = 2; i < tokens.size(); ++i)
            c.prerequisites.push_back(toUpper(tokens[i]));
        tempCourses.push_back(c);
    }
    fin.close();

    vector<string> numbers;
    for (const Course& c : tempCourses) numbers.push_back(c.number);

    for (const Course& c : tempCourses) {
        for (const string& pre : c.prerequisites) {
            if (find(numbers.begin(), numbers.end(), pre) == numbers.end()) {
                cout << "Format error: prerequisite '" << pre
                     << "' not found for course " << c.number << "." << endl;
                return false;
            }
        }
        bst.insert(c);
    }

    cout << tempCourses.size() << " courses loaded." << endl;
    return true;
}

/* -------------------- Print single course info -------------------- */
void printCourseInfo(const Course* c) {
    if (!c) {
        cout << "Course not found." << endl;
        return;
    }
    cout << c->number << ", " << c->title << endl;
    if (c->prerequisites.empty()) {
        cout << "Prerequisites: None" << endl;
    } else {
        cout << "Prerequisites: ";
        for (size_t i = 0; i < c->prerequisites.size(); ++i) {
            cout << c->prerequisites[i];
            if (i + 1 < c->prerequisites.size()) cout << ", ";
        }
        cout << endl;
    }
}

/* -------------------- Menu loop -------------------- */
void displayMenu() {
    cout << "
1. Load Data Structure.
"
         << "2. Print Course List.
"
         << "3. Print Course.
"
         << "9. Exit
"
         << "
What would you like to do? ";
}

int main() {
    CourseBST bst;
    bool dataLoaded = false;

    cout << "Welcome to the course planner.
";

    while (true) {
        displayMenu();
        int choice;
        if (!(cin >> choice)) {
            cin.clear();
            cin.ignore(numeric_limits<streamsize>::max(), '\n');
            cout << "Invalid input. Please enter a number." << endl;
            continue;
        }

        if (choice == 1) {
            string filename;
            cout << "Enter file name: ";
            cin >> filename;
            bst = CourseBST();
            dataLoaded = loadCourses(filename, bst);
        }
        else if (choice == 2) {
            if (!dataLoaded || bst.isEmpty()) {
                cout << "Please load course data first." << endl;
            } else {
                cout << "
Here is a sample schedule:
" << endl;
                bst.printCourseList();
            }
        }
        else if (choice == 3) {
            if (!dataLoaded || bst.isEmpty()) {
                cout << "Please load course data first." << endl;
            } else {
                string courseNum;
                cout << "What course do you want to know about? ";
                cin >> courseNum;
                courseNum = toUpper(courseNum);
                printCourseInfo(bst.find(courseNum));
            }
        }
        else if (choice == 9) {
            cout << "Thank you for using the course planner!" << endl;
            break;
        }
        else {
            cout << choice << " is not a valid option." << endl;
        }
    }

    return 0;
}
