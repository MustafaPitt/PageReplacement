
/*
 * Author: Mustafa Al Azzawi
 * Project 3
 * Page replacement Simulator
 * CS-1550
 *
 * example how to run the program
 *./vmsim 8 fifo gcc.trace
 *./vmsim 8 nru 16 gcc.trace
 *
 *
 * */


#include <iostream>
#include <string>
#include <string.h>
#include <stdio.h>
#include <fstream>
#include <sstream>
#include <queue>
#define PAGE_SIZE 4096
using namespace std;



class Opt {

public:

    struct frame {
        int addr;
        char dirty;
        int dist; // how to recall again
    };

    // public variable
    int num_of_frame = 0;
    int frame_used = 0; // num of frames that used
    int pf = 0; // page fault
    int accss = 0; // accessed memory
    int write_disk = 0;

    frame *ptr_frame_arry;
    int cur_indx = 0;

private:
    int collected_data[1000000];
    int data_ndx = 0;
    // constructor
public:
    explicit Opt(int frame_num) { // construct a class with frame input
        num_of_frame = frame_num; //
        ptr_frame_arry = new frame[frame_num];
    }


    void insert_data(int add) {
        collected_data[data_ndx] = add;
        data_ndx++;
    }

    int calc_dist(int add, int accs, const int data[]) { // check how far the address to be found again in the index
        int j = accs-1;
        for (; j < 1000000; ++j) {
            if (data[j] == add) return j - accs+1;
        }
        // else if not avaialble return a far distance so it will evict asap
        return 1000000 + accs;

    }
    int find_bigest_dist(struct frame *ptr_fram_arry) {
        int bigest = ptr_fram_arry[0].dist;
        int indx = 0;
        for (int i = 1; i < num_of_frame; i++) {
            if (bigest < ptr_fram_arry[i].dist) {
                bigest = ptr_fram_arry[i].dist;
                indx = i;
            }
        }
        return indx;
    }

    void print_result() {
        cout << "Algorithm: OPT \n";
        cout << "Number of frames:           " << num_of_frame << '\n';
        cout << "Total memory accesses:  " << accss << '\n';
        cout << "Total page faults:      " << pf << '\n';
        cout << "Total writes to disk:     " << write_disk << '\n';
    }

    void run_opt(int add, char wr, struct frame *ptr_fram_arry) {
        static bool frame_table_flag = true;
        accss++; // memory accessed
        frame newFrame; // create temp new frame
        newFrame.addr = add;
        newFrame.dirty = wr == 'W' ? 1 : 0;
        bool full = true;

        if (frame_used == 0) {
            ptr_fram_arry[frame_used] = newFrame;  // insert a new frame to the frame array
            frame_used++; // increment the used frame
            pf++; // page fault occurred
            return;
        }
            // if the number frame array not full yet
        else if (frame_used < num_of_frame) { // if there is available frames
            for (int i = 0; i < frame_used; ++i) {// search to its already there
                if (ptr_fram_arry[i].addr == add) { // if found
                    if (newFrame.dirty == 1) {
                        ptr_fram_arry[i].dirty = 1;
                    }
                    return; // no page fault , no need to add new memory to the array
                }
            }
            // if not already in frames array
            ptr_fram_arry[frame_used] = newFrame; // add the new frame the array
            frame_used++; // frame has been used
            pf++; // page fault
            return; //
        }
            // if frame array full
        else if (frame_used == num_of_frame) {
            if (frame_table_flag) {
                frame_table_flag = false;
                for (int i=0; i<frame_used; i++) {
                    ptr_fram_arry[i].dist = calc_dist(ptr_fram_arry[i].addr, accss+1, collected_data)+1;
                }
            }
            int largest_dist = 0;
            int largest_dist_idx = 0;
            bool evict = true;
            for (int i = 0; i < frame_used; ++i) {// search if the page already there
                ptr_fram_arry[i].dist--;

                if (ptr_fram_arry[i].dist > largest_dist) {
                    largest_dist = ptr_fram_arry[i].dist;
                    largest_dist_idx = i;
                }
                if (ptr_fram_arry[i].addr == add) { // if found
                    ptr_fram_arry[i].dist = calc_dist(add, accss+1, collected_data)+1;
                    evict = false;
                }
            }
            if (evict) {
                if (ptr_fram_arry[largest_dist_idx].dirty == 1) {
                    write_disk++;
                }
                ptr_fram_arry[largest_dist_idx] = newFrame;
                ptr_fram_arry[largest_dist_idx].dist = calc_dist(newFrame.addr, accss+1, collected_data)+1;
                pf++;
            }
        }
    }

    };


    class Fifo {
    public:
        struct frame {
            int addr;
            char dirty;
        };

        int num_of_frame = 0;
        int frame_used = 0; // num of frames that used
        int pf = 0; // page fault
        int accss = 0; // accessed memory
        int write_disk = 0;
        frame *ptr_fram_arry;
        int cur_old = 0;

        // constructor
        explicit Fifo(int frame_num) { // construct a class with frame input
            num_of_frame = frame_num; // number of avialable frames
            ptr_fram_arry = new frame[frame_num];

        }

        void run_fifo_q(int add, char wr) {
            accss++; // memory accessed
            frame newFrame; // create temp new frame
            newFrame.addr = add;
            newFrame.dirty = (wr == 'W') ? 1 : 0;

            // if empty no frame used yet
            if (frame_used == 0) {
                ptr_fram_arry[frame_used] = newFrame;  // insert first frame
                frame_used++; // increment the used frame
                pf++; // page fault occurred
                return;
            } else if (frame_used < num_of_frame) { // if there is available frames
                for (int i = 0; i < frame_used; ++i) {// search if its already there
                    if (ptr_fram_arry[i].addr == add) { // if found
                        if (newFrame.dirty == 1) {
                            ptr_fram_arry[i].dirty = 1;
                        }
                        return; // no page fault , no need to add new memory to the array
                    }
                }
                // if not already in frames array
                ptr_fram_arry[frame_used] = newFrame; // add the new frame the array
                frame_used++; // frame has been used
                pf++; // page fault
                return;
            }
                // if frame array full
            else if (frame_used == num_of_frame) {
                for (int i = 0; i < frame_used; ++i) {// search if the page already there
                    if (ptr_fram_arry[i].addr == add) { // if found
                        if (newFrame.dirty == 1) {
                            ptr_fram_arry[i].dirty = 1;
                        }
                        return; // no page fault , no need to add new frame to the array
                    }
                }
                // if the page is not in frame we need to evict the old frame
                if (ptr_fram_arry[cur_old].dirty == 1) {
                    write_disk++;
                }
                ptr_fram_arry[cur_old] = newFrame; // evict the old frame
                pf++; // page fault occurs
                cur_old = ++cur_old % num_of_frame; // move the current old pointer to the new old and keep circulate
            }
        }
        void print_result() {
            cout << "Algorithm: FIFO \n";
            cout << "Number of frames:           " << num_of_frame << '\n';
            cout << "Total memory accesses:  " << accss << '\n';
            cout << "Total page faults:      " << pf << '\n';
            cout << "Total writes to disk:     " << write_disk << '\n';
        }
    };

    class Clock {
    public:
        struct frame {
            int addr;
            char dirty= 0;
            char ref= 1;
        };

        int num_of_frame = 0;
        int frame_used = 0; // num of frames that used
        int pf = 0; // page fault
        int accss = 0; // accessed memory
        int write_disk = 0;
        frame *ptr_fram_arry;
        int cur_old = 0;

        // constructor
        explicit Clock(int frame_num) { // construct a class with frame input
            num_of_frame = frame_num; // number of avialable frames
            ptr_fram_arry = new frame[frame_num];
        }

        void print_result() {
            cout << "Algorithm: Clock \n";
            cout << "Number of frames:           " << num_of_frame << '\n';
            cout << "Total memory accesses:  " << accss << '\n';
            cout << "Total page faults:      " << pf << '\n';
            cout << "Total writes to disk:     " << write_disk << '\n';
        }
        void run_clock(int add, char wr) {
            accss++; // memory accessed
            frame newFrame; // create temp new frame
            newFrame.addr = add;
            newFrame.dirty = (wr == 'W') ? 1 : 0;

            // if empty no frame used yet
            if (frame_used == 0) {
                ptr_fram_arry[frame_used] = newFrame;  // the the frame info to frames array or blocks
                frame_used++; // increment the used frame
                pf++; // page fault occurred
                cur_old = ++cur_old % num_of_frame; // dbg-1
                return;
            } else if (frame_used < num_of_frame) { // if there is available frames
                for (int i = 0; i < frame_used; ++i) {// search if its already there
                    if (ptr_fram_arry[i].addr == add) { // if found
                        if (newFrame.dirty == 1) {
                            ptr_fram_arry[i].dirty = 1;
                        }
                        ptr_fram_arry[i].ref =1;
                        return; // no page fault , no need to add new memory to the array
                    }
                }
                // if not already in frames array
                ptr_fram_arry[frame_used] = newFrame; // add the new frame the array
                cur_old = ++cur_old % num_of_frame; // dbg-1
                frame_used++; // frame has been used
                pf++; // psage fault

                return; //
            }
                // if frame array full
            else if (frame_used == num_of_frame) { // check if it hit
                for (int i = 0; i < frame_used; ++i) {// search if the page already there
                    if (ptr_fram_arry[i].addr == add) { // if found
                        if (newFrame.dirty == 1) {
                            ptr_fram_arry[i].dirty = 1;
                        }
                        ptr_fram_arry[i].ref=1;
                        return; // no page fault , no need to add new frame to the array
                    }
                }
                // if the page is not in frame we need to evict the old frame and unrefrenced
                for (int j = 0; j <num_of_frame ; ++j) {
                    if(ptr_fram_arry[cur_old].ref == 0){
                        if (ptr_fram_arry[cur_old].dirty == 1) write_disk++;
                        ptr_fram_arry[cur_old] = newFrame; // evict the old frame
                        pf++; // page fault occurs
                        cur_old = ++cur_old % num_of_frame; // move the current old pointer to the new old and keep circulate
                     return;
                    }
                    if (ptr_fram_arry[cur_old].ref == 1) {
                        ptr_fram_arry[cur_old].ref = 0;
                        cur_old = ++cur_old % num_of_frame; // move the current old pointer to the new old and keep circulate
                    }
                }
                if(ptr_fram_arry[cur_old].dirty == 1)write_disk++;
                pf++;
                ptr_fram_arry[cur_old] = newFrame; // evict frame
                cur_old = ++cur_old % num_of_frame;
            }
        }

    };

    class Nru {
    public:
        struct frame {
            int addr;
            char dirty= 0;
            char refrence= 1;
        };

        int num_of_frame = 0;
        int frame_used = 0; // num of frames that used
        int pf = 0; // page fault
        int accss = 0; // accessed memory
        int write_disk = 0;
        frame *ptr_fram_arry;
        int cur_old = 0;
        int  refresh= 0;

        // constructor
        explicit Nru(int frame_num, int refrsh) { // construct a class with frame input
            num_of_frame = frame_num; // number of avialable frames
            ptr_fram_arry = new frame[frame_num];
            refresh = refrsh;
        }

        void print_result() {
            cout << " Algorithm:NRU \n";
            cout << "Number of frames:           " << num_of_frame << '\n';
            cout << "Total memory accesses:  " << accss << '\n';
            cout << "Total page faults:      " << pf << '\n';
            cout << "Total writes to disk:     " << write_disk << '\n';
            cout << "Refresh = "<< refresh << "\n";
        }


        void run_nru(int add, char wr) {
            accss++; // memory accessed
            frame newFrame; // create temp new frame
            newFrame.addr = add;
            newFrame.dirty = (wr == 'W') ? 1 : 0;

            // if empty no frame used yet
            if (frame_used == 0) {
                ptr_fram_arry[frame_used] = newFrame;  // the the frame info to frames array or blocks
                frame_used++; // increment the used frame
                pf++; // page fault occurred
                cur_old = ++cur_old % num_of_frame;
                return;
            } else if (frame_used < num_of_frame) { // if there is available frames
                for (int i = 0; i < frame_used; ++i) {// search if its already there
                    if (ptr_fram_arry[i].addr == add) { // if found
                        if (newFrame.dirty == 1) {
                            ptr_fram_arry[i].dirty = 1;
                        }
                        ptr_fram_arry[i].refrence =1;
                        return; // no page fault , no need to add new memory to the array
                    }
                }
                // if not already in frames array
                ptr_fram_arry[frame_used] = newFrame; // add the new frame to the array
                cur_old = ++cur_old % num_of_frame;
                frame_used++; // frame has been used
                pf++; // psage fault
                return; //
            }
                // if frame array full
            else if (frame_used == num_of_frame) { // check if it hit
                if(accss% refresh == 0){ // if refresh time is true
                    for (int i = 0; i <num_of_frame ; ++i) {
                        ptr_fram_arry[i].refrence =0; // set all ref bits to zero
                    }
                }
                for (int i = 0; i < frame_used; ++i) {// search if the page already there
                    if (ptr_fram_arry[i].addr == add) { // if found
                        if (newFrame.dirty == 1) {
                            ptr_fram_arry[i].dirty = 1;
                        }
                        ptr_fram_arry[i].refrence=1;
                        return; // no page fault , no new frame added
                    }
                }
                // if the page is not in frames array
                for (int j = 0; j <num_of_frame ; ++j) {
                    if(ptr_fram_arry[cur_old].refrence == 0 && ptr_fram_arry[cur_old].dirty ==0 ){ // First Choice (0,0) if it unrefrenced and clean
                        ptr_fram_arry[cur_old] = newFrame; // evict the old frame
                        pf++; // page fault occurs
                        cur_old = ++cur_old % num_of_frame; // move the current old pointer to the new old and keep circulate
                        return;
                    }
                    cur_old = ++cur_old % num_of_frame; // move the current old pointer to the new old and keep circulate
                }
                for (int j = 0; j <num_of_frame ; ++j) {
                    if(ptr_fram_arry[cur_old].refrence == 0 && ptr_fram_arry[cur_old].dirty ==1 ){ // second Choice (0,1) if it unrefrenced and dirty
                        ptr_fram_arry[cur_old] = newFrame; // evict the old frame
                        pf++; // page fault occurs
                        write_disk++;
                        cur_old = ++cur_old % num_of_frame; // move the current old pointer to the new old and keep circulate
                        return;
                    }
                    cur_old = ++cur_old % num_of_frame; // move the current old pointer to the new old and keep circulate
                }
                for (int j = 0; j <num_of_frame ; ++j) {
                    if(ptr_fram_arry[cur_old].refrence == 1 && ptr_fram_arry[cur_old].dirty ==0 ){ // third choice (1,0) if it refrenced and clean
                        ptr_fram_arry[cur_old] = newFrame; // evict the old frame
                        pf++; // page fault occurs
                        cur_old = ++cur_old % num_of_frame; // move the current old pointer to the new old and keep circulate
                        return;
                    }
                    cur_old = ++cur_old % num_of_frame; // move the current old pointer to the new old and keep circulate
                }
                //last choice  evict anyframe
                if(ptr_fram_arry[cur_old].dirty == 1)write_disk++;
                pf++;
                ptr_fram_arry[cur_old] = newFrame; //evict frame
                cur_old = ++cur_old % num_of_frame;
            }
        }
    };


    int main(int argc, char **argv) {
        // for fifo selection
        if (strcmp(argv[2],"fifo") == 0) {  // open trace file
            char algo_selc[30];
            ifstream myfile;
            myfile.open(argv[3], std::ifstream::in); // return er
            if(!myfile.is_open()) {
                cout << "EROR file name Fifo\n";
                exit(1);
            }
            string str;
            char char_array[str.length()];

            if(atoi(argv[1]) < 1) {
                cout << "Error Frame Number FIFO";
                exit(1);
            }
            Fifo myfifo(atoi(argv[1]));

            while (getline(myfile, str)) {
                //   cout << str << '\n';
                int space_idx = str.find(' ');
                int mem_loc = strtol(str.c_str(), NULL, 16);
                char rw = str[space_idx + 1];
                // printf("%x", mem_loc);
                //   cout << ' ' << rw << '\n';
                myfifo.run_fifo_q(mem_loc / PAGE_SIZE, rw);
            }
            myfifo.print_result();
        }


        if (strcmp(argv[2],"opt") == 0) { // run opt
            char algo_selc[30];
            ifstream myfile;
            myfile.open(argv[3], std::ifstream::in);
            if(!myfile.is_open()){
                cout<<"Error file name | OPT \n";
                exit(1);
            }
            string str;
            char char_array[str.length()];

            if(atoi(argv[1]) < 1) {
                cout << "Error Frame Number: OPT \n";
                exit(1);
            }

            Opt myOpt(atoi(argv[1]));
            while (getline(myfile, str)) {
                //   cout << str << '\n';
                int space_idx = str.find(' ');
                int mem_loc = strtol(str.c_str(), NULL, 16);
                myOpt.insert_data(mem_loc / PAGE_SIZE); // first  collect data
            }

            myfile.clear();
            myfile.seekg(0, ios::beg);
            while (getline(myfile, str)) { // second simulate
                //   cout << str << '\n';
                int space_idx = str.find(' ');
                int mem_loc = strtol(str.c_str(), NULL, 16);
                char rw = str[space_idx + 1];
                myOpt.run_opt(mem_loc / PAGE_SIZE, rw, myOpt.ptr_frame_arry);
            }
            myOpt.print_result();
            myfile.clear();
            myfile.seekg(0, ios::beg);
        }
        if (strcmp(argv[2],"clock") == 0) {
            char algo_selc[30];
            ifstream myfile;
            myfile.open(argv[3], std::ifstream::in);
            if(!myfile.is_open()){
                cout << "Error File name  clock\n";
                exit(1);
            }
            string str;
            char char_array[str.length()];
            if(atoi(argv[1]) < 1) {
                cout << "Error Frame Number: Clock \n";
                exit(1);
            }
            Clock myclock(atoi(argv[1]));
            while (getline(myfile, str)) { // second simulate
                //   cout << str << '\n';
                int space_idx = str.find(' ');
                int mem_loc = strtol(str.c_str(), NULL, 16);
                char rw = str[space_idx + 1];
                myclock.run_clock(mem_loc /PAGE_SIZE, rw);
                //cout<<"loading" <<'\n';
            }
            myclock.print_result();
        }
        if (strcmp(argv[2],"nru") == 0) {
            char algo_selc[30];
            ifstream myfile;
            myfile.open(argv[4], std::ifstream::in);
            if(!myfile.is_open()){
                cout << "Error File name | NRU\n";
            }
            string str;
            char char_array[str.length()];
            if(atoi(argv[1]) < 1) {
                cout << "Error Frame Number NRU";
                exit(1);
            }
            if(atoi(argv[3]) < 1) {
                cout << "Error Refresh Number NRU";
                exit(1);
            }
            Nru myNru(atoi(argv[1]),atoi(argv[3]));
            while (getline(myfile, str)) { // second simulate
                //   cout << str << '\n';
                int space_idx = str.find(' ');
                int mem_loc = strtol(str.c_str(), NULL, 16);
                char rw = str[space_idx + 1];
                myNru.run_nru(mem_loc / PAGE_SIZE, rw);
            }
            myNru.print_result();
        }
        cout<<argv[1];
        // =====================================    DBG ===============================
        if (strcmp(argv[1],"test") == 0) {
            // test algo delete late

            int address[] = {7, 0, 1, 2, 0, 3, 0, 4, 2, 3, 0, 3, 2, 1, 2, 0, 1, 7, 0, 1};
            //int address [] = {2,3,2,1,5,2,4,5,3,2,5,2};
            char wr_ary[] = {'W','R','W','R','W','R','W','R','W','R','W','R','W','R','W','R','W','R','W','R','W','R','W','R','W','R','W','R','W','R','W','R','W','R','W','R','W','R','W','R'};
            int len = sizeof(address) / sizeof(int);
            Clock myClock(100);
            // clock

            for (int i = 0; i < len  ; ++i) {
                //int add, char wr, struct frame *ptr_fram_arry
                myClock.run_clock(address[i],wr_ary[i]);
            }
            myClock.print_result();


            Fifo myFifo(3);
            for (int i = 0; i < len  ; ++i) {
                //int add, char wr, struct frame *ptr_fram_arry
                myFifo.run_fifo_q(address[i],wr_ary[i]);
            }
            myFifo.print_result();
            //        // test opt
            Opt testOpt(3); // collecting data
            for (int i = 0; i <len ; ++i) {
                testOpt.insert_data(address[i]);
            }
            // run opt
            for (int i = 0; i <len ; ++i) {
                testOpt.run_opt(address[i],wr_ary[i],testOpt.ptr_frame_arry);
            }
            testOpt.print_result();

            Nru testNru(3,2);
            for (int i=0; i<len; i++) {
                testNru.run_nru(address[i],wr_ary[i]);
            }
            testNru.print_result();

            return 0;
        }
    }

