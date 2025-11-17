#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include <iostream>
#include <numbers>
#include <vector>
#include <cstdlib>
#include <ctime>

#include "./header/Shader.h"
#include "./header/Object.h"

// Settings
const int INITIAL_SCR_WIDTH = 800;
const int INITIAL_SCR_HEIGHT = 600;
const float AQUARIUM_BOUNDARY = 15.0f;

// Animation constants
const float TAIL_ANIMATION_SPEED = 5.0f;
const float WAVE_FREQUENCY = 1.5f;

int SCR_WIDTH = INITIAL_SCR_WIDTH;
int SCR_HEIGHT = INITIAL_SCR_HEIGHT;

// Global objects
Shader* shader = nullptr;
Object* cube = nullptr;
Object* fish1 = nullptr;
Object* fish2 = nullptr;
Object* fish3 = nullptr;

struct Fish {
    glm::vec3 position;
    glm::vec3 direction;
    std::string fishType = "fish1";
    float angle = 0.0f;
    float speed = 3.0f;
    glm::vec3 scale = glm::vec3(2.0f, 2.0f, 2.0f);
    glm::vec3 color = glm::vec3(1.0f, 0.5f, 0.3f);
};

struct SeaweedSegment {
    glm::vec3 localPos;
    glm::vec3 color;
    float phase;
    glm::vec3 scale;
    SeaweedSegment* next = nullptr;
};

struct Seaweed {
    glm::vec3 basePosition;
    SeaweedSegment* rootSegment = nullptr;
    float swayOffset = 0.0f;
};

struct playerFish {
    glm::vec3 position = glm::vec3(0.0f, 5.0f, 0.0f);
    float angle = 0.0f; // Heading direction in radians
    float speed = 2.0f;
    float rotationSpeed = 2.0f;
    bool mouthOpen = false;
    float tailAnimation = 0.0f;
    // for tooth
    float duration = 1.0f;     
    float elapsed = 0.0f;    
    struct tooth{
        glm::vec3 pos0, pos1;
    }toothUpperLeft, toothUpperRight, toothLowerLeft, toothLowerRight;
   bool rocketOpen = false;
} playerFish;

struct FireBall {
    glm::vec3 position;
    glm::vec3 direction;
    float speed = 5.0f;
    float duration = 1.0f; // seconds
    float elapsed = 0.0f; // seconds
};

// Aquarium elements
std::vector<Seaweed> seaweeds;
std::vector<Fish> schoolFish;
std::vector<FireBall> fireballs;

float globalTime = 0.0f;

void framebuffer_size_callback(GLFWwindow* window, int width, int height);
void keyCallback(GLFWwindow* window, int key, int scancode, int action, int mods);
void processInput(GLFWwindow* window, float deltaTime);
void drawModel(std::string type, const glm::mat4& model, const glm::mat4& view, const glm::mat4& projection, const glm::vec3& color);
void drawPlayerFish(const glm::vec3& position, float angle, float tailPhase,
                    const glm::mat4& view, const glm::mat4& projection, bool mouthOpen, float deltaTime);
void updateSchoolFish(float deltaTime);
void initializeAquarium();
void cleanup();
void init();
void updateFireballs(float deltaTime);
void drawFireballs(const glm::mat4& view, const glm::mat4& projection);

int main() {
    // Initialize random seed for aquarium elements
    srand(static_cast<unsigned int>(time(nullptr)));
    
    // GLFW: initialize and configure
    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

#ifdef __APPLE__
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
#endif

    // GLFW window creation
    GLFWwindow* window = glfwCreateWindow(SCR_WIDTH, SCR_HEIGHT, "GPU-Accelerated Aquarium", nullptr, nullptr);
    if (!window) {
        std::cerr << "Failed to create GLFW window" << std::endl;
        glfwTerminate();
        return -1;
    }
    
    glfwMakeContextCurrent(window);
    glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);
    glfwSetKeyCallback(window, keyCallback);
    glfwSwapInterval(1);

    // GLAD: load all OpenGL function pointers
    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
        std::cerr << "Failed to initialize GLAD" << std::endl;
        return -1;
    }

    // TODO: Enable depth test, face culling
    glEnable(GL_DEPTH_TEST);
    glDepthFunc(GL_LEQUAL);
    glEnable(GL_CULL_FACE);
    glFrontFace(GL_CCW);
    glCullFace(GL_BACK);
    // Initialize Object and Shader
    init();
    initializeAquarium();

    float lastFrame = glfwGetTime();

    while (!glfwWindowShouldClose(window)) {
        // Calculate delta time for the usage of animation
        float currentFrame = glfwGetTime();
        float deltaTime = currentFrame - lastFrame;
        lastFrame = currentFrame;
        globalTime = currentFrame;

        playerFish.tailAnimation += deltaTime * TAIL_ANIMATION_SPEED;

        // Render background
        glClearColor(0.2f, 0.5f, 0.8f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        shader->use();

        /*=================== Example of creating model matrix ======================= 
        1. translate
        glm::mat4 model(1.0f);
        model = glm::translate(model, glm::vec3(2.0f, 1.0f, 0.0f));
        drawModel("cube",model,view,projection, glm::vec3(0.9f, 0.8f, 0.6f));
        
        2. scale
        glm::mat4 model(1.0f);
        model = glm::scale(model, glm::vec3(0.5f, 1.0f, 2.0f)); 
        drawModel("cube",model,view,projection, glm::vec3(0.9f, 0.8f, 0.6f));
        
        3. rotate
        glm::mat4 model(1.0f);
        model = glm::rotate(model, glm::radians(45.0f), glm::vec3(0.0f, 0.0f, 1.0f));
        drawModel("cube",model,view,projection, glm::vec3(0.9f, 0.8f, 0.6f));
        ==============================================================================*/

        // TODO: Create model, view, and perspective matrix
        glm::mat4 view = glm::lookAt(glm::vec3(0.0f, 10.0f, 25.0f), 
                                     glm::vec3(0.0f, 8.0f, 0.0f), 
                                     glm::vec3(0.0f, 1.0f, 0.0f));
        glm::mat4 projection = glm::perspective(
                                                glm::radians(45.0f), 
                                                (float)SCR_WIDTH / (float)SCR_HEIGHT, 
                                                0.1f, 
                                                1000.0f);

        // TODO: Aquarium Base
        glm::mat4 model(1.0f);
        model = glm::translate(model, glm::vec3(0.0f, 0.0f, 0.0f));
        model = glm::scale(model, glm::vec3(70.0f, 1.0f, 40.0f));
        glm::vec3 color(0.9f, 0.8f, 0.6f);
        drawModel("cube", model, view, projection, color);
        // TODO: Draw seaweeds with hierarchical structure and wave motion
        // Wave motion is sine wave based on global time and segment phase
        // Each segment sways slightly differently for natural effect
        // E.g. Amplitude * sin(keepingChangingX + delayPhase)
        // delayPhase is different for each segment
        // the deeper the segment is, the larger the delayPhase is.
        // so that you can create a forward wave motion.

        for (const auto& seaweed : seaweeds) {
            glm::mat4 parentModel(1.0f);
            parentModel = glm::translate(parentModel, seaweed.basePosition);
            SeaweedSegment* current_segment = seaweed.rootSegment;
            while(current_segment != nullptr) {
                float swayOffset = seaweed.swayOffset;
                float swayAngle = 2.0f * sin(current_segment->phase + globalTime * WAVE_FREQUENCY + seaweed.swayOffset); // phase: delayPhase
                // glm::mat4 rotationModel = glm::rotate(glm::mat4(1.0f), glm::radians(swayAngle), glm::vec3(0.0f, 0.0f, 1.0f));
                // glm::mat4 model = parentModel * rotationModel; // 在位置還在原點的時候，先做rotation
                // model = glm::translate(model, glm::vec3(0.0f, current_segment->scale.y / 2.0f, 0.0f));
                // model = glm::scale(model, current_segment->scale);
                glm::mat4 rotationModel = glm::rotate(glm::mat4(1.0f), glm::radians(swayAngle), glm::vec3(0.0f, 0.0f, 1.0f));
                glm::mat4 model = parentModel * rotationModel; // 在位置還在local原點的時候，先做rotation
                model = glm::translate(model, glm::vec3(0.0f, current_segment->scale.y / 2.0f, 0.0f));
                model = glm::scale(model, current_segment->scale);
                drawModel("cube", model, view, projection, current_segment->color);
                glm::mat4 topModel = glm::translate(glm::mat4(1.0f), glm::vec3(0.0f, current_segment->scale.y, 0.0f)); // 海草頂端
                parentModel = parentModel * rotationModel * topModel; // 該segment的頂點 -> 下一segment的起點
                current_segment = current_segment->next;
            }   
        }

        // TODO: Draw school of fish
        // The fish movement logic is implemented.
        // All you need is to set up the position like the example in initAquarium()
        for (const auto& fish : schoolFish) {
            glm::mat4 model(1.0f);
            model = glm::translate(model, fish.position);
            model = glm::rotate(model, fish.angle, glm::vec3(0.0f, 1.0f, 0.0f));
            model = glm::scale(model, fish.scale);
            drawModel(fish.fishType, model, view, projection, fish.color);
        }
        // Update aquarium elements
        updateSchoolFish(deltaTime);

        // TODO: Draw Player Fish
        // You can use the provided function drawPlayerFish() or implement your own version.
        // The key idea of hierarchy is to reuse the model matrix to the children.
        // E.g. 
        // glm::mat4 model(1.0f);
        // glm::mat4 bodyModel;
        // model = glm::translate(model, position);
        // ^-- "position": Move the whole body to the desired position.
        // model = glm::rotate(model, angle, glm::vec3(0.0f, 1.0f, 0.0f));  
        //  ^-- "angle": Rotate the whole body but in the homework case, no need to rotate the fish.
        // bodyModel = glm::scale(model, glm::vec3(5.0f, 3.0f, 2.5f)); // Elongated for shark body
        // drawModel("cube", bodyModel, view, projection, glm::vec3(0.4f, 0.4f, 0.6f)); // Dark blue-gray shark color
        // Reuse "model" for the children of the body.
        // glm::mat4 dorsalFinModel;
        // dorsalFinModel = glm::translate(model, glm::vec3(0.0f, 2.0f, 0.0f));
        // dorsalFinModel = glm::rotate(dorsalFinModel, glm::radians(-50.0f), glm::vec3(0.0f, 0.0f, 1.0f));
        // dorsalFinModel = glm::scale(dorsalFinModel, glm::vec3(3.0f, 1.5f, 1.0f));
        // drawModel("cube", dorsalFinModel, view, projection, glm::vec3(0.35f, 0.35f, 0.55f)); // Fin color
        //
        // Notice that to keep the scale of the children is not affected by the body scale,
        // you need to apply the inverse scale to the fin model matrix, 
        // or separate the scale computation from the parent model matrix.
        //
        // For the wave motion of the tail, you can use a sine function based on time,
        // which is provided as playerFish.tailAnimation that would act as tail phase in the drawPlayerFish().
        // To make the tail motion, follow the formula: Amplitude * sin(tailPhase);

        drawPlayerFish(playerFish.position, playerFish.angle, playerFish.tailAnimation,
                        view, projection, playerFish.mouthOpen, deltaTime);

        updateFireballs(deltaTime);
        drawFireballs(view, projection);

        // TODO: Implement input processing
        processInput(window, deltaTime);

        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    cleanup();
    glfwTerminate();
    return 0;
}

void framebuffer_size_callback(GLFWwindow* window, int width, int height) {
    glViewport(0, 0, width, height);
    SCR_WIDTH = width;
    SCR_HEIGHT = height;
}

void processInput(GLFWwindow* window, float deltaTime) {
    // We use process_input in the display/render loop instead of relying solely on keyCallback
    // because key events (GLFW_PRESS, GLFW_RELEASE, GLFW_REPEAT) are not emitted every frame.
    // keyCallback only triggers on discrete key events, but for continuous key behavior (e.g., holding down a key),
    // we need to check key states every frame using glfwGetKey.
    // This ensures smooth, frame-consistent input handling such as movement or rotation.

    // TODO:
    // Controls:
    // - W / S           : Move the fish along the X-axis.
    // - A / D           : Move the fish along the Z-axis.
    // - SPACE / LSHIFT  : Move the fish up / down along the Y-axis.
    //
    // Behavior:
    // - Movement is directly applied to the fish's position on the X, Y, and Z axes.
    // - Vertical movement (Y-axis) is independent of horizontal movement.
    // - The fish can move freely in all three axes but should be clamped within
    //   the aquarium boundaries to stay visible.
    
    if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS) {
        glm::mat4 rotation = glm::rotate(glm::mat4(1.0f), playerFish.angle, glm::vec3(0.0f, 1.0f, 0.0f));
        playerFish.position += glm::vec3(rotation * glm::vec4(2.0f, 0.0f, 0.0f, 0.0f)) * playerFish.speed * deltaTime;
        playerFish.rocketOpen = true;
    }
    else{
        playerFish.rocketOpen = false;
    }
    if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS) {
        glm::mat4 rotation = glm::rotate(glm::mat4(1.0f), playerFish.angle, glm::vec3(0.0f, 1.0f, 0.0f));
        playerFish.position += glm::vec3(rotation * glm::vec4(-1.0f, 0.0f, 0.0f, 0.0f)) * playerFish.speed * deltaTime;
    }
    if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS){
        glm::mat4 rotation = glm::rotate(glm::mat4(1.0f), playerFish.angle, glm::vec3(0.0f, 1.0f, 0.0f));
        playerFish.position += glm::vec3(rotation * glm::vec4(0.0f, 0.0f, -1.0f, 0.0f)) * playerFish.speed * deltaTime;
    }
    if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS){
        glm::mat4 rotation = glm::rotate(glm::mat4(1.0f), playerFish.angle, glm::vec3(0.0f, 1.0f, 0.0f));
        playerFish.position += glm::vec3(rotation * glm::vec4(0.0f, 0.0f, 1.0f, 0.0f)) * playerFish.speed * deltaTime;
    }
    if (glfwGetKey(window, GLFW_KEY_SPACE) == GLFW_PRESS){
        playerFish.position += glm::vec3(0.0f, 1.0f, 0.0f) * playerFish.speed * deltaTime;
    }
    if (glfwGetKey(window, GLFW_KEY_LEFT_SHIFT) == GLFW_PRESS){
        playerFish.position += glm::vec3(0.0f, -1.0f, 0.0f) * playerFish.speed * deltaTime;
        if(playerFish.position.y < 0.5f) playerFish.position.y = 0.5f; // base的高度
    }
    if (glfwGetKey(window, GLFW_KEY_M) == GLFW_PRESS){
        playerFish.mouthOpen = true;
        if (fireballs.empty() || fireballs.back().elapsed > 0.5f){ // 避免連發
            if (glfwGetKey(window, GLFW_KEY_F) == GLFW_PRESS){
                FireBall fireball;
                glm::mat4 rotation = glm::rotate(glm::mat4(1.0f), playerFish.angle, glm::vec3(0.0f, 1.0f, 0.0f));
                glm::vec3 positionOffset = glm::vec3(rotation * glm::vec4(5.0f, 0.0f, 0.0f, 1.0f));
                fireball.position = playerFish.position + positionOffset;
                // fireball.position = glm::vec3(rotation * glm::vec4(fireball.position, 1.0f));

                fireball.direction = glm::vec3(rotation * glm::vec4(1.0f, 0.0f, 0.0f, 0.0f));
                fireballs.push_back(fireball);
            }
        }

    }
    else{
        playerFish.mouthOpen = false;
    }
    if (glfwGetKey(window, GLFW_KEY_Q) == GLFW_PRESS){
        playerFish.angle += playerFish.rotationSpeed * deltaTime;
    }
    if (glfwGetKey(window, GLFW_KEY_E) == GLFW_PRESS){
        playerFish.angle -= playerFish.rotationSpeed * deltaTime;
    }   
    // TODO: Keep fish within aquarium bounds
}


void keyCallback(GLFWwindow* window, int key, int scancode, int action, int mods) {
    // The action is one of GLFW_PRESS, GLFW_REPEAT or GLFW_RELEASE. 
    // Events with GLFW_PRESS and GLFW_RELEASE actions are emitted for every key press.
    // Most keys will also emit events with GLFW_REPEAT actions while a key is held down.
    // https://www.glfw.org/docs/3.3/input_guide.html
    
    if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS) {
        glfwSetWindowShouldClose(window, true);
    }

    // TODO: Implement mouth toggle logic
}

void drawModel(std::string type, const glm::mat4& model, const glm::mat4& view, const glm::mat4& projection, const glm::vec3& color) {
    shader->set_uniform("projection", projection);
    shader->set_uniform("view", view);
    shader->set_uniform("model", model);
    shader->set_uniform("objectColor", color);
    if (type == "fish1") {
        fish1->draw();
    } else if (type == "fish2") {
        fish2->draw();
    } else if (type == "fish3") {
        fish3->draw();
    }else if (type == "cube") {
        cube->draw();
    }
}

void init() {
#if defined(__linux__) || defined(__APPLE__)
    std::string dirShader = "shaders/";
    std::string dirAsset = "asset/";
#else
    std::string dirShader = "shaders\\";
    std::string dirAsset = "asset\\";
#endif

    shader = new Shader((dirShader + "easy.vert").c_str(), (dirShader + "easy.frag").c_str());
   
    cube = new Object(dirAsset + "cube.obj");
    fish1 = new Object(dirAsset + "fish1.obj");
    fish2 = new Object(dirAsset + "fish2.obj");
    fish3 = new Object(dirAsset + "fish3.obj");
}

void cleanup() {
    if (shader) {
        delete shader;
        shader = nullptr;
    }
    
    if (cube) {
        delete cube;
        cube = nullptr;
    }
    
    for (auto& seaweed : seaweeds) {
        SeaweedSegment* current = seaweed.rootSegment;
        while (current != nullptr) {
            SeaweedSegment* next = current->next;
            delete current;
            current = next;
        }
    }
    seaweeds.clear();
    
    schoolFish.clear();
}

        // E.g. 
        // glm::mat4 model(1.0f);
        // glm::mat4 bodyModel;
        // model = glm::translate(model, position);
        // ^-- "position": Move the whole body to the desired position.
        // model = glm::rotate(model, angle, glm::vec3(0.0f, 1.0f, 0.0f));  
        //  ^-- "angle": Rotate the whole body but in the homework case, no need to rotate the fish.
        // bodyModel = glm::scale(model, glm::vec3(5.0f, 3.0f, 2.5f)); // Elongated for shark body
        // drawModel("cube", bodyModel, view, projection, glm::vec3(0.4f, 0.4f, 0.6f)); // Dark blue-gray shark color
        // Reuse "model" for the children of the body.
        // glm::mat4 dorsalFinModel;
        // dorsalFinModel = glm::translate(model, glm::vec3(0.0f, 2.0f, 0.0f));
        // dorsalFinModel = glm::rotate(dorsalFinModel, glm::radians(-50.0f), glm::vec3(0.0f, 0.0f, 1.0f));
        // dorsalFinModel = glm::scale(dorsalFinModel, glm::vec3(3.0f, 1.5f, 1.0f));
        // drawModel("cube", dorsalFinModel, view, projection, glm::vec3(0.35f, 0.35f, 0.55f)); // Fin color
        //
        // Notice that to keep the scale of the children is not affected by the body scale,
        // you need to apply the inverse scale to the fin model matrix, 
        // or separate the scale computation from the parent model matrix.
        //
        // For the wave motion of the tail, you can use a sine function based on time,
        // which is provided as playerFish.tailAnimation that would act as tail phase in the drawPlayerFish().
        // To make the tail motion, follow the formula: Amplitude * sin(tailPhase);

void drawPlayerFish(const glm::vec3& position, float angle, float tailPhase,
    const glm::mat4& view, const glm::mat4& projection, bool mouthOpen, float deltaTime) {
    glm::mat4 model(1.0f);

    // TODO: Draw body using cube (main body)
    glm::mat4 bodyModel(1.0f);
    model = glm::translate(model, position);
    model = glm::rotate(model, angle, glm::vec3(0.0f, 1.0f, 0.0f));
    bodyModel = glm::scale(model, glm::vec3(5.0f, 3.0f, 2.5f));
    drawModel("cube", bodyModel, view, projection, glm::vec3(0.4f, 0.4f, 0.6f));

    glm::mat4 dorsalFinModel;
    dorsalFinModel = glm::translate(model, glm::vec3(0.0f, 2.0f, 0.0f));
    dorsalFinModel = glm::rotate(dorsalFinModel, glm::radians(-50.0f), glm::vec3(0.0f, 0.0f, 1.0f));
    dorsalFinModel = glm::scale(dorsalFinModel, glm::vec3(3.0f, 1.5f, 1.0f));
    drawModel("cube", dorsalFinModel, view, projection, glm::vec3(0.35f, 0.35f, 0.55f));

    // pectoral fin right
    glm::mat4 pectoralFinModel;
    pectoralFinModel = glm::translate(model, glm::vec3(0.5f, -1.5f, 1.5f));
    pectoralFinModel = glm::rotate(pectoralFinModel, glm::radians(30.0f), glm::vec3(0.0f, 1.0f, 0.0f));
    pectoralFinModel = glm::rotate(pectoralFinModel, glm::radians(30.0f), glm::vec3(0.0f, 0.0f, 1.0f));
    pectoralFinModel = glm::scale(pectoralFinModel, glm::vec3(3.0f, 0.5f, 1.0f));
    drawModel("cube", pectoralFinModel, view, projection, glm::vec3(0.35f, 0.35f, 0.55f));
    // pectoral fin left
    pectoralFinModel = glm::translate(model, glm::vec3(0.5f, -1.5f, -1.5f));
    pectoralFinModel = glm::rotate(pectoralFinModel, glm::radians(-30.0f), glm::vec3(0.0f, 1.0f, 0.0f));
    pectoralFinModel = glm::rotate(pectoralFinModel, glm::radians(30.0f), glm::vec3(0.0f, 0.0f, 1.0f));
    pectoralFinModel = glm::scale(pectoralFinModel, glm::vec3(3.0f, 0.5f, 1.0f));
    drawModel("cube", pectoralFinModel, view, projection, glm::vec3(0.35f, 0.35f, 0.55f));

    // TODO: Draw head and Mouth using cube with mouth open/close feature

    glm::mat4 headModel;
    headModel = glm::translate(model, glm::vec3(3.0f, 0.2f, 0.0f));
    headModel = glm::rotate(headModel, glm::radians(-20.0f), glm::vec3(0.0f, 0.0f, 1.0f));

    glm::mat4 jawModel;
    jawModel = glm::translate(model, glm::vec3(3.2f, -0.8f, 0.0f));
    jawModel = glm::rotate(jawModel, glm::radians(15.0f), glm::vec3(0.0f, 0.0f, 1.0f));

    if (mouthOpen) {
        // TODO: head and mouth model matrix adjustment
        headModel = glm::translate(headModel, glm::vec3(-0.5f, 0.3f, 0.0f));
        headModel = glm::rotate(headModel, glm::radians(40.0f), glm::vec3(0.0f, 0.0f, 1.0f));
        headModel = glm::translate(headModel, glm::vec3(0.5f, 0.3f, 0.0f));
        jawModel = glm::translate(jawModel, glm::vec3(-0.7f, 0.0f, 0.0f));
        jawModel = glm::rotate(jawModel, glm::radians(-40.0f), glm::vec3(0.0f, 0.0f, 1.0f));
        jawModel = glm::translate(jawModel, glm::vec3(0.7f, 0.0f, 0.0f));
        // TODO: Calculate elapse time for tooth animation
        playerFish.elapsed += deltaTime;
        if (playerFish.elapsed > playerFish.duration) playerFish.elapsed = playerFish.duration;
        // TODO: Upper teeth right
        glm::mat4 upperTeethRightModel(1.0f);
        upperTeethRightModel = glm::translate(headModel, playerFish.toothUpperRight.pos0);// glm::vec3(1.2f, -1.0f, 0.5f));
        glm::vec3 teethEndPoint = glm::mix(playerFish.toothUpperRight.pos0, playerFish.toothUpperRight.pos1, playerFish.elapsed / playerFish.duration);
        float y_diff = teethEndPoint.y - playerFish.toothUpperRight.pos0.y;
        glm::vec3 teethScale = glm::vec3(0.3f, abs(y_diff), 0.3f);
        upperTeethRightModel = glm::translate(upperTeethRightModel, glm::vec3(0.0f, y_diff / 2.0f, 0.0f));
        upperTeethRightModel = glm::scale(upperTeethRightModel, teethScale);
        drawModel("cube", upperTeethRightModel, view, projection, glm::vec3(0.9f, 0.9f, 0.9f));
        // TODO: Upper teeth left
        glm::mat4 upperTeethLeftModel(1.0f);
        upperTeethLeftModel = glm::translate(headModel, playerFish.toothUpperLeft.pos0);// glm::vec3(1.2f, -1.0f, 0.5f));
        teethEndPoint = glm::mix(playerFish.toothUpperLeft.pos0, playerFish.toothUpperLeft.pos1, playerFish.elapsed / playerFish.duration);
        y_diff = teethEndPoint.y - playerFish.toothUpperLeft.pos0.y;
        teethScale = glm::vec3(0.3f, abs(y_diff), 0.3f);
        upperTeethLeftModel = glm::translate(upperTeethLeftModel, glm::vec3(0.0f, y_diff / 2.0f, 0.0f));
        upperTeethLeftModel = glm::scale(upperTeethLeftModel, teethScale);
        drawModel("cube", upperTeethLeftModel, view, projection, glm::vec3(0.9f, 0.9f, 0.9f));
        // TODO: Lower teeth right
        glm::mat4 lowerTeethRightModel(1.0f);
        lowerTeethRightModel = glm::translate(jawModel, playerFish.toothLowerRight.pos0);
        teethEndPoint = glm::mix(playerFish.toothLowerRight.pos0, playerFish.toothLowerRight.pos1, playerFish.elapsed / playerFish.duration);
        y_diff = teethEndPoint.y - playerFish.toothLowerRight.pos0.y;
        teethScale = glm::vec3(0.3f, abs(y_diff), 0.3f);
        lowerTeethRightModel = glm::translate(lowerTeethRightModel, glm::vec3(0.0f, y_diff / 2.0f, 0.0f));
        lowerTeethRightModel = glm::scale(lowerTeethRightModel, teethScale);
        drawModel("cube", lowerTeethRightModel, view, projection, glm::vec3(0.9f, 0.9f, 0.9f));
        // TODO: Lower teeth left
        glm::mat4 lowerTeethLeftModel(1.0f);
        lowerTeethLeftModel = glm::translate(jawModel, playerFish.toothLowerLeft.pos0);
        teethEndPoint = glm::mix(playerFish.toothLowerLeft.pos0, playerFish.toothLowerLeft.pos1, playerFish.elapsed / playerFish.duration);
        y_diff = teethEndPoint.y - playerFish.toothLowerLeft.pos0.y;
        teethScale = glm::vec3(0.3f, abs(y_diff), 0.3f);
        lowerTeethLeftModel = glm::translate(lowerTeethLeftModel, glm::vec3(0.0f, y_diff / 2.0f, 0.0f));
        lowerTeethLeftModel = glm::scale(lowerTeethLeftModel, teethScale);
        drawModel("cube", lowerTeethLeftModel, view, projection, glm::vec3(0.9f, 0.9f, 0.9f));
        
    } else {
        playerFish.elapsed = 0.0f;
        // TODO: head and mouth model matrix adjustment
    } 

    glm::mat4 scaled_headModel = glm::scale(headModel, glm::vec3(3.0f, 2.0f, 2.3f));
    drawModel("cube", scaled_headModel, view, projection, glm::vec3(0.35f, 0.35f, 0.55f));
    jawModel = glm::scale(jawModel, glm::vec3(2.0f, 0.9f, 2.2f));
    drawModel("cube", jawModel, view, projection, glm::vec3(0.9f, 0.9f, 0.9f));


    // TODO: Draw Eyes right
     glm::mat4 eyeWhiteModel;
    eyeWhiteModel = glm::translate(headModel, glm::vec3(0.0f, 0.0f, 1.15f));
    eyeWhiteModel = glm::rotate(eyeWhiteModel, glm::radians(30.0f), glm::vec3(0.0f, 0.0f, 1.0f));
    glm::mat4 scaled_eyeWhiteModel = glm::scale(eyeWhiteModel, glm::vec3(0.5f, 0.5f, 0.1f));
    drawModel("cube", scaled_eyeWhiteModel, view, projection, glm::vec3(1.0f, 1.0f, 1.0f));

    glm::mat4 eyeBlackModel;
    eyeBlackModel = glm::translate(eyeWhiteModel, glm::vec3(0.0f, 0.0f, 0.1f));
    glm::mat4 scaled_eyeBlackModel = glm::scale(eyeBlackModel, glm::vec3(0.3f, 0.3f, 0.1f));
    drawModel("cube", scaled_eyeBlackModel, view, projection, glm::vec3(0.0f, 0.0f, 0.0f));


    // TODO: Draw Eyes left
    eyeWhiteModel = glm::translate(headModel, glm::vec3(0.0f, 0.0f, -1.15f));
    eyeWhiteModel = glm::rotate(eyeWhiteModel, glm::radians(30.0f), glm::vec3(0.0f, 0.0f, 1.0f));
    scaled_eyeWhiteModel = glm::scale(eyeWhiteModel, glm::vec3(0.5f, 0.5f, 0.1f));
    drawModel("cube", scaled_eyeWhiteModel, view, projection, glm::vec3(1.0f, 1.0f, 1.0f));

    eyeBlackModel = glm::translate(eyeWhiteModel, glm::vec3(0.0f, 0.0f, -0.1f));
    scaled_eyeBlackModel = glm::scale(eyeBlackModel, glm::vec3(0.3f, 0.3f, 0.1f));
    drawModel("cube", scaled_eyeBlackModel, view, projection, glm::vec3(0.0f, 0.0f, 0.0f));

    // TODO: Draw hierarchical animated tail with multiple segments
    glm::mat4 tailModel1;
    tailModel1 = glm::translate(model, glm::vec3(-3.0f, 0.0f, 0.0f));
    tailModel1 = glm::translate(tailModel1, glm::vec3(2.0f, 0.0f, 0.0f));
    tailModel1 = glm::rotate(tailModel1, glm::radians(20.0f * sin(tailPhase * 0.5f)), glm::vec3(0.0f, 1.0f, 0.0f));
    tailModel1 = glm::translate(tailModel1, glm::vec3(-2.0f, 0.0f, 0.0f));
    glm::mat4 scaled_tailModel1 = glm::scale(tailModel1, glm::vec3(4.0f, 1.3f, 1.7f));
    drawModel("cube", scaled_tailModel1, view, projection, glm::vec3(0.4f, 0.4f, 0.6f));
    glm::mat4 tailModel2;
    tailModel2 = glm::translate(tailModel1, glm::vec3(-2.5f, 0.0f, 0.0f));
    tailModel2 = glm::translate(tailModel2, glm::vec3(1.5f, 0.0f, 0.0f));
    tailModel2 = glm::rotate(tailModel2, glm::radians(20.0f * sin(tailPhase * 0.5f)), glm::vec3(0.0f, 1.0f, 0.0f));
    tailModel2 = glm::translate(tailModel2, glm::vec3(-1.5f, 0.0f, 0.0f));
    glm::mat4 scaled_tailModel2 = glm::scale(tailModel2, glm::vec3(3.0f, 1.0f, 1.2f));
    drawModel("cube", scaled_tailModel2, view, projection, glm::vec3(0.4f, 0.4f, 0.6f));
    glm::mat4 tailModel3;
    tailModel3 = glm::translate(tailModel2, glm::vec3(-2.0f, 0.0f, 0.0f));
    tailModel3 = glm::translate(tailModel3, glm::vec3(1.0f, 0.0f, 0.0f));
    tailModel3 = glm::rotate(tailModel3, glm::radians(20.0f * sin(tailPhase  * 0.5f)), glm::vec3(0.0f, 1.0f, 0.0f));
    tailModel3 = glm::translate(tailModel3, glm::vec3(-1.0f, 0.0f, 0.0f));
    glm::mat4 scaled_tailModel3 = glm::scale(tailModel3, glm::vec3(2.0f, 0.7f, 0.7f));
    drawModel("cube", scaled_tailModel3, view, projection, glm::vec3(0.4f, 0.4f, 0.6f));

    // TODO: Draw tail fin at the end
    glm::mat4 tailFinModel;
    tailFinModel = glm::translate(tailModel3, glm::vec3(-1.0f, 0.0f, 0.0f));
    tailFinModel = glm::scale(tailFinModel, glm::vec3(0.8f, 4.0f, 0.5f));
    drawModel("cube", tailFinModel, view, projection, glm::vec3(0.35f, 0.35f, 0.55f));

    // RGB stripe right
    glm::mat4 stripeModel;
    stripeModel = glm::translate(model, glm::vec3(1.3f, 0.0f, 1.25f));
    stripeModel = glm::rotate(stripeModel, glm::radians(-20.0f), glm::vec3(0.0f, 0.0f, 1.0f));
    stripeModel = glm::scale(stripeModel, glm::vec3(0.2f, 1.5f, 0.2f));
    float RTime = abs(sin(globalTime));
    drawModel("cube", stripeModel, view, projection, glm::vec3(RTime, 0.0f, 0.0f));
    stripeModel = glm::translate(model, glm::vec3(1.7f, 0.0f, 1.25f));
    stripeModel = glm::rotate(stripeModel, glm::radians(-20.0f), glm::vec3(0.0f, 0.0f, 1.0f));
    stripeModel = glm::scale(stripeModel, glm::vec3(0.2f, 1.5f, 0.2f));
    float GTime = abs(sin(globalTime + 0.2));
    drawModel("cube", stripeModel, view, projection, glm::vec3(0.0f, GTime, 0.0f));
    stripeModel = glm::translate(model, glm::vec3(2.1f, 0.0f, 1.25f));
    stripeModel = glm::rotate(stripeModel, glm::radians(-20.0f), glm::vec3(0.0f, 0.0f, 1.0f));
    stripeModel = glm::scale(stripeModel, glm::vec3(0.2f, 1.5f, 0.2f));
    float BTime = abs(sin(globalTime + 0.4));
    drawModel("cube", stripeModel, view, projection, glm::vec3(0.0f, 0.0f, BTime));

    // RGB stripe left
    stripeModel = glm::translate(model, glm::vec3(1.3f, 0.0f, -1.25f));
    stripeModel = glm::rotate(stripeModel, glm::radians(-20.0f), glm::vec3(0.0f, 0.0f, 1.0f));
    stripeModel = glm::scale(stripeModel, glm::vec3(0.2f, 1.5f, 0.2f));
    drawModel("cube", stripeModel, view, projection, glm::vec3(RTime, 0.0f, 0.0f));
    stripeModel = glm::translate(model, glm::vec3(1.7f, 0.0f, -1.25f));
    stripeModel = glm::rotate(stripeModel, glm::radians(-20.0f), glm::vec3(0.0f, 0.0f, 1.0f));
    stripeModel = glm::scale(stripeModel, glm::vec3(0.2f, 1.5f, 0.2f));
    drawModel("cube", stripeModel, view, projection, glm::vec3(0.0f, GTime, 0.0f));
    stripeModel = glm::translate(model, glm::vec3(2.1f, 0.0f, -1.25f));
    stripeModel = glm::rotate(stripeModel, glm::radians(-20.0f), glm::vec3(0.0f, 0.0f, 1.0f));
    stripeModel = glm::scale(stripeModel, glm::vec3(0.2f, 1.5f, 0.2f));
    drawModel("cube", stripeModel, view, projection, glm::vec3(0.0f, 0.0f, BTime));

    // Rocket right
    glm::mat4 rocketShellModel;
    rocketShellModel = glm::translate(model, glm::vec3(-2.3f, -1.3f, 1.25f + 0.5f));
    rocketShellModel = glm::scale(rocketShellModel, glm::vec3(2.0f, 1.0f, 1.0f));
    drawModel("cube", rocketShellModel, view, projection, glm::vec3(0.7f, 0.7f, 0.7f));
    glm::mat4 rocketFireModel;
    rocketFireModel = glm::translate(model, glm::vec3(-2.4f, -1.3f, 1.25f + 0.5f));
    glm::mat4 scaled_rocketFireModel = glm::scale(rocketFireModel, glm::vec3(2.0f, 0.7f, 0.7f));
    drawModel("cube", scaled_rocketFireModel, view, projection, glm::vec3(1.0f, 0.2f, 0.0f));
    // 粒子效果
    if(playerFish.rocketOpen){
        const int particleCount = 10; // 粒子數量
        const float radius = 0.8f;
        for (int i = 0; i < particleCount; i++) {
            float rx = ((rand() % 100) / 100.0f - 0.5f) * 2.0f * radius - 2.0f;
            float ry = ((rand() % 100) / 100.0f - 0.5f) * 2.0f * radius;
            float rz = ((rand() % 100) / 100.0f - 0.5f) * 2.0f * radius;
            glm::mat4 rotation = glm::rotate(glm::mat4(1.0f), playerFish.angle, glm::vec3(0.0f, 1.0f, 0.0f));
            glm::vec3 particleOffset = glm::vec3(rotation * glm::vec4(rx, ry, rz, 0.0f));
            glm::vec3 particlePos = glm::vec3(rocketFireModel[3]) + particleOffset;

            glm::mat4 particleModel(1.0f);
            particleModel = glm::translate(particleModel, particlePos);

            // 小小的 cube 當作粒子
            float scale = 0.1f + (rand() % 100) / 1000.0f; // [0.1, 0.2]
            particleModel = glm::scale(particleModel, glm::vec3(scale));

            // 隨機顏色：接近火焰色 (黃紅橙)
            glm::vec3 color(
                1.0f,
                0.2f + (rand() % 50) / 100.0f, // 0.2 ~ 0.7 (橘→黃)
                0.0f
            );

            drawModel("cube", particleModel, view, projection, color);
        }
    }

    // Rocket left
    rocketShellModel;
    rocketShellModel = glm::translate(model, glm::vec3(-2.3f, -1.3f, -1.25f - 0.35f));
    rocketShellModel = glm::scale(rocketShellModel, glm::vec3(2.0f, 1.0f, 1.0f));
    drawModel("cube", rocketShellModel, view, projection, glm::vec3(0.7f, 0.7f, 0.7f));
    rocketFireModel;
    rocketFireModel = glm::translate(model, glm::vec3(-2.4f, -1.3f, -1.25f - 0.35f));
    scaled_rocketFireModel = glm::scale(rocketFireModel, glm::vec3(2.0f, 0.7f, 0.7f));
    drawModel("cube", scaled_rocketFireModel, view, projection, glm::vec3(1.0f, 0.2f, 0.0f));
    // 粒子效果
    if(playerFish.rocketOpen){
        const int particleCount = 10; // 粒子數量
        const float radius = 0.8f;
        for (int i = 0; i < particleCount; i++) {
            float rx = ((rand() % 100) / 100.0f - 0.5f) * 2.0f * radius - 2.0f;
            float ry = ((rand() % 100) / 100.0f - 0.5f) * 2.0f * radius;
            float rz = ((rand() % 100) / 100.0f - 0.5f) * 2.0f * radius;
            glm::mat4 rotation = glm::rotate(glm::mat4(1.0f), playerFish.angle, glm::vec3(0.0f, 1.0f, 0.0f));
            glm::vec3 particleOffset = glm::vec3(rotation * glm::vec4(rx, ry, rz, 0.0f));
            glm::vec3 particlePos = glm::vec3(rocketFireModel[3]) + particleOffset;

            glm::mat4 particleModel(1.0f);
            particleModel = glm::translate(particleModel, particlePos);

            // 小小的 cube 當作粒子
            float scale = 0.1f + (rand() % 100) / 1000.0f; // [0.1, 0.2]
            particleModel = glm::scale(particleModel, glm::vec3(scale));

            // 隨機顏色：接近火焰色 (黃紅橙)
            glm::vec3 color(
                1.0f,
                0.2f + (rand() % 50) / 100.0f, // 0.2 ~ 0.7 (橘→黃)
                0.0f
            );

            drawModel("cube", particleModel, view, projection, color);
        }
    }

 }

 
void updateSchoolFish(float deltaTime) {
    for (auto& fish : schoolFish) {
        // Move fish in their direction
        fish.position += fish.direction * fish.speed * deltaTime;
        
        // Bounce off walls
        // The Movement is clamped within aquarium boundaries to prevent
        // fish from escaping the visible scene.
        // atan2 calculates the angle of the fish's direction vector on the XZ plane.
        // To make the fish movement natural.
        if (fish.position.x > AQUARIUM_BOUNDARY - 2.0f || fish.position.x < -AQUARIUM_BOUNDARY + 2.0f) {
            if (fish.position.x < -AQUARIUM_BOUNDARY + 2.0f)
                fish.position.x = -AQUARIUM_BOUNDARY + 2.0f;
            else
                fish.position.x = AQUARIUM_BOUNDARY - 2.0f;
            fish.direction.x *= -1;
            fish.angle = atan2(-fish.direction.z, fish.direction.x);
        }
    }
}

void drawFireballs(const glm::mat4& view, const glm::mat4& projection) {
    for (const auto& fireball : fireballs) {
        glm::mat4 model(1.0f);
        model = glm::translate(model, fireball.position);
        model = glm::rotate(model, playerFish.angle, glm::vec3(0.0f, 1.0f, 0.0f));
        glm::mat4 scaled_model = glm::scale(model, glm::vec3(0.5f, 1.0f, 1.0f));
        drawModel("cube", scaled_model, view, projection, glm::vec3(1.0f, 0.3f, 0.0f));
        model = glm::translate(model, glm::vec3(0.25f + 0.15f, 0.0f, 0.0f));
        scaled_model = glm::scale(model, glm::vec3(0.3f, 0.95f, 0.95f));
        drawModel("cube", scaled_model, view, projection, glm::vec3(1.0f, 0.5f, 0.0f));
        model = glm::translate(model, glm::vec3(0.15f + 0.1f, 0.0f, 0.0f));
        scaled_model = glm::scale(model, glm::vec3(0.2f, 0.9f, 0.9f));
        drawModel("cube", scaled_model, view, projection, glm::vec3(1.0f, 0.7f, 0.0f));

        // 粒子效果
        const int particleCount = 10; // 粒子數量
        const float radius = 0.8f;
        for (int i = 0; i < particleCount; i++) {
            float rx = ((rand() % 100) / 100.0f - 0.5f) * 2.0f * radius - fireball.direction.x * 0.5f;
            float ry = ((rand() % 100) / 100.0f - 0.5f) * 2.0f * radius - fireball.direction.y * 0.5f;
            float rz = ((rand() % 100) / 100.0f - 0.5f) * 2.0f * radius - fireball.direction.z * 0.5f;

            glm::vec3 particlePos = fireball.position + glm::vec3(rx, ry, rz);

            glm::mat4 particleModel(1.0f);
            particleModel = glm::translate(particleModel, particlePos);

            // 小小的 cube 當作粒子
            float scale = 0.1f + (rand() % 100) / 1000.0f; // [0.1, 0.2]
            particleModel = glm::scale(particleModel, glm::vec3(scale));

            // 隨機顏色：接近火焰色 (黃紅橙)
            glm::vec3 color(
                1.0f,
                0.2f + (rand() % 50) / 100.0f, // 0.2 ~ 0.7 (橘→黃)
                0.0f
            );

            drawModel("cube", particleModel, view, projection, color);
        }

    }
}

void updateFireballs(float deltaTime) {
    for (auto& fireball : fireballs) {
        fireball.position += fireball.direction * fireball.speed * deltaTime;
        fireball.elapsed += deltaTime;
        if (fireball.elapsed > fireball.duration) {
            fireball = fireballs.back();
            fireballs.pop_back();
        }
    }
}

void initializeAquarium() {
    srand(static_cast<unsigned int>(time(nullptr)));

    // You can init the aquarium elements here
    // e.g.
    playerFish.toothUpperRight.pos0 = glm::vec3(1.2f, -1.0f, 0.5f);
    playerFish.toothUpperRight.pos1 = glm::vec3(1.2f, -2.3f, 0.5f);
    playerFish.toothUpperLeft.pos0 = glm::vec3(1.2f, -1.0f, -0.5f);
    playerFish.toothUpperLeft.pos1 = glm::vec3(1.2f, -2.3f, -0.5f);
    playerFish.toothLowerRight.pos0 = glm::vec3(0.8f, 0.45f, 0.5f);
    playerFish.toothLowerRight.pos1 = glm::vec3(0.8f, 1.2f, 0.5f);
    playerFish.toothLowerLeft.pos0 = glm::vec3(0.8f, 0.45f, -0.5f);
    playerFish.toothLowerLeft.pos1 = glm::vec3(0.8f, 1.2f, -0.5f);
    schoolFish.clear();
    const std::map<std::string, std::tuple<glm::vec3, glm::vec3, glm::vec3>> FishPropertys {
        // name, position, scale, direction
        {"fish1", {glm::vec3(0.0f, 15.0f, 0.0f), glm::vec3(2.0f, 2.0f, 2.0f), glm::vec3(1.0f, 0.0f, 0.0f)}},
        {"fish2", {glm::vec3(7.0f, 3.0f, 0.0f), glm::vec3(2.0f, 2.0f, 2.0f), glm::vec3(1.0f, 0.0f, 0.0f)}},
        {"fish3", {glm::vec3(-3.0f, 7.0f, -7.0f), glm::vec3(2.0f, 2.0f, 2.0f), glm::vec3(1.0f, 0.0f, 0.0f)}},
    };
    for (const auto& [name, props] : FishPropertys) {
        Fish fish;
        fish.position = std::get<0>(props);
        fish.scale = std::get<1>(props);
        fish.direction = glm::normalize(std::get<2>(props));
        fish.fishType = name;
        fish.color = glm::vec3(static_cast<float>(rand()) / RAND_MAX, static_cast<float>(rand()) / RAND_MAX, static_cast<float>(rand()) / RAND_MAX);
        schoolFish.push_back(fish);
    }

    // Seaweeds
    seaweeds.clear();
    const std::map<std::string, std::tuple<glm::vec3, SeaweedSegment*, float>> SeaweedPropertys {
        // name, basePosition, rootSegment, swayOffset
        {"seaweed1", {glm::vec3(7.0f, 0.0f, 0.0f),
                                        // localPos, color, phase, scale
                      new SeaweedSegment(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, 0.8f, 0.0f), 0.0f, glm::vec3(1.0f, 2.0f, 1.0f)),
                      0.0f}},
        {"seaweed2", {glm::vec3(-7.0f, 0.0f, -10.0f),
                                        // localPos, color, phase, scale
                      new SeaweedSegment(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, 0.8f, 0.0f), 0.0f, glm::vec3(1.0f, 2.0f, 1.0f)),
                      5.0f}},
        {"seaweed3", {glm::vec3(-7.0f, 0.0f, 5.0f),
                                        // localPos, color, phase, scale
                      new SeaweedSegment(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, 0.8f, 0.0f), 0.0f, glm::vec3(1.0f, 2.0f, 1.0f)),
                      10.0f}},
    };
    // 延伸海草的segment
    for (const auto& [name, props] : SeaweedPropertys) {
        SeaweedSegment* current = std::get<1>(props);
        for (int i = 2; i <= 7; i++) {
            current->next = new SeaweedSegment(
                current->localPos,
                current->color,
                current->phase + 0.5f,
                current->scale
            );
            current = current->next;
        }
    }
    for (const auto& [name, props] : SeaweedPropertys) {
        Seaweed seaweed;
        seaweed.basePosition = std::get<0>(props);
        seaweed.rootSegment = std::get<1>(props);
        seaweed.swayOffset = std::get<2>(props);
        seaweeds.push_back(seaweed);
    }

}