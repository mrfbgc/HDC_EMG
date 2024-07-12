#include <systemc.h>
#include <vector>
#include <random>
#include <string>

// Define the size of the hypervector
#define HV_SIZE 10000

SC_MODULE(binding) {
    // definition of the signals
    sc_signal<bool> init;
    sc_signal<bool> training;
    sc_signal<bool> test;

    // Ports for hypervectors as vectors of signals. This date typpe used for HV, because sc_trace is suitable with this data format.
    sc_vector<sc_signal<int>> hv_port1;
    sc_vector<sc_signal<int>> hv_port2;

    // Item Memories
    std::vector<int> item_memory;
    std::vector<int> continuous_item_memory;

    //creating a result hv for binding operation of IM and CiM 
    // in the future this output will be bunded
    std::vector<int> output_hv;

    // Flag to indicate binary or bipolar hypervector
    bool is_binary; //if is_binary=true, then it will create hv as {0,1}
                    // if is_binary=false, then it will create hv as {-1,1}


    // Constructor of the module
    SC_CTOR(binding) : hv_port1("hv_port1", HV_SIZE), hv_port2("hv_port2", HV_SIZE) {
        // Initializing phase of the item memories with random HVs
        SC_THREAD(init_phase);

        // !! THIS IS THE CONFIGURATION PART FOR BINARY OR BIPOLAR HV !!
        is_binary = true; 
    }

    // Function to generate random hypervector
    void generate_random_hv(std::vector<int>&hv) {
        std::random_device rd; // A random device object used to seed the random number generator.
        std::mt19937 gen(rd()); // A Mersenne Twister random number generator seeded with the random device. This is a common and efficient random number generator.

        if (is_binary) {
            std::uniform_int_distribution<> dis(0, 1); //  Defines a uniform integer distribution that generates numbers between 0 and 1
            for (int i = 0; i < HV_SIZE; ++i) {
                hv.push_back(dis(gen)); // Generates a random number (0 or 1) using the uniform distribution and the random number generator, and appends it to the vector hv
            }
        }
        else {
            std::uniform_int_distribution<> dis(0, 1); // Again defines a uniform integer distribution for generating numbers between 0 and 1
            for (int i = 0; i < HV_SIZE; ++i) {
                hv.push_back(dis(gen) * 2 - 1); //dis(gen)-->Generates a random number (0 or 1)again, However, 
                                                // multiplies it by 2 (resulting in 0 or 2), and then subtracts 1, resulting in -1 or 1. 
                                                // This effectively generates a bipolar value (-1 or 1) and appends it to the vector hv.
            }
        }
    }

    // Function to perform XOR operation for binary hypervectors
    void perform_xor_operation() {
        for (int i = 0; i < HV_SIZE; ++i) {
            output_hv[i] = item_memory[i] ^ continuous_item_memory[i];
            // 1 XOR 1 = 0
            // 1 XOR 0 = 1
        }
    }

    // Function to perform pointwise multiplication for bipolar hypervectors
    void perform_pointwise_multiplication() {
        for (int i = 0; i < HV_SIZE; ++i) {
            output_hv[i] = item_memory[i] * continuous_item_memory[i];
            // -1 * -1= 1
            // -1 * 1= -1
        }
    }

    // Init phase thread
    void init_phase() {
        while (true) {
            wait(init.posedge_event());

            // Generate position HV to encode the feature position information --> SO CALLED ID
            generate_random_hv(item_memory);

            // Create continuous item memory and quantize feature value information
            generate_random_hv(continuous_item_memory);

            // Resize output_hv to match HV_SIZE
            output_hv.resize(HV_SIZE);

            

            // !!BINDING OPERATIONS!!
            //for binary vectors
            if (is_binary) {
                perform_xor_operation();
            }
            // for bipolar vectors
            else {
                perform_pointwise_multiplication();
            }

            // Set hypervectors to ports
            for (int i = 0; i < HV_SIZE; ++i) {
                hv_port1[i].write(item_memory[i]);
                hv_port2[i].write(output_hv[i]);
            }

            // Wait for a clock cycle or a specified duration
            wait(10, SC_NS); 
        }
    }
};

int sc_main(int argc, char* argv[]) {
    // Instantiate the binding module
    binding bind("binding");

    /*A trace file:
    1. records a time-ordered sequence of value changes during simulation.
    2. uses VCD (Value change dump) file format.*/

    sc_trace_file* Tf = sc_create_vcd_trace_file("traces");
    if (!Tf) {
        std::cerr << "Could not create trace file." << std::endl;
        return 1;
    }
    sc_trace(Tf, bind.init, "init");
    sc_trace(Tf, bind.training, "training");
    sc_trace(Tf, bind.test, "test");

    // Trace hypervector ports
    for (int i = 0; i < HV_SIZE; ++i) {
        std::string hv1_name = "hv1_" + std::to_string(i);
        std::string hv2_name = "hv2_" + std::to_string(i);
        sc_trace(Tf, bind.hv_port1[i], hv1_name.c_str());
        sc_trace(Tf, bind.hv_port2[i], hv2_name.c_str());
    }

    sc_start(0, SC_NS);

    // Set the init signal to true to start the init_phase
    bind.init.write(false);
    sc_start(10, SC_NS);

    // Set the init signal to false to stop the init_phase
    bind.init.write(true);

    // Run the simulation for a sufficient time
    sc_start(60, SC_SEC);

    sc_close_vcd_trace_file(Tf);

    return 0;
}
