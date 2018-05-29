#pragma once
#include <vector>
#include <string>
#include <memory>
#include <exception>
#include <fstream>
#include <map>

#include "glm/glm.hpp"
#include "glm/gtc/matrix_transform.hpp"
#include "glm/gtc/type_ptr.hpp"
#include "glm/gtx/euler_angles.hpp"

class Joint
{
	friend void GUIUpdate();
	std::string jointName;
	std::weak_ptr<Joint> parentJoint;
	std::vector<std::shared_ptr<Joint>> childJoints;

	glm::vec3 translate;
	glm::vec3 rotate;	//yaw pitch roll
	glm::vec3 scale;

	glm::mat4 jointMatrix;
	void serializeTo(std::map<std::string, int> &currentMap,std::ofstream &fstream);

public:
	Joint(std::string name, glm::vec3 translate, glm::vec3 rotate, glm::vec3 scale = { 1,1,1 });
	~Joint();

	static void attachTo(std::shared_ptr<Joint> &child, std::shared_ptr<Joint> &parent);
	std::shared_ptr<Joint> detach();	//detach this joint from parent and return parent joint,raise exception if not currently attached
	std::shared_ptr<Joint> findJointPtr(const std::string &jointName);		//find joint of jointName in this joint tree by pointer,return null if not exist
	void caculateMatrix();		//update joint matrix
	std::shared_ptr<Joint> getParent();
	std::vector<std::shared_ptr<Joint>> getChildren();
	glm::mat4 getMatrix();
	std::string getJointName();

	void serializeTo(const char *file);
	static std::shared_ptr<Joint> serializeFrom(const char *str);
	static std::shared_ptr<Joint> serializeFrom(std::ifstream & ifs);


};
