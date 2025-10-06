// dynamic_qos_simulation_consistent.cpp
#include <iostream>
#include <fstream>
#include <vector>
#include <random>
#include <iomanip>
#include <chrono>
#include <cmath>
#include <algorithm> // För std::transform
#include <cctype>    // För std::tolower

using namespace std;

struct DeviceMetrics {
    string deviceName;
    double throughput; // Mbps
    double packetLoss; // %
    double energy;     // J
};

// Base values that maintain the RELATIONSHIP from Python 1 figure
const struct DynamicBaseMetrics {
    // Dynamic QoS pattern: Lower throughput ↔ Better packet loss
    double urllc_throughput_ratio = 0.95;  // 5% lower than static
    double urllc_packetloss_ratio = 0.60;  // 40% better than static
    double urllc_energy_ratio = 1.04;      // 4% higher
    
    double embb_throughput_ratio = 0.93;   // 7% lower than static  
    double embb_packetloss_ratio = 0.80;   // 20% better than static
    double embb_energy_ratio = 0.94;       // 6% lower
    
    double mmtc_throughput_ratio = 0.80;   // 20% lower than static
    double mmtc_packetloss_ratio = 0.86;   // 14% better than static
    double mmtc_energy_ratio = 0.86;       // 14% lower
} DYNAMIC_RATIOS;

// Helper function to convert string to lowercase
string toLowerCase(const string& str) {
    string result = str;
    transform(result.begin(), result.end(), result.begin(), 
              [](unsigned char c){ return tolower(c); });
    return result;
}

vector<DeviceMetrics> generateConsistentDynamicMetrics(const string& sliceType, double static_throughput, double static_packetloss, double static_energy) {
    unsigned seed = chrono::system_clock::now().time_since_epoch().count();
    mt19937 gen(seed);
    vector<DeviceMetrics> devices;
    
    // Small variation around the consistent ratios (±2-5%)
    uniform_real_distribution<double> small_var(0.98, 1.02);
    
    if (sliceType == "URLLC") {
        vector<string> urllcDevices = {"Robot (URLLC)", "Drone (URLLC)"};
        
        for (const auto& device : urllcDevices) {
            DeviceMetrics metrics;
            metrics.deviceName = device;
            
            // Apply consistent ratios to static values + small variation
            metrics.throughput = static_throughput * DYNAMIC_RATIOS.urllc_throughput_ratio * small_var(gen);
            metrics.packetLoss = static_packetloss * DYNAMIC_RATIOS.urllc_packetloss_ratio * small_var(gen);
            metrics.energy = static_energy * DYNAMIC_RATIOS.urllc_energy_ratio * small_var(gen);
            
            // Ensure realistic bounds
            metrics.throughput = max(80.0, min(120.0, metrics.throughput));
            metrics.packetLoss = max(0.01, min(0.06, metrics.packetLoss));
            metrics.energy = max(4.0, min(6.5, metrics.energy));
            
            devices.push_back(metrics);
        }
        
    } else if (sliceType == "eMBB") {
        vector<string> embbDevices = {"8K Video (eMBB)", "VR (eMBB)"};
        
        for (const auto& device : embbDevices) {
            DeviceMetrics metrics;
            metrics.deviceName = device;
            
            metrics.throughput = static_throughput * DYNAMIC_RATIOS.embb_throughput_ratio * small_var(gen);
            metrics.packetLoss = static_packetloss * DYNAMIC_RATIOS.embb_packetloss_ratio * small_var(gen);
            metrics.energy = static_energy * DYNAMIC_RATIOS.embb_energy_ratio * small_var(gen);
            
            metrics.throughput = max(300.0, min(550.0, metrics.throughput));
            metrics.packetLoss = max(0.10, min(0.35, metrics.packetLoss));
            metrics.energy = max(2.0, min(4.0, metrics.energy));
            
            devices.push_back(metrics);
        }
        
    } else { // mMTC
        vector<string> mmtcDevices = {"Smart Meter (mMTC)", "Sensor (mMTC)"};
        
        for (const auto& device : mmtcDevices) {
            DeviceMetrics metrics;
            metrics.deviceName = device;
            
            metrics.throughput = static_throughput * DYNAMIC_RATIOS.mmtc_throughput_ratio * small_var(gen);
            metrics.packetLoss = static_packetloss * DYNAMIC_RATIOS.mmtc_packetloss_ratio * small_var(gen);
            metrics.energy = static_energy * DYNAMIC_RATIOS.mmtc_energy_ratio * small_var(gen);
            
            metrics.throughput = max(5.0, min(20.0, metrics.throughput));
            metrics.packetLoss = max(1.5, min(5.0, metrics.packetLoss));
            metrics.energy = max(0.15, min(0.45, metrics.energy));
            
            devices.push_back(metrics);
        }
    }
    
    return devices;
}

// Function to read static values from CSV files
bool readStaticValues(const string& sliceType, double& throughput, double& packetloss, double& energy) {
    string filename = toLowerCase(sliceType) + "_performance.csv"; // <-- FIXAT!
    ifstream inFile(filename);
    
    if (!inFile.is_open()) {
        cerr << "Error: Could not open file " << filename << endl;
        return false;
    }
    
    string line;
    getline(inFile, line); // Skip header
    
    double total_throughput = 0, total_packetloss = 0, total_energy = 0;
    int count = 0;
    
    while (getline(inFile, line)) {
        stringstream ss(line);
        string device, tp, pl, en;
        
        getline(ss, device, ',');
        getline(ss, tp, ',');
        getline(ss, pl, ',');
        getline(ss, en, ',');
        
        total_throughput += stod(tp);
        total_packetloss += stod(pl);
        total_energy += stod(en);
        count++;
    }
    
    inFile.close();
    
    if (count > 0) {
        throughput = total_throughput / count;
        packetloss = total_packetloss / count;
        energy = total_energy / count;
        return true;
    }
    
    return false;
}

void writeDynamicResultsToCSV(const vector<DeviceMetrics>& devices, const string& filename) {
    ofstream outFile(filename);
    outFile << "Device,Throughput(Mbps),PacketLoss(%),Energy(J)\n";
    for (const auto& device : devices) {
        outFile << device.deviceName << ","
                << fixed << setprecision(2) << device.throughput << ","
                << fixed << setprecision(3) << device.packetLoss << ","
                << fixed << setprecision(2) << device.energy << "\n";
    }
    outFile.close();
}

int main() {
    cout << "Generating Consistent Dynamic QoS performance data..." << endl;
    cout << "Dynamic QoS pattern: Lower throughput ↔ Better packet loss" << endl;
    
    // Read static values to base dynamic values on them
    double urllc_static_tp, urllc_static_pl, urllc_static_en;
    double embb_static_tp, embb_static_pl, embb_static_en;
    double mmtc_static_tp, mmtc_static_pl, mmtc_static_en;
    
    if (!readStaticValues("URLLC", urllc_static_tp, urllc_static_pl, urllc_static_en) ||
        !readStaticValues("eMBB", embb_static_tp, embb_static_pl, embb_static_en) ||
        !readStaticValues("mMTC", mmtc_static_tp, mmtc_static_pl, mmtc_static_en)) {
        cout << "Error: Could not read static CSV files. Run C++1 first!" << endl;
        return 1;
    }
    
    cout << "Static values read successfully:" << endl;
    cout << "URLLC: " << urllc_static_tp << " Mbps, " << urllc_static_pl << "%, " << urllc_static_en << "J" << endl;
    cout << "eMBB: " << embb_static_tp << " Mbps, " << embb_static_pl << "%, " << embb_static_en << "J" << endl;
    cout << "mMTC: " << mmtc_static_tp << " Mbps, " << mmtc_static_pl << "%, " << mmtc_static_en << "J" << endl;
    
    // Generate dynamic metrics based on static values
    auto urllcDynamic = generateConsistentDynamicMetrics("URLLC", urllc_static_tp, urllc_static_pl, urllc_static_en);
    auto embbDynamic = generateConsistentDynamicMetrics("eMBB", embb_static_tp, embb_static_pl, embb_static_en);
    auto mmtcDynamic = generateConsistentDynamicMetrics("mMTC", mmtc_static_tp, mmtc_static_pl, mmtc_static_en);
    
    writeDynamicResultsToCSV(urllcDynamic, "urllc_dynamic.csv");
    writeDynamicResultsToCSV(embbDynamic, "embb_dynamic.csv");
    writeDynamicResultsToCSV(mmtcDynamic, "mmtc_dynamic.csv");
    
    cout << "Consistent Dynamic QoS data saved to CSV files!" << endl;
    cout << "Pattern maintained: Throughput ↓ ↔ Packet Loss ↓" << endl;
    
    return 0;
}
