#include <iostream>
#include <string>
#include <sys/time.h>
#include <fstream>
#include <sstream>
#include <cstdlib>
#include <string.h>
using namespace std;

class Timer
{
    timeval timer[2];

public:

    timeval start()
    {
        gettimeofday(&this->timer[0], NULL);
        return this->timer[0];
    }

    timeval stop()
    {
        gettimeofday(&this->timer[1], NULL);
        return this->timer[1];
    }

    int duration() const
    {
        int secs(this->timer[1].tv_sec - this->timer[0].tv_sec);
        int usecs(this->timer[1].tv_usec - this->timer[0].tv_usec);

        if(usecs < 0)
        {
            --secs;
            usecs += 1000000;
        }

        return static_cast<int>(secs * 1000 + usecs / 1000.0 + 0.5);
    }
};

int main(int argc, char** argv)
{
    char* cpuData = "/sys/class/thermal/thermal_zone0/temp";
    ifstream cpuTemp(cpuData);

    if (cpuTemp.fail()){
        cerr << "Failed to get cpu info." << endl;
        return 1;
    }
    cpuTemp.close();
	
    int speed = 30000000;
    string path = strcat(getenv("HOME"), "/Benchmark.txt");
    if(argc > 2)
    {
        try {
            path = argv[1];
            speed = atoi(argv[2]);
        } catch (int e) { 
            cerr << "Error parsing args.";
            return -1;
        }
    }
    cout << "Using file " << path << "\n";

    ofstream file(path.c_str(), ios::out | ios::app);

    if(!file.is_open()){
        cout << "Opening in trunc mode...\n";
        file.open(path.c_str(), ios::in | ios::out | ios::trunc);
        if(!file.is_open()){
            cout << "Encountered an error opening the file.\n";
            return -1;
        }
    }
    
    string cpuRead;
    Timer timer;
    
    for(int i = 0;; i++){
        cpuTemp.open(cpuData);
        getline(cpuTemp, cpuRead);
        cpuTemp.close();
        timer.start();
        string str;
        for(int i = 0; i < speed; i++) {
            str += i;
        }
        timer.stop();

        stringstream w;
        w << "At " << (atoi(cpuRead.c_str()) / 1000.0) << "°C, the cpu took: " << timer.duration() << "ms\n";
        cout << w.str();
        file << w.str();

        file.flush();
    }
    file.close();

    return 0;
}
