//==============================================================================//
// Name        : ProjectTwo.cpp													//
// Author      : Matthew Oliphant												//
// Version     : 2.0															//
// Copyright   :																//
// Description : Load courses from file and add to data structure				//
//==============================================================================//

#include <iostream>
#include <fstream>

#include "CSVparser.hpp"

using namespace std;

//==============================================================================//
// Global definitions visible to all methods and classes						//
//==============================================================================//

/**
 * Each course contains an Id, Name, and a vector to hold as many prereqs
 * as the course has
 */
struct Course {
	string courseNum;
	string courseName;
	vector<string> preReqs;

	Course() {}
};

/**
 * Each node in the BST contains the course info and left and right children
 * Multiple constructors allow a node with or without a defined course
 */
struct Node {
	Course course;
	Node* left;
	Node* right;

	Node() {
		left = nullptr;
		right = nullptr;
	}

	Node(Course aCourse) : Node() {
		this->course = aCourse;
	}
};


//==============================================================================//
// CourseBST Class Definition													//
//==============================================================================//

class CourseBST {

private:
	Node* root;

	void addNode(Node* node, Course course);
	void printSampleSchedule(Node* node);
	void printCourseInformation(Node* node, string courseNum);


public:
	CourseBST();
	virtual ~CourseBST();
	void DeleteRecursive(Node* node);
	void Insert(Course course);
	int NumPrerequisiteCourses(Course course);
	void PrintSampleSchedule();
	void PrintCourseInformation(string courseNum);

};

/**
 * Construct a BST and initialize the root
 */
CourseBST::CourseBST() {
	root = nullptr;
}

/**
 * Deconstruct a BST by passing the root to the function that recursively deletes each node
 */
CourseBST::~CourseBST() {
	DeleteRecursive(root);
}

/**
 * Delete a BST nodes recursively - used by the deconstructor
 */
void CourseBST::DeleteRecursive(Node* node) {
	if (node) {
		DeleteRecursive(node->left);
		DeleteRecursive(node->right);
		delete node;
	}
}

/**
 * Insert a node into the BST
 *
 * If the root is not initialized, initialize it with the current course info
 * Else, send the node to the addNode method to be added to the BST
 */
void CourseBST::Insert(Course course) {
	if (root == nullptr)
		root = new Node(course);
	else
		this->addNode(root, course);
}

/**
 * Returns the number of prereqs the passed course has
 */
int CourseBST::NumPrerequisiteCourses(Course course) {
	int count = 0;
	for (unsigned int i = 0; i < course.preReqs.size(); i++) {
		if (course.preReqs.at(i).length() > 0)
			count++;
	}
	return count;
}

/**
 * Passes the BST root to the private method
 */
void CourseBST::PrintSampleSchedule() {
	this->printSampleSchedule(root);
}

/**
 * Passes the BST root and the course Id to be searched for & displayed if found
 */
void CourseBST::PrintCourseInformation(string courseNum) {
	this->printCourseInformation(root, courseNum);
}

/**
 * Utilizes the public Insert method
 *
 * Determines & sets correct alphanumeric location & if the current child is a nullptr,
 * adds the node to the BST, else keeps traversing BST until a nullptr is found
 */
void CourseBST::addNode(Node* node, Course course) {

	// Current courseNum is less than the current node's courseNum
	if (node->course.courseNum.compare(course.courseNum) > 0) {
		if (node->left == nullptr)
			node->left = new Node(course);
		else
			this->addNode(node->left, course);
	}
	// Current courseNum is equal or greater than the current node's courseNum
	else {
		if (node->right == nullptr)
			node->right = new Node(course);
		else
			this->addNode(node->right, course);
	}
}

/**
 * Recursively prints the loaded courses in-order
 */
void CourseBST::printSampleSchedule(Node* node) {

	if (node != nullptr) {
		printSampleSchedule(node->left);
		cout << node->course.courseNum << ", " << node->course.courseName << endl;
		printSampleSchedule(node->right);
	}
	return;
}

/**
 * Displays a single courses information including its prereqs
 */
void CourseBST::printCourseInformation(Node* curr, string courseNum) {

	// Traverse BST until bottom reached or matching courseNum found
	while (curr != nullptr) {

		// Passed courseNum matches the current courseNum
		if (curr->course.courseNum.compare(courseNum) == 0) {

			// Display course & get num prereqs
			cout << endl << curr->course.courseNum << ", " << curr->course.courseName << endl;
			unsigned int size = NumPrerequisiteCourses(curr->course);
			cout << "Prerequisite(s): ";

			// If prereqs exist, display each prereq with proper formatting
			unsigned int i = 0;
			for (i = 0; i < size; i++) {
				cout << curr->course.preReqs.at(i);
				if (i != size - 1)
					cout << ", ";
			}
			// If no prereqs, inform user and return to stop the search
			if (i == 0)
				cout << "No prerequisites required.";
			cout << endl;
			return;
		}

		// Passed courseNum is smaller than the current courseNum so traverse left
		else if (courseNum.compare(curr->course.courseNum) < 0)
			curr = curr->left;

		// Passed courseNum is larger than the current courseNum so traverse right
		else
			curr = curr->right;
	}
	// nullptr has been reached but course hasn't been found/ printed, so inform user
	cout << "Course " << courseNum << " not found." << endl;
}


//==============================================================================//
// Static methods used for testing												//
//==============================================================================//

/**
 * If the course file has a header file, remove the comments from the
 * section below & comment the try { ... } section that follows it. Default code
 * assumes the file will not have a header.
 */
bool loadCourses(string csvPath, CourseBST* coursesBST) {

	/*

	csv::Parser file = csv::Parser(csvPath);

	try {
	 	for (unsigned int i = 0; i < file.rowCount(); i++) {
			Course course;
			course.courseNum = file[i][0];
			course.courseName = file[i][1];
			if (file[i][2].length() > 0)
				course.preReqs.push_back(file[i][2]);
			else
				course.preReqs.push_back("");
			if (file[i][3].length() > 0)
				course.preReqs.push_back(file[i][3]);
			else
				course.preReqs.push_back("");

			coursesBST->Insert(course);
		}
	}

	*/

	// Open course file, get/ separate each line & insert into BST
	try {
		ifstream courseFile( csvPath );

		if (courseFile.is_open()) {
			while (!courseFile.eof()) {

				// A vector will hold each csv that a course contains
				vector<string> courseInfo;
				string courseData;

				getline(courseFile, courseData);
				while (courseData.length() > 0) {

					// Get substring of each course data, add to vector & delete from str
					unsigned int comma = courseData.find(',');
					if (comma < 100) {  // Any data field is allowed 99 chars or less
						courseInfo.push_back(courseData.substr(0, comma));
						courseData.erase(0, comma + 1);
					}
					// Add the last course after the final comma
					else {
						courseInfo.push_back(courseData.substr(0, courseData.length()));
						courseData = "";
					}
				}

				// Load the separated values into a course, insert into BST & close file
				Course course;
				course.courseNum = courseInfo[0];
				course.courseName = courseInfo[1];

				for (unsigned int i = 2; i < courseInfo.size(); i++) {
					course.preReqs.push_back(courseInfo[i]);
				}
				coursesBST->Insert(course);
			}
			courseFile.close();
			return true;
		}
	}
	catch (csv::Error& e) {
		cerr << e.what() << endl;
	}
	return false;
}


//==============================================================================//
// Main method																	//
//==============================================================================//

int main(int argc, char* argv[]) {

	// Process command line argument
	string csvPath, courseId;
	switch (argc) {
	case 2:
		csvPath = argv[1];
		break;
	case 3:
		csvPath = argv[1];
		courseId = argv[2];
		break;
	default:
		csvPath = "";
		break;
	}

	// Define BST & welcome user
	CourseBST* coursesBST = nullptr;
	cout << "\nWelcome to the course planner!\n" << endl;

	// Making user choice a string & converting first char to int prevents invalid data
	string choice = "0";
	int userChoice = choice[0] - '0';

	while (userChoice != 9) {

		cout << "   1. Load Data Structure" << endl;
		cout << "   2. Print Course List" << endl;
		cout << "   3. Print Course" << endl;
		cout << "   9. Exit" << endl;
		cout << "\nWhat would you like to do? ";
		cin >> choice;

		// Check if user choice is a double digit
		if (choice.length() == 1)
			userChoice = choice[0] - '0';
		else
			userChoice = 0;

		bool success;

		// Handle users choice from menu
		switch (userChoice) {

		// Instantiate BST, get file path name & load courses into BST
		case 1:

			if (coursesBST == nullptr)
				coursesBST = new CourseBST();
			if (csvPath.length() == 0) {
				cout << "Enter the file name that contains the course data: ";
				cin >> csvPath;
			}
			// Determine if the file was opened/ loaded successfully
			success = loadCourses(csvPath, coursesBST);
			if (success)
				cout << "Courses have been loaded.\n" << endl;
			else
				cout << "File not found.\n" << endl;
			csvPath = "";
			break;

		// Validate that a BST exists & print a sample schedule
		case 2:

			if (coursesBST != nullptr && success) {
				cout << "Here is a sample schedule:\n" << endl;
				coursesBST->PrintSampleSchedule();
				cout << endl;
			} else
				cout << "Load courses first - option 1\n" << endl;
			break;

		// Validate that a BST exists & search/ display course info
		case 3:

			if (coursesBST != nullptr && success) {
				if (courseId.length() == 0) {
					cout << "What course do you want to know about? ";
					cin >> courseId;
					for (auto & userChoice: courseId) userChoice = toupper(userChoice);
				}
				coursesBST->PrintCourseInformation(courseId);
				cout << endl;
			} else
				cout << "Load courses first - option 1\n" << endl;
			courseId = "";
			break;

		// User entered invalid data or exited the program
		default:

			if (userChoice != 9)
				cout << choice << " is not a valid option\n" << endl;
			break;
		}
	}
	cout << "\nThank you for using the course planner!" << endl;

	return 0;
}
