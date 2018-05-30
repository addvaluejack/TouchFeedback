#define GLEW_STATIC
#include "gl/glew.h"
#include "glfw/glfw3.h"

#include "Camera.h"
#include "Shader.h"

#include "TouchForce.h"

#include<vector>
#include<map>

#include <imgui/imgui.h>
#include <imgui/imgui_impl_glfw_gl3.h>

#include <cstdlib>
#include <cstdio>
#include <iostream>
#include <vector>

#include "Mesh.h"
#include "joint.h"

#pragma comment(lib,"opengl32.lib")
#pragma comment(lib,"glew32s.lib")
#pragma comment(lib,"glfw3.lib")

using namespace glm;
using namespace std;

GLFWwindow *window = NULL;

bool keyPressState[1024] = { 0 };

Camera camera;

GLuint renderFBO = 0;
GLuint depthTexture = 0;
GLuint colorTexture = 0;

int windowWidth = 0;
int windowHeight = 0;

GLuint quadVAO=0;

map<string, Mesh> models;
string modelNames[] = {
    "palm",
    "thumb1",
    "thumb2",
    "index1",
    "index2",
    "index3",
    "middle1",
    "middle2",
    "middle3",
    "ring1",
    "ring2",
    "ring3",
    "little1",
    "little2",
    "little3",
};
Mesh plane;

std::shared_ptr<Joint> handPoseJoints;

std::shared_ptr<Joint> selectedJoint;

void resizeCallBack(GLFWwindow *window, int w, int h);

mat4 projection;
mat4 view;
mat4 model;

Shader quadProgram;
Shader regularProgram;

bool useOriginal = true;
TouchForce touchForce;

void attachFrameBufferComponent(GLuint FBO,std::vector<GLuint> &colorTextures,GLuint DepthTexture)
{
	glBindFramebuffer(GL_FRAMEBUFFER, FBO);
	std::vector<GLuint> drawBuffers;
	for (int i = 0; i < colorTextures.size();++i)
	{
		glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0+i, GL_TEXTURE_2D, colorTextures[i], 0);
		drawBuffers.push_back(GL_COLOR_ATTACHMENT0 + i);
	}
	if (DepthTexture)
	{
		glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, DepthTexture, 0);
	}

	if (drawBuffers.size())
	{
		glDrawBuffers(drawBuffers.size(), &drawBuffers[0]);
	}
	else
	{
		glDrawBuffer(GL_NONE);
		glReadBuffer(GL_NONE);
	}

	if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
	{
		printf("frame buffer error!\n");
	}
}

void loadModels()
{
	for (auto &name : modelNames)
	{
		models[name].loadObjFile(("models/" + name + ".obj").c_str());
        models[name].loadTexture(("texture/" + name + ".jpg").c_str());
	}
}

void initOpenGLResource()
{
	quadProgram.LoadShadersFromFile("shaders/quad.vert", "shaders/quad.frag");
	regularProgram.LoadShadersFromFile("shaders/regular.vert", "shaders/regular.frag");
	//set clear color
// 	glClearColor(0, 0, 0, 1);
	glClearColor(0.2, 0.2, 0.2, 1);
	glClearDepth(1);

	glEnable(GL_DEPTH_TEST);

	glGenFramebuffers(1, &renderFBO);
	glGenTextures(1, &depthTexture);
	glBindTexture(GL_TEXTURE_2D, depthTexture);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT,
		100, 100, 0, GL_DEPTH_COMPONENT, GL_FLOAT, NULL);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);

	glGenTextures(1, &colorTexture);
	glBindTexture(GL_TEXTURE_2D, colorTexture);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA,
		100, 100, 0, GL_RGBA, GL_UNSIGNED_BYTE, NULL);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);

	glBindFramebuffer(GL_FRAMEBUFFER, renderFBO);

	attachFrameBufferComponent(renderFBO, vector<GLuint>{ colorTexture }, depthTexture);

	glBindFramebuffer(GL_FRAMEBUFFER, 0);

	float quadVertice[] = {
		-1,-1,
		-1,1,
		1,1,
		1,-1,
	};

	glGenVertexArrays(1, &quadVAO);
	glBindVertexArray(quadVAO);
	GLuint temp;
	glGenBuffers(1, &temp);
	glBindBuffer(GL_ARRAY_BUFFER, temp);
	glBufferData(GL_ARRAY_BUFFER, sizeof(quadVertice), quadVertice, GL_STATIC_DRAW);
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 0, 0);
	glBindVertexArray(0);
	glBindBuffer(GL_ARRAY_BUFFER, 0);

	plane.loadObjFile("models/panel.obj");
    plane.loadTexture("texture/panel.jpg");

	loadModels();
	const char * initPose = "root -1 0 0 0 0 0 0 1 1 1\n\
palm 0 0 0 0 0 0 0 1 1 1\n\
thumb1 1 2.12 0 0 -29.1 0 0 1.5 1.5 1.5\n\
thumb2 2 0 1 0 0 0 0 1 1 1\n\
index1 1 1.6 1.65 0 0 0 0 1.5 1.5 1.5\n\
index2 4 0 1.09 0 0 0 0 1 1 1\n\
index3 5 0 0.8 0 0 0 0 1 1 1\n\
middle1 1 0.55 2.03 0 0 0 0 1.5 1.5 1.5\n\
middle2 7 0 1.17 0 0 0 0 1 1 1\n\
middle3 8 0 0.9 0 0 0 0 1 1 1\n\
ring1 1 -0.5 1.98 0 0 0 0 1.5 1.5 1.5\n\
ring2 10 0 1.1 0 0 0 0 1 1 1\n\
ring3 11 0 0.82 0 0 0 0 1 1 1\n\
little1 1 -1.57 1.6 0 0 0 0 1.4 1.4 1.4\n\
little2 13 0 0.93 0 0 0 0 1 1 1\n\
little3 14 0 0.68 0 0 0 0 1 1 1"
;
	handPoseJoints = Joint::serializeFrom(initPose);

	touchForce.loadResource();

	model=scale(mat4(), { 50,50,50 });


	resizeCallBack(NULL, 800, 600);	//hack
}

void resizeCallBack(GLFWwindow *window, int w, int h)
{
	if (!w||!h)
	{
		return;
	}
	if (w!=windowWidth||h!=windowHeight)
	{
		glBindTexture(GL_TEXTURE_2D, depthTexture);
		glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT,
			w, h, 0, GL_DEPTH_COMPONENT, GL_FLOAT, NULL);
		glBindTexture(GL_TEXTURE_2D, colorTexture);
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA,
			w, h, 0, GL_RGBA, GL_UNSIGNED_BYTE, NULL);
	}
	windowWidth = w;
	windowHeight = h;
	projection = perspective(radians(60.0f), float(w) / h, 0.1f, 100000.0f);
}

void keyFunCallBack(GLFWwindow*window, int key, int scancode, int action, int mod)
{
	// Forward event to GUI
	ImGui_ImplGlfwGL3_KeyCallback(window, key, scancode, action, mod);
	if (ImGui::GetIO().WantCaptureKeyboard) { return; }  // Skip other handling

	if (key<0||key>=1024)
	{
		return;
	}

	if (action==GLFW_PRESS)
	{
		keyPressState[key] = true;
	}
	else if(action == GLFW_RELEASE)
	{
		keyPressState[key] = false;
	}
}

shared_ptr<Joint> getPickedJoint(vec2 &cursorPos, mat4 &mvp, shared_ptr<Joint> &joint)
{
	vec4 pos = mvp*joint->getMatrix()*vec4 { 0, 0, 0, 1 };
	pos /= pos.w;
	float len = length(cursorPos - vec2{ pos.x, pos.y });
	if (len<0.05)
	{
		return joint;
	}
	for (auto &child:joint->getChildren())
	{
		auto picked=getPickedJoint(cursorPos, mvp, child);
		if (picked)
		{
			return picked;
		}
	}
	return nullptr;
}

void mouseButtonCallback(GLFWwindow* window, int button, int action, int mods)
{
	// Forward event to GUI
	ImGui_ImplGlfwGL3_MouseButtonCallback(window, button, action, mods);
	if (ImGui::GetIO().WantCaptureMouse) { return; }  // Skip other handling

	double x, y;
	glfwGetCursorPos(window, &x, &y);

	if (button==GLFW_MOUSE_BUTTON_RIGHT)
	{
		action == GLFW_PRESS ?
			camera.startTrackMouse(x, y) :
			camera.stopTrackMouse();
	}
	else if (button == GLFW_MOUSE_BUTTON_LEFT)
	{
		int w, h;
		glfwGetWindowSize(window, &w, &h);
		auto MVP = projection*camera.getCameraViewMatrix()*model;
		handPoseJoints->caculateMatrix();
		selectedJoint = getPickedJoint(vec2{ x / w,1 - y / h }*2.0f - vec2{1, 1}, MVP, handPoseJoints);

	}
}

void cursorPosCallback(GLFWwindow* window, double x, double y)
{
	if (ImGui::GetIO().WantCaptureMouse) { return; }  // Skip other handling
	camera.mouseMove(x, y);
}

void charCallback(GLFWwindow* window, unsigned int codepoint)
{
	// Forward event to GUI
	ImGui_ImplGlfwGL3_CharCallback(window, codepoint);
	if (ImGui::GetIO().WantTextInput) { return; }  // Skip other handling
}

void serializeJointMatrix(vector<mat4> &destVec, shared_ptr<Joint> &joint)
{
	destVec.push_back(joint->getMatrix());
	for (auto &child:joint->getChildren())
	{
		serializeJointMatrix(destVec, child);
	}
}


void update()
{
	camera.update();
	handPoseJoints->caculateMatrix();
	useOriginal ?
		touchForce.caculateForceOriginal(models["index3"], handPoseJoints->findJointPtr("index3")->getMatrix())
		: touchForce.caculateForceEnhanced(models["index3"], handPoseJoints->findJointPtr("index3")->getMatrix());

}

void render()
{
	attachFrameBufferComponent(renderFBO, std::vector<GLuint>{colorTexture}, depthTexture);
	glBindFramebuffer(GL_FRAMEBUFFER, renderFBO);

	glViewport(0, 0, windowWidth, windowHeight);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	regularProgram.use();
	regularProgram.setUniform("projection", projection);
	regularProgram.setUniform("view", camera.getCameraViewMatrix()*model);

	//draw plane
	regularProgram.setUniform("model", mat4());
	regularProgram.setUniform("isPlane", 1);
	plane.drawMesh();

	regularProgram.setUniform("isPlane", 0);

	for (auto kv:models)
	{
		auto joint=handPoseJoints->findJointPtr(kv.first);
		regularProgram.setUniform("model", joint->getMatrix());
		kv.second.drawMesh();
	}

	regularProgram.done();

	touchForce.drawForce(projection* camera.getCameraViewMatrix()*model);

	glBindFramebuffer(GL_FRAMEBUFFER, 0);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glViewport(0, 0, windowWidth, windowHeight);

	quadProgram.use();
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, colorTexture);
	quadProgram.setUniform("quadTex", 0);
	glBindVertexArray(quadVAO);
	glDrawArrays(GL_TRIANGLE_FAN, 0, 4);
	glBindVertexArray(0);
	quadProgram.done();
}

void GUIUpdate()
{
	static auto time = glfwGetTime();
	static int fpsCount = 0;
	++fpsCount;
	static char fpsStr[128]="FPS:0";
	if (glfwGetTime()-time>1.0)
	{
		sprintf_s(fpsStr, "FPS:%d", fpsCount);
		fpsCount = 0;
		time = glfwGetTime();
	}

	ImGui_ImplGlfwGL3_NewFrame();
	ImGui::BulletText(fpsStr);
	ImGui::Separator();

	if (ImGui::Button(useOriginal? "enchanced" :"original"))
	{
		useOriginal=!useOriginal;
	}

	ImGui::Separator();
	ImGui::BulletText("Right click and drag to change camera direction");
	ImGui::BulletText("Hold WSAD to perform a FPS like camera movement");
	ImGui::BulletText("Hold Shift move camera faster");
	ImGui::Separator();
	static char buff[256]="";
	ImGui::InputText("path", buff,256);
	if (ImGui::Button("save joints"))
	{
		handPoseJoints->serializeTo(buff);
	}
	if (ImGui::Button("load joints"))
	{
		ifstream ifs(buff);
		if (ifs)
		{
			handPoseJoints = Joint::serializeFrom(ifs);
			ifs.close();
		}
	}
	ImGui::Separator();
	if (ImGui::Button("reset camera"))
	{
		camera.reset();
	}
	ImGui::Separator();
	if (ImGui::Button("reload models"))
	{
		loadModels();
	}
	ImGui::Separator();
	if (selectedJoint)
	{
		ImGui::BulletText(selectedJoint->jointName.c_str());
		ImGui::DragFloat3("translate", (float *)&(selectedJoint->translate), 0.01);
		ImGui::DragFloat3("rotate", (float *)&(selectedJoint->rotate), 0.1);
		ImGui::DragFloat3("scale", (float *)&(selectedJoint->scale), 0.01);
	}
}

int main(int, char **)
{
	pythonInitialize();
	GLFWwindow* window;

	if (!glfwInit()) {
		exit(1);
	}

	//设置opengl版本
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

	// 创建窗口
	window = glfwCreateWindow(800, 600, "OpenGL", NULL, NULL);
	if (!window)
	{
		glfwTerminate();
		exit(1);
	}
	// Initialize GUI
	ImGui_ImplGlfwGL3_Init(window, false /*do not install callbacks*/);

	glfwMakeContextCurrent(window);
// 	glfwSwapInterval(1);

	// Initialize GLEW
	glewExperimental = true; // Needed for core profile
	if (glewInit() != GLEW_OK) {
		fprintf(stderr, "Failed to initialize GLEW\n");
		exit(1);
	}

	initOpenGLResource();

	glfwSetWindowSizeCallback(window, resizeCallBack);
	glfwSetKeyCallback(window, keyFunCallBack);
	glfwSetMouseButtonCallback(window, mouseButtonCallback);
	glfwSetCursorPosCallback(window, cursorPosCallback);
	glfwSetCharCallback(window, charCallback);


	while (!glfwWindowShouldClose(window))
	{
		glfwPollEvents();
		update();
		render();

		GUIUpdate();

		ImGui::Render();
		glfwSwapBuffers(window);
	}

	glfwDestroyWindow(window);

	pythonFinalize();
	glfwTerminate();
}