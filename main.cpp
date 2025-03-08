#include <iostream>
#include <algorithm>
#include <stack>
#include <fstream>
#include <sstream>

using namespace std;

int id = 0;
const string RED = "\033[31m";    // Red color
const string GREEN = "\033[32m";  // Green color
const string YELLOW = "\033[33m"; // Yellow color
const string BLUE = "\033[34m";   // Blue color
const string RESET = "\033[0m";
const string ORANGE = "\033[38;5;214m";
const string WHITE = "\033[97m";
void menu();

void loadData();
void saveData();
struct Employee
{
    string name;
    string lname;
    int id;
    string joinedDate;
    string position;
    double salary;
    Employee *employeeUnder;
    Employee *coworker;
    Employee *prevCoworker;
};

void displayTree(Employee *, int);
const int MAX_EMPLOYEES = 100;
Employee *employeeArray[MAX_EMPLOYEES];
int employeeCount = 0;
Employee *manager; // Global manager node

void navigate()
{
    string command;
    cout << WHITE << "\n\nDo you want to get back? (y/n): ";
    cin >> command;
    if (command == "y")
    {
        menu();
    }
    else
    {
        return;
    }
}
// Debugged validateEmployeeByID function
Employee *validateEmployeeByID(Employee *node, int command)
{
    if (node == nullptr)
    {
        return nullptr;
    }
    else if (node->id == command)
    {
        return node;
    }

    Employee *found = validateEmployeeByID(node->employeeUnder, command); // Search in subordinates
    if (found == nullptr)
    {
        found = validateEmployeeByID(node->coworker, command); // Search in coworkers
    }
    return found; // Return the found employee if any
}

Employee *findParentByID(Employee *node, int targetID)
{
    if (node == nullptr)
        return nullptr;

    Employee *current = node->employeeUnder;
    while (current != nullptr)
    {
        if (current->id == targetID)
        {
            return node;
        }
        current = current->coworker;
    }

    Employee *search = node->employeeUnder;
    while (search != nullptr)
    {
        Employee *foundParent = findParentByID(search, targetID);
        if (foundParent != nullptr)
            return foundParent;
        search = search->coworker;
    }

    return nullptr;
}

void promoteEmployee()
{
    int command;
    string command2;

re_prompt: // Label for re-prompt6ing
    cout << WHITE << "Enter employee ID: ";
    cin >> command;
    if (command == 0)
    {
        cout << RED << "You can't promote manager \nAlreadt at the Highest Level!";
    }
    else
    {

        Employee *temp = validateEmployeeByID(manager, command);

        if (temp == nullptr)
        {
            cout << RED << "\nThis ID does not exist\n";
            goto re_prompt;
        }
        else
        {
            cout << ORANGE << "\n\nName: " << temp->name << " " << temp->lname;
            cout << "\nPosition: " << temp->position;
            cout << "\nSalary: " << temp->salary;
            cout << "\nDate Joined: " << temp->joinedDate;
            // cout << "\n\nEnter new position: ";
            // cin >> command2;

            if (temp->position == "ass")
            {

                cout << RED << "\nEmployee is at the highest Position!\nCant be promoted";
            }
            else
            {

                // Promote one level up to be a coworker of their current manager
                Employee *parent = findParentByID(manager, command);

                // Ensure the parent is found
                if (parent != nullptr)
                {
                    // Detach from current position
                    if (temp->prevCoworker != nullptr)
                    {
                        temp->prevCoworker->coworker = temp->coworker;
                    }
                    else
                    {
                        parent->employeeUnder = temp->coworker;
                    }
                    if (temp->coworker != nullptr)
                    {
                        temp->coworker->prevCoworker = temp->prevCoworker;
                    }

                    temp->coworker = nullptr;
                    temp->prevCoworker = nullptr;

                    // Move as a coworker of the parent
                    Employee *newCoworker = parent->coworker;
                    if (newCoworker == nullptr)
                    {
                        parent->coworker = temp;
                        temp->prevCoworker = parent;
                    }
                    else
                    {
                        while (newCoworker->coworker != nullptr)
                        {
                            newCoworker = newCoworker->coworker;
                        }
                        newCoworker->coworker = temp;
                        temp->prevCoworker = newCoworker;
                    }
                    if (parent->position == "ass")
                        temp->position = "ass";
                    else if (parent->position == "man")
                    {
                        if (manager->coworker == nullptr)
                        {
                            temp->position = "man";
                            manager->coworker = temp;
                            temp->prevCoworker = manager;
                            temp->coworker = nullptr;
                        }
                        else
                        {
                            Employee *manger2 = manager;
                            while (manger2->coworker != nullptr)
                                manger2 = manger2->coworker;
                            manger2->coworker = temp;
                            temp->prevCoworker = manger2;
                            temp->coworker = nullptr;
                        }
                    }
                    // temp->position = command2;
                    cout << GREEN << "\nEmployee promoted to " << " and moved up one level.\n";
                }
                else
                {
                    cout << RED << "\nError:(Head error) not found!\n";
                }
            }
        }
    }
    navigate();
}

void addEmployee()
{
    system("cls");
    string pos;
    double salary;
    Employee *newEmployee = new Employee;
    cout << "First Name: " << ORANGE;
    cin >> newEmployee->name;
    cout << WHITE << "Last Name: " << ORANGE;
    cin >> newEmployee->lname;
    cout << WHITE << "Date Joined(Format: YYYY-MM-DD): " << ORANGE;
    cin >> newEmployee->joinedDate;
    string positions[20] = {"ass", "Che", "diWash", "wtr", "jan", "bart"};
    cout << WHITE << "\n\nPosition List\t   \tCode" << ORANGE << "\n\n~Assistant Manager:  \t ass"
         << "\n~Lead Chef:\t  \t Che"
         << "\n~Dishwasher:\t  \t diWash"
         << "\n~Waiter:\t   \t wtr"
         << "\n~Janitor:\t  \t jan"
         << "\n~Bartender:\t  \t bart";
    cout << WHITE << "\n\nPosition: " << ORANGE;
    cin >> pos;
    cout << WHITE << "\nEnter Salary: " << ORANGE;

    int size = sizeof(positions) / sizeof(positions[0]);
    int count = std::count(positions, positions + size, pos);
    if (count > 0 && cin >> salary)
    {
        id++;
        newEmployee->salary = salary;
        newEmployee->id = id;
        newEmployee->position = pos;
        newEmployee->coworker = nullptr;

        newEmployee->employeeUnder = nullptr;

        if (pos == "ass" && manager->coworker == nullptr)
        {
            if (manager->employeeUnder == nullptr)
            {
                manager->employeeUnder = newEmployee;
            }
            else
            {
                Employee *temp = manager->employeeUnder;
                while (temp->coworker != nullptr)
                    temp = temp->coworker;
                temp->coworker = newEmployee;
                newEmployee->prevCoworker = temp;
            }
        }

        else
        {
            if (manager->employeeUnder == nullptr)
            {
                cout << GREEN << "You only have a Manager!";
                manager->employeeUnder = newEmployee;
                newEmployee->prevCoworker = nullptr;
                newEmployee->coworker = nullptr;
            }
            else
            {
                int command;
            label:
                cout << WHITE << "\nChoose Manager to this employee \n";
                Employee *temp = manager->employeeUnder;
                displayTree(manager, 0);
                cout << WHITE << "Enter id here: " << ORANGE;
                if (cin >> command && command >= 0 && command < id)
                {
                    if (command == 0)
                    {
                        if (manager->employeeUnder == nullptr)
                        {
                            manager->employeeUnder = newEmployee;
                            newEmployee->prevCoworker = nullptr;
                            newEmployee->coworker = nullptr;
                        }
                        else
                        {
                            Employee *node2 = manager->employeeUnder;
                            while (node2->coworker != nullptr)
                                node2 = node2->coworker;
                            node2->coworker = newEmployee;
                            newEmployee->prevCoworker = node2;
                        }
                    }
                    else
                    {
                        Employee *temp2 = validateEmployeeByID(manager, command);

                        if (temp2 != nullptr)
                        {
                            if (temp2->employeeUnder == nullptr)
                            {
                                temp2->employeeUnder = newEmployee;
                                newEmployee->prevCoworker = nullptr;
                                newEmployee->coworker = nullptr;
                            }
                            else
                            {
                                Employee *temp3 = temp2->employeeUnder;
                                while (temp3->coworker != nullptr)
                                    temp3 = temp3->coworker;
                                temp3->coworker = newEmployee;
                                newEmployee->prevCoworker = temp3;
                            }
                        }
                    }
                }
                else
                {
                    system("cls");
                    cout << RED << "\nPlease Enter valid id\n";
                    goto label;
                }
            }
        }
        cout << GREEN << "\nSuccessfully added employee!\n";
    }
    else
    {
        cout << RED << "\nInvalid Position or Salary!\n";
    }
    navigate();
}
void displayTree(Employee *node, int level = 0)
{
    if (node == nullptr)
    {
        return;
    }

    // Print the current employee with indentation based on their level in the hierarchy
    for (int i = 0; i < level; i++)
    {
        cout << "    "; // Indentation for hierarchy levels
    }
    cout << ORANGE << "|-- " << node->name << " ( Position: " << node->position << ", ID: " << node->id << ")" << endl;

    // Display employees directly under the current node
    displayTree(node->employeeUnder, level + 1);
    // Display coworkers at the same level
    displayTree(node->coworker, level);
}
void deleteEmployeeByID(int command)
{
    Employee *temp = validateEmployeeByID(manager, command);

    if (temp == nullptr)
    {
        cout << RED << "\nThis ID does not exist\n";
        return;
    }

    Employee *parent = findParentByID(manager, command);

    // If the employee has subordinates
    if (temp->employeeUnder != nullptr)
    {
        Employee *promoted = temp->employeeUnder;             // The first subordinate becomes the new manager
        Employee *remainingSubordinates = promoted->coworker; // The rest should be placed under promoted

        // If deleting the top-level manager, update global reference
        if (temp == manager)
        {
            manager = promoted;
            manager->prevCoworker = nullptr; // The new top manager has no previous coworker
        }
        else
        {
            // Reassign the promoted employee to the parent
            if (parent->employeeUnder == temp)
            {
                parent->employeeUnder = promoted;
            }
            else
            {
                Employee *sibling = parent->employeeUnder;
                while (sibling != nullptr && sibling->coworker != temp)
                {
                    sibling = sibling->coworker;
                }
                if (sibling != nullptr)
                {
                    sibling->coworker = promoted;
                }
            }
        }

        // Ensure the promoted employee takes the position of the deleted one
        promoted->position = temp->position;
        promoted->coworker = temp->coworker; // Maintain previous coworker linkage

        // If there are remaining subordinates, place them under the new manager
        promoted->employeeUnder = remainingSubordinates;
        if (remainingSubordinates != nullptr)
        {
            remainingSubordinates->prevCoworker = nullptr; // Reset previous coworker pointer
        }
    }
    else
    {
        // If no subordinates, update parent’s link
        if (parent->employeeUnder == temp)
        {
            parent->employeeUnder = temp->coworker;
        }
        else if (temp->prevCoworker != nullptr)
        {
            temp->prevCoworker->coworker = temp->coworker;
        }
    }

    // Maintain coworker linkage
    if (temp->coworker != nullptr)
    {
        temp->coworker->prevCoworker = temp->prevCoworker;
    }

    // Delete the employee
    delete temp;

    cout << GREEN << "\nEmployee with ID " << command << " has been deleted and replaced.\n";
}

void searchEmployeeByID(int v)
{
    Employee *found = validateEmployeeByID(manager, v);
    cout << WHITE << "\n\n\tSearch Result: ";
    if (found == nullptr)
    {
        cout << RED << " There is No employee with id " << v;
    }
    else
    {
        cout << GREEN << " Employee Founded!" << YELLOW << "\n\nName: " << found->name << "\n\nID: " << found->id << "\n\nPosition: " << found->position;
    }
}
void extractEmployeesToArray(Employee *node, Employee *arr[], int &count)
{
    if (node == nullptr || count >= MAX_EMPLOYEES)
        return;

    arr[count++] = node;
    extractEmployeesToArray(node->employeeUnder, arr, count);
    extractEmployeesToArray(node->coworker, arr, count);
}
int partitionBySalary(Employee *arr[], int low, int high)
{
    double pivot = arr[low]->salary; // Using low element as pivot
    int start = low + 1;
    int end_ = high;

    while (start <= end_)
    {
        while (start <= end_ && arr[start]->salary <= pivot)
        {
            start++;
        }
        while (arr[end_]->salary > pivot)
        {
            end_--;
        }
        if (start < end_)
        {
            swap(arr[start], arr[end_]);
        }
    }
    swap(arr[low], arr[end_]);
    return end_;
}

void quickSortBySalary(Employee *arr[], int low, int high)
{
    if (low < high)
    {
        int pi = partitionBySalary(arr, low, high);
        quickSortBySalary(arr, low, pi - 1);
        quickSortBySalary(arr, pi + 1, high);
    }
}

int partitionByDate(Employee *arr[], int low, int high)
{
    string pivot = arr[low]->joinedDate; // Using low element as pivot
    int start = low + 1;
    int end_ = high;

    while (start <= end_)
    {
        while (start <= end_ && arr[start]->joinedDate <= pivot)
        {
            start++;
        }
        while (arr[end_]->joinedDate > pivot)
        {
            end_--;
        }
        if (start < end_)
        {
            swap(arr[start], arr[end_]);
        }
    }
    swap(arr[low], arr[end_]);
    return end_;
}

void quickSortByDate(Employee *arr[], int low, int high)
{
    if (low < high)
    {
        int pi = partitionByDate(arr, low, high);
        quickSortByDate(arr, low, pi - 1);
        quickSortByDate(arr, pi + 1, high);
    }
}

void sortAndPrintBySalary(Employee *node)
{
    Employee *employeeArray[100]; // Assuming a max of 100 employees
    int employeeCount = 0;
    extractEmployeesToArray(node, employeeArray, employeeCount);
    quickSortBySalary(employeeArray, 0, employeeCount - 1);

    // Print sorted employees
    cout << WHITE << "\nEmployees sorted by salary:\n";
    cout << "-------------------------------------------------------------------\n";
    cout << "\nID\tName\t\t\tPosition\t\tSalary\n";
    cout << WHITE << "-------------------------------------------------------------------\n";

    for (int i = 0; i < employeeCount; i++)
    {
        cout << GREEN << employeeArray[i]->id << ORANGE << "\t" << employeeArray[i]->name << " " << employeeArray[i]->lname
             << "\t\t " << GREEN << employeeArray[i]->position << "\t\t\t" << ORANGE << employeeArray[i]->salary << endl;
    }
}

void sortAndPrintByDate(Employee *node)
{
    Employee *employeeArray[100];
    int employeeCount = 0;
    extractEmployeesToArray(node, employeeArray, employeeCount);

    quickSortByDate(employeeArray, 0, employeeCount - 1);

    // Print sorted employees
    cout << WHITE << "\nEmployees sorted by date joined:\n";
    cout << "-------------------------------------------------------------------\n";
    cout << "\nID\tName\t\t\tPosition\t\tDate Joined\n";
    cout << WHITE << "-------------------------------------------------------------------\n";

    for (int i = 0; i < employeeCount; i++)
    {
        cout << GREEN << employeeArray[i]->id << "\t" << ORANGE << employeeArray[i]->name << " " << employeeArray[i]->lname
             << "\t\t " << GREEN << employeeArray[i]->position << "\t\t\t" << ORANGE << employeeArray[i]->joinedDate << endl;
    }
}

void analytics()
{
    system("cls");
    cout << WHITE << "Analytics Page";
    // listLeveofAuthority(manager, 0);
    cout << "\n\n";
    sortAndPrintByDate(manager);
    cout << "\n\n";
    sortAndPrintBySalary(manager);
    navigate();
}

void increaseSalary(int v)
{
    Employee *found = validateEmployeeByID(manager, v);
    cout << WHITE << "\n\n\tSearch Result: ";
    if (found == nullptr)
    {
        cout << RED << " There is  No employee with id " << v;
    }
    else
    {
        double salary;
        cout << GREEN << " Employee Founded!\n\n"
             << ORANGE << "Name: " << found->name << " " << found->lname << "\n\nID: " << found->id << "\n\nPosition: " << found->position << "\n\nSalary: " << found->salary << "\n\nDate Joined: " << found->joinedDate;
    label3:
        cout << WHITE << "\n\nEnter new salary: ";
        if (cin >> salary)
        {
            found->salary = salary;
            cout << GREEN << "\nSalary is updated successfully!";
        }
        else
        {
            system("cls");
            cout << RED << "\nPlease insert valid salary!";
            goto label3;
        }
    }
    navigate();
}

void menu()
{
    int v;
    system("cls");
    string command;
    cout << ORANGE << "\n1. Add employee\t\t 2. Display\t\t 3. Promote Employee\n"
         << WHITE << "4. Head of Employee\t 5. Fire Employee"
         << "\t 6.Search Emplyee\n"
         << ORANGE << "7. Analytics\t\t 8. Increase Salary\t 9. Save and Exit\n\n"
         << WHITE << "Enter command: ";
    cin >> command;
    if (command == "1")
    {
        addEmployee();
    }
    else if (command == "2")
    {
        system("cls");
        displayTree(manager, 0);
        navigate();
    }
    else if (command == "3")
        promoteEmployee();
    else if (command == "4")
    {
        system("cls");

        cout << "\nEnter id of employee : ";
        cin >> v;
        Employee *employee1 = validateEmployeeByID(manager, id);
        if (employee1 == nullptr)
            cout << "There is no Employee with" << v << " this id\n";
        else
        {
            Employee *parent = findParentByID(manager, v);
            if (parent == nullptr)
                cout << "\nNo Head boss above " << employee1->name;
            else
                cout << "\nBoss of " << employee1->name << " ( Position: " << employee1->position << ", ID: " << employee1->id << ")" << " is: " << parent->name << endl;
        }
        navigate();
    }
    else if (command == "5")
    {
        system("cls");
        cout << "\nEnter id: ";
        cin >> v;
        deleteEmployeeByID(v);
        navigate();
    }
    else if (command == "6")
    {
        system("cls");
        cout << "\nEnter id to search employee: ";
        cin >> v;
        searchEmployeeByID(v);
        navigate();
    }
    else if (command == "7")
    {
        analytics();
    }
    else if (command == "8")
    {
        system("cls");
        cout << "\nEnter id of employee that salary is to be updated: ";
        cin >> v;
        increaseSalary(v);
    }
    else if (command == "9")
    {
        saveData();
        return;
    }
}

int main()
{

    loadData();
    menu();

    return 0;
}

const string FILE_NAME = "employees.dat";

// Save function that preserves hierarchy
void saveToFile(Employee *root, ofstream &file)
{
    if (!root)
        return;

    // Indicate a new employee entry
    file << root->id << " " << root->name << " " << root->lname << " "
         << root->joinedDate << " " << root->salary << " " << root->position << endl;

    if (root->employeeUnder)
    {
        file << "{" << endl; // Mark start of subordinates
        saveToFile(root->employeeUnder, file);
        file << "}" << endl; // Mark end of subordinates
    }

    saveToFile(root->coworker, file);
}

// Save entire employee tree to file
void saveData()
{
    ofstream file(FILE_NAME, ios::trunc);
    if (file.is_open())
    {
        saveToFile(manager, file);
        file.close();
    }
    else
    {
        cout << "Error: Unable to open file for writing." << endl;
    }
}

// Function to load employee data from file and build hierarchy
Employee *loadFromFile(std::ifstream &file)
{
    Employee *root = nullptr;
    Employee *last = nullptr;
    std::stack<Employee *> parentStack; // Stack to track hierarchy
    int maxID = 0;                      // Track the maximum ID encountered

    std::string line;
    while (getline(file, line))
    {
        if (line == "{")
        {
            parentStack.push(last); // Enter new hierarchy level
            continue;
        }
        if (line == "}")
        {
            parentStack.pop(); // Exit hierarchy level
            continue;
        }

        std::istringstream ss(line);
        std::string idStr, salaryStr;
        std::string name, lname, joinedDate, position;
        ss >> idStr >> name >> lname >> joinedDate >> salaryStr >> position;

        int id = std::stoi(idStr);
        double salary = std::stod(salaryStr);

        maxID = std::max(maxID, id); // Update max ID encountered

        Employee *emp = new Employee;
        emp->id = id;
        emp->name = name;
        emp->lname = lname;
        emp->joinedDate = joinedDate;
        emp->salary = salary;
        emp->position = position;
        emp->prevCoworker = nullptr;
        emp->employeeUnder = nullptr;
        emp->coworker = nullptr;

        if (!root)
        {
            root = emp;
        }
        else
        {
            if (!parentStack.empty())
            {
                Employee *parent = parentStack.top();
                if (!parent->employeeUnder)
                {
                    parent->employeeUnder = emp;
                }
                else
                {
                    Employee *sibling = parent->employeeUnder;
                    while (sibling->coworker)
                    {
                        sibling = sibling->coworker;
                    }
                    sibling->coworker = emp;
                    emp->prevCoworker = sibling;
                }
            }
        }
        last = emp;
    }

    id = maxID;
    cout << endl
         << id << endl;

    return root;
}

void loadData()
{
    ifstream file(FILE_NAME);
    if (file.is_open())
    {
        manager = loadFromFile(file);
        file.close();
    }
    else
    {
        manager = new Employee;
        manager->name = "Getchew";
        manager->lname = "Tasew";
        manager->prevCoworker = nullptr;
        manager->joinedDate = "2025-1-1";
        manager->coworker = nullptr;
        manager->id = 0;

        manager->salary = 40000;
        manager->employeeUnder = nullptr;
        manager->position = "man";

        saveData(); // Save initial data
    }
}
