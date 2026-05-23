#include <iostream>
#include <fstream>
#include <vector>
#include <sstream>
#include <iomanip>
#include <algorithm>
#include <cctype>
#include <limits>

using namespace std;

/*
 Smart Student Management System (C++)
 Features:
 - Admin login
 - Add / view / search / update / delete students
 - Marks management
 - Attendance tracking
 - GPA calculation
 - Topper list
 - Persistent storage using file handling

 Default login:
 Username: admin
 Password: admin123
*/

class Student {
private:
    int rollNo;
    string name;
    string branch;
    int semester;
    float marks[5];
    int totalClasses;
    int attendedClasses;

public:
    Student() {
        rollNo = 0;
        name = "";
        branch = "";
        semester = 1;
        for (int i = 0; i < 5; i++) marks[i] = 0;
        totalClasses = 0;
        attendedClasses = 0;
    }

    Student(int r, string n, string b, int s) {
        rollNo = r;
        name = n;
        branch = b;
        semester = s;
        for (int i = 0; i < 5; i++) marks[i] = 0;
        totalClasses = 0;
        attendedClasses = 0;
    }

    int getRollNo() const { return rollNo; }
    string getName() const { return name; }
    string getBranch() const { return branch; }
    int getSemester() const { return semester; }

    void setName(string n) { name = n; }
    void setBranch(string b) { branch = b; }
    void setSemester(int s) { semester = s; }

    void enterMarks() {
        cout << "\nEnter marks for 5 subjects out of 100:\n";
        for (int i = 0; i < 5; i++) {
            do {
                cout << "Subject " << i + 1 << ": ";
                cin >> marks[i];
                if (marks[i] < 0 || marks[i] > 100) {
                    cout << "Invalid marks. Enter between 0 and 100.\n";
                }
            } while (marks[i] < 0 || marks[i] > 100);
        }
    }

    void updateAttendance() {
        cout << "\nEnter total classes conducted: ";
        cin >> totalClasses;
        cout << "Enter classes attended: ";
        cin >> attendedClasses;

        if (attendedClasses > totalClasses) {
            cout << "Attended classes cannot be more than total classes. Resetting attendance.\n";
            totalClasses = 0;
            attendedClasses = 0;
        }
    }

    float getAverageMarks() const {
        float total = 0;
        for (int i = 0; i < 5; i++) total += marks[i];
        return total / 5.0f;
    }

    float getGPA() const {
        return getAverageMarks() / 10.0f;
    }

    float getAttendancePercentage() const {
        if (totalClasses == 0) return 0;
        return (attendedClasses * 100.0f) / totalClasses;
    }

    string getGrade() const {
        float avg = getAverageMarks();
        if (avg >= 90) return "A+";
        if (avg >= 80) return "A";
        if (avg >= 70) return "B";
        if (avg >= 60) return "C";
        if (avg >= 50) return "D";
        return "F";
    }

    bool isFailing() const {
        for (int i = 0; i < 5; i++) {
            if (marks[i] < 35) return true;
        }
        return false;
    }

    void displayShort() const {
        cout << left << setw(10) << rollNo
             << setw(22) << name
             << setw(15) << branch
             << setw(10) << semester
             << setw(12) << fixed << setprecision(2) << getGPA()
             << setw(14) << fixed << setprecision(2) << getAttendancePercentage()
             << setw(8) << getGrade() << "\n";
    }

    void displayReportCard() const {
        cout << "\n====================================================\n";
        cout << "                 STUDENT REPORT CARD\n";
        cout << "====================================================\n";
        cout << "Roll No        : " << rollNo << "\n";
        cout << "Name           : " << name << "\n";
        cout << "Branch         : " << branch << "\n";
        cout << "Semester       : " << semester << "\n";
        cout << "----------------------------------------------------\n";
        for (int i = 0; i < 5; i++) {
            cout << "Subject " << i + 1 << " Marks : " << marks[i] << "/100\n";
        }
        cout << "----------------------------------------------------\n";
        cout << "Average Marks  : " << fixed << setprecision(2) << getAverageMarks() << "%\n";
        cout << "GPA            : " << fixed << setprecision(2) << getGPA() << "/10\n";
        cout << "Grade          : " << getGrade() << "\n";
        cout << "Attendance     : " << fixed << setprecision(2) << getAttendancePercentage() << "%\n";
        cout << "Status         : " << (isFailing() ? "Needs Improvement" : "Pass") << "\n";
        cout << "====================================================\n";
    }

    string toFileString() const {
        stringstream ss;
        ss << rollNo << "|"
           << name << "|"
           << branch << "|"
           << semester << "|";
        for (int i = 0; i < 5; i++) {
            ss << marks[i];
            if (i != 4) ss << ",";
        }
        ss << "|" << totalClasses << "|" << attendedClasses;
        return ss.str();
    }

    static Student fromFileString(const string& line) {
        Student s;
        stringstream ss(line);
        string part;
        vector<string> data;

        while (getline(ss, part, '|')) {
            data.push_back(part);
        }

        if (data.size() >= 7) {
            s.rollNo = stoi(data[0]);
            s.name = data[1];
            s.branch = data[2];
            s.semester = stoi(data[3]);

            stringstream markStream(data[4]);
            string mark;
            int i = 0;
            while (getline(markStream, mark, ',') && i < 5) {
                s.marks[i] = stof(mark);
                i++;
            }

            s.totalClasses = stoi(data[5]);
            s.attendedClasses = stoi(data[6]);
        }

        return s;
    }
};

class StudentManagementSystem {
private:
    vector<Student> students;
    const string filename = "students.txt";

    int findStudentIndex(int rollNo) {
        for (int i = 0; i < (int)students.size(); i++) {
            if (students[i].getRollNo() == rollNo) return i;
        }
        return -1;
    }

    static string toLowerCase(string text) {
        transform(text.begin(), text.end(), text.begin(),
                  [](unsigned char c) { return tolower(c); });
        return text;
    }

public:
    void loadFromFile() {
        students.clear();
        ifstream file(filename);
        string line;

        if (!file.is_open()) return;

        while (getline(file, line)) {
            if (!line.empty()) {
                try {
                    students.push_back(Student::fromFileString(line));
                } catch (...) {
                    // Skip corrupted line
                }
            }
        }
        file.close();
    }

    void saveToFile() {
        ofstream file(filename);
        for (const Student& s : students) {
            file << s.toFileString() << "\n";
        }
        file.close();
    }

    void addStudent() {
        int rollNo, semester;
        string name, branch;

        cout << "\nEnter Roll Number: ";
        cin >> rollNo;

        if (findStudentIndex(rollNo) != -1) {
            cout << "Student with this roll number already exists.\n";
            return;
        }

        cin.ignore(numeric_limits<streamsize>::max(), '\n');

        cout << "Enter Student Name: ";
        getline(cin, name);

        cout << "Enter Branch: ";
        getline(cin, branch);

        cout << "Enter Semester: ";
        cin >> semester;

        Student s(rollNo, name, branch, semester);
        students.push_back(s);
        saveToFile();

        cout << "\nStudent added successfully.\n";
    }

    void viewAllStudents() {
        if (students.empty()) {
            cout << "\nNo student records found.\n";
            return;
        }

        cout << "\n====================================================================================\n";
        cout << left << setw(10) << "Roll No"
             << setw(22) << "Name"
             << setw(15) << "Branch"
             << setw(10) << "Sem"
             << setw(12) << "GPA"
             << setw(14) << "Attendance"
             << setw(8) << "Grade" << "\n";
        cout << "====================================================================================\n";

        for (const Student& s : students) {
            s.displayShort();
        }
    }

    void searchStudent() {
        int choice;
        cout << "\nSearch By:\n";
        cout << "1. Roll Number\n";
        cout << "2. Name\n";
        cout << "Enter choice: ";
        cin >> choice;

        if (choice == 1) {
            int rollNo;
            cout << "Enter Roll Number: ";
            cin >> rollNo;

            int index = findStudentIndex(rollNo);
            if (index == -1) {
                cout << "Student not found.\n";
            } else {
                students[index].displayReportCard();
            }
        } else if (choice == 2) {
            cin.ignore(numeric_limits<streamsize>::max(), '\n');
            string name;
            cout << "Enter name keyword: ";
            getline(cin, name);

            string keyword = toLowerCase(name);
            bool found = false;

            for (const Student& s : students) {
                if (toLowerCase(s.getName()).find(keyword) != string::npos) {
                    s.displayReportCard();
                    found = true;
                }
            }

            if (!found) cout << "No matching student found.\n";
        } else {
            cout << "Invalid choice.\n";
        }
    }

    void updateStudent() {
        int rollNo;
        cout << "\nEnter Roll Number to update: ";
        cin >> rollNo;

        int index = findStudentIndex(rollNo);
        if (index == -1) {
            cout << "Student not found.\n";
            return;
        }

        int choice;
        cout << "\n1. Update basic details\n";
        cout << "2. Update marks\n";
        cout << "3. Update attendance\n";
        cout << "Enter choice: ";
        cin >> choice;

        if (choice == 1) {
            string name, branch;
            int semester;

            cin.ignore(numeric_limits<streamsize>::max(), '\n');

            cout << "Enter new name: ";
            getline(cin, name);
            cout << "Enter new branch: ";
            getline(cin, branch);
            cout << "Enter new semester: ";
            cin >> semester;

            students[index].setName(name);
            students[index].setBranch(branch);
            students[index].setSemester(semester);

            cout << "Details updated successfully.\n";
        } else if (choice == 2) {
            students[index].enterMarks();
            cout << "Marks updated successfully.\n";
        } else if (choice == 3) {
            students[index].updateAttendance();
            cout << "Attendance updated successfully.\n";
        } else {
            cout << "Invalid choice.\n";
            return;
        }

        saveToFile();
    }

    void deleteStudent() {
        int rollNo;
        cout << "\nEnter Roll Number to delete: ";
        cin >> rollNo;

        int index = findStudentIndex(rollNo);
        if (index == -1) {
            cout << "Student not found.\n";
            return;
        }

        students.erase(students.begin() + index);
        saveToFile();
        cout << "Student deleted successfully.\n";
    }

    void showTopperList() {
        if (students.empty()) {
            cout << "\nNo records available.\n";
            return;
        }

        vector<Student> sortedStudents = students;
        sort(sortedStudents.begin(), sortedStudents.end(), [](const Student& a, const Student& b) {
            return a.getGPA() > b.getGPA();
        });

        cout << "\n================ TOPPER LIST ================\n";
        cout << left << setw(10) << "Rank"
             << setw(10) << "Roll"
             << setw(22) << "Name"
             << setw(12) << "GPA"
             << setw(8) << "Grade" << "\n";
        cout << "=============================================\n";

        for (int i = 0; i < (int)sortedStudents.size(); i++) {
            cout << left << setw(10) << i + 1
                 << setw(10) << sortedStudents[i].getRollNo()
                 << setw(22) << sortedStudents[i].getName()
                 << setw(12) << fixed << setprecision(2) << sortedStudents[i].getGPA()
                 << setw(8) << sortedStudents[i].getGrade() << "\n";
        }
    }

    void analyticsReport() {
        if (students.empty()) {
            cout << "\nNo records available.\n";
            return;
        }

        float totalGPA = 0;
        float totalAttendance = 0;
        int failingCount = 0;

        for (const Student& s : students) {
            totalGPA += s.getGPA();
            totalAttendance += s.getAttendancePercentage();
            if (s.isFailing()) failingCount++;
        }

        cout << "\n================ ACADEMIC ANALYTICS ================\n";
        cout << "Total Students              : " << students.size() << "\n";
        cout << "Average Class GPA           : " << fixed << setprecision(2) << totalGPA / students.size() << "\n";
        cout << "Average Attendance          : " << fixed << setprecision(2) << totalAttendance / students.size() << "%\n";
        cout << "Students Needing Improvement: " << failingCount << "\n";
        cout << "Pass Percentage             : " << fixed << setprecision(2)
             << ((students.size() - failingCount) * 100.0f) / students.size() << "%\n";
        cout << "====================================================\n";
    }
};

bool login() {
    string username, password;

    cout << "\n=============================================\n";
    cout << "      SMART STUDENT MANAGEMENT SYSTEM\n";
    cout << "=============================================\n";
    cout << "Admin Login\n";
    cout << "Username: ";
    cin >> username;
    cout << "Password: ";
    cin >> password;

    return username == "admin" && password == "admin123";
}

void showMenu() {
    cout << "\n\n========== MAIN MENU ==========\n";
    cout << "1. Add Student\n";
    cout << "2. View All Students\n";
    cout << "3. Search Student\n";
    cout << "4. Update Student\n";
    cout << "5. Delete Student\n";
    cout << "6. Show Topper List\n";
    cout << "7. Academic Analytics Report\n";
    cout << "8. Exit\n";
    cout << "Enter your choice: ";
}

int main() {
    StudentManagementSystem sms;
    sms.loadFromFile();

    if (!login()) {
        cout << "\nInvalid login. Program closed.\n";
        return 0;
    }

    cout << "\nLogin successful.";

    int choice;

    do {
        showMenu();
        cin >> choice;

        switch (choice) {
            case 1:
                sms.addStudent();
                break;
            case 2:
                sms.viewAllStudents();
                break;
            case 3:
                sms.searchStudent();
                break;
            case 4:
                sms.updateStudent();
                break;
            case 5:
                sms.deleteStudent();
                break;
            case 6:
                sms.showTopperList();
                break;
            case 7:
                sms.analyticsReport();
                break;
            case 8:
                cout << "\nThank you for using Smart Student Management System.\n";
                break;
            default:
                cout << "\nInvalid choice. Try again.\n";
        }

    } while (choice != 8);

    return 0;
}
