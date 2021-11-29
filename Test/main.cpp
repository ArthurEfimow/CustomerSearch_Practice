/*
 * main.cpp
 *
 *  Created on: Jun 5, 2019
 *      Author: efimaster
 */

#include <iostream>
#include <dlfcn.h>

typedef std::string (*Send_Request)(std::string);
typedef void (*Visualize)(std::string);

int main(void)
{
    void* handle = dlopen("libSearcher.so", RTLD_LAZY); // creates dynamic linker to the shared library
    if (!handle) { // tests if the shared library could be found
        std::cout << "Could not open the library" << std::endl;
        return 1;
    }

    Send_Request get = reinterpret_cast<Send_Request>(dlsym(handle, "send_request")); // links send_request from shared object to get
    Visualize vizualize = reinterpret_cast<Visualize>(dlsym(handle, "visualize_response"));
    if (!get) { //tests if the linking worked
        std::cout << "Could not find symbol send_request" << std::endl;
        dlclose(handle);
        return 1;
    }
    if (!vizualize) {
        std::cout << "Could not find symbol visualize_response" << std::endl;
        dlclose(handle);
        return 1;
    }

    std::string Response = get("jsonplaceholder.typicode.com/posts?userId=1"); // makes the request and returns the response
    vizualize(Response); // visualizes the response
    dlclose(handle);


}
