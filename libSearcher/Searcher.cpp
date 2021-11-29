/*
 * Searcher.cpp
 *
 *  Created on: Jun 7, 2019
 *      Author: Arthur Efimow
 */

#include "Searcher.h"

static size_t create_response(void* contents, size_t size, size_t size2, void* func)
{
    ((std::string*)func)->append((char*)contents, size * size2);
    return size * size2;
}


std::string send_request(std::string Request) {
	  CURL *curl;
	  CURLcode res;
	  std::string Response = "";
	  curl = curl_easy_init();
	  if(curl) {
	    curl_easy_setopt(curl, CURLOPT_URL,Request.c_str());
	    curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION,  create_response);
	    curl_easy_setopt(curl, CURLOPT_WRITEDATA, &Response);
	    curl_easy_setopt(curl, CURLOPT_FOLLOWLOCATION, 1L);
	    res = curl_easy_perform(curl); // perform the request
	    if(res != CURLE_OK){
	      std::cout << "curl_easy_perform() failed: " << res << std::endl;
	      Response = "[]";
	    }
	    curl_easy_cleanup(curl);
	  }
	  correct_response(Response);
	  return Response;
}

void visualize_response(std::string Response){
	std::cout << Response << std::endl;
}

void correct_response(std::string& Response){
	  if(Response.at(0) == '[')
		  return;
	  std::stringstream s;
	  s << "[" << Response << "]";
	  Response = s.str();
}
