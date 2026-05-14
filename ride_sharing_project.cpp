#include <iostream>
#include <fstream>
#include <vector>
#include <string>
#include <algorithm>
#include <ctime>
#include <iomanip>
#include <queue>
#include <stack>
#include <map>
#include <chrono>
#include <sstream>
#include <windows.h>
#include <limits>
using namespace std;
using namespace std::chrono;

// ==================== DATA STRUCTURES ====================
struct Driver {
    int id;
    string name;
    string contact;
    float rating;
    bool available;
    int totalRides;
    double totalEarnings;
    string vehicleType;  // NEW: "Bike", "Car", "AC Car", "SUV"
    
    Driver() {
        id = 0;
        rating = 5.0;
        available = true;
        totalRides = 0;
        totalEarnings = 0.0;
        vehicleType = "Car";  // Default
    }
};

struct Rider {
    int id;
    string name;
    string contact;
    int totalBookings;
    
    Rider() {
        id = 0;
        totalBookings = 0;
    }
};

struct Ride {
    int rideId;
    int driverId;
    int riderId;
    string pickup;
    string destination;
    double distance;
    double fare;
    string status;
    string timestamp;
    string vehicleType;  // NEW
    
    Ride() {
        rideId = 0;
        driverId = 0;
        riderId = 0;
        distance = 0.0;
        fare = 0.0;
        status = "Requested";
        vehicleType = "Car";  // Default
    }
};
// ==================== GLOBAL DATA STRUCTURES ====================
map<int, Driver> drivers;
map<int, Rider> riders;
queue<Ride> rideRequests;
vector<Ride> completedRides;
stack<Ride> cancelledRides;
vector<Ride> assignedRides;

// ==================== FILE NAMES ====================
const string DRIVERS_FILE = "drivers.txt";
const string RIDERS_FILE = "riders.txt";
const string RIDES_FILE = "rides.txt";
const string CANCELLED_FILE = "cancelled_rides.txt";
const string REPORTS_FILE = "reports.txt";
const string CSV_REPORT = "analytics_report.csv";

// ==================== ADMIN & VEHICLE CONSTANTS ====================
// Admin credentials
const string ADMIN_USERNAME = "admin";
const string ADMIN_PASSWORD = "admin123";  // Change this to your preferred password

// Vehicle types and their multipliers
const double BIKE_MULTIPLIER = 0.6;      // 40% cheaper
const double CAR_MULTIPLIER = 1.0;       // Base price
const double AC_CAR_MULTIPLIER = 1.3;    // 30% more expensive
const double SUV_MULTIPLIER = 1.6;       // 60% more expensive

// ==================== FUNCTION PROTOTYPES ====================
void displayMainMenu();
void loadAllData();
void saveAllData();
void adminMenu();
void driverMenu();
void riderMenu();

// Driver Management
void addDriver();
void viewDrivers();
void searchDriver();
void sortDrivers();
void updateDriver();

// Rider Management
void addRider();
void viewRiders();
void searchRider();
void sortRiders();
void updateRider();

// Ride Management
void requestRide();
void requestRideWithDriverSelection(); // OLD version (no parameters)
void requestRideQuick(int riderId);  // NEW - Quick booking
void requestRideWithDriverSelectionNew(int riderId);  // NEW - Choose driver
void viewMyRides(int riderId);  // NEW - View only my rides
void cancelMyRide(int riderId);  // NEW - Cancel only my rides
void rateDriver(int riderId);  // NEW - Rate my driver
void assignRides();
void completeRide();
void cancelRide();
void viewAllRides();
void viewAvailableDrivers();

// Reports & Analytics
void showAnalytics();
void generateReport();
void displaySummary();
void exportToCSV();

// Algorithms
void bubbleSortDriversByName(vector<Driver>& driverList);
void mergeSortDriversByRating(vector<Driver>& driverList, int left, int right);
void quickSortRidersByName(vector<Rider>& riderList, int left, int right);
int linearSearchDriver(int id);
int binarySearchRider(vector<int>& sortedIds, int id);
void compareSearchAlgorithms();
void compareSortAlgorithms();

// Utility Functions
// Utility Functions
string getCurrentTime();
void clearScreen();
void pressAnyKey();
int generateId();
double calculateFare(double distance);
bool adminLogin();
double calculateFareWithVehicle(double distance, string vehicleType);
void displayVehicleOptions();
string getVehicleType(int choice);
void createSampleData();
bool validateFileData(const string& filename);
void handleParsingError(const string& message);
int getValidIntegerInput(const string& prompt, int min, int max);  // ADD THIS
double getValidDoubleInput(const string& prompt, double min, double max);  // ADD THIS

// ==================== UTILITY FUNCTIONS ====================

void clearScreen() {
    #ifdef _WIN32
        system("cls");
    #else
        system("clear");
    #endif
}

void pressAnyKey() {
    cout << "\n\nPress Enter to continue...";
    cin.ignore(numeric_limits<streamsize>::max(), '\n');
    cin.get();
}

string getCurrentTime() {
    time_t now = time(0);
    char* dt = ctime(&now);
    string timeStr(dt);
    timeStr = timeStr.substr(0, timeStr.length()-1);
    return timeStr;
}

int generateId() {
    static int id = 100;
    return id++;
}

double calculateFare(double distance) {
    const double BASE_FARE = 50.0;
    const double PER_KM = 30.0;
    return BASE_FARE + (distance * PER_KM);
}

// NEW: Validate file format
bool validateFileData(const string& filename) {
    ifstream file(filename);
    if (!file) {
        return false;
    }
    
    string line;
    getline(file, line);
    file.close();
    
    return !line.empty();
}

// NEW: Handle parsing errors
void handleParsingError(const string& message) {
    cerr << "\n========================================\n";
    cerr << "ERROR: Data Parsing Failed!\n";
    cerr << "========================================\n";
    cerr << message << "\n";
    cerr << "Please check file format and try again.\n";
    cerr << "========================================\n\n";
}

// NEW FUNCTIONS - ADD BELOW handleParsingError
int getValidIntegerInput(const string& prompt, int min, int max) {
    int value;
    
    while (true) {
        cout << prompt;
        cin >> value;
        
        // Check if input failed (wrong type entered)
        if (cin.fail()) {
            cin.clear();
            cin.ignore(numeric_limits<streamsize>::max(), '\n');
            cout << "❌ Invalid input! Please enter a number: ";
            continue;
        }
        
        // Check if value is in valid range
        if (value < min || value > max) {
            cout << "❌ Please enter a number between " << min << " and " << max << ": ";
            cin.ignore(numeric_limits<streamsize>::max(), '\n');
            continue;
        }
        
        cin.ignore(numeric_limits<streamsize>::max(), '\n');
        return value;
    }
}

double getValidDoubleInput(const string& prompt, double min, double max) {
    double value;
    
    while (true) {
        cout << prompt;
        cin >> value;
        
        if (cin.fail()) {
            cin.clear();
            cin.ignore(numeric_limits<streamsize>::max(), '\n');
            cout << "❌ Invalid input! Please enter a valid number: ";
            continue;
        }
        
        if (value < min || value > max) {
            cout << "❌ Please enter a number between " << min << " and " << max << ": ";
            cin.ignore(numeric_limits<streamsize>::max(), '\n');
            continue;
        }
        
        cin.ignore(numeric_limits<streamsize>::max(), '\n');
        return value;
    }
}
// ==================== ADMIN & VEHICLE FUNCTIONS ====================

// Function to verify admin login
bool adminLogin() {
    clearScreen();
    cout << "========================================\n";
    cout << "           ADMIN LOGIN                  \n";
    cout << "========================================\n\n";
    
    string username, password;
    int attempts = 0;
    const int MAX_ATTEMPTS = 3;
    
    while (attempts < MAX_ATTEMPTS) {
        cout << "Username: ";
        cin >> username;
        
        cout << "Password: ";
        cin >> password;
        cin.ignore();
        
        if (username == ADMIN_USERNAME && password == ADMIN_PASSWORD) {
            cout << "\n[OK] Login successful!\n";
            Sleep(1000);
            return true;
        } else {
            attempts++;
            if (attempts < MAX_ATTEMPTS) {
                cout << "\n[X] Invalid credentials! Attempts remaining: " 
                     << (MAX_ATTEMPTS - attempts) << "\n\n";
            }
        }
    }
    
    cout << "\n[X] Maximum login attempts exceeded!\n";
    Sleep(2000);
    return false;
}

// Function to calculate fare based on distance and vehicle type
double calculateFareWithVehicle(double distance, string vehicleType) {
    const double BASE_FARE = 50.0;
    const double PER_KM = 30.0;
    double baseFare = BASE_FARE + (distance * PER_KM);
    
    double multiplier = CAR_MULTIPLIER;
    
    if (vehicleType == "Bike") {
        multiplier = BIKE_MULTIPLIER;
    } else if (vehicleType == "Car") {
        multiplier = CAR_MULTIPLIER;
    } else if (vehicleType == "AC Car") {
        multiplier = AC_CAR_MULTIPLIER;
    } else if (vehicleType == "SUV") {
        multiplier = SUV_MULTIPLIER;
    }
    
    return baseFare * multiplier;
}

// Function to display vehicle options
void displayVehicleOptions() {
    cout << "\n=================================================================\n";
    cout << "                    VEHICLE OPTIONS                              \n";
    cout << "=================================================================\n";
    cout << "  1. Bike        - Affordable & Fast       (40% cheaper)        \n";
    cout << "  2. Car         - Standard Comfort        (Base price)         \n";
    cout << "  3. AC Car      - Cool & Comfortable      (+30%)               \n";
    cout << "  4. SUV         - Spacious & Premium      (+60%)               \n";
    cout << "=================================================================\n";
}

// Function to get vehicle type from choice
string getVehicleType(int choice) {
    switch(choice) {
        case 1: return "Bike";
        case 2: return "Car";
        case 3: return "AC Car";
        case 4: return "SUV";
        default: return "Car";
    }
}

// ==================== FILE HANDLING WITH IMPROVED FORMATTING ====================

void saveDrivers() {
    ofstream file(DRIVERS_FILE);
    if (!file) {
        handleParsingError("Cannot create " + DRIVERS_FILE);
        return;
    }
    
    file << "================================================================================================\n";
    file << "                                    DRIVERS DATABASE                                            \n";
    file << "================================================================================================\n\n";
    
    file << left << setw(8) << "ID" 
         << setw(25) << "NAME" 
         << setw(18) << "CONTACT"
         << setw(10) << "RATING"
         << setw(12) << "STATUS"
         << setw(12) << "RIDES"
         << setw(15) << "EARNINGS" << "\n";
    file << string(100, '_') << "\n";
    
    for (auto& pair : drivers) {
        Driver& d = pair.second;
        file << left << setw(8) << d.id
             << setw(25) << d.name
             << setw(18) << d.contact
             << setw(10) << fixed << setprecision(2) << d.rating
             << setw(12) << (d.available ? "Available" : "Busy")
             << setw(12) << d.totalRides
             << "Rs. " << fixed << setprecision(2) << d.totalEarnings << "\n";
    }
    
    file << string(100, '_') << "\n";
    file << "Total Drivers: " << drivers.size() << "\n";
    file.close();
}

void saveRiders() {
    ofstream file(RIDERS_FILE);
    if (!file) {
        handleParsingError("Cannot create " + RIDERS_FILE);
        return;
    }
    
    file << "================================================================================\n";
    file << "                            RIDERS DATABASE                                     \n";
    file << "================================================================================\n\n";
    
    file << left << setw(8) << "ID" 
         << setw(25) << "NAME" 
         << setw(18) << "CONTACT"
         << setw(15) << "BOOKINGS" << "\n";
    file << string(66, '_') << "\n";
    
    for (auto& pair : riders) {
        Rider& r = pair.second;
        file << left << setw(8) << r.id
             << setw(25) << r.name
             << setw(18) << r.contact
             << r.totalBookings << "\n";
    }
    
    file << string(66, '_') << "\n";
    file << "Total Riders: " << riders.size() << "\n";
    file.close();
}

void saveRides() {
    ofstream file(RIDES_FILE);
    if (!file) {
        handleParsingError("Cannot create " + RIDES_FILE);
        return;
    }
    
    file << "=======================================================================================================\n";
    file << "                                        RIDES DATABASE                                                 \n";
    file << "=======================================================================================================\n\n";
    
    // Save requested rides
    queue<Ride> tempQueue = rideRequests;
    if (!tempQueue.empty()) {
        file << "+-------------------------------------------------------------------------------------------------------+\n";
        file << "|                                    REQUESTED RIDES (QUEUE)                                            |\n";
        file << "+-------------------------------------------------------------------------------------------------------+\n\n";
        
        while (!tempQueue.empty()) {
            Ride r = tempQueue.front();
            tempQueue.pop();
            file << "Ride ID: " << r.rideId << "\n";
            file << "Driver ID: " << r.driverId << "\n";
            file << "Rider ID: " << r.riderId << "\n";
            file << "Pickup: " << r.pickup << "\n";
            file << "Destination: " << r.destination << "\n";
            file << "Distance: " << r.distance << " km\n";
            file << "Fare: Rs. " << fixed << setprecision(2) << r.fare << "\n";
            file << "Status: " << r.status << "\n";
            file << "Timestamp: " << r.timestamp << "\n";
            file << string(80, '_') << "\n";
        }
    }
    
    // Save assigned rides
    if (!assignedRides.empty()) {
        file << "\n+-------------------------------------------------------------------------------------------------------+\n";
        file << "|                                      ASSIGNED RIDES                                                   |\n";
        file << "+-------------------------------------------------------------------------------------------------------+\n\n";
        
        for (const auto& r : assignedRides) {
            file << "Ride ID: " << r.rideId << "\n";
            file << "Driver ID: " << r.driverId << "\n";
            file << "Rider ID: " << r.riderId << "\n";
            file << "Pickup: " << r.pickup << "\n";
            file << "Destination: " << r.destination << "\n";
            file << "Distance: " << r.distance << " km\n";
            file << "Fare: Rs. " << fixed << setprecision(2) << r.fare << "\n";
            file << "Status: " << r.status << "\n";
            file << "Timestamp: " << r.timestamp << "\n";
            file << string(80, '_') << "\n";
        }
    }
    
    // Save completed rides
    if (!completedRides.empty()) {
        file << "\n+-------------------------------------------------------------------------------------------------------+\n";
        file << "|                                     COMPLETED RIDES                                                   |\n";
        file << "+-------------------------------------------------------------------------------------------------------+\n\n";
        
        for (const auto& r : completedRides) {
            file << "Ride ID: " << r.rideId << "\n";
            file << "Driver ID: " << r.driverId << "\n";
            file << "Rider ID: " << r.riderId << "\n";
            file << "Pickup: " << r.pickup << "\n";
            file << "Destination: " << r.destination << "\n";
            file << "Distance: " << r.distance << " km\n";
            file << "Fare: Rs. " << fixed << setprecision(2) << r.fare << "\n";
            file << "Status: " << r.status << "\n";
            file << "Timestamp: " << r.timestamp << "\n";
            file << string(80, '_') << "\n";
        }
    }
    
    file.close();
    
    // Save cancelled rides
    ofstream cancelFile(CANCELLED_FILE);
    if (cancelFile) {
        cancelFile << "=================================================================================================\n";
        cancelFile << "                              CANCELLED RIDES                                     \n";
        cancelFile << "=================================================================================================\n\n";
        
        stack<Ride> tempStack = cancelledRides;
        while (!tempStack.empty()) {
            Ride r = tempStack.top();
            tempStack.pop();
            cancelFile << "Ride ID: " << r.rideId << "\n";
            cancelFile << "Driver ID: " << r.driverId << "\n";
            cancelFile << "Rider ID: " << r.riderId << "\n";
            cancelFile << "Pickup: " << r.pickup << "\n";
            cancelFile << "Destination: " << r.destination << "\n";
            cancelFile << "Distance: " << r.distance << " km\n";
            cancelFile << "Fare: Rs. " << fixed << setprecision(2) << r.fare << "\n";
            cancelFile << "Status: " << r.status << "\n";
            cancelFile << "Timestamp: " << r.timestamp << "\n";
            cancelFile << string(80, '_') << "\n";
        }
        cancelFile.close();
    }
}

void loadDrivers() {
    ifstream file(DRIVERS_FILE);
    if (!file) {
        cout << "Warning: " << DRIVERS_FILE << " not found. Starting with empty data.\n";
        return;
    }
    
    try {
        drivers.clear();
        string line;
        int lineCount = 0;
        
        // Read and skip until we find the separator line (dashes)
        while (getline(file, line)) {
            lineCount++;
            if (line.find("----") != string::npos || line.find("====") != string::npos) {
                break;  // Found separator, data starts next
            }
            if (lineCount > 20) {  // Safety check
                throw runtime_error("Invalid file format: No data separator found");
            }
        }
        
        // Now read actual driver data
        while (getline(file, line)) {
            // Skip empty lines, separator lines, or total line
            if (line.empty() || 
                line.find("----") != string::npos || 
                line.find("====") != string::npos ||
                line.find("Total") != string::npos) {
                continue;
            }
            
            // Try to parse the line
            istringstream ss(line);
            Driver d;
            
            // Read ID first
            if (!(ss >> d.id)) {
                continue;  // Skip if can't read ID
            }
            
            // Verify ID is reasonable
            if (d.id < 100 || d.id > 999999) {
                continue;  // Skip invalid IDs
            }
            
            // Read name - collect words until we hit a phone number pattern
            string word;
            d.name = "";
            bool foundContact = false;
            
            while (ss >> word) {
                // Check if this is a phone number (starts with 0 or contains multiple digits with dash)
                if ((word[0] == '0' && word.length() > 4) || 
                    (word.find('-') != string::npos && word.length() > 5)) {
                    d.contact = word;
                    foundContact = true;
                    break;
                }
                // Otherwise it's part of the name
                if (!d.name.empty()) d.name += " ";
                d.name += word;
            }
            
            if (!foundContact || d.name.empty()) {
                continue;  // Skip if no valid contact or name
            }
            
            // Read rating
            if (!(ss >> d.rating)) {
                continue;
            }
            
            // Read status
            string status;
            if (!(ss >> status)) {
                continue;
            }
            d.available = (status == "Available");
            
            // Read total rides
            if (!(ss >> d.totalRides)) {
                continue;
            }
            
            // Read "Rs." and earnings
            string rsSymbol;
            if (!(ss >> rsSymbol >> d.totalEarnings)) {
                d.totalEarnings = 0.0;  // Default if can't read
            }
            
            // Only add if we got valid data
            if (d.id > 0 && !d.name.empty() && !d.contact.empty()) {
                drivers[d.id] = d;
            }
        }
        
        file.close();
        
        if (drivers.empty()) {
            cout << "⚠ Warning: No valid drivers loaded from file.\n";
        } else {
            cout << "✓ Loaded " << drivers.size() << " drivers successfully.\n";
        }
        
    } catch (const exception& e) {
        handleParsingError(string("Error loading drivers: ") + e.what());
        drivers.clear();  // Clear any partial data
        file.close();
    }
}

void loadRiders() {
    ifstream file(RIDERS_FILE);
    if (!file) {
        cout << "Warning: " << RIDERS_FILE << " not found. Starting with empty data.\n";
        return;
    }
    
    try {
        riders.clear();
        string line;
        int lineCount = 0;
        
        // Read and skip until we find the separator line
        while (getline(file, line)) {
            lineCount++;
            if (line.find("----") != string::npos || line.find("====") != string::npos) {
                break;
            }
            if (lineCount > 20) {
                throw runtime_error("Invalid file format: No data separator found");
            }
        }
        
        // Now read actual rider data
        while (getline(file, line)) {
            // Skip empty lines, separators, or total line
            if (line.empty() || 
                line.find("----") != string::npos || 
                line.find("====") != string::npos ||
                line.find("Total") != string::npos) {
                continue;
            }
            
            istringstream ss(line);
            Rider r;
            
            // Read ID
            if (!(ss >> r.id)) {
                continue;
            }
            
            // Verify ID is reasonable
            if (r.id < 100 || r.id > 999999) {
                continue;
            }
            
            // Read name - collect words until we hit a phone number
            string word;
            r.name = "";
            bool foundContact = false;
            
            while (ss >> word) {
                // Check if this is a phone number
                if ((word[0] == '0' && word.length() > 4) || 
                    (word.find('-') != string::npos && word.length() > 5)) {
                    r.contact = word;
                    foundContact = true;
                    break;
                }
                // Otherwise it's part of the name
                if (!r.name.empty()) r.name += " ";
                r.name += word;
            }
            
            if (!foundContact || r.name.empty()) {
                continue;
            }
            
            // Read total bookings
            if (!(ss >> r.totalBookings)) {
                r.totalBookings = 0;
            }
            
            // Only add if we got valid data
            if (r.id > 0 && !r.name.empty() && !r.contact.empty()) {
                riders[r.id] = r;
            }
        }
        
        file.close();
        
        if (riders.empty()) {
            cout << "⚠ Warning: No valid riders loaded from file.\n";
        } else {
            cout << "✓ Loaded " << riders.size() << " riders successfully.\n";
        }
        
    } catch (const exception& e) {
        handleParsingError(string("Error loading riders: ") + e.what());
        riders.clear();
        file.close();
    }
}
void loadRides() {
    ifstream file(RIDES_FILE);
    if (!file) {
        cout << "Warning: " << RIDES_FILE << " not found. Starting with empty data.\n";
        return;
    }
    
    try {
        while (!rideRequests.empty()) rideRequests.pop();
        completedRides.clear();
        while (!cancelledRides.empty()) cancelledRides.pop();
        assignedRides.clear();
        
        Ride ride;
        string line;
        
        while (getline(file, line)) {
            if (line.find("Ride ID:") != string::npos) {
                sscanf(line.c_str(), "Ride ID: %d", &ride.rideId);
                
                getline(file, line);
                sscanf(line.c_str(), "Driver ID: %d", &ride.driverId);
                
                getline(file, line);
                sscanf(line.c_str(), "Rider ID: %d", &ride.riderId);
                
                getline(file, line);
                ride.pickup = line.substr(line.find(":") + 2);
                
                getline(file, line);
                ride.destination = line.substr(line.find(":") + 2);
                
                getline(file, line);
                sscanf(line.c_str(), "Distance: %lf", &ride.distance);
                
                getline(file, line);
                sscanf(line.c_str(), "Fare: Rs. %lf", &ride.fare);
                
                getline(file, line);
                ride.status = line.substr(line.find(":") + 2);
                
                getline(file, line);
                ride.timestamp = line.substr(line.find(":") + 2);
                
                if (ride.status == "Completed") {
                    completedRides.push_back(ride);
                } else if (ride.status == "Cancelled") {
                    cancelledRides.push(ride);
                } else if (ride.status == "Assigned") {
                    assignedRides.push_back(ride);
                } else if (ride.status == "Requested") {
                    rideRequests.push(ride);
                }
                
                getline(file, line);
            }
        }
        
        file.close();
        cout << "✓ Loaded " << completedRides.size() << " completed rides.\n";
        
    } catch (const exception& e) {
        handleParsingError(string("Error loading rides: ") + e.what());
        file.close();
    }
}

void loadAllData() {
    cout << "\n==============================\n";
    cout << "    Loading System Data       \n";
    cout << "==============================\n\n";
    
    loadDrivers();
    loadRiders();
    loadRides();
    
    cout << "\n✓ Data loaded successfully!\n";
}

void saveAllData() {
    cout << "\n==============================\n";
    cout << "    Saving System Data        \n";
    cout << "==============================\n\n";
    
    saveDrivers();
    saveRiders();
    saveRides();
    
    cout << "✓ All data saved successfully!\n";
}

// ==================== NEW FUNCTIONS ====================

// NEW: View available drivers for rider selection
void viewAvailableDrivers() {
    clearScreen();
    cout << "================================================================================\n";
    cout << "                          AVAILABLE DRIVERS                                     \n";
    cout << "================================================================================\n\n";
    
    vector<Driver> availableDrivers;
    for (auto& pair : drivers) {
        if (pair.second.available) {
            availableDrivers.push_back(pair.second);
        }
    }
    
    if (availableDrivers.empty()) {
        cout << "❌ No drivers currently available!\n";
        pressAnyKey();
        return;
    }
    
    cout << left << setw(8) << "ID" 
         << setw(22) << "NAME" 
         << setw(16) << "CONTACT"
         << setw(10) << "RATING"
         << setw(10) << "RIDES" << endl;
    cout << string(66, '_') << endl;
    
    for (auto& d : availableDrivers) {
        cout << left << setw(8) << d.id
             << setw(22) << d.name
             << setw(16) << d.contact
             << setw(10) << fixed << setprecision(1) << d.rating
             << d.totalRides << endl;
    }
    
    cout << "\n✓ Total Available Drivers: " << availableDrivers.size() << "\n";
}

// NEW: Request ride with driver selection option
void requestRideWithDriverSelection() {
    clearScreen();
    cout << "================================================================================\n";
    cout << "                    BOOK A RIDE (SELECT DRIVER)                                 \n";
    cout << "================================================================================\n\n";
    
    // Check available drivers first
    vector<Driver> availableDrivers;
    for (auto& pair : drivers) {
        if (pair.second.available) {
            availableDrivers.push_back(pair.second);
        }
    }
    
    if (availableDrivers.empty()) {
        cout << "❌ Sorry! No drivers are currently available.\n";
        cout << "   Your ride will be queued and assigned when a driver becomes available.\n\n";
        
        char choice;
        cout << "Do you want to queue your ride request? (y/n): ";
        cin >> choice;
        cin.ignore();
        
        if (choice == 'y' || choice == 'Y') {
            requestRide();
        }
        return;
    }
    
    // Display available drivers
    cout << "Available Drivers:\n\n";
    cout << left << setw(4) << "#" << setw(8) << "ID" 
         << setw(22) << "NAME" 
         << setw(10) << "RATING"
         << setw(10) << "RIDES" << endl;
    cout << string(54, '_') << endl;
    
    for (size_t i = 0; i < availableDrivers.size(); i++) {
        Driver& d = availableDrivers[i];
        cout << left << setw(4) << i+1
             << setw(8) << d.id
             << setw(22) << d.name
             << setw(10) << fixed << setprecision(1) << d.rating
             << d.totalRides << endl;
    }
    
    cout << "\n" << string(54, '_') << "\n\n";
    
    // Get ride details
    Ride ride;
    ride.rideId = generateId();
    
    ride.riderId = getValidIntegerInput("Enter your Rider ID: ", 100, 999999);
    
    if (riders.find(ride.riderId) == riders.end()) {
        cout << "\n❌ Error: Rider ID not found!\n";
        pressAnyKey();
        return;
    }
    
    cout << "Enter Pickup Location: ";
    getline(cin, ride.pickup);
    cout << "Enter Destination: ";
    getline(cin, ride.destination);
    
    ride.distance = getValidDoubleInput("Enter Distance (km): ", 0.1, 1000.0);
    
    ride.fare = calculateFare(ride.distance);
    
    cout << "\n============================================================\n";
    cout << "                   RIDE DETAILS                             \n";
    cout << "============================================================\n";
    cout << "From: " << ride.pickup << "\n";
    cout << "To: " << ride.destination << "\n";
    cout << "Distance: " << ride.distance << " km\n";
    cout << "Estimated Fare: Rs. " << fixed << setprecision(2) << ride.fare << "\n\n";
    
    // Select driver
    int driverChoice = getValidIntegerInput("Select driver (enter number): ", 1, availableDrivers.size());
    
    // Assign selected driver
    Driver& selectedDriver = availableDrivers[driverChoice - 1];
    ride.driverId = selectedDriver.id;
    ride.status = "Assigned";
    ride.timestamp = getCurrentTime();
    
    // Update driver status
    drivers[selectedDriver.id].available = false;
    
    // Add to assigned rides
    assignedRides.push_back(ride);
    
    // Update rider stats
    riders[ride.riderId].totalBookings++;
    
    cout << "\n============================================================\n";
    cout << "              ✓ RIDE BOOKED SUCCESSFULLY!                   \n";
    cout << "============================================================\n\n";
    cout << "Ride ID: " << ride.rideId << "\n";
    cout << "Driver: " << selectedDriver.name << " (ID: " << selectedDriver.id << ")\n";
    cout << "Rating: " << fixed << setprecision(1) << selectedDriver.rating << " ⭐\n";
    cout << "Fare: Rs. " << fixed << setprecision(2) << ride.fare << "\n";
    cout << "\nYour driver is on the way!\n";
    
    saveAllData();
    pressAnyKey();
}
// ==================== SORTING ALGORITHMS ====================

void bubbleSortDriversByName(vector<Driver>& driverList) {
    int n = driverList.size();
    for (int i = 0; i < n - 1; i++) {
        for (int j = 0; j < n - i - 1; j++) {
            if (driverList[j].name > driverList[j + 1].name) {
                swap(driverList[j], driverList[j + 1]);
            }
        }
    }
}

void merge(vector<Driver>& arr, int left, int mid, int right) {
    int n1 = mid - left + 1;
    int n2 = right - mid;
    
    vector<Driver> L(n1), R(n2);
    
    for (int i = 0; i < n1; i++)
        L[i] = arr[left + i];
    for (int j = 0; j < n2; j++)
        R[j] = arr[mid + 1 + j];
    
    int i = 0, j = 0, k = left;
    
    while (i < n1 && j < n2) {
        if (L[i].rating >= R[j].rating) {
            arr[k] = L[i];
            i++;
        } else {
            arr[k] = R[j];
            j++;
        }
        k++;
    }
    
    while (i < n1) {
        arr[k] = L[i];
        i++;
        k++;
    }
    
    while (j < n2) {
        arr[k] = R[j];
        j++;
        k++;
    }
}

void mergeSortDriversByRating(vector<Driver>& driverList, int left, int right) {
    if (left < right) {
        int mid = left + (right - left) / 2;
        mergeSortDriversByRating(driverList, left, mid);
        mergeSortDriversByRating(driverList, mid + 1, right);
        merge(driverList, left, mid, right);
    }
}

int partition(vector<Rider>& arr, int left, int right) {
    string pivot = arr[right].name;
    int i = left - 1;
    
    for (int j = left; j < right; j++) {
        if (arr[j].name <= pivot) {
            i++;
            swap(arr[i], arr[j]);
        }
    }
    swap(arr[i + 1], arr[right]);
    return i + 1;
}

void quickSortRidersByName(vector<Rider>& riderList, int left, int right) {
    if (left < right) {
        int pi = partition(riderList, left, right);
        quickSortRidersByName(riderList, left, pi - 1);
        quickSortRidersByName(riderList, pi + 1, right);
    }
}

// ==================== SEARCH ALGORITHMS ====================

int linearSearchDriver(int id) {
    for (auto& pair : drivers) {
        if (pair.first == id) {
            return id;
        }
    }
    return -1;
}

int binarySearchRider(vector<int>& sortedIds, int id) {
    int left = 0;
    int right = sortedIds.size() - 1;
    
    while (left <= right) {
        int mid = left + (right - left) / 2;
        
        if (sortedIds[mid] == id)
            return mid;
        
        if (sortedIds[mid] < id)
            left = mid + 1;
        else
            right = mid - 1;
    }
    return -1;
}

// ==================== ALGORITHM COMPARISON ====================

void compareSortAlgorithms() {
    clearScreen();
    cout << "=== SORTING ALGORITHM COMPARISON ===\n\n";
    
    if (drivers.empty()) {
        cout << "No drivers available for comparison!\n";
        pressAnyKey();
        return;
    }
    
    vector<Driver> driverList1, driverList2;
    for (auto& pair : drivers) {
        driverList1.push_back(pair.second);
        driverList2.push_back(pair.second);
    }
    
    cout << "Data Size: " << driverList1.size() << " drivers\n\n";
    
    cout << "Testing Bubble Sort (by name)...\n";
    auto start = high_resolution_clock::now();
    bubbleSortDriversByName(driverList1);
    auto end = high_resolution_clock::now();
    auto duration1 = duration_cast<microseconds>(end - start);
    cout << "Time taken: " << duration1.count() << " microseconds\n";
    cout << "Big-O Complexity: O(n²)\n\n";
    
    cout << "Testing Merge Sort (by rating)...\n";
    start = high_resolution_clock::now();
    mergeSortDriversByRating(driverList2, 0, driverList2.size() - 1);
    end = high_resolution_clock::now();
    auto duration2 = duration_cast<microseconds>(end - start);
    cout << "Time taken: " << duration2.count() << " microseconds\n";
    cout << "Big-O Complexity: O(n log n)\n\n";
    
    cout << "=== RESULTS ===\n";
    cout << "Bubble Sort: " << duration1.count() << " μs (O(n²))\n";
    cout << "Merge Sort:  " << duration2.count() << " μs (O(n log n))\n";
    
    if (duration2.count() < duration1.count()) {
        cout << "\nMerge Sort is faster by " << (duration1.count() - duration2.count()) << " μs\n";
    } else {
        cout << "\nBubble Sort is faster for small data sets\n";
    }
    
    pressAnyKey();
}

void compareSearchAlgorithms() {
    clearScreen();
    cout << "=== SEARCH ALGORITHM COMPARISON ===\n\n";
    
    if (riders.empty()) {
        cout << "No riders available for comparison!\n";
        pressAnyKey();
        return;
    }
    
    int searchId = getValidIntegerInput("Enter Rider ID to search: ", 100, 999999);
    
    cout << "\nTesting Linear Search...\n";
    auto start = high_resolution_clock::now();
    int result1 = linearSearchDriver(searchId);
    auto end = high_resolution_clock::now();
    auto duration1 = duration_cast<nanoseconds>(end - start);
    cout << "Time taken: " << duration1.count() << " nanoseconds\n";
    cout << "Result: " << (result1 != -1 ? "Found" : "Not Found") << "\n";
    cout << "Big-O Complexity: O(n)\n\n";
    
    vector<int> sortedIds;
    for (auto& pair : riders) {
        sortedIds.push_back(pair.first);
    }
    sort(sortedIds.begin(), sortedIds.end());
    
    cout << "Testing Binary Search (on sorted array)...\n";
    start = high_resolution_clock::now();
    int result2 = binarySearchRider(sortedIds, searchId);
    end = high_resolution_clock::now();
    auto duration2 = duration_cast<nanoseconds>(end - start);
    cout << "Time taken: " << duration2.count() << " nanoseconds\n";
    cout << "Result: " << (result2 != -1 ? "Found" : "Not Found") << "\n";
    cout << "Big-O Complexity: O(log n)\n\n";
    
    cout << "=== RESULTS ===\n";
    cout << "Linear Search: " << duration1.count() << " ns (O(n))\n";
    cout << "Binary Search: " << duration2.count() << " ns (O(log n))\n";
    
    if (duration2.count() < duration1.count()) {
        cout << "\nBinary Search is faster by " << (duration1.count() - duration2.count()) << " ns\n";
    }
    
    pressAnyKey();
}
// ==================== DRIVER MANAGEMENT ====================

void addDriver() {
    clearScreen();
    cout << "=== ADD NEW DRIVER ===\n\n";
    
    Driver d;
    d.id = generateId();
    
    cout << "Driver ID (auto-generated): " << d.id << "\n";
    cout << "Enter Name: ";
    cin.ignore();
    getline(cin, d.name);
    cout << "Enter Contact: ";
    getline(cin, d.contact);
    
    // Select vehicle type
    cout << "\nSelect Vehicle Type:\n";
    cout << "1. Bike\n";
    cout << "2. Car\n";
    cout << "3. AC Car\n";
    cout << "4. SUV\n";
    int vehicleChoice = getValidIntegerInput("Enter choice: ", 1, 4);
    d.vehicleType = getVehicleType(vehicleChoice);
    
    d.rating = 5.0;
    d.available = true;
    d.totalRides = 0;
    d.totalEarnings = 0.0;
    
    drivers[d.id] = d;
    
    cout << "\n[OK] Driver added successfully with ID: " << d.id << "\n";
    cout << "Vehicle Type: " << d.vehicleType << "\n";
    saveDrivers();
    pressAnyKey();
}

void viewDrivers() {
    clearScreen();
    cout << "=== ALL DRIVERS ===\n\n";
    
    if (drivers.empty()) {
        cout << "No drivers in the system!\n";
        pressAnyKey();
        return;
    }
    
    cout << left << setw(8) << "ID" 
         << setw(20) << "NAME" 
         << setw(15) << "CONTACT"
         << setw(10) << "VEHICLE"
         << setw(10) << "RATING"
         << setw(12) << "STATUS"
         << setw(10) << "RIDES"
         << setw(12) << "EARNINGS" << endl;
    cout << string(97, '-') << endl;
    
    for (auto& pair : drivers) {
        Driver& d = pair.second;
        cout << left << setw(8) << d.id
             << setw(20) << d.name
             << setw(15) << d.contact
             << setw(10) << d.vehicleType
             << setw(10) << fixed << setprecision(1) << d.rating
             << setw(12) << (d.available ? "Available" : "Busy")
             << setw(10) << d.totalRides
             << "Rs. " << fixed << setprecision(2) << d.totalEarnings << endl;
    }
    
    pressAnyKey();
}

void searchDriver() {
    clearScreen();
    cout << "=== SEARCH DRIVER ===\n\n";
    cout << "1. Search by ID\n";
    cout << "2. Search by Name\n";
    
    int choice = getValidIntegerInput("Enter choice: ", 1, 2);
    
    if (choice == 1) {
        int id = getValidIntegerInput("Enter Driver ID: ", 100, 999999);
        
        auto it = drivers.find(id);
        if (it != drivers.end()) {
            Driver& d = it->second;
            cout << "\n=== DRIVER FOUND ===\n";
            cout << "ID: " << d.id << "\n";
            cout << "Name: " << d.name << "\n";
            cout << "Contact: " << d.contact << "\n";
            cout << "Rating: " << fixed << setprecision(1) << d.rating << "\n";
            cout << "Status: " << (d.available ? "Available" : "Busy") << "\n";
            cout << "Total Rides: " << d.totalRides << "\n";
            cout << "Total Earnings: Rs. " << fixed << setprecision(2) << d.totalEarnings << "\n";
        } else {
            cout << "\nDriver not found!\n";
        }
    } else if (choice == 2) {
        cout << "Enter Driver Name: ";
        string name;
        getline(cin, name);
        
        bool found = false;
        for (auto& pair : drivers) {
            Driver& d = pair.second;
            if (d.name.find(name) != string::npos) {
                if (!found) {
                    cout << "\n=== MATCHING DRIVERS ===\n";
                    found = true;
                }
                cout << "\nID: " << d.id << "\n";
                cout << "Name: " << d.name << "\n";
                cout << "Contact: " << d.contact << "\n";
                cout << "Rating: " << fixed << setprecision(1) << d.rating << "\n";
            }
        }
        
        if (!found) {
            cout << "\nNo drivers found matching '" << name << "'\n";
        }
    }
    
    pressAnyKey();
}

void sortDrivers() {
    clearScreen();
    cout << "=== SORT DRIVERS ===\n\n";
    cout << "1. Sort by Name (Bubble Sort)\n";
    cout << "2. Sort by Rating (Merge Sort)\n";
    
    int choice = getValidIntegerInput("Enter choice: ", 1, 2);
    
    vector<Driver> driverList;
    for (auto& pair : drivers) {
        driverList.push_back(pair.second);
    }
    
    if (choice == 1) {
        cout << "\nSorting by name using Bubble Sort...\n";
        bubbleSortDriversByName(driverList);
    } else if (choice == 2) {
        cout << "\nSorting by rating using Merge Sort...\n";
        mergeSortDriversByRating(driverList, 0, driverList.size() - 1);
    }
    
    cout << "\n=== SORTED DRIVERS ===\n\n";
    cout << left << setw(8) << "ID" 
         << setw(20) << "NAME" 
         << setw(10) << "RATING" << endl;
    cout << string(38, '-') << endl;
    
    for (auto& d : driverList) {
        cout << left << setw(8) << d.id
             << setw(20) << d.name
             << fixed << setprecision(1) << d.rating << endl;
    }
    
    pressAnyKey();
}
void updateDriver() {
    clearScreen();
    cout << "=== UPDATE DRIVER ===\n\n";
    
    int id = getValidIntegerInput("Enter Driver ID: ", 100, 999999);
    
    auto it = drivers.find(id);
    if (it == drivers.end()) {
        cout << "Driver not found!\n";
        pressAnyKey();
        return;
    }
    
    Driver& d = it->second;
    cout << "\nCurrent Details:\n";
    cout << "Name: " << d.name << "\n";
    cout << "Contact: " << d.contact << "\n";
    cout << "Rating: " << d.rating << "\n";
    cout << "Available: " << (d.available ? "Yes" : "No") << "\n";
    
    cout << "\nEnter new name (or press Enter to skip): ";
    string name;
    getline(cin, name);
    if (!name.empty()) d.name = name;
    
    cout << "Enter new contact (or press Enter to skip): ";
    string contact;
    getline(cin, contact);
    if (!contact.empty()) d.contact = contact;
    
    cout << "Update availability? (y/n): ";
    char ch;
    cin >> ch;
    if (ch == 'y' || ch == 'Y') {
        d.available = !d.available;
    }
    
    cout << "\n✓ Driver updated successfully!\n";
    saveDrivers();
    pressAnyKey();
}
// ==================== RIDER MANAGEMENT ====================

void addRider() {
    clearScreen();
    cout << "=== ADD NEW RIDER ===\n\n";
    
    Rider r;
    r.id = generateId();
    
    cout << "Rider ID (auto-generated): " << r.id << "\n";
    cout << "Enter Name: ";
    cin.ignore();
    getline(cin, r.name);
    cout << "Enter Contact: ";
    getline(cin, r.contact);
    
    r.totalBookings = 0;
    
    riders[r.id] = r;
    
    cout << "\n✓ Rider added successfully with ID: " << r.id << "\n";
    saveRiders();
    pressAnyKey();
}

void viewRiders() {
    clearScreen();
    cout << "=== ALL RIDERS ===\n\n";
    
    if (riders.empty()) {
        cout << "No riders in the system!\n";
        pressAnyKey();
        return;
    }
    
    cout << left << setw(8) << "ID" 
         << setw(20) << "NAME" 
         << setw(15) << "CONTACT"
         << setw(15) << "BOOKINGS" << endl;
    cout << string(58, '-') << endl;
    
    for (auto& pair : riders) {
        Rider& r = pair.second;
        cout << left << setw(8) << r.id
             << setw(20) << r.name
             << setw(15) << r.contact
             << r.totalBookings << endl;
    }
    
    pressAnyKey();
}

void searchRider() {
    clearScreen();
    cout << "=== SEARCH RIDER ===\n\n";
    
    int id = getValidIntegerInput("Enter Rider ID: ", 100, 999999);
    
    auto it = riders.find(id);
    if (it != riders.end()) {
        Rider& r = it->second;
        cout << "\n=== RIDER FOUND ===\n";
        cout << "ID: " << r.id << "\n";
        cout << "Name: " << r.name << "\n";
        cout << "Contact: " << r.contact << "\n";
        cout << "Total Bookings: " << r.totalBookings << "\n";
    } else {
        cout << "\nRider not found!\n";
    }
    
    pressAnyKey();
}
void sortRiders() {
    clearScreen();
    cout << "=== SORT RIDERS BY NAME (Quick Sort) ===\n\n";
    
    vector<Rider> riderList;
    for (auto& pair : riders) {
        riderList.push_back(pair.second);
    }
    
    if (riderList.empty()) {
        cout << "No riders to sort!\n";
        pressAnyKey();
        return;
    }
    
    quickSortRidersByName(riderList, 0, riderList.size() - 1);
    
    cout << left << setw(8) << "ID" 
         << setw(20) << "NAME" 
         << setw(15) << "BOOKINGS" << endl;
    cout << string(43, '-') << endl;
    
    for (auto& r : riderList) {
        cout << left << setw(8) << r.id
             << setw(20) << r.name
             << r.totalBookings << endl;
    }
    
    pressAnyKey();
}

void updateRider() {
    clearScreen();
    cout << "=== UPDATE RIDER ===\n\n";
    
    int id = getValidIntegerInput("Enter Rider ID: ", 100, 999999);
    
    auto it = riders.find(id);
    if (it == riders.end()) {
        cout << "Rider not found!\n";
        pressAnyKey();
        return;
    }
    
    Rider& r = it->second;
    cout << "\nCurrent Details:\n";
    cout << "Name: " << r.name << "\n";
    cout << "Contact: " << r.contact << "\n";
    
    cout << "\nEnter new name (or press Enter to skip): ";
    string name;
    getline(cin, name);
    if (!name.empty()) r.name = name;
    
    cout << "Enter new contact (or press Enter to skip): ";
    string contact;
    getline(cin, contact);
    if (!contact.empty()) r.contact = contact;
    
    cout << "\n✓ Rider updated successfully!\n";
    saveRiders();
    pressAnyKey();
}

// ==================== RIDE MANAGEMENT ====================

void assignRides() {
    clearScreen();
    cout << "=== ASSIGN RIDES TO DRIVERS ===\n";
    cout << "Algorithm: First-Available Driver (Linear Search)\n\n";
    
    // ========== STEP 1: CHECK QUEUE ==========
    queue<Ride> tempCheck = rideRequests;
    int queueSize = 0;
    
    cout << "Checking ride requests queue...\n";
    cout << string(60, '-') << "\n";
    
    while (!tempCheck.empty()) {
        Ride r = tempCheck.front();
        tempCheck.pop();
        queueSize++;
        cout << queueSize << ". Ride ID: " << r.rideId 
             << " | Rider: " << r.riderId
             << " | Vehicle: " << r.vehicleType
             << " | From: " << r.pickup << "\n";
    }
    cout << string(60, '-') << "\n";
    cout << "Total rides in queue: " << queueSize << "\n\n";
    
    if (rideRequests.empty()) {
        cout << "[X] No pending ride requests!\n";
        pressAnyKey();
        return;
    }
    
    // ========== STEP 2: CHECK AVAILABLE DRIVERS ==========
    cout << "Checking available drivers...\n";
    cout << string(60, '-') << "\n";
    
    int availableCount = 0;
    for (auto& pair : drivers) {
        Driver& d = pair.second;
        if (d.available) {
            availableCount++;
            cout << "  [OK] " << d.name 
                 << " (ID: " << d.id << ") - " 
                 << d.vehicleType << "\n";
        }
    }
    cout << string(60, '-') << "\n";
    cout << "Total available: " << availableCount << "\n\n";
    
    if (availableCount == 0) {
        cout << "[X] No available drivers!\n";
        pressAnyKey();
        return;
    }
    
    // ========== STEP 3: ASSIGN RIDES USING FIRST-AVAILABLE ==========
    cout << "Starting ride assignment...\n";
    cout << "Using: First-Available Driver Logic\n\n";
    
    int assignedCount = 0;
    
    while (!rideRequests.empty()) {
        // Get ride from front of queue
        Ride ride = rideRequests.front();
        rideRequests.pop();
        
        cout << "Processing Ride " << ride.rideId << "...\n";
        cout << "  Vehicle needed: " << ride.vehicleType << "\n";
        cout << "  Pickup: " << ride.pickup << "\n\n";
        
        bool assigned = false;
        int driversChecked = 0;
        
        // ========== LINEAR SEARCH: Find first available driver ==========
        cout << "  Searching for available driver:\n";
        
        for (auto& pair : drivers) {
            Driver& d = pair.second;
            driversChecked++;
            
            cout << "    " << driversChecked << ". Checking " << d.name 
                 << " (" << d.vehicleType << ")... ";
            
            // Check 1: Is driver available?
            if (!d.available) {
                cout << "[BUSY]\n";
                continue;
            }
            
            // Check 2: Does vehicle type match?
            if (d.vehicleType != ride.vehicleType) {
                cout << "[WRONG VEHICLE]\n";
                continue;
            }
            
            // FOUND! First available driver
            cout << "[AVAILABLE]\n\n";
            
            // ========== ASSIGN THE RIDE ==========
            cout << "  [ASSIGNING...]\n";
            ride.driverId = d.id;
            ride.status = "Assigned";
            ride.timestamp = getCurrentTime();
            
            // ========== MARK DRIVER AS UNAVAILABLE ==========
            d.available = false;
            
            // ========== MOVE TO ASSIGNED RIDES ==========
            assignedRides.push_back(ride);
            
            assigned = true;
            assignedCount++;
            
            cout << "  [OK] Ride " << ride.rideId 
                 << " assigned to Driver " << d.name 
                 << " (ID: " << d.id << ")\n";
            cout << "  Driver " << d.name << " is now BUSY\n";
            cout << "  Drivers checked: " << driversChecked << "\n";
            
            break;  // Stop searching (found first available)
        }
        
        if (!assigned) {
            // No available driver found
            cout << "  [!] No available " << ride.vehicleType 
                 << " driver found!\n";
            cout << "  Keeping Ride " << ride.rideId << " in queue.\n";
            rideRequests.push(ride);  // Put back in queue
            break;  // Stop processing more rides
        }
        
        cout << "\n" << string(60, '=') << "\n\n";
    }
    
    // ========== SUMMARY ==========
    cout << "\n" << string(60, '=') << "\n";
    cout << "ASSIGNMENT SUMMARY\n";
    cout << string(60, '=') << "\n";
    cout << "Algorithm used: First-Available Driver (Linear Search)\n";
    cout << "[OK] Rides assigned: " << assignedCount << "\n";
    
    // Count remaining in queue
    tempCheck = rideRequests;
    int remaining = 0;
    while (!tempCheck.empty()) {
        tempCheck.pop();
        remaining++;
    }
    cout << "[-] Still in queue: " << remaining << "\n";
    cout << "[*] Now in progress: " << assignedRides.size() << "\n";
    
    if (assignedCount > 0) {
        saveDrivers();
        saveRides();
        cout << "\n[SAVED] Data saved successfully!\n";
    }
    
    pressAnyKey();
}
void requestRide() {
    clearScreen();
    cout << "=== BOOK A RIDE ===\n\n";
    
    Ride ride;
    ride.rideId = generateId();
    
    ride.riderId = getValidIntegerInput("Enter Rider ID: ", 100, 999999);
    
    if (riders.find(ride.riderId) == riders.end()) {
        cout << "Error: Rider ID not found!\n";
        pressAnyKey();
        return;
    }
    
    cout << "Enter Pickup Location: ";
    getline(cin, ride.pickup);
    cout << "Enter Destination: ";
    getline(cin, ride.destination);
    
    ride.distance = getValidDoubleInput("Enter Distance (km): ", 0.1, 1000.0);
    
    // Display vehicle options
    displayVehicleOptions();
    int vehicleChoice = getValidIntegerInput("\nSelect vehicle type: ", 1, 4);
    ride.vehicleType = getVehicleType(vehicleChoice);
    
    // Calculate fare based on vehicle type
    ride.fare = calculateFareWithVehicle(ride.distance, ride.vehicleType);
    ride.status = "Requested";
    ride.timestamp = getCurrentTime();
    ride.driverId = 0;
    
    rideRequests.push(ride);
    riders[ride.riderId].totalBookings++;
    
    cout << "\n========================================\n";
    cout << "      RIDE REQUEST SUMMARY              \n";
    cout << "========================================\n";
    cout << "Ride ID: " << ride.rideId << "\n";
    cout << "From: " << ride.pickup << "\n";
    cout << "To: " << ride.destination << "\n";
    cout << "Distance: " << ride.distance << " km\n";
    cout << "Vehicle: " << ride.vehicleType << "\n";
    cout << "Estimated Fare: Rs. " << fixed << setprecision(2) << ride.fare << "\n";
    cout << "\n[OK] Ride request submitted successfully!\n";
    
    saveRides();
    saveRiders();
    pressAnyKey();
}

void completeRide() {
    clearScreen();
    cout << "=== COMPLETE RIDE ===\n\n";
    
    if (assignedRides.empty()) {
        cout << "No assigned rides to complete!\n";
        pressAnyKey();
        return;
    }
    
    cout << "Assigned Rides:\n\n";
    for (size_t i = 0; i < assignedRides.size(); i++) {
        cout << i + 1 << ". Ride ID: " << assignedRides[i].rideId
             << ", Driver ID: " << assignedRides[i].driverId
             << ", From: " << assignedRides[i].pickup
             << ", To: " << assignedRides[i].destination << "\n";
    }
    
    int num = getValidIntegerInput("\nEnter ride number to complete: ", 1, assignedRides.size());
    
    Ride& ride = assignedRides[num - 1];
    ride.status = "Completed";
    ride.timestamp = getCurrentTime();
    
    auto driverIt = drivers.find(ride.driverId);
    if (driverIt != drivers.end()) {
        Driver& d = driverIt->second;
        d.totalRides++;
        d.totalEarnings += ride.fare;
        d.available = true;
    }
    
    completedRides.push_back(ride);
    assignedRides.erase(assignedRides.begin() + num - 1);
    
    cout << "\n✓ Ride completed successfully!\n";
    cout << "Driver is now available for new rides.\n";
    
    saveDrivers();
    saveRides();
    pressAnyKey();
}
void cancelRide() {
    clearScreen();
    cout << "=== CANCEL RIDE ===\n\n";
    
    if (rideRequests.empty() && assignedRides.empty()) {
        cout << "No active rides to cancel!\n";
        pressAnyKey();
        return;
    }
    
    cout << "Active Rides:\n\n";
    
    queue<Ride> tempQueue = rideRequests;
    vector<Ride> requestedList;
    int index = 1;
    
    while (!tempQueue.empty()) {
        Ride r = tempQueue.front();
        tempQueue.pop();
        cout << index++ << ". Ride ID: " << r.rideId
             << " [Requested], From: " << r.pickup
             << ", To: " << r.destination << "\n";
        requestedList.push_back(r);
    }
    
    for (size_t i = 0; i < assignedRides.size(); i++) {
        cout << index++ << ". Ride ID: " << assignedRides[i].rideId
             << " [Assigned], Driver ID: " << assignedRides[i].driverId
             << ", From: " << assignedRides[i].pickup << "\n";
    }
    
    int num = getValidIntegerInput("\nEnter ride number to cancel: ", 1, index - 1);
    
    Ride cancelledRide;
    
    if (num <= requestedList.size()) {
        cancelledRide = requestedList[num - 1];
        
        queue<Ride> newQueue;
        tempQueue = rideRequests;
        while (!tempQueue.empty()) {
            Ride r = tempQueue.front();
            tempQueue.pop();
            if (r.rideId != cancelledRide.rideId) {
                newQueue.push(r);
            }
        }
        rideRequests = newQueue;
    } else {
        int assignedIndex = num - requestedList.size() - 1;
        cancelledRide = assignedRides[assignedIndex];
        
        auto driverIt = drivers.find(cancelledRide.driverId);
        if (driverIt != drivers.end()) {
            driverIt->second.available = true;
        }
        
        assignedRides.erase(assignedRides.begin() + assignedIndex);
    }
    
    cancelledRide.status = "Cancelled";
    cancelledRide.timestamp = getCurrentTime();
    cancelledRides.push(cancelledRide);
    
    cout << "\n✓ Ride cancelled successfully!\n";
    
    saveDrivers();
    saveRides();
    pressAnyKey();
}
void viewAllRides() {
    clearScreen();
    cout << "=== ALL RIDES ===\n\n";
    
    if (!rideRequests.empty()) {
        cout << "--- REQUESTED RIDES (Queue) ---\n";
        queue<Ride> tempQueue = rideRequests;
        while (!tempQueue.empty()) {
            Ride r = tempQueue.front();
            tempQueue.pop();
            cout << "Ride ID: " << r.rideId
                 << ", Rider ID: " << r.riderId
                 << ", From: " << r.pickup
                 << ", To: " << r.destination
                 << ", Fare: Rs. " << fixed << setprecision(2) << r.fare << "\n";
        }
        cout << endl;
    }
    
    if (!assignedRides.empty()) {
        cout << "--- ASSIGNED RIDES ---\n";
        for (const auto& r : assignedRides) {
            cout << "Ride ID: " << r.rideId
                 << ", Driver ID: " << r.driverId
                 << ", Rider ID: " << r.riderId
                 << ", From: " << r.pickup
                 << ", To: " << r.destination << "\n";
        }
        cout << endl;
    }
    
    if (!completedRides.empty()) {
        cout << "--- COMPLETED RIDES ---\n";
        for (const auto& r : completedRides) {
            cout << "Ride ID: " << r.rideId
                 << ", Driver ID: " << r.driverId
                 << ", Fare: Rs. " << fixed << setprecision(2) << r.fare
                 << ", Distance: " << r.distance << " km\n";
        }
        cout << endl;
    }
    
    if (!cancelledRides.empty()) {
        cout << "--- CANCELLED RIDES (Stack - LIFO) ---\n";
        stack<Ride> tempStack = cancelledRides;
        while (!tempStack.empty()) {
            Ride r = tempStack.top();
            tempStack.pop();
            cout << "Ride ID: " << r.rideId
                 << ", Rider ID: " << r.riderId
                 << ", From: " << r.pickup
                 << ", Cancelled at: " << r.timestamp << "\n";
        }
        cout << endl;
    }
    
    if (rideRequests.empty() && assignedRides.empty() && 
        completedRides.empty() && cancelledRides.empty()) {
        cout << "No rides in the system!\n";
    }
    
    pressAnyKey();
}

// ==================== ANALYTICS & REPORTING ====================

void showAnalytics() {
    clearScreen();
    cout << "=== SYSTEM ANALYTICS ===\n\n";
    
    int totalRequested = 0;
    queue<Ride> temp = rideRequests;
    while (!temp.empty()) {
        temp.pop();
        totalRequested++;
    }
    
    int totalCancelled = 0;
    stack<Ride> tempStack = cancelledRides;
    while (!tempStack.empty()) {
        tempStack.pop();
        totalCancelled++;
    }
    
    double totalEarnings = 0;
    double totalDistance = 0;
    double avgRating = 0;
    int activeDrivers = 0;
    
    for (auto& pair : drivers) {
        Driver& d = pair.second;
        totalEarnings += d.totalEarnings;
        if (d.available) activeDrivers++;
        avgRating += d.rating;
    }
    
    for (const auto& r : completedRides) {
        totalDistance += r.distance;
    }
    
    if (!drivers.empty()) {
        avgRating /= drivers.size();
    }
    
    double avgDistance = completedRides.empty() ? 0 : totalDistance / completedRides.size();
    
    cout << "Total Drivers: " << drivers.size() << "\n";
    cout << "Active Drivers: " << activeDrivers << "\n";
    cout << "Total Riders: " << riders.size() << "\n";
    cout << "Average Driver Rating: " << fixed << setprecision(2) << avgRating << "\n\n";
    
    cout << "Requested Rides: " << totalRequested << "\n";
    cout << "Assigned Rides: " << assignedRides.size() << "\n";
    cout << "Completed Rides: " << completedRides.size() << "\n";
    cout << "Cancelled Rides: " << totalCancelled << "\n\n";
    
    cout << "Total Earnings: Rs. " << fixed << setprecision(2) << totalEarnings << "\n";
    cout << "Total Distance: " << totalDistance << " km\n";
    cout << "Average Distance per Ride: " << avgDistance << " km\n\n";
    
    if (!drivers.empty()) {
        Driver* mostActive = nullptr;
        for (auto& pair : drivers) {
            if (mostActive == nullptr || pair.second.totalRides > mostActive->totalRides) {
                mostActive = &pair.second;
            }
        }
        
        if (mostActive && mostActive->totalRides > 0) {
            cout << "=== MOST ACTIVE DRIVER ===\n";
            cout << "Name: " << mostActive->name << "\n";
            cout << "Total Rides: " << mostActive->totalRides << "\n";
            cout << "Total Earnings: Rs. " << fixed << setprecision(2) << mostActive->totalEarnings << "\n";
            cout << "Rating: " << mostActive->rating << "\n\n";
        }
    }
    
    if (!riders.empty()) {
        Rider* topRider = nullptr;
        for (auto& pair : riders) {
            if (topRider == nullptr || pair.second.totalBookings > topRider->totalBookings) {
                topRider = &pair.second;
            }
        }
        
        if (topRider && topRider->totalBookings > 0) {
            cout << "=== MOST ACTIVE RIDER ===\n";
            cout << "Name: " << topRider->name << "\n";
            cout << "Total Bookings: " << topRider->totalBookings << "\n";
        }
    }
    
    pressAnyKey();
}

void exportToCSV() {
    ofstream file(CSV_REPORT);
    if (!file) {
        cerr << "Error: Cannot create CSV file!\n";
        pressAnyKey();
        return;
    }
    
    clearScreen();
    cout << "=== EXPORTING TO CSV ===\n\n";
    
    file << "RIDE-SHARING SYSTEM ANALYTICS REPORT\n";
    file << "Generated: " << getCurrentTime() << "\n\n";
    
    file << "DRIVER STATISTICS\n";
    file << "ID,Name,Contact,Rating,Available,Total Rides,Total Earnings\n";
    
    for (auto& pair : drivers) {
        Driver& d = pair.second;
        file << d.id << ","
             << d.name << ","
             << d.contact << ","
             << fixed << setprecision(2) << d.rating << ","
             << (d.available ? "Yes" : "No") << ","
             << d.totalRides << ","
             << d.totalEarnings << "\n";
    }
    
    file << "\n";
    
    file << "COMPLETED RIDES\n";
    file << "Ride ID,Driver ID,Rider ID,Pickup,Destination,Distance (km),Fare (Rs),Timestamp\n";
    
    for (const auto& r : completedRides) {
        file << r.rideId << ","
             << r.driverId << ","
             << r.riderId << ","
             << r.pickup << ","
             << r.destination << ","
             << r.distance << ","
             << r.fare << ","
             << r.timestamp << "\n";
    }
    
    file.close();
    
    cout << "✓ Analytics exported to: " << CSV_REPORT << "\n";
    cout << "You can open this file in Excel or any spreadsheet application.\n";
    
    pressAnyKey();
}

void generateReport() {
    ofstream file(REPORTS_FILE);
    if (!file) {
        cerr << "Error: Cannot create report file!\n";
        pressAnyKey();
        return;
    }
    
    clearScreen();
    cout << "=== GENERATING REPORT ===\n\n";
    
    file << "========================================================\n";
    file << "     RIDE-SHARING SYSTEM - COMPREHENSIVE REPORT        \n";
    file << "========================================================\n";
    file << "Generated: " << getCurrentTime() << "\n\n";
    
    file << "SYSTEM SUMMARY\n";
    file << "---------------\n";
    file << "Total Drivers: " << drivers.size() << "\n";
    file << "Total Riders: " << riders.size() << "\n";
    file << "Completed Rides: " << completedRides.size() << "\n";
    file << "Cancelled Rides: ";
    
    stack<Ride> tempStack = cancelledRides;
    int cancelCount = 0;
    while (!tempStack.empty()) {
        tempStack.pop();
        cancelCount++;
    }
    file << cancelCount << "\n\n";
    
    double totalRevenue = 0;
    for (const auto& r : completedRides) {
        totalRevenue += r.fare;
    }
    
    file << "FINANCIAL SUMMARY\n";
    file << "-----------------\n";
    file << "Total Revenue: Rs. " << fixed << setprecision(2) << totalRevenue << "\n";
    file << "Average Fare: Rs. ";
    if (!completedRides.empty()) {
        file << (totalRevenue / completedRides.size()) << "\n\n";
    } else {
        file << "0.00\n\n";
    }
    
    file << "TOP PERFORMING DRIVERS\n";
    file << "----------------------\n";
    
    vector<Driver> topDrivers;
    for (auto& pair : drivers) {
        topDrivers.push_back(pair.second);
    }
    
    sort(topDrivers.begin(), topDrivers.end(), [](const Driver& a, const Driver& b) {
        return a.totalEarnings > b.totalEarnings;
    });
    
    int count = 0;
    for (const auto& d : topDrivers) {
        if (count++ >= 5) break;
        file << count << ". " << d.name 
             << " - Rs. " << fixed << setprecision(2) << d.totalEarnings
             << " (" << d.totalRides << " rides)\n";
    }
    
    file << "\n========================================================\n";
    file << "                    END OF REPORT                       \n";
    file << "========================================================\n";
    
    file.close();
    
    cout << "✓ Report generated successfully!\n";
    cout << "File saved as: " << REPORTS_FILE << "\n";
    
    pressAnyKey();
}

void displaySummary() {
    clearScreen();
    cout << "=== SYSTEM SUMMARY ===\n\n";
    
    cout << "Data Structures Used:\n";
    cout << "- Map: For drivers and riders (O(log n) search)\n";
    cout << "- Queue: For ride requests (FIFO)\n";
    cout << "- Stack: For cancelled rides (LIFO)\n";
    cout << "- Vector: For completed/assigned rides\n\n";
    
    cout << "Algorithms Implemented:\n";
    cout << "- Bubble Sort: O(n²) for sorting drivers by name\n";
    cout << "- Merge Sort: O(n log n) for sorting by rating\n";
    cout << "- Quick Sort: O(n log n) average for sorting riders\n";
    cout << "- Linear Search: O(n) for simple searches\n";
    cout << "- Binary Search: O(log n) for sorted data\n\n";
    
    cout << "Current System Status:\n";
    cout << "Drivers: " << drivers.size() << "\n";
    cout << "Riders: " << riders.size() << "\n";
    
    queue<Ride> temp = rideRequests;
    int reqCount = 0;
    while (!temp.empty()) {
        temp.pop();
        reqCount++;
    }
    cout << "Pending Requests: " << reqCount << "\n";
    cout << "Assigned Rides: " << assignedRides.size() << "\n";
    cout << "Completed Rides: " << completedRides.size() << "\n";
    
    pressAnyKey();
}

// ==================== MENU FUNCTIONS ====================

void adminMenu() {
    int choice;
    do {
        clearScreen();
        cout << "=== ADMIN MENU ===\n\n";
        cout << "Driver Management:\n";
        cout << "1. Add Driver\n";
        cout << "2. View All Drivers\n";
        cout << "3. Search Driver\n";
        cout << "4. Sort Drivers\n";
        cout << "5. Update Driver\n\n";
        
        cout << "Rider Management:\n";
        cout << "6. Add Rider\n";
        cout << "7. View All Riders\n";
        cout << "8. Search Rider\n";
        cout << "9. Sort Riders\n";
        cout << "10. Update Rider\n\n";
        
        cout << "Ride Management:\n";
        cout << "11. Assign Rides to Drivers\n";
        cout << "12. View All Rides\n\n";
        
        cout << "Analytics & Reports:\n";
        cout << "13. Show Analytics\n";
        cout << "14. Generate Report (TXT)\n";
        cout << "15. Export to CSV\n";
        cout << "16. Display Summary\n\n";
        
        cout << "Algorithm Comparison:\n";
        cout << "17. Compare Sorting Algorithms\n";
        cout << "18. Compare Search Algorithms\n\n";
        
        cout << "0. Back to Main Menu\n";
        
        choice = getValidIntegerInput("\nEnter choice: ", 0, 18);
        
        switch(choice) {
            case 1: addDriver(); break;
            case 2: viewDrivers(); break;
            case 3: searchDriver(); break;
            case 4: sortDrivers(); break;
            case 5: updateDriver(); break;
            case 6: addRider(); break;
            case 7: viewRiders(); break;
            case 8: searchRider(); break;
            case 9: sortRiders(); break;
            case 10: updateRider(); break;
            case 11: assignRides(); break;
            case 12: viewAllRides(); break;
            case 13: showAnalytics(); break;
            case 14: generateReport(); break;
            case 15: exportToCSV(); break;
            case 16: displaySummary(); break;
            case 17: compareSortAlgorithms(); break;
            case 18: compareSearchAlgorithms(); break;
            case 0: break;
            default: 
                cout << "Invalid choice!\n"; 
                pressAnyKey();
        }
    } while(choice != 0);
}
void driverMenu() {
    int choice;
    do {
        clearScreen();
        cout << "=== DRIVER MENU ===\n\n";
        cout << "1. View My Profile\n";
        cout << "2. Complete Ride\n";
        cout << "3. View Available Rides\n";
        cout << "0. Back to Main Menu\n";
        
        choice = getValidIntegerInput("\nEnter choice: ", 0, 3);
        
        switch(choice) {
            case 1: viewDrivers(); break;
            case 2: completeRide(); break;
            case 3: 
                clearScreen();
                cout << "=== AVAILABLE RIDE REQUESTS ===\n\n";
                if (rideRequests.empty()) {
                    cout << "No ride requests available.\n";
                } else {
                    queue<Ride> temp = rideRequests;
                    while (!temp.empty()) {
                        Ride r = temp.front();
                        temp.pop();
                        cout << "Ride ID: " << r.rideId 
                             << ", From: " << r.pickup
                             << ", To: " << r.destination
                             << ", Fare: Rs. " << fixed << setprecision(2) << r.fare << endl;
                    }
                }
                pressAnyKey();
                break;
            case 0: break;
            default: 
                cout << "Invalid choice!\n"; 
                pressAnyKey();
        }
    } while(choice != 0);
}
void riderMenu() {
    clearScreen();
    cout << "========================================\n";
    cout << "         WELCOME RIDER!                 \n";
    cout << "========================================\n\n";
    
    // Ask for rider ID (like phone number in Uber)
    int riderId = getValidIntegerInput("Enter your Rider ID (or 0 to go back): ", 0, 999999);
    
    if (riderId == 0) {
        return;  // Go back to main menu
    }
    
    // Check if rider exists
    if (riders.find(riderId) == riders.end()) {
        cout << "\n[X] Rider ID not found!\n";
        cout << "Please register first (Main Menu -> Option 2)\n";
        pressAnyKey();
        return;
    }
    
    // Show rider info
    Rider& currentRider = riders[riderId];
    cout << "\n========================================\n";
    cout << "Hello, " << currentRider.name << "!\n";
    cout << "Total Bookings: " << currentRider.totalBookings << "\n";
    cout << "========================================\n";
    Sleep(1500);
    
    int choice;
    do {
        clearScreen();
        cout << "========================================\n";
        cout << "  " << currentRider.name << "'s Menu\n";
        cout << "========================================\n\n";
        cout << "1. Book a Ride (Quick)\n";
        cout << "2. Book a Ride (Choose Driver)\n";
        cout << "3. View My Rides\n";
        cout << "4. Cancel a Ride\n";
        cout << "5. Rate a Driver\n";
        cout << "6. View Available Drivers\n";
        cout << "0. Logout\n";
        
        choice = getValidIntegerInput("\nEnter choice: ", 0, 6);
        
        switch(choice) {
            case 1: 
                requestRideQuick(riderId);
                break;
            case 2: 
                requestRideWithDriverSelectionNew(riderId);
                break;
            case 3: 
                viewMyRides(riderId);
                break;
            case 4: 
                cancelMyRide(riderId);
                break;
            case 5: 
                rateDriver(riderId);
                break;
            case 6: 
                viewAvailableDrivers();
                pressAnyKey();
                break;
            case 0: 
                cout << "\nLogging out...\n";
                Sleep(1000);
                break;
            default: 
                cout << "Invalid choice!\n"; 
                pressAnyKey();
        }
    } while(choice != 0);
}

void requestRideQuick(int riderId) {
    clearScreen();
    cout << "========================================\n";
    cout << "         BOOK A RIDE                    \n";
    cout << "========================================\n\n";
    
    Ride ride;
    ride.rideId = generateId();
    ride.riderId = riderId;
    
    cout << "Enter Pickup Location: ";
    getline(cin, ride.pickup);
    cout << "Enter Destination: ";
    getline(cin, ride.destination);
    
    ride.distance = getValidDoubleInput("Enter Distance (km): ", 0.1, 1000.0);
    
    // Display vehicle options
    displayVehicleOptions();
    int vehicleChoice = getValidIntegerInput("\nSelect vehicle type: ", 1, 4);
    ride.vehicleType = getVehicleType(vehicleChoice);
    
    // Calculate fare
    ride.fare = calculateFareWithVehicle(ride.distance, ride.vehicleType);
    
    cout << "\n========================================\n";
    cout << "      RIDE SUMMARY                      \n";
    cout << "========================================\n";
    cout << "From: " << ride.pickup << "\n";
    cout << "To: " << ride.destination << "\n";
    cout << "Distance: " << ride.distance << " km\n";
    cout << "Vehicle: " << ride.vehicleType << "\n";
    cout << "Fare: Rs. " << fixed << setprecision(2) << ride.fare << "\n";
    cout << "========================================\n\n";
    
    cout << "Confirm booking? (y/n): ";
    char confirm;
    cin >> confirm;
    cin.ignore();
    
    if (confirm != 'y' && confirm != 'Y') {
        cout << "\nBooking cancelled.\n";
        pressAnyKey();
        return;
    }
    
    ride.timestamp = getCurrentTime();
    
    // ========== AUTO-ASSIGN DRIVER (NEW LOGIC) ==========
    
    // Find available driver with matching vehicle type
    bool driverFound = false;
    Driver* assignedDriver = nullptr;
    
    for (auto& pair : drivers) {
        if (pair.second.available && 
            pair.second.vehicleType == ride.vehicleType) {
            assignedDriver = &pair.second;
            driverFound = true;
            break;
        }
    }
    
    if (driverFound && assignedDriver != nullptr) {
        // Driver found - assign immediately
        ride.driverId = assignedDriver->id;
        ride.status = "Assigned";
        assignedDriver->available = false;  // Mark driver as busy
        assignedRides.push_back(ride);
        
        cout << "\n[OK] Ride booked successfully!\n";
        cout << "Ride ID: " << ride.rideId << "\n";
        cout << "Driver: " << assignedDriver->name << "\n";
        cout << "Vehicle: " << assignedDriver->vehicleType << "\n";
        cout << "Rating: " << fixed << setprecision(1) << assignedDriver->rating << " stars\n";
        cout << "\nYour driver is on the way!\n";
    } else {
        // No driver available - add to queue
        ride.driverId = 0;
        ride.status = "Requested";
        rideRequests.push(ride);
        
        cout << "\n[OK] Ride request queued!\n";
        cout << "Ride ID: " << ride.rideId << "\n";
        cout << "\nNo " << ride.vehicleType << " drivers available right now.\n";
        cout << "We'll assign a driver as soon as one becomes available.\n";
    }
    
    riders[riderId].totalBookings++;
    
    saveDrivers();  // Save driver availability status
    saveRides();
    saveRiders();
    pressAnyKey();
}
void requestRideWithDriverSelectionNew(int riderId) {
    clearScreen();
    cout << "========================================\n";
    cout << "   BOOK RIDE - CHOOSE YOUR DRIVER      \n";
    cout << "========================================\n\n";
    
    Ride ride;
    ride.rideId = generateId();
    ride.riderId = riderId;
    
    cout << "Enter Pickup Location: ";
    getline(cin, ride.pickup);
    cout << "Enter Destination: ";
    getline(cin, ride.destination);
    
    ride.distance = getValidDoubleInput("Enter Distance (km): ", 0.1, 1000.0);
    
    // Select vehicle type
    displayVehicleOptions();
    int vehicleChoice = getValidIntegerInput("\nSelect vehicle type: ", 1, 4);
    ride.vehicleType = getVehicleType(vehicleChoice);
    
    // Calculate fare
    ride.fare = calculateFareWithVehicle(ride.distance, ride.vehicleType);
    
    // Filter drivers by vehicle type
    vector<Driver> availableDrivers;
    for (auto& pair : drivers) {
        if (pair.second.available && pair.second.vehicleType == ride.vehicleType) {
            availableDrivers.push_back(pair.second);
        }
    }
    
    if (availableDrivers.empty()) {
        cout << "\n[X] No " << ride.vehicleType << " drivers available!\n";
        cout << "Would you like to queue your request? (y/n): ";
        char ch;
        cin >> ch;
        cin.ignore();
        
        if (ch == 'y' || ch == 'Y') {
            ride.status = "Requested";
            ride.timestamp = getCurrentTime();
            ride.driverId = 0;
            rideRequests.push(ride);
            riders[riderId].totalBookings++;
            cout << "\n[OK] Ride queued!\n";
            saveRides();
            saveRiders();
        }
        pressAnyKey();
        return;
    }
    
    // Show available drivers
    cout << "\n\nAvailable " << ride.vehicleType << " Drivers:\n\n";
    cout << left << setw(4) << "#" << setw(8) << "ID" 
         << setw(20) << "NAME" 
         << setw(10) << "RATING"
         << setw(10) << "RIDES" << endl;
    cout << string(52, '-') << endl;
    
    for (size_t i = 0; i < availableDrivers.size(); i++) {
        Driver& d = availableDrivers[i];
        cout << left << setw(4) << i+1
             << setw(8) << d.id
             << setw(20) << d.name
             << setw(10) << fixed << setprecision(1) << d.rating
             << d.totalRides << endl;
    }
    
    cout << "\n========================================\n";
    cout << "Fare: Rs. " << fixed << setprecision(2) << ride.fare << "\n";
    cout << "========================================\n";
    
    int driverChoice = getValidIntegerInput("\nSelect driver number: ", 1, availableDrivers.size());
    
    Driver& selectedDriver = availableDrivers[driverChoice - 1];
    ride.driverId = selectedDriver.id;
    ride.status = "Assigned";
    ride.timestamp = getCurrentTime();
    
    drivers[selectedDriver.id].available = false;
    assignedRides.push_back(ride);
    riders[riderId].totalBookings++;
    
    cout << "\n[OK] Ride booked with " << selectedDriver.name << "!\n";
    cout << "Ride ID: " << ride.rideId << "\n";
    cout << "Driver is on the way!\n";
    
    saveAllData();
    pressAnyKey();
}
void viewMyRides(int riderId) {
    clearScreen();
    cout << "========================================\n";
    cout << "         MY RIDES HISTORY               \n";
    cout << "========================================\n\n";
    
    bool found = false;
    
    // Check completed rides
    cout << "--- COMPLETED RIDES ---\n";
    for (const auto& r : completedRides) {
        if (r.riderId == riderId) {
            found = true;
            cout << "Ride ID: " << r.rideId << "\n";
            cout << "From: " << r.pickup << " -> To: " << r.destination << "\n";
            cout << "Vehicle: " << r.vehicleType << "\n";
            cout << "Fare: Rs. " << fixed << setprecision(2) << r.fare << "\n";
            cout << "Date: " << r.timestamp << "\n";
            cout << string(50, '-') << "\n";
        }
    }
    
    // Check assigned rides
    cout << "\n--- ACTIVE RIDES ---\n";
    for (const auto& r : assignedRides) {
        if (r.riderId == riderId) {
            found = true;
            cout << "Ride ID: " << r.rideId << "\n";
            cout << "From: " << r.pickup << " -> To: " << r.destination << "\n";
            cout << "Status: In Progress\n";
            cout << string(50, '-') << "\n";
        }
    }
    
    // Check pending requests
    queue<Ride> temp = rideRequests;
    cout << "\n--- PENDING REQUESTS ---\n";
    while (!temp.empty()) {
        Ride r = temp.front();
        temp.pop();
        if (r.riderId == riderId) {
            found = true;
            cout << "Ride ID: " << r.rideId << "\n";
            cout << "From: " << r.pickup << " -> To: " << r.destination << "\n";
            cout << "Status: Searching for driver...\n";
            cout << string(50, '-') << "\n";
        }
    }
    
    if (!found) {
        cout << "You haven't booked any rides yet!\n";
    }
    
    pressAnyKey();
}
void cancelMyRide(int riderId) {
    clearScreen();
    cout << "========================================\n";
    cout << "         CANCEL A RIDE                  \n";
    cout << "========================================\n\n";
    
    vector<Ride> myRides;
    queue<Ride> tempQueue = rideRequests;
    vector<Ride> requestedList;
    
    // Get requested rides
    while (!tempQueue.empty()) {
        Ride r = tempQueue.front();
        tempQueue.pop();
        if (r.riderId == riderId) {
            requestedList.push_back(r);
        }
    }
    
    // Get assigned rides
    vector<Ride> myAssignedRides;
    for (const auto& r : assignedRides) {
        if (r.riderId == riderId) {
            myAssignedRides.push_back(r);
        }
    }
    
    if (requestedList.empty() && myAssignedRides.empty()) {
        cout << "You have no active rides to cancel!\n";
        pressAnyKey();
        return;
    }
    
    int index = 1;
    
    cout << "Your Active Rides:\n\n";
    
    for (const auto& r : requestedList) {
        cout << index++ << ". Ride ID: " << r.rideId
             << " [Searching for driver]\n";
        cout << "   From: " << r.pickup << " -> " << r.destination << "\n\n";
    }
    
    for (const auto& r : myAssignedRides) {
        cout << index++ << ". Ride ID: " << r.rideId
             << " [Driver assigned]\n";
        cout << "   From: " << r.pickup << " -> " << r.destination << "\n\n";
    }
    
    int choice = getValidIntegerInput("Enter ride number to cancel (0 to go back): ", 0, index - 1);
    
    if (choice == 0) return;
    
    Ride cancelledRide;
    
    if (choice <= requestedList.size()) {
        // Cancel from requested
        cancelledRide = requestedList[choice - 1];
        queue<Ride> newQueue;
        tempQueue = rideRequests;
        while (!tempQueue.empty()) {
            Ride r = tempQueue.front();
            tempQueue.pop();
            if (r.rideId != cancelledRide.rideId) {
                newQueue.push(r);
            }
        }
        rideRequests = newQueue;
    } else {
        // Cancel from assigned
        int assignedIndex = choice - requestedList.size() - 1;
        cancelledRide = myAssignedRides[assignedIndex];
        
        // Make driver available again
        if (drivers.find(cancelledRide.driverId) != drivers.end()) {
            drivers[cancelledRide.driverId].available = true;
        }
        
        // Remove from assigned rides
        for (auto it = assignedRides.begin(); it != assignedRides.end(); ++it) {
            if (it->rideId == cancelledRide.rideId) {
                assignedRides.erase(it);
                break;
            }
        }
    }
    
    cancelledRide.status = "Cancelled";
    cancelledRide.timestamp = getCurrentTime();
    cancelledRides.push(cancelledRide);
    
    cout << "\n[OK] Ride cancelled successfully!\n";
    saveDrivers();
    saveRides();
    pressAnyKey();
}

void rateDriver(int riderId) {
    clearScreen();
    cout << "========================================\n";
    cout << "         RATE YOUR DRIVER               \n";
    cout << "========================================\n\n";
    
    vector<Ride> myCompletedRides;
    for (size_t i = 0; i < completedRides.size(); i++) {
        if (completedRides[i].riderId == riderId) {
            myCompletedRides.push_back(completedRides[i]);
        }
    }
    
    if (myCompletedRides.empty()) {
        cout << "You have no completed rides to rate!\n";
        pressAnyKey();
        return;
    }
    
    cout << "Your Completed Rides:\n\n";
    for (size_t i = 0; i < myCompletedRides.size(); i++) {
        cout << i + 1 << ". Ride ID: " << myCompletedRides[i].rideId
             << ", Driver ID: " << myCompletedRides[i].driverId << "\n";
        cout << "   From: " << myCompletedRides[i].pickup 
             << " -> " << myCompletedRides[i].destination << "\n\n";
    }
    
    int rideNum = getValidIntegerInput("Select ride to rate: ", 1, myCompletedRides.size());
    int driverId = myCompletedRides[rideNum - 1].driverId;
    
    float rating = getValidDoubleInput("Rate driver (1-5 stars): ", 1.0, 5.0);
    
    auto it = drivers.find(driverId);
    if (it != drivers.end()) {
        Driver& d = it->second;
        d.rating = (d.rating * d.totalRides + rating) / (d.totalRides + 1);
        cout << "\n[OK] Thank you for rating!\n";
        cout << "Driver's new rating: " << fixed << setprecision(1) << d.rating << " stars\n";
        saveDrivers();
    }
    
    pressAnyKey();
}

void displayMainMenu() {
    int choice;
    do {
        clearScreen();
        cout << "========================================\n";
        cout << "   RIDE-SHARING SYSTEM (Mini-Uber)     \n";
        cout << "========================================\n\n";
        cout << "1. Book a Ride (Rider)\n";
        cout << "2. Register as New Rider\n";
        cout << "3. Driver Login\n";
        cout << "4. Admin Management Panel\n";
        cout << "0. Exit\n";
        
        choice = getValidIntegerInput("\nEnter choice: ", 0, 4);
        
        switch(choice) {
            case 1: 
                riderMenu();  // Direct access!
                break;
            case 2:
                addRider();  // Anyone can register
                break;
            case 3: 
                driverMenu(); 
                break;
            case 4: 
                if (adminLogin()) {  // Only admin needs password
                    adminMenu();
                }
                break;
            case 0: 
                saveAllData();
                cout << "\nThank you for using Ride-Sharing System!\n";
                cout << "Data saved successfully.\n";
                break;
            default: 
                cout << "Invalid choice!\n";
                pressAnyKey();
        }
    } while(choice != 0);
}

// ==================== SAMPLE DATA GENERATOR ====================

void createSampleData() {
    cout << "\nCreating sample data...\n";
    
    // Create 5 drivers with different vehicles
    Driver d1, d2, d3, d4, d5;
    
    d1.id = 101; d1.name = "Ali Khan"; d1.contact = "0300-1111111"; 
    d1.rating = 4.5; d1.available = true; d1.totalRides = 5; 
    d1.totalEarnings = 1250.0; d1.vehicleType = "Bike";
    
    d2.id = 102; d2.name = "Ahmed Raza"; d2.contact = "0300-2222222"; 
    d2.rating = 4.2; d2.available = true; d2.totalRides = 3; 
    d2.totalEarnings = 780.0; d2.vehicleType = "Car";
    
    d3.id = 103; d3.name = "Bilal Ahmed"; d3.contact = "0300-3333333"; 
    d3.rating = 4.8; d3.available = false; d3.totalRides = 8; 
    d3.totalEarnings = 2100.0; d3.vehicleType = "AC Car";
    
    d4.id = 104; d4.name = "Usman Ali"; d4.contact = "0300-4444444"; 
    d4.rating = 4.0; d4.available = true; d4.totalRides = 2; 
    d4.totalEarnings = 450.0; d4.vehicleType = "SUV";
    
    d5.id = 105; d5.name = "Zain Malik"; d5.contact = "0300-5555555"; 
    d5.rating = 4.7; d5.available = true; d5.totalRides = 6; 
    d5.totalEarnings = 1500.0; d5.vehicleType = "Car";
    
    drivers[d1.id] = d1;
    drivers[d2.id] = d2;
    drivers[d3.id] = d3;
    drivers[d4.id] = d4;
    drivers[d5.id] = d5;
    
    // Create 5 riders
    Rider r1, r2, r3, r4, r5;
    r1.id = 201; r1.name = "Sara Khan"; r1.contact = "0311-1111111"; r1.totalBookings = 5;
    r2.id = 202; r2.name = "Fatima Ali"; r2.contact = "0311-2222222"; r2.totalBookings = 3;
    r3.id = 203; r3.name = "Ayesha Ahmed"; r3.contact = "0311-3333333"; r3.totalBookings = 8;
    r4.id = 204; r4.name = "Zara Malik"; r4.contact = "0311-4444444"; r4.totalBookings = 2;
    r5.id = 205; r5.name = "Hina Raza"; r5.contact = "0311-5555555"; r5.totalBookings = 6;
    
    riders[r1.id] = r1;
    riders[r2.id] = r2;
    riders[r3.id] = r3;
    riders[r4.id] = r4;
    riders[r5.id] = r5;
    
    // Create sample completed rides (20+)
    for (int i = 0; i < 22; i++) {
        Ride ride;
        ride.rideId = 301 + i;
        ride.driverId = 101 + (i % 5);
        ride.riderId = 201 + (i % 5);
        ride.pickup = "Location " + to_string(i % 10);
        ride.destination = "Destination " + to_string((i + 5) % 10);
        ride.distance = 3.0 + (i % 10);
        ride.fare = calculateFare(ride.distance);
        ride.status = "Completed";
        ride.timestamp = getCurrentTime();
        completedRides.push_back(ride);
    }
    
    // Create 3 pending ride requests
    for (int i = 0; i < 3; i++) {
        Ride ride;
        ride.rideId = 401 + i;
        ride.driverId = 0;
        ride.riderId = 201 + i;
        ride.pickup = "Pickup " + to_string(i);
        ride.destination = "Drop " + to_string(i);
        ride.distance = 5.0 + i;
        ride.fare = calculateFare(ride.distance);
        ride.status = "Requested";
        ride.timestamp = getCurrentTime();
        rideRequests.push(ride);
    }
    
    cout << "[OK] Sample data created successfully!\n";
    cout << "- 5 Drivers added (Bike, Car, AC Car, SUV)\n";
    cout << "- 5 Riders added\n";
    cout << "- 22 Completed rides added\n";
    cout << "- 3 Pending ride requests added\n";
    cout << "\nAdmin Login Credentials:\n";
    cout << "   Username: admin\n";
    cout << "   Password: admin123\n";
}

// ==================== MAIN FUNCTION ====================

int main() {
    srand(time(0));
    
    cout << "===========================================\n";
    cout << "  Loading Ride-Sharing System (Mini-Uber) \n";
    cout << "===========================================\n\n";
    
    ifstream test(DRIVERS_FILE);
    if (!test) {
        cout << "No data files found. Creating sample data...\n";
        createSampleData();
        saveAllData();
        cout << "\nPress Enter to continue...";
        cin.get();
    } else {
        test.close();
        loadAllData();
        cout << "\nPress Enter to continue...";
        cin.get();
    }
    
    Sleep(2000);
    displayMainMenu();
    
    return 0;
}