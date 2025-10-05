/**
 *
 * @file interrupts.cpp
 * @author Sasisekhar Govind
 *
 */

#include<interrupts.hpp>

int main(int argc, char** argv) {

    //vectors is a C++ std::vector of strings that contain the address of the ISR
    //delays  is a C++ std::vector of ints that contain the delays of each device
    //the index of these elemens is the device number, starting from 0
    auto [vectors, delays] = parse_args(argc, argv);
    std::ifstream input_file(argv[1]);

    std::string trace;      //!< string to store single line of trace file
    std::string execution;  //!< string to accumulate the execution output

    /******************ADD YOUR VARIABLES HERE*************************/

    int current_time = 0;
    

    /******************************************************************/

    //parse each line of the input trace file
    while(std::getline(input_file, trace)) {
        auto [activity, duration_intr] = parse_trace(trace);

        /******************ADD YOUR SIMULATION CODE HERE*************************/

        if (activity == "CPU") {
            // CPU burst
            execution += std::to_string(current_time) + ", " + std::to_string(duration_intr) + ", CPU burst\n";
            current_time += duration_intr;
        }
        else if (activity == "SYSCALL" || activity == "END_IO") {  
            int intr_num = duration_intr;

            // Boilerplate: switch to kernel, save context, find vector, load PC
            auto [intr_log, new_time] = intr_boilerplate(current_time, intr_num, 10, vectors);  //changed the 10 to 20 or 30 for tests 1 to 20
            execution += intr_log;
            current_time = new_time;

                
            int service_time = delays.at(intr_num);

            if (activity == "SYSCALL") {
                    
                //run ISR
                int step1 = 40; // made it (step1 = service_time * 0.25 ) for Table 2: Effect of ISR Delay in the report (tests 11 to 20)
                execution += std::to_string(current_time) + ", " + std::to_string(step1) + ", SYSCALL: run the ISR (device driver)\n";
                current_time += step1;

                //data transfer 
                int step2 = 40; // made it (step2 = service_time * 0.25 ) for Table 2: Effect of ISR Delay in the report (tests 11 to 20)
                execution += std::to_string(current_time) + ", " + std::to_string(step2) + ", transfer data from device to memory\n";
                current_time += step2;

                //checking for errors
                int step3 = service_time - (step1 + step2);
                execution += std::to_string(current_time) + ", " + std::to_string(step3) + ", check for errors\n";
                current_time += step3;
            }
            else if (activity == "END_IO") {
                    
                //run ISR
                int step1 = 40; // made it (step1 = service_time * 0.3 ) for Table 2: Effect of ISR Delay in the report (tests 11 to 20)
                execution += std::to_string(current_time) + ", " + std::to_string(step1) + ", END_IO: run the ISR (device driver)\n";
                current_time += step1;

                //checking device status
                int step2 = service_time - step1;
                if (step2 < 0) step2 = 0;
                execution += std::to_string(current_time) + ", " + std::to_string(step2) + ", check device status\n";
                current_time += step2;
            }

                // IRET
                execution += std::to_string(current_time) + ", 1, IRET\n";
                current_time += 1;
        }
    }


        /************************************************************************/

    

    input_file.close();

    write_output(execution);

    return 0;
}
