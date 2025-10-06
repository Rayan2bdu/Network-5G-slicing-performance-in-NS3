// slice_simulation_variable.cpp
#include <iostream>
#include <fstream>
#include <vector>
#include <random>
#include <iomanip>
#include <chrono>
#include <cmath>

using namespace std;

struct DeviceMetrics {
    string deviceName;
    double throughput; // Mbps
    double packetLoss; // %
    double energy;     // J
};

// Base values for each slice type (can be adjusted)
const struct BaseMetrics {
    double urllc_throughput = 100.0;
    double urllc_packetLoss = 0.03;
    double urllc_energy = 5.2;
    
    double embb_throughput = 450.0;
    double embb_packetLoss = 0.2;
    double embb_energy = 2.8;
    
    double mmtc_throughput = 12.0;
    double mmtc_packetLoss = 3.0;
    double mmtc_energy = 0.3;
} BASE_VALUES;

vector<DeviceMetrics> generateSliceMetrics(const string& sliceType) {
    // Use time-based seed for different results each run
    unsigned seed = chrono::system_clock::now().time_since_epoch().count();
    mt19937 gen(seed);
    vector<DeviceMetrics> devices;
    
    if (sliceType == "URLLC") {
        vector<string> urllcDevices = {"Industrial Robot", "Autonomous Drone"};
        
        // Controlled variation around base values (±15-25%)
        uniform_real_distribution<double> throughput_var(0.80, 1.15);  // ±15-20%
        uniform_real_distribution<double> loss_var(0.75, 1.30);        // ±25-30%
        uniform_real_distribution<double> energy_var(0.85, 1.15);      // ±15%
        
        for (const auto& device : urllcDevices) {
            DeviceMetrics metrics;
            metrics.deviceName = device;
            metrics.throughput = BASE_VALUES.urllc_throughput * throughput_var(gen);
            metrics.packetLoss = BASE_VALUES.urllc_packetLoss * loss_var(gen);
            metrics.energy = BASE_VALUES.urllc_energy * energy_var(gen);
            
            // Ensure values stay within realistic bounds
            metrics.throughput = max(70.0, min(130.0, metrics.throughput));
            metrics.packetLoss = max(0.008, min(0.06, metrics.packetLoss));
            metrics.energy = max(4.0, min(6.5, metrics.energy));
            
            devices.push_back(metrics);
        }
        
    } else if (sliceType == "eMBB") {
        vector<string> embbDevices = {"8K Video", "VR Headset"};
        
        uniform_real_distribution<double> throughput_var(0.75, 1.20);  // ±25%
        uniform_real_distribution<double> loss_var(0.70, 1.35);        // ±30-35%
        uniform_real_distribution<double> energy_var(0.80, 1.20);      // ±20%
        
        for (const auto& device : embbDevices) {
            DeviceMetrics metrics;
            metrics.deviceName = device;
            metrics.throughput = BASE_VALUES.embb_throughput * throughput_var(gen);
            metrics.packetLoss = BASE_VALUES.embb_packetLoss * loss_var(gen);
            metrics.energy = BASE_VALUES.embb_energy * energy_var(gen);
            
            metrics.throughput = max(250.0, min(700.0, metrics.throughput));
            metrics.packetLoss = max(0.08, min(0.4, metrics.packetLoss));
            metrics.energy = max(1.8, min(4.0, metrics.energy));
            
            devices.push_back(metrics);
        }
        
    } else { // mMTC
        vector<string> mmtcDevices = {"Smart Meter", "Weather Sensor"};
        
        uniform_real_distribution<double> throughput_var(0.60, 1.40);  // ±40%
        uniform_real_distribution<double> loss_var(0.50, 1.60);        // ±50-60%
        uniform_real_distribution<double> energy_var(0.70, 1.40);      // ±30-40%
        
        for (const auto& device : mmtcDevices) {
            DeviceMetrics metrics;
            metrics.deviceName = device;
            metrics.throughput = BASE_VALUES.mmtc_throughput * throughput_var(gen);
            metrics.packetLoss = BASE_VALUES.mmtc_packetLoss * loss_var(gen);
            metrics.energy = BASE_VALUES.mmtc_energy * energy_var(gen);
            
            metrics.throughput = max(3.0, min(25.0, metrics.throughput));
            metrics.packetLoss = max(0.8, min(6.0, metrics.packetLoss));
            metrics.energy = max(0.08, min(0.6, metrics.energy));
            
            devices.push_back(metrics);
        }
    }
    
    return devices;
}

void writeResultsToCSV(const vector<DeviceMetrics>& devices, const string& filename) {
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

void printSummary(const vector<DeviceMetrics>& devices, const string& sliceType) {
    cout << sliceType << " Slice Summary:" << endl;
    for (const auto& device : devices) {
        cout << "  " << device.deviceName << ": "
             << device.throughput << " Mbps, "
             << device.packetLoss << "%, "
             << device.energy << "J" << endl;
    }
    cout << endl;
}

int main() {
    cout << "Generating new performance data with controlled variation..." << endl;
    cout << "Variation ranges: URLLC (±15-30%), eMBB (±20-35%), mMTC (±30-60%)" << endl << endl;
    
    auto urllcDevices = generateSliceMetrics("URLLC");
    auto embbDevices = generateSliceMetrics("eMBB");
    auto mmtcDevices = generateSliceMetrics("mMTC");
    
    printSummary(urllcDevices, "URLLC");
    printSummary(embbDevices, "eMBB");
    printSummary(mmtcDevices, "mMTC");
    
    writeResultsToCSV(urllcDevices, "urllc_performance.csv");
    writeResultsToCSV(embbDevices, "embb_performance.csv");
    writeResultsToCSV(mmtcDevices, "mmtc_performance.csv");
    
    cout << "Data saved to CSV files. Run Python visualization to see new results." << endl;
    return 0;
}
