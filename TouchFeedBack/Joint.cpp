#include "Joint.h"
#include <sstream>
using namespace std;
using namespace glm;

Joint::Joint(std::string name, glm::vec3 translate, glm::vec3 rotate, glm::vec3 scale):
	jointName(name),translate(translate),rotate(rotate),scale(scale)
{
}


Joint::~Joint()
{
// 	jointName = "deleted!";
	printf("Joint %s destroy!\n", jointName.c_str());
}

void Joint::attachTo(std::shared_ptr<Joint> &child, std::shared_ptr<Joint> &parent)
{
	child->parentJoint = parent;
	for (auto& childJoint: parent->childJoints)
	{
		if (childJoint->jointName== child->jointName)
		{
			return;
		}
	}
	parent->childJoints.push_back(child);
}

shared_ptr<Joint> Joint::detach()
{
	if (parentJoint.expired())
	{
		throw exception("currently not attached!");
	}
	shared_ptr<Joint> parentRef = parentJoint.lock();
	parentJoint.reset();
	for (auto iter = parentRef->childJoints.begin(); iter != parentRef->childJoints.end();++iter)
	{
		if ((*iter)->jointName==jointName)
		{
			parentRef->childJoints.erase(iter);
			break;
		}
	}
	return parentRef;
}

std::shared_ptr<Joint> Joint::findJointPtr(const std::string &jointName)
{
	for (auto &childJoint:childJoints)
	{
		if (childJoint->jointName== jointName)
		{
			return childJoint;
		}
		else
		{
			auto desendent = childJoint->findJointPtr(jointName);
			if (desendent)
			{
				return desendent;
			}
		}
	}
	return nullptr;
}

void Joint::caculateMatrix()
{
// 	glm::vec3 rotate;	//yaw pitch roll
	mat4 rotateMat = 
		glm::rotate(mat4(), radians(rotate.x), {0,0,1})*
		glm::rotate(mat4(), radians(rotate.y), { 1,0,0 })*
		glm::rotate(mat4(), radians(rotate.z), { 0,1,0 })
		;
	
	mat4 parentMat;
	if (!parentJoint.expired())
	{
		parentMat = parentJoint.lock()->getMatrix();
	}

	jointMatrix = parentMat*glm::translate(mat4(), translate)*rotateMat*glm::scale(mat4(), scale);

	for (auto &child:childJoints)
	{
		child->caculateMatrix();
	}
}

std::shared_ptr<Joint> Joint::getParent()
{
	return parentJoint.lock();
}

std::vector<std::shared_ptr<Joint>> Joint::getChildren()
{
	return childJoints;
}

glm::mat4 Joint::getMatrix()
{
	return jointMatrix;
}

std::string Joint::getJointName()
{
	return jointName;
}

void Joint::serializeTo(const char *file)
{
	map<string,int> jointNameIndexMap;
	ofstream ofs(file);

	serializeTo(jointNameIndexMap, ofs);
	ofs.close();
}

void Joint::serializeTo(std::map<string, int> &currentMap, std::ofstream &fstream)
{
	fstream
		<< jointName << " "
		<< (parentJoint.lock() ? currentMap[parentJoint.lock()->jointName] : -1) << " "
		<< translate.x << " " << translate.y << " " << translate.z << " "
		<< rotate.x << " " << rotate.y << " " << rotate.z << " "
		<< scale.x << " " << scale.y << " " << scale.z << endl;
	currentMap[jointName] = currentMap.size();

	for (auto& child:childJoints)
	{
		child->serializeTo(currentMap, fstream);
	}
}

std::shared_ptr<Joint> Joint::serializeFrom(const char *str)
{
	vector<shared_ptr<Joint>> joints;
	istringstream iss(str);
	if (iss)
	{
		string line;
		while (getline(iss,line))
		{
			istringstream iss(line);
			string name;
			int parentIdnex;
			vec3 translate, rotate, scale;

			iss >> name >> parentIdnex 
				>> translate.x >> translate.y >> translate.z
				>> rotate.x >> rotate.y >> rotate.z
				>> scale.x >> scale.y >> scale.z;
			
			auto joint = make_shared<Joint>(name, translate, rotate, scale);

			if (parentIdnex>=0)
			{
				attachTo(joint, joints[parentIdnex]);
			}
			joints.push_back(joint);
		}
	}
	return joints.empty() ? nullptr : joints[0];
}

std::shared_ptr<Joint> Joint::serializeFrom(std::ifstream & ifs)
{
	vector<shared_ptr<Joint>> joints;
	if (ifs)
	{
		string line;
		while (getline(ifs, line))
		{
			istringstream iss(line);
			string name;
			int parentIdnex;
			vec3 translate, rotate, scale;

			iss >> name >> parentIdnex
				>> translate.x >> translate.y >> translate.z
				>> rotate.x >> rotate.y >> rotate.z
				>> scale.x >> scale.y >> scale.z;

			auto joint = make_shared<Joint>(name, translate, rotate, scale);

			if (parentIdnex >= 0)
			{
				attachTo(joint, joints[parentIdnex]);
			}
			joints.push_back(joint);
		}
	}
	return joints.empty() ? nullptr : joints[0];
}
