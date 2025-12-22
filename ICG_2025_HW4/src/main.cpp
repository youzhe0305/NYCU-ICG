#include <bits/stdc++.h>

#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include "header/cube.h"
#include "header/Object.h"
#include "header/shader.h"
#include "header/stb_image.h"

void framebufferSizeCallback(GLFWwindow *window, int width, int height);
void keyCallback(GLFWwindow *window, int key, int scancode, int action, int mods);
void processInput(GLFWwindow *window);
void updateCamera();
void applyOrbitDelta(float yawDelta, float pitchDelta, float radiusDelta);
unsigned int loadCubemap(std::vector<std::string> &mFileName);

struct material_t{
    glm::vec3 ambient;
    glm::vec3 diffuse;
    glm::vec3 specular;
    float gloss;
};

struct light_t{
    glm::vec3 position;
    glm::vec3 ambient;
    glm::vec3 diffuse;
    glm::vec3 specular;
};

struct camera_t{
    glm::vec3 position;
    glm::vec3 front;
    glm::vec3 up;
    glm::vec3 right;
    glm::vec3 worldUp;
    glm::vec3 target;

    float yaw;
    float pitch;
    float radius;
    float minRadius;
    float maxRadius;
    float orbitRotateSpeed;
    float orbitZoomSpeed;
    float minOrbitPitch;
    float maxOrbitPitch;
    bool enableAutoOrbit;
    float autoOrbitSpeed;
};

// settings
int SCR_WIDTH = 800;
int SCR_HEIGHT = 600;

// cube map 
unsigned int cubemapTexture;
unsigned int cubemapVAO, cubemapVBO;

// shader programs
int shaderProgramIndex = 0;
std::vector<shader_program_t*> shaderPrograms;
shader_program_t* cubemapShader;

// 如果要個別obj用自己個shader，在這邊加 然後去 shader_setup_w_geo...那邊新增
shader_program_t* portalShader = nullptr;
shader_program_t* meteorShader = nullptr;
shader_program_t* frogShader = nullptr;

light_t light;
material_t material;
camera_t camera;

Object* cubeModel = nullptr;
bool isCube = false;

// marada init
Object* maradaModel = nullptr;
glm::mat4 maradaMatrix(1.0f);

// portal init
Object* portalModel = nullptr;
glm::mat4 portalMatrix(1.0f);
bool showPortal = false;
float portalTimer = 0.0f;
float currentProgress = 0.0f;
const float PORTAL_ANIMATION_DURATION = 6.0f;

float portalScale = 0.0f;
const float PORTAL_TARGET_SCALE = 800.0f;

float portalRotation = 0.0f;
float currentSpinSpeed = 0.0f;
const float MAX_SPIN_SPEED = 600.0f; // 剛打開時轉超快
const float MIN_SPIN_SPEED = 20.0f; //最後維持的速度
// 在角色後上方顯示 可以自行更改：y = 高度 z = 角色多後面
glm::vec3 portalPosition = glm::vec3(0.0f, 500.0f, 1400.0f);

//隕石相關變數
Object* meteorModel = nullptr;
glm::mat4 meteorMatrix(1.0f);
bool showMeteor = false;              
float meteorTimer = 0.0f;            
float meteorFallProgress = 0.0f;      
float meteorExplosionProgress = 0.0f; 
const float METEOR_FALL_DURATION = 5.0f;      
const float METEOR_EXPLOSION_DURATION = 5.0f; // 爆炸持續時間（秒）
const float METEOR_START_HEIGHT = 500.0f;     // 起始高度（從portal位置）
const float METEOR_GROUND_Y = -50.0f;         // 地面高度
glm::vec3 meteorPosition = glm::vec3(0.0f, 0.0f, 0.0f);

//青蛙相關變數
Object* frogModel = nullptr;
unsigned int frogTexture = 0;
glm::mat4 frogMatrix(1.0f);
bool showFrog = false;          
glm::vec3 frogPosition = glm::vec3(0.0f, METEOR_GROUND_Y - 500, 0.0f); // 青蛙位置

float currentTime = 0.0f;
float deltaTime = 0.0f;
float lastFrame = 0.0f;

// Snowflake particle system
struct Snowflake {
    glm::vec3 position;
    float size;
    float rotation;
    float fallSpeed;
    float swayPhase;   // for horizontal swaying
};

std::vector<Snowflake> snowflakes;
unsigned int snowflakeVAO, snowflakeVBO;
shader_program_t* snowflakeShader = nullptr;
bool snowflakeEnabled = false;
const int SNOWFLAKE_COUNT = 2000;
const float SNOW_AREA_SIZE = 1200.0f;  // snowflake distribution area
const float SNOW_HEIGHT_MAX = 600.0f;  // maximum snowflake height
const float SNOW_HEIGHT_MIN = -200.0f; // minimum snowflake height

void model_setup(){
#if defined(__linux__) || defined(__APPLE__)
    // std::string cube_obj_path = "../../src/asset/obj/cube.obj";
    std::string cube_obj_path = "..\\..\\src\\asset\\obj\\cube.obj";

    // std::string madara_obj_path = "../../src/asset/obj/Madara_Uchiha.obj";
    std::string madara_obj_path = "..\\..\\src\\asset\\obj\\Madara_Uchiha.obj";
    // std::string madara_texture_path = "../../src/asset/texture/_Madara_texture_main_mAIN.png";
    std::string madara_texture_path = "..\\..\\src\\asset\\texture\\_Madara_texture_main_mAIN.png";

    // std::string portal_obj_path = "../../src/asset/obj/portal.obj";
    std::string portal_obj_path = "..\\..\\src\\asset\\obj\\portal.obj";
    // std::string portal_texture_path = "../../src/asset/texture/portal.png";
    std::string portal_texture_path = "..\\..\\src\\asset\\texture\\portal.png";

    // std::string meteor_obj_path = "../../src/asset/obj/Meteor_mp.obj";
    std::string meteor_obj_path = "..\\..\\src\\asset\\obj\\Meteor_mp.obj";
    // std::string meteor_base_color_path = "../../src/asset/texture/meteor_Base_Color.png";
    std::string meteor_base_color_path = "..\\..\\src\\asset\\texture\\meteor_Base_Color.png";

    // std::string frog_obj_path = "../../src/asset/obj/ranita.obj";
    std::string frog_obj_path = "..\\..\\src\\asset\\obj\\ranita.obj";
    // std::string frog_texture_path = "../../src/asset/texture/frog.png";
    std::string frog_texture_path = "..\\..\\src\\asset\\texture\\frog.png";

#else
    // std::string cube_obj_path = "../../src/asset/obj/cube.obj";
    std::string cube_obj_path = "..\\..\\src\\asset\\obj\\cube.obj";
    
    // std::string madara_obj_path = "../../src/asset/obj/Madara_Uchiha.obj";
    std::string madara_obj_path = "..\\..\\src\\asset\\obj\\Madara_Uchiha.obj";
    // std::string madara_texture_path = "../../src/asset/texture/_Madara_texture_main_mAIN.png";
    std::string madara_texture_path = "..\\..\\src\\asset\\texture\\_Madara_texture_main_mAIN.png";

    // std::string portal_obj_path = "../../src/asset/obj/portal.obj";
    std::string portal_obj_path = "..\\..\\src\\asset\\obj\\portal.obj";
    // std::string portal_texture_path = "../../src/asset/texture/portal.png";
    std::string portal_texture_path = "..\\..\\src\\asset\\texture\\portal.png";

    // std::string meteor_obj_path = "../../src/asset/obj/Meteor_mp.obj";
    std::string meteor_obj_path = "..\\..\\src\\asset\\obj\\Meteor_mp.obj";
    // std::string meteor_base_color_path = "../../src/asset/texture/meteor_Base_Color.png";
    std::string meteor_base_color_path = "..\\..\\src\\asset\\texture\\meteor_Base_Color.png";

    // std::string frog_obj_path = "../../src/asset/obj/ranita.obj";  // 使用ranita.obj（青蛙模型）
    std::string frog_obj_path = "..\\..\\src\\asset\\obj\\ranita.obj";  // 使用ranita.obj（青蛙模型）
    // std::string frog_texture_path = "../../src/asset/texture/frog.png";
    std::string frog_texture_path = "..\\..\\src\\asset\\texture\\frog.png";

#endif
    cubeModel = new Object(cube_obj_path);

    // 已更改變數名稱, 需要加obj這邊都要改, 全域變數新增請參照上面~75行處

    // load marada
    maradaModel = new Object(madara_obj_path);
    maradaModel->loadTexture(madara_texture_path);

    maradaMatrix = glm::mat4(1.0f);
    maradaMatrix = glm::translate(maradaMatrix, glm::vec3(0.0f, -50.0f, 0.0f));
    maradaMatrix = glm::scale(maradaMatrix, glm::vec3(50.0f));

    // load portal
    portalModel = new Object(portal_obj_path);
    portalModel->loadTexture(portal_texture_path);

    portalMatrix = glm::mat4(1.0f);

    // load meteor
    meteorModel = new Object(meteor_obj_path);
    meteorModel->loadTexture(meteor_base_color_path); 

    // load frog
    frogModel = new Object(frog_obj_path);
    
    glGenTextures(1, &frogTexture);
    glBindTexture(GL_TEXTURE_2D, frogTexture);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    
    int width, height, nrChannels;
    stbi_set_flip_vertically_on_load(true);
    unsigned char *data = stbi_load(frog_texture_path.c_str(), &width, &height, &nrChannels, 0);
    if (data) {
        GLenum format = GL_RGB;
        if (nrChannels == 1) format = GL_RED;
        else if (nrChannels == 3) format = GL_RGB;
        else if (nrChannels == 4) format = GL_RGBA;
        
        glTexImage2D(GL_TEXTURE_2D, 0, format, width, height, 0, format, GL_UNSIGNED_BYTE, data);
        glGenerateMipmap(GL_TEXTURE_2D);
    } else {
        std::cerr << "Failed to load frog texture: " << frog_texture_path << std::endl;
    }
    stbi_image_free(data);
    
}

void camera_setup(){
    camera.worldUp = glm::vec3(0.0f, 1.0f, 0.0f);
    camera.yaw = 90.0f;
    camera.pitch = 10.0f;
    camera.radius = 400.0f;
    camera.minRadius = 150.0f;
    camera.maxRadius = 800.0f;
    camera.orbitRotateSpeed = 60.0f;
    camera.orbitZoomSpeed = 400.0f;
    camera.minOrbitPitch = -80.0f;
    camera.maxOrbitPitch = 80.0f;
    camera.target = glm::vec3(0.0f);
    camera.enableAutoOrbit = false; // 關掉自動旋轉
    camera.autoOrbitSpeed = 20.0f;

    updateCamera();
}

void updateCamera(){
    float yawRad = glm::radians(camera.yaw);
    float pitchRad = glm::radians(camera.pitch);
    float cosPitch = cos(pitchRad);

    camera.position.x = camera.target.x + camera.radius * cosPitch * cos(yawRad);
    camera.position.y = camera.target.y + camera.radius * sin(pitchRad);
    camera.position.z = camera.target.z + camera.radius * cosPitch * sin(yawRad);

    camera.front = glm::normalize(camera.target - camera.position);
    camera.right = glm::normalize(glm::cross(camera.front, camera.worldUp));
    camera.up = glm::normalize(glm::cross(camera.right, camera.front));
}

void applyOrbitDelta(float yawDelta, float pitchDelta, float radiusDelta) {
    camera.yaw += yawDelta;
    camera.pitch = glm::clamp(camera.pitch + pitchDelta, camera.minOrbitPitch, camera.maxOrbitPitch);
    camera.radius = glm::clamp(camera.radius + radiusDelta, camera.minRadius, camera.maxRadius);
    updateCamera();
}

void light_setup(){
    light.position = glm::vec3(1000.0, 1000.0, 0.0);
    light.ambient = glm::vec3(1.0);
    light.diffuse = glm::vec3(1.0);
    light.specular = glm::vec3(1.0);
}

void material_setup(){
    material.ambient = glm::vec3(0.5);
    material.diffuse = glm::vec3(1.0);
    material.specular = glm::vec3(0.7);
    material.gloss = 50.0;
}

float randomFloat(float min, float max) {
    return min + static_cast<float>(rand()) / (static_cast<float>(RAND_MAX / (max - min)));
}

void snowflake_setup() {
    srand(static_cast<unsigned int>(std::time(nullptr)));
    
    // Initialize snowflake particles
    snowflakes.resize(SNOWFLAKE_COUNT);
    for (int i = 0; i < SNOWFLAKE_COUNT; i++) {
        snowflakes[i].position = glm::vec3(
            randomFloat(-SNOW_AREA_SIZE / 2, SNOW_AREA_SIZE / 2),
            randomFloat(SNOW_HEIGHT_MIN, SNOW_HEIGHT_MAX),
            randomFloat(-SNOW_AREA_SIZE / 2, SNOW_AREA_SIZE / 2)
        );
        snowflakes[i].size = randomFloat(2.0f, 6.0f);
        snowflakes[i].rotation = randomFloat(0.0f, 6.28318f);
        snowflakes[i].fallSpeed = randomFloat(20.0f, 60.0f);
        snowflakes[i].swayPhase = randomFloat(0.0f, 6.28318f);
    }
    
    // Create VAO and VBO
    glGenVertexArrays(1, &snowflakeVAO);
    glGenBuffers(1, &snowflakeVBO);
    
    glBindVertexArray(snowflakeVAO);
    glBindBuffer(GL_ARRAY_BUFFER, snowflakeVBO);
    
    // Each snowflake has: position(3) + size(1) + rotation(1) = 5 floats
    glBufferData(GL_ARRAY_BUFFER, SNOWFLAKE_COUNT * 5 * sizeof(float), nullptr, GL_DYNAMIC_DRAW);
    
    // position
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)0);
    
    // size
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1, 1, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)(3 * sizeof(float)));
    
    // rotation
    glEnableVertexAttribArray(2);
    glVertexAttribPointer(2, 1, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)(4 * sizeof(float)));
    
    glBindVertexArray(0);
    
    // Create shader program
#if defined(__linux__) || defined(__APPLE__)
    // std::string shaderDir = "../../src/shaders/";
    std::string shaderDir = "..\\..\\src\\shaders\\";
#else
    // std::string shaderDir = "../../src/shaders/";
    std::string shaderDir = "..\\..\\src\\shaders\\";
#endif
    
    std::string vpath = shaderDir + "snowflake.vert";
    std::string gpath = shaderDir + "snowflake.geom";
    std::string fpath = shaderDir + "snowflake.frag";
    
    snowflakeShader = new shader_program_t();
    snowflakeShader->create();
    snowflakeShader->add_shader(vpath, GL_VERTEX_SHADER);
    snowflakeShader->add_shader(gpath, GL_GEOMETRY_SHADER);
    snowflakeShader->add_shader(fpath, GL_FRAGMENT_SHADER);
    snowflakeShader->link_shader();
}

void snowflake_update() {
    if (!snowflakeEnabled) return;
    
    for (int i = 0; i < SNOWFLAKE_COUNT; i++) {
        // Snowflake falling
        snowflakes[i].position.y -= snowflakes[i].fallSpeed * deltaTime;
        
        // Horizontal swaying effect
        float sway = sin(currentTime * 2.0f + snowflakes[i].swayPhase) * 15.0f * deltaTime;
        snowflakes[i].position.x += sway;
        
        // If snowflake falls below ground, respawn from top
        if (snowflakes[i].position.y < SNOW_HEIGHT_MIN) {
            snowflakes[i].position.y = SNOW_HEIGHT_MAX;
            snowflakes[i].position.x = randomFloat(-SNOW_AREA_SIZE / 2, SNOW_AREA_SIZE / 2);
            snowflakes[i].position.z = randomFloat(-SNOW_AREA_SIZE / 2, SNOW_AREA_SIZE / 2);
        }
    }
    
    // Update VBO data
    std::vector<float> data(SNOWFLAKE_COUNT * 5);
    for (int i = 0; i < SNOWFLAKE_COUNT; i++) {
        data[i * 5 + 0] = snowflakes[i].position.x;
        data[i * 5 + 1] = snowflakes[i].position.y;
        data[i * 5 + 2] = snowflakes[i].position.z;
        data[i * 5 + 3] = snowflakes[i].size;
        data[i * 5 + 4] = snowflakes[i].rotation;
    }
    
    glBindBuffer(GL_ARRAY_BUFFER, snowflakeVBO);
    glBufferSubData(GL_ARRAY_BUFFER, 0, data.size() * sizeof(float), data.data());
}

void renderSnowflakes(const glm::mat4& view, const glm::mat4& projection) {
    if (!snowflakeEnabled || snowflakeShader == nullptr) return;
    
    // Enable blending for transparency effect
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glDepthMask(GL_FALSE);  // Disable depth write to avoid transparency occlusion issues
    
    snowflakeShader->use();
    snowflakeShader->set_uniform_value("view", view);
    snowflakeShader->set_uniform_value("projection", projection);
    snowflakeShader->set_uniform_value("time", currentTime);
    
    glBindVertexArray(snowflakeVAO);
    glDrawArrays(GL_POINTS, 0, SNOWFLAKE_COUNT);
    glBindVertexArray(0);
    
    snowflakeShader->release();
    
    glDepthMask(GL_TRUE);
    glDisable(GL_BLEND);
}

// 渲染青蛙
void renderFrog(const glm::mat4& view, const glm::mat4& projection) {
    if (!showFrog || frogShader == nullptr || frogModel == nullptr) {
        return;
    }
    
    frogShader->use();
    
    // 設置shader需要的變數
    frogShader->set_uniform_value("view", view);
    frogShader->set_uniform_value("projection", projection);
    frogShader->set_uniform_value("viewPos", camera.position - glm::vec3(0.0f, 0.2f, 0.1f));
    frogShader->set_uniform_value("time", (float)glfwGetTime());
    
    // 設置光照
    frogShader->set_uniform_value("light.position", light.position);
    frogShader->set_uniform_value("light.ambient", light.ambient);
    frogShader->set_uniform_value("light.diffuse", light.diffuse);
    frogShader->set_uniform_value("light.specular", light.specular);
    
    // 設置材質
    frogShader->set_uniform_value("material.ambient", glm::vec3(1.0f));
    frogShader->set_uniform_value("material.diffuse", glm::vec3(1.0f)); // 白色，不影響貼圖顏色
    frogShader->set_uniform_value("material.specular", glm::vec3(0.3f));
    frogShader->set_uniform_value("material.gloss", 20.0f);
    
    // 綁定青蛙紋理
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, frogTexture);
    frogShader->set_uniform_value("frogTexture", 0);
    
    // 渲染青蛙
    glm::mat4 currentFrogMatrix = glm::mat4(1.0f);
    // 調整青蛙位置，使其融入隕石中心
    glm::vec3 adjustedFrogPosition = frogPosition + glm::vec3(0.0f, -70.0f, 0.0f);
    currentFrogMatrix = glm::translate(currentFrogMatrix, adjustedFrogPosition);
    currentFrogMatrix = glm::rotate(currentFrogMatrix, glm::radians(180.0f), glm::vec3(0.0f, 1.0f, 0.0f));
    
    // 青蛙生長動畫
    float frogScale = 0.1f;
    if (meteorExplosionProgress > 0.0f) {
        frogScale = glm::mix(0.1f, 15.0f, meteorExplosionProgress);
    }
    currentFrogMatrix = glm::scale(currentFrogMatrix, glm::vec3(frogScale));
    
    frogShader->set_uniform_value("model", currentFrogMatrix);
    frogModel->draw();
    
    frogShader->release();
}

void shader_setup(){
#if defined(__linux__) || defined(__APPLE__)
    // std::string shaderDir = "../../src/shaders/";
    std::string shaderDir = "..\\..\\src\\shaders\\";
#else
    // std::string shaderDir = "../../src/shaders/";
    std::string shaderDir = "..\\..\\src\\shaders\\";
#endif

    std::vector<std::string> shadingMethod = {
        "default", "bling-phong", "gouraud", "metallic", "glass_schlick", "Toon"
    };

    for(int i=0; i<shadingMethod.size(); i++){
        std::string vpath = shaderDir + shadingMethod[i] + ".vert";
        std::string fpath = shaderDir + shadingMethod[i] + ".frag";

        shader_program_t* shaderProgram = new shader_program_t();
        shaderProgram->create();
        shaderProgram->add_shader(vpath, GL_VERTEX_SHADER);
        shaderProgram->add_shader(fpath, GL_FRAGMENT_SHADER);
        shaderProgram->link_shader();
        shaderPrograms.push_back(shaderProgram);
    }
}

void shader_setup_w_geometry_shader(){
    #if defined(__linux__) || defined(__APPLE__)
        // std::string shaderDir = "../../src/shaders/";
        std::string shaderDir = "..\\..\\src\\shaders\\";
    #else
        // std::string shaderDir = "../../src/shaders/";
        std::string shaderDir = "..\\..\\src\\shaders\\";
    #endif

    // 目前暫時做的marada被炸成稀巴爛shader
    std::vector<std::string> shadingMethod = {
        "default", "explode"
    };

    for(int i=0; i<shadingMethod.size(); i++){
        std::string vpath = shaderDir + shadingMethod[i] + ".vert";
        std::string fpath = shaderDir + shadingMethod[i] + ".frag";
        std::string gpath = shaderDir + shadingMethod[i] + ".geom";

        shader_program_t* shaderProgram = new shader_program_t();
        shaderProgram->create();
        shaderProgram->add_shader(vpath, GL_VERTEX_SHADER);
        shaderProgram->add_shader(fpath, GL_FRAGMENT_SHADER);
        shaderProgram->add_shader(gpath, GL_GEOMETRY_SHADER);
        shaderProgram->link_shader();
        shaderPrograms.push_back(shaderProgram);
    }

    /* 
        NOTE :
        如果後續覺得這樣有點亂那看你們要不要把各個obj都自己一個shader_setup function
    */

    // portal的shader
    std::string portal_vpath = shaderDir + "portal.vert";
    std::string portal_fpath = shaderDir + "portal.frag"; 

    portalShader = new shader_program_t();
    portalShader->create();
    portalShader->add_shader(portal_vpath, GL_VERTEX_SHADER);
    portalShader->add_shader(portal_fpath, GL_FRAGMENT_SHADER);
    portalShader->link_shader();

    // meteor的shader - 隕石爆炸效果shader（使用geometry shader）
    std::string meteor_vpath = shaderDir + "meteor.vert";
    std::string meteor_gpath = shaderDir + "meteor.geom";
    std::string meteor_fpath = shaderDir + "meteor.frag";

    meteorShader = new shader_program_t();
    meteorShader->create();
    meteorShader->add_shader(meteor_vpath, GL_VERTEX_SHADER);
    meteorShader->add_shader(meteor_gpath, GL_GEOMETRY_SHADER);
    meteorShader->add_shader(meteor_fpath, GL_FRAGMENT_SHADER);
    meteorShader->link_shader();

    // frog的shader
    std::string frog_vpath = shaderDir + "frog.vert";
    std::string frog_fpath = shaderDir + "frog.frag";

    frogShader = new shader_program_t();
    frogShader->create();
    frogShader->add_shader(frog_vpath, GL_VERTEX_SHADER);
    frogShader->add_shader(frog_fpath, GL_FRAGMENT_SHADER);
    frogShader->link_shader();
}

void cubemap_setup(){
#if defined(__linux__) || defined(__APPLE__)
    // std::string cubemapDir = "../../src/asset/texture/skybox/";
    std::string cubemapDir = "..\\..\\src\\asset\\texture\\skybox\\";
    // std::string shaderDir = "../../src/shaders/";
    std::string shaderDir = "..\\..\\src\\shaders\\";
#else
    // std::string cubemapDir = "../../src/asset/texture/skybox/";
    std::string cubemapDir = "..\\..\\src\\asset\\texture\\skybox\\";
    // std::string shaderDir = "../../src/shaders/";
    std::string shaderDir = "..\\..\\src\\shaders\\";
#endif

    std::vector<std::string> faces
    {
        cubemapDir + "right.jpg",
        cubemapDir + "left.jpg",
        cubemapDir + "top.jpg",
        cubemapDir + "bottom.jpg",
        cubemapDir + "front.jpg",
        cubemapDir + "back.jpg"
    };
    cubemapTexture = loadCubemap(faces);

    std::string vpath = shaderDir + "cubemap.vert";
    std::string fpath = shaderDir + "cubemap.frag";
    
    cubemapShader = new shader_program_t();
    cubemapShader->create();
    cubemapShader->add_shader(vpath, GL_VERTEX_SHADER);
    cubemapShader->add_shader(fpath, GL_FRAGMENT_SHADER);
    cubemapShader->link_shader();

    glGenVertexArrays(1, &cubemapVAO);
    glGenBuffers(1, &cubemapVBO);
    glBindVertexArray(cubemapVAO);
    glBindBuffer(GL_ARRAY_BUFFER, cubemapVBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(cubemapVertices), &cubemapVertices, GL_STATIC_DRAW);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
    glBindVertexArray(0);
}

void setup(){
    light_setup();
    model_setup();
    // shader_setup();
    shader_setup_w_geometry_shader();
    camera_setup();
    cubemap_setup();
    material_setup();
    snowflake_setup();

    glEnable(GL_DEPTH_TEST);
    glDepthFunc(GL_LEQUAL);
    glEnable(GL_CULL_FACE);
    glFrontFace(GL_CCW);
    glCullFace(GL_BACK);
}

//meteor animation
void updateMeteorAnimation() {
    if (!showMeteor) return;

    meteorTimer += deltaTime;

    if (meteorTimer < METEOR_FALL_DURATION) {
        if (!showFrog) {
            showFrog = true;
        }
        
        meteorFallProgress = meteorTimer / METEOR_FALL_DURATION;
        
        float startY = portalPosition.y;
        float endY = METEOR_GROUND_Y;

        float t = meteorFallProgress;
        float height = startY + (endY - startY) * t * t;
        
        meteorPosition = glm::vec3(
            portalPosition.x,
            height - 100.0f,
            portalPosition.z - 80.0f
        );
        
        // 掉落時隕石旋轉
        meteorMatrix = glm::mat4(1.0f);
        meteorMatrix = glm::translate(meteorMatrix, meteorPosition);
        meteorMatrix = glm::rotate(meteorMatrix, meteorTimer * 3.0f, glm::vec3(1.0f, 1.0f, 0.0f));
        meteorMatrix = glm::scale(meteorMatrix, glm::vec3(100.0f));
        
        frogPosition = meteorPosition;
        
        meteorExplosionProgress = 0.0f;
    }
    //爆炸階段
    else if (meteorTimer < METEOR_FALL_DURATION + METEOR_EXPLOSION_DURATION) {
        float explosionTimer = meteorTimer - METEOR_FALL_DURATION;
        meteorExplosionProgress = explosionTimer / METEOR_EXPLOSION_DURATION;
        
        meteorPosition.y = METEOR_GROUND_Y;
        
        meteorMatrix = glm::mat4(1.0f);
        meteorMatrix = glm::translate(meteorMatrix, meteorPosition);
        meteorMatrix = glm::scale(meteorMatrix, glm::vec3(100.0f));
        
        frogPosition = meteorPosition;
    }
    //動畫結束
    else {
        showMeteor = false;
        meteorExplosionProgress = 1.0f;
    }
}

// 處理portal animation
void updatePortalAnimation() {
    if (showPortal) {
        // update timer
        if (portalTimer < PORTAL_ANIMATION_DURATION) {
            portalTimer += deltaTime;
        }
        float t = glm::clamp(portalTimer / PORTAL_ANIMATION_DURATION, 0.0f, 1.0f);

        // fast to slow cool math (idk bro)
        float easeT = (t == 1.0f) ? 1.0f : 1.0f - pow(2.0f, -10.0f * t);
        currentProgress = easeT;
        portalScale = PORTAL_TARGET_SCALE * easeT;

        currentSpinSpeed = glm::mix(MAX_SPIN_SPEED, MIN_SPIN_SPEED, easeT);
        
        // 累積旋轉角度
        portalRotation += currentSpinSpeed * deltaTime;
        if (portalRotation > 360.0f) portalRotation -= 360.0f;

    } else {
        portalTimer = 0.0f;
        portalScale = 1.0f;
        portalRotation = 0.0f;
        currentProgress = 0.0f;
    }
}

void update(){
    currentTime = glfwGetTime();
    deltaTime = currentTime - lastFrame;
    lastFrame = currentTime;

    if (camera.enableAutoOrbit) {
        float yawDelta = camera.autoOrbitSpeed * deltaTime;
        applyOrbitDelta(yawDelta, 0.0f, 0.0f);
    }
    
    snowflake_update();

    // 同時更新portal位置 選轉 大小等等資料
    updatePortalAnimation();
    
    // 更新隕石動畫
    updateMeteorAnimation();
}

void render(){
    glClearColor(0.0, 0.0, 0.0, 1.0);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    glm::mat4 view = glm::lookAt(camera.position - glm::vec3(0.0f, 0.2f, 0.1f), camera.position + camera.front, camera.up);
    float aspect = (SCR_HEIGHT > 0) ? (float)SCR_WIDTH / (float)SCR_HEIGHT : 1.0f;
    glm::mat4 projection = glm::perspective(glm::radians(45.0f), aspect, 0.1f, 5000.0f); // 1000 -> 5000避免被obj被卡掉

    // set matrix for view, projection, model transformation
    shaderPrograms[shaderProgramIndex]->use();
    shaderPrograms[shaderProgramIndex]->set_uniform_value("model", maradaMatrix);
    shaderPrograms[shaderProgramIndex]->set_uniform_value("view", view);
    shaderPrograms[shaderProgramIndex]->set_uniform_value("projection", projection);
    shaderPrograms[shaderProgramIndex]->set_uniform_value("viewPos", camera.position - glm::vec3(0.0f, 0.2f, 0.1f));

    shaderPrograms[shaderProgramIndex]->set_uniform_value("time", (float)glfwGetTime());
    // TODO: set additional uniform value for shader program

    shaderPrograms[shaderProgramIndex]->set_uniform_value("light.position", light.position);
    shaderPrograms[shaderProgramIndex]->set_uniform_value("light.ambient",  light.ambient);
    shaderPrograms[shaderProgramIndex]->set_uniform_value("light.diffuse",  light.diffuse);
    shaderPrograms[shaderProgramIndex]->set_uniform_value("light.specular", light.specular);

    shaderPrograms[shaderProgramIndex]->set_uniform_value("material.ambient",  material.ambient);
    shaderPrograms[shaderProgramIndex]->set_uniform_value("material.diffuse",  material.diffuse);
    shaderPrograms[shaderProgramIndex]->set_uniform_value("material.specular", material.specular);
    shaderPrograms[shaderProgramIndex]->set_uniform_value("material.gloss",    material.gloss);

    // specifying sampler for shader program

    glActiveTexture(GL_TEXTURE0);
    shaderPrograms[shaderProgramIndex]->set_uniform_value("objectTexture", 0); // object texture

    glActiveTexture(GL_TEXTURE1);
    glBindTexture(GL_TEXTURE_CUBE_MAP, cubemapTexture);
    shaderPrograms[shaderProgramIndex]->set_uniform_value("skybox", 1); // Set cubemap texture for reflection

    // Draw character model
    if(isCube)
        cubeModel->draw();
    else
        maradaModel->draw();


    // 如果觸發顯示portal -> draw
    // transformation 要在這邊處理!!!
    // ***記得要用正確的shader 
    // 直接用上面的shaderPrograms[shaderProgramIndex]的話會跟marada用到一樣的 (就是會爆炸的意思)

    if (showPortal) {
        portalShader->use();

        // 傳該用的variables進去
        portalShader->set_uniform_value("view", view);
        portalShader->set_uniform_value("projection", projection);
        portalShader->set_uniform_value("viewPos", camera.position - glm::vec3(0.0f, 0.2f, 0.1f));
        portalShader->set_uniform_value("time", (float)glfwGetTime());
        portalShader->set_uniform_value("progress", currentProgress);
        // transformation
        glm::mat4 currentPortalMatrix = glm::mat4(1.0f);
        // 移到初始位置
        currentPortalMatrix = glm::translate(currentPortalMatrix, portalPosition);

        // 傾斜portal
        currentPortalMatrix = glm::rotate(currentPortalMatrix, glm::radians(-160.0f), glm::vec3(1.0f, 0.0f, 0.0f));

        // rotation
        currentPortalMatrix = glm::rotate(currentPortalMatrix, glm::radians(portalRotation), glm::vec3(0.0f, 1.0f, 0.0f));

        // 放大
        currentPortalMatrix = glm::scale(currentPortalMatrix, glm::vec3(portalScale));

        portalShader->set_uniform_value("model", currentPortalMatrix);
        glActiveTexture(GL_TEXTURE0);
        portalShader->set_uniform_value("objectTexture", 0);
        portalModel->draw();

        portalShader->release();
    }

    // 渲染隕石（如果觸發了隕石動畫）
    if (showMeteor && meteorShader != nullptr) {
        meteorShader->use();
        
        // 設置shader需要的變數
        meteorShader->set_uniform_value("view", view);
        meteorShader->set_uniform_value("projection", projection);
        meteorShader->set_uniform_value("viewPos", camera.position - glm::vec3(0.0f, 0.2f, 0.1f));
        meteorShader->set_uniform_value("time", (float)glfwGetTime());
        meteorShader->set_uniform_value("explosionProgress", meteorExplosionProgress);
        
        // 設置光照
        meteorShader->set_uniform_value("light.position", light.position);
        meteorShader->set_uniform_value("light.ambient", light.ambient);
        meteorShader->set_uniform_value("light.diffuse", light.diffuse);
        meteorShader->set_uniform_value("light.specular", light.specular);
        
        meteorShader->set_uniform_value("model", meteorMatrix);
        
        glActiveTexture(GL_TEXTURE0);
        meteorShader->set_uniform_value("objectTexture", 0);
        meteorModel->draw();
        
        meteorShader->release();
    }

    // 先shade青蛙，使其被隕石蓋掉
    renderFrog(view, projection);
    
    if (showMeteor && meteorShader && meteorModel && meteorTimer < METEOR_FALL_DURATION) {
        meteorShader->use();
        meteorShader->set_uniform_value("view", view);
        meteorShader->set_uniform_value("projection", projection);
        meteorShader->set_uniform_value("viewPos", camera.position);
        meteorShader->set_uniform_value("explosionProgress", 0.0f);
        
        meteorShader->set_uniform_value("material.ambient", material.ambient);
        meteorShader->set_uniform_value("material.diffuse", material.diffuse);
        meteorShader->set_uniform_value("material.specular", material.specular);
        meteorShader->set_uniform_value("material.gloss", material.gloss);
        
        meteorShader->set_uniform_value("light.position", light.position);
        meteorShader->set_uniform_value("light.ambient", light.ambient);
        meteorShader->set_uniform_value("light.diffuse", light.diffuse);
        meteorShader->set_uniform_value("light.specular", light.specular);
        
        meteorShader->set_uniform_value("model", meteorMatrix);
        
        glActiveTexture(GL_TEXTURE0);
        meteorShader->set_uniform_value("objectTexture", 0);
        meteorModel->draw();
        
        meteorShader->release();
    }

    shaderPrograms[shaderProgramIndex]->release();

    // TODO 
    // Rendering cubemap environment
    // Hint:
    // 1. All the needed things are already set up in cubemap_setup() function.
    // 2. You can use the vertices in cubemapVertices provided in the header/cube.h
    // 3. You can use the cubemapShader to render the cubemap 
    //    (refer to the above code to get an idea of how to use the shader program)

    glDepthFunc(GL_LEQUAL); // draw equal depth (=1), let cubemap can be always the max depth (=1)
    cubemapShader->use();
    
    glm::mat4 viewSkybox = glm::mat4(glm::mat3(view)); // remove camera translation, let cubemap stay around the camera
    
    cubemapShader->set_uniform_value("view", viewSkybox);
    cubemapShader->set_uniform_value("projection", projection);
    
    glBindVertexArray(cubemapVAO);
    
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_CUBE_MAP, cubemapTexture);
    cubemapShader->set_uniform_value("skybox", 0);
    
    // cubemapVertices is typically the vertex count of a cube (6 faces * 2 triangles * 3 vertices = 36)
    glDrawArrays(GL_TRIANGLES, 0, 36);
    
    glBindVertexArray(0);
    
    glDepthFunc(GL_LESS);
    
    // Render snowflakes
    renderSnowflakes(view, projection);
}

int main() {
    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

#ifdef __APPLE__
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
#endif

    GLFWwindow *window = glfwCreateWindow(SCR_WIDTH, SCR_HEIGHT, "HW3-Static Model", NULL, NULL);
    if (window == NULL) {
        std::cerr << "Failed to create GLFW window" << std::endl;
        glfwTerminate();
        return -1;
    }
    glfwMakeContextCurrent(window);
    glfwSetFramebufferSizeCallback(window, framebufferSizeCallback);
    glfwSetKeyCallback(window, keyCallback);
    glfwSwapInterval(1);

    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
        std::cout << "Failed to initialize GLAD" << std::endl;
        return -1;
    }

    glfwGetFramebufferSize(window, &SCR_WIDTH, &SCR_HEIGHT);
    glViewport(0, 0, SCR_WIDTH, SCR_HEIGHT);

    setup();
    
    while (!glfwWindowShouldClose(window)) {
        processInput(window);
        update(); 
        render(); 
        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    // 記得delete model
    delete maradaModel;
    delete portalModel; 
    delete cubeModel;
    delete meteorModel;
    delete frogModel;
    
    for (auto shader : shaderPrograms) {
        delete shader;
    }
    delete cubemapShader;
    delete snowflakeShader;
    delete portalShader;
    delete meteorShader;
    delete frogShader;
    
    // 清理青蛙紋理
    if (frogTexture != 0) {
        glDeleteTextures(1, &frogTexture);
    }
    
    glDeleteVertexArrays(1, &snowflakeVAO);
    glDeleteBuffers(1, &snowflakeVBO);

    glfwTerminate();
    return 0;
}

void processInput(GLFWwindow *window) {
    if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
        glfwSetWindowShouldClose(window, true);

    glm::vec2 orbitInput(0.0f);
    float zoomInput = 0.0f;

    if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
        orbitInput.x += 1.0f;
    if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
        orbitInput.x -= 1.0f;
    if (glfwGetKey(window, GLFW_KEY_SPACE) == GLFW_PRESS)
        orbitInput.y += 1.0f;
    if (glfwGetKey(window, GLFW_KEY_LEFT_SHIFT) == GLFW_PRESS)
        orbitInput.y -= 1.0f;
    if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
        zoomInput -= 1.0f;
    if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
        zoomInput += 1.0f;

    if (orbitInput.x != 0.0f || orbitInput.y != 0.0f || zoomInput != 0.0f) {
        float yawDelta = orbitInput.x * camera.orbitRotateSpeed * deltaTime;
        float pitchDelta = orbitInput.y * camera.orbitRotateSpeed * deltaTime;
        float radiusDelta = zoomInput * camera.orbitZoomSpeed * deltaTime;
        applyOrbitDelta(yawDelta, pitchDelta, radiusDelta);
    }
}

void keyCallback(GLFWwindow *window, int key, int scancode, int action, int mods) {
    if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS)
        glfwSetWindowShouldClose(window, true);

    if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS)
        glfwSetWindowShouldClose(window, true);

    if (key == GLFW_KEY_P && action == GLFW_PRESS)
        showPortal = !showPortal;

    if (key == GLFW_KEY_N && action == GLFW_PRESS)
        snowflakeEnabled = !snowflakeEnabled;

    if (key == GLFW_KEY_M && action == GLFW_PRESS) {
        // 按下M鍵觸發隕石從portal掉落
        if (!showMeteor) {
            showMeteor = true;
            meteorTimer = 0.0f;
            meteorFallProgress = 0.0f;
            meteorExplosionProgress = 0.0f;
            showFrog = false;
        }
    }

    if (key == GLFW_KEY_0 && (action == GLFW_REPEAT || action == GLFW_PRESS)) 
        shaderProgramIndex = 0;
    if (key == GLFW_KEY_1 && (action == GLFW_REPEAT || action == GLFW_PRESS)) 
        shaderProgramIndex = 1;


    // 暫時移除下面的 不然按到會crash

    // if (key == GLFW_KEY_2 && (action == GLFW_REPEAT || action == GLFW_PRESS)) 
    //     shaderProgramIndex = 2;
    // if (key == GLFW_KEY_3 && action == GLFW_PRESS)
    //     shaderProgramIndex = 3;
    // if (key == GLFW_KEY_4 && action == GLFW_PRESS)
    //     shaderProgramIndex = 4;
    // if (key == GLFW_KEY_5 && action == GLFW_PRESS)
    //     shaderProgramIndex = 5;
    // if (key == GLFW_KEY_6 && action == GLFW_PRESS)
    //     shaderProgramIndex = 6;
    // if (key == GLFW_KEY_7 && action == GLFW_PRESS)
    //     shaderProgramIndex = 7;
    // if (key == GLFW_KEY_8 && action == GLFW_PRESS)
    //     shaderProgramIndex = 8;
    // if( key == GLFW_KEY_9 && action == GLFW_PRESS)
    //     isCube = !isCube;
}

void framebufferSizeCallback(GLFWwindow *window, int width, int height) {
    glViewport(0, 0, width, height);
    SCR_WIDTH = width;
    SCR_HEIGHT = height;
}

unsigned int loadCubemap(vector<std::string>& faces)
{
    unsigned int texture;
    glGenTextures(1, &texture);
    glBindTexture(GL_TEXTURE_CUBE_MAP, texture);

    int width, height, nrChannels;
    for (unsigned int i = 0; i < faces.size(); i++)
    {
        stbi_set_flip_vertically_on_load(false);
        unsigned char *data = stbi_load(faces[i].c_str(), &width, &height, &nrChannels, 0);
        if (data)
        {
            glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 
                         0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data
            );
            stbi_image_free(data);
        }
        else
        {
            std::cout << "Cubemap tex failed to load at path: " << faces[i] << std::endl;
            stbi_image_free(data);
        }
    }
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);

    return texture;
}  
