#include "Application.h"

#include <iostream>

int main(int argc, char **argv) {
    int width = 1280, height = 720;
    if (argc > 1 && argc != 3) {
        std::cerr << "To specify initial window parametes use: [width] [height]\n";
        return -1;
    } else if (argc == 3) {
        width = atoi(argv[0]);
        height = atoi(argv[1]);
    }

    Application application(width, height);
    
    return application.Run();
}
