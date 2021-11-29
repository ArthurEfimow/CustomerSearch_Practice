/*
 * Searcher.h
 *
 *  Created on: Jun 7, 2019
 *      Author: Arthur Efimow
 */

#include <stdio.h>
#include <iostream>
#include <sstream>
#include <string>
#include <memory>
#include <map>
#include <vector>
#include <curl/curl.h>
//#include "nlohmann/json.hpp"

#ifndef SEARCHER_H_
#pragma once

#ifdef  __cplusplus
extern "C" {
#endif

std::string send_request(std::string);
void visualize_response(std::string);

#ifdef  __cplusplus
}
#endif

void correct_response(std::string&);

#define SEARCHER_H_





#endif /* SEARCHER_H_ */
