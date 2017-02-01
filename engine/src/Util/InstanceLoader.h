/*Author: Derek Nola*/
#pragma once
#include <sstream>
#include <fstream>
#include <vector>
#include <string>
#include "glm/glm.hpp"
#include "Geometry/Transform.h"
#include "Logger.h"
using namespace std;
namespace MoonEngine
{
	namespace Util {
		
		inline vector<glm::mat4> InstanceLoader(string fileName) {
			
			vector<glm::mat4> result;
			glm::vec3 pos;
			glm::vec3 scl;
			glm::vec3 rot;
			std::ifstream File(fileName);
			if (File.is_open() == false) {
				LOG(ERROR, "Instance data file " + fileName + " Could not be loaded");
			}
			std::string line;
			while (std::getline(File, line))
			{
				Transform trans;
				std::istringstream iss(line);
				if (!(iss >> pos.x >> pos.y >> pos.z)) { 
					break; // error in position
				} 
				if (!(iss >> scl.x >> scl.y >> scl.z)) {
					break; // error in scale
				}
				if (!(iss >> rot.x >> rot.y >> rot.z)) {
					break; // error in rotation
				}		
				trans.setScale(scl);
				trans.setRotation(rot);
				trans.setPosition(pos);
				result.push_back(trans.getMatrix());
			}
			return   result;
		}

	}
}