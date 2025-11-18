#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp> // 為了 glm::translate, glm::rotate, glm::scale, glm::lookAt, glm::perspective
#include <glm/gtc/type_ptr.hpp>

#include <iostream>
#include <fstream>
#include <sstream>
#include <string>

#include "./header/Object.h"
#define STB_IMAGE_IMPLEMENTATION // 確保 stb_image.h 在這裡實作
#include "./header/stb_image.h"

using namespace std;

void framebufferSizeCallback(GLFWwindow *window, int width, int height);
void keyCallback(GLFWwindow *window, int key, int scancode, int action, int mods);
unsigned int createShader(const string &filename, const string &type);
unsigned int createProgram(unsigned int vertexShader, unsigned int fragmentShader);
unsigned int modelVAO(Object &model);
unsigned int loadTexture(const string &filename); // 修改參數類型以匹配 init() 中的呼叫
void init();

// settings
int SCR_WIDTH = 800;
int SCR_HEIGHT = 600;

// Shader
unsigned int vertexShader, fragmentShader, shaderProgram;

// Texture
unsigned int groundTexture, fishTexture, columnTexture;

// VAO, VBO
unsigned int groundVAO, fishVAO, columnVAO;

// Objects to display
Object *groundObject, *fishObject, *columnObject;

// Uniform locations
GLint modelLoc, viewLoc, projLoc;
GLint squeezeFactorLoc, breathingColorLoc, intensityLoc, isFishLoc, texLoc;

// Constants you may need
const int rotateColumnSpeed = 10;     // 柱子旋轉速度 (度/秒)
const int revolveFishSpeed = 20;    // 魚環繞速度 (度/秒)
const int rotateFishSpeed = 180;    // 魚自轉速度 (度/秒)
const float fishColumnDist = 3;     // 魚和柱子的距離
const float heightSpeed = 0.25;     // 魚上下游動速度
const int squeezeSpeed = 90;        // 魚擠壓速度 (度/秒)

// Variables you can use
float rotateColumnDegree = 0;   // 柱子Y軸旋轉角度
float revolveFishDegree = 0;    // 魚環繞柱子的Y軸角度
float rotateFishDegree = 0;     // 魚自轉的X軸角度
float fishColumnOffset = 0;     // (未使用，但保留)
int heightDir = 1;              // 魚游動方向 (1:上, -1:下)
float fishHeight = 0;           // 魚的Y軸高度

bool useSelfRotation = false;   // 是否啟用 'R' 鍵自轉
bool useSqueeze = false;        // 是否啟用 'S' 鍵擠壓
float squeezeFactor = 0;        // 擠壓因子 (角度)
bool useBreathing = false;      // 是否啟用 'B' 鍵呼吸
float intensity = 1.0;          // 呼吸光強度
glm::vec3 breathingColor = glm::vec3(1.0f, 1.0f, 1.0f); // 呼吸光顏色

int main() {
    // glfw: initialize and configure
    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

#ifdef __APPLE__
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
#endif
    /* TODO#0: Change window title to "HW2 - [your student id]"
     * Ex. HW2-112550000
     */
    // glfw window creation
    // 已根據骨架檔案要求填寫 "HW2-112550069" [cite: 366, 361]
    GLFWwindow *window = glfwCreateWindow(SCR_WIDTH, SCR_HEIGHT, "HW2-112550069", NULL, NULL);
    if (window == NULL) {
        std::cout << "Failed to create GLFW window" << std::endl;
        glfwTerminate();
        return -1;
    }
    glfwMakeContextCurrent(window);
    glfwSetFramebufferSizeCallback(window, framebufferSizeCallback);
    glfwSetKeyCallback(window, keyCallback);
    glfwSwapInterval(1);

    // glad: load all OpenGL function pointers
    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
        std::cout << "Failed to initialize GLAD" << std::endl;
        return -1;
    }

    // TODO#1: Finish function createShader [cite: 369]
    // TODO#2: Finish function createProgram [cite: 373]
    // TODO#3: Finish function modelVAO [cite: 370]
    // TODO#4: Finish function loadTexture [cite: 371]
    // (以上函數已在 main 下方實作)

    // Initialize Object, Shader, Texture, VAO, VBO
    init();

    // Enable depth test, face culling
    glEnable(GL_DEPTH_TEST);
    glDepthFunc(GL_LEQUAL);
    glEnable(GL_CULL_FACE);
    glFrontFace(GL_CCW);
    glCullFace(GL_BACK);

    // Set viewport
    glfwGetFramebufferSize(window, &SCR_WIDTH, &SCR_HEIGHT);
    glViewport(0, 0, SCR_WIDTH, SCR_HEIGHT);

    // render loop variables
    double dt;
    double lastTime = glfwGetTime();
    double currentTime;

    /* TODO#5: Data connection - Retrieve uniform variable locations
     * 1. Retrieve locations for model, view, and projection matrices.
     * 2. Retrieve locations for squeezeFactor, breathingColor, intensity, and other parameters.
     * Hint:
     * glGetUniformLocation 
     */
    // 獲取著色器中 uniform 變數的位置，以便後續傳遞數據
    modelLoc = glGetUniformLocation(shaderProgram, "model");
    viewLoc = glGetUniformLocation(shaderProgram, "view");
    projLoc = glGetUniformLocation(shaderProgram, "projection");
    squeezeFactorLoc = glGetUniformLocation(shaderProgram, "squeezeFactor");
    breathingColorLoc = glGetUniformLocation(shaderProgram, "breathingColor");
    intensityLoc = glGetUniformLocation(shaderProgram, "intensity");
    isFishLoc = glGetUniformLocation(shaderProgram, "isFish"); // 傳遞一個布林值(整數)告訴 shader 這是不是魚
    texLoc = glGetUniformLocation(shaderProgram, "Texture"); // 獲取紋理採樣器 "Texture" 的位置 [cite: 286, 290]


    // render loop
    while (!glfwWindowShouldClose(window)) {
        // render color of water
        glClearColor(0.15, 0.50, 0.65, 1.0);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        // 啟用著色器程式 [cite: 49, 192]
        glUseProgram(shaderProgram);

        // 設定視圖矩陣 (View Matrix) [cite: 190]
        glm::mat4 view = glm::lookAt(glm::vec3(0.0f, 8.5f, 13.0f), glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, 1.0f, 0.0f));
        // 設定投影矩陣 (Projection Matrix)
        glm::mat4 projection = glm::perspective(glm::radians(45.0f), (float)SCR_WIDTH / (float)SCR_HEIGHT, 0.1f, 1000.0f);
        
        // 傳遞 view 和 projection 矩陣 (這兩個矩陣在本次繪製中保持不變)
        glUniformMatrix4fv(viewLoc, 1, GL_FALSE, glm::value_ptr(view)); [cite: 193]
        glUniformMatrix4fv(projLoc, 1, GL_FALSE, glm::value_ptr(projection));

        // 初始化三個物件的模型矩陣
        glm::mat4 groundModel(1.0f), fishModel(1.0f), columnModel(1.0f);

        /* TODO#6-1: Render Ground 
         * 1. Set up ground model matrix.
         * 2. Send model, view, and projection matrices to the program.
         * 3. Send squeezeFactor, breathingColor, intensity, or other parameters to the program.
         * 4. Apply the texture, and render the ground.
         */
        
        // --- 繪製地面 ---
        groundModel = glm::mat4(1.0f);
        groundModel = glm::translate(groundModel, glm::vec3(0.0f, -5.0f, 8.0f)); // 7. 平移到指定位置 [cite: 315]
        groundModel = glm::scale(groundModel, glm::vec3(35.0f, 1.0f, 25.0f)); // 6. 縮放 [cite: 314]
        glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(groundModel)); // 5. 傳送模型矩陣
        
        // 4. 設定地面專用的 uniform (無特效)
        glUniform1f(squeezeFactorLoc, 0.0f);
        glUniform3f(breathingColorLoc, 1.0f, 1.0f, 1.0f);
        glUniform1f(intensityLoc, 1.0f);
        glUniform1i(isFishLoc, 0); // 3. 告知 shader 這不是魚
        
        glActiveTexture(GL_TEXTURE0); // 2. 啟用紋理單元 0 [cite: 275]
        glBindTexture(GL_TEXTURE_2D, groundTexture); // 1. 綁定地面紋理 [cite: 277]
        glUniform1i(texLoc, 0); // 0. 告訴 shader 紋理在單元 0
        
        glBindVertexArray(groundVAO); // 綁定地面的 VAO [cite: 180, 187]
        glDrawArrays(GL_TRIANGLES, 0, groundObject->positions.size()); // 繪製 [cite: 181, 188]
        

        /* TODO#6-2: Render Column 
         * 1. Set up column model matrix.
         * 2. Send model, view, and projection matrices to the program.
         * 3. Send squeezeFactor, breathingColor, intensity, or other parameters to the program.
         * 4. Apply the texture, and render the column.
         */
        
        // --- 繪製柱子 ---
        columnModel = glm::mat4(1.0f);
        columnModel = glm::translate(columnModel, glm::vec3(0.0f, -4.0f, 0.0f)); // 7. 平移到指定位置 [cite: 313]
        columnModel = glm::rotate(columnModel, glm::radians(rotateColumnDegree), glm::vec3(0.0f, 1.0f, 0.0f)); // 6. Y 軸自轉 
        columnModel = glm::rotate(columnModel, glm::radians(-90.0f), glm::vec3(1.0f, 0.0f, 0.0f)); // 5. X 軸旋轉-90度 [cite: 312]
        columnModel = glm::scale(columnModel, glm::vec3(0.05f, 0.05f, 0.05f)); // 4. 縮放 [cite: 311]
        glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(columnModel)); // 3. 傳送模型矩陣
        
        // 2. 設定柱子專用的 uniform (無特效)
        glUniform1f(squeezeFactorLoc, 0.0f);
        glUniform3f(breathingColorLoc, 1.0f, 1.0f, 1.0f);
        glUniform1f(intensityLoc, 1.0f);
        glUniform1i(isFishLoc, 0); // 1. 告知 shader 這不是魚
        
        glActiveTexture(GL_TEXTURE0); // 0. 啟用紋理單元 0
        glBindTexture(GL_TEXTURE_2D, columnTexture); // 綁定柱子紋理
        glUniform1i(texLoc, 0); // 告訴 shader 紋理在單元 0
        
        glBindVertexArray(columnVAO); // 綁定柱子的 VAO
        glDrawArrays(GL_TRIANGLES, 0, columnObject->positions.size()); // 繪製
        

        /* TODO#6-3: Render Fish 
         * 1. Set up fish model matrix.
         * 2. Send model, view, and projection matrices to the program.
         * 3. Send squeezeFactor, breathingColor, intensity, or other parameters to the program.
         * 4. Apply the texture, and render the fish.
         */

        // --- 繪製魚 ---
        // 矩陣變換順序與程式碼撰寫順序相反
        fishModel = glm::mat4(1.0f);
        // 7. 環繞柱子 (Y軸旋轉) 
        fishModel = glm::rotate(fishModel, glm::radians(revolveFishDegree), glm::vec3(0.0f, 1.0f, 0.0f));
        // 6. 放置到 (0, 0, 3) 位置 [cite: 310]，並加上下浮動 
        fishModel = glm::translate(fishModel, glm::vec3(0.0f, fishHeight, fishColumnDist));
        // 5. 'R' 鍵自轉 (-X 軸) 
        fishModel = glm::rotate(fishModel, glm::radians(rotateFishDegree), glm::vec3(-1.0f, 0.0f, 0.0f));
        // 4. 初始 X 軸旋轉-90度 [cite: 309]
        fishModel = glm::rotate(fishModel, glm::radians(-90.0f), glm::vec3(1.0f, 0.0f, 0.0f));
        // 3. 縮放 [cite: 309]
        fishModel = glm::scale(fishModel, glm::vec3(0.05f, 0.05f, 0.05f));
        glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(fishModel)); // 2. 傳送模型矩陣
        
        // 1. 設定魚專用的 uniform (有特效)
        glUniform1f(squeezeFactorLoc, glm::radians(squeezeFactor)); // 擠壓因子(轉為弧度) [cite: 329, 330]
        glUniform3fv(breathingColorLoc, 1, glm::value_ptr(breathingColor)); // 呼吸顏色 [cite: 336]
        glUniform1f(intensityLoc, intensity); // 呼吸強度 [cite: 336]
        glUniform1i(isFishLoc, 1); // 告知 shader 這是魚
        
        glActiveTexture(GL_TEXTURE0); // 0. 啟用紋理單元 0
        glBindTexture(GL_TEXTURE_2D, fishTexture); // 綁定魚的紋理
        glUniform1i(texLoc, 0); // 告訴 shader 紋理在單元 0
        
        glBindVertexArray(fishVAO); // 綁定魚的 VAO
        glDrawArrays(GL_TRIANGLES, 0, fishObject->positions.size()); // 繪製


        // Status update
        currentTime = glfwGetTime();
        dt = currentTime - lastTime;
        lastTime = currentTime;

        /* TODO#7: Update "revolveFishDegree", "rotateColumnDegree", "rotateFishDegree", 
         * "fishHeight", "heightDir", 
         * "squeezeFactor", "breathingColor", "intensity"
         */
        
        // 更新柱子旋轉角度 (度/秒) 
        rotateColumnDegree += rotateColumnSpeed * dt;
        // 更新魚環繞角度 (度/秒) 
        revolveFishDegree += -revolveFishSpeed * dt;

        // 更新 'R' 鍵自轉 
        if (useSelfRotation) {
            rotateFishDegree += rotateFishSpeed * dt;
            if (rotateFishDegree >= 360.0f) {
                rotateFishDegree = 0.0f; // 轉完一圈
                useSelfRotation = false; // 停止自轉 (不可中斷)
            }
        }

        // 更新魚上下游動 
        fishHeight += heightDir * heightSpeed * dt;
        // 限制游動範圍在 -0.5 到 0.5 之間
        if (fishHeight > 0.5f || fishHeight < -0.5f) {
            heightDir *= -1; // 改變方向
            fishHeight = glm::clamp(fishHeight, -0.5f, 0.5f); // 確保不超出範圍
        }

        // 更新 'S' 鍵擠壓 [cite: 323, 331]
        if (useSqueeze) {
            squeezeFactor += squeezeSpeed * dt; // 累加角度 (90度/秒)
        }

        // 更新 'B' 鍵呼吸 [cite: 324, 336, 338, 339]
        if (useBreathing) {
            breathingColor = glm::vec3(1.0f, 1.0f, 0.0f); // 呼吸顏色: 黃色 [cite: 338]
            // 強度: 使用 sin(time) ，範圍在 [0.5, 1.0] 之間
            intensity = (sin(currentTime * 3.0f) * 0.25f) + 0.75f;
        } else {
            breathingColor = glm::vec3(1.0f, 1.0f, 1.0f); // 正常: 白色
            intensity = 1.0f; // 正常: 強度 1.0
        }
        
        // glfw: swap buffers and poll IO events (keys pressed/released, mouse moved etc.)
        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    // glfw: terminate, clearing all previously allocated GLFW resources.
    glfwTerminate();
    return 0;
}

void keyCallback(GLFWwindow *window, int key, int scancode, int action, int mods) {

    /* TODO#8: Key callback 
    * 1. Press 'r' to rotate the fish. 
    * 2. Press 's' to squeeze the fish. 
    * 3. Press 'b' to make the breathing light. 
    * Hint:
    * GLFW_KEY_R, GLFW_KEY_S, GLFW_KEY_B
    * GLFW_PRESS, GLFW_REPEAT
    */
    
    // 只在按下(PRESS)時觸發
    if (action == GLFW_PRESS) {
        switch (key) {
            case GLFW_KEY_R: // 'R' 鍵
                if (!useSelfRotation) { // 檢查是否已在旋轉 (不可中斷 )
                    useSelfRotation = true;
                    rotateFishDegree = 0.0f; // 重置角度
                }
                break;
            case GLFW_KEY_S: // 'S' 鍵
                useSqueeze = !useSqueeze; // 切換擠壓狀態 
                if (!useSqueeze) {
                    squeezeFactor = 0.0f; // 停止時重置擠壓因子
                }
                break;
            case GLFW_KEY_B: // 'B' 鍵
                useBreathing = !useBreathing; // 切換呼吸狀態 
                break;
            case GLFW_KEY_ESCAPE: // 增加一個 Esc 鍵來關閉視窗
                glfwSetWindowShouldClose(window, true);
                break;
        }
    }
}

// glfw: whenever the window size changed (by OS or user resize) this callback function executes
void framebufferSizeCallback(GLFWwindow *window, int width, int height) {
    glViewport(0, 0, width, height);
    SCR_WIDTH = width;
    SCR_HEIGHT = height;
}

/* TODO#1: createShader
 * input:
 * filename: shader file name
 * type: shader type, "vert" means vertex shader, "frag" means fragment shader
 * output: shader object
 * Hint:
 * glCreateShader [cite: 29], glShaderSource [cite: 32], glCompileShader [cite: 35]
 */
unsigned int createShader(const string &filename, const string &type) {
    // 1. 讀取著色器原始碼
    string shaderCode;
    ifstream shaderFile;
    shaderFile.exceptions(ifstream::failbit | ifstream::badbit);
    try {
        shaderFile.open(filename);
        stringstream shaderStream;
        shaderStream << shaderFile.rdbuf();
        shaderFile.close();
        shaderCode = shaderStream.str();
    } catch (ifstream::failure &e) {
        std::cout << "ERROR::SHADER::FILE_NOT_SUCCESSFULLY_READ: " << filename << std::endl;
        return 0;
    }
    const char *shaderSource = shaderCode.c_str();

    // 2. 判斷著色器類型並創建 [cite: 31]
    unsigned int shader;
    GLenum shaderType;
    if (type == "vert")
        shaderType = GL_VERTEX_SHADER;
    else if (type == "frag")
        shaderType = GL_FRAGMENT_SHADER;
    else {
        std::cout << "ERROR::SHADER::UNKNOWN_TYPE: " << type << std::endl;
        return 0;
    }
    shader = glCreateShader(shaderType); [cite: 29]

    // 3. 附加原始碼並編譯 [cite: 32, 35]
    glShaderSource(shader, 1, &shaderSource, NULL);
    glCompileShader(shader);

    // 4. 檢查編譯錯誤
    int success;
    char infoLog[512];
    glGetShaderiv(shader, GL_COMPILE_STATUS, &success);
    if (!success) {
        glGetShaderInfoLog(shader, 512, NULL, infoLog);
        std::cout << "ERROR::SHADER::" << type << "::COMPILATION_FAILED\n" << infoLog << std::endl;
        return 0;
    }
    return shader;
}

/* TODO#2: createProgram
 * input:
 * vertexShader: vertex shader object
 * fragmentShader: fragment shader object
 * output: shader program
 * Hint:
 * glCreateProgram [cite: 38], glAttachShader [cite: 40], glLinkProgram [cite: 42], glDetachShader [cite: 43]
 */
unsigned int createProgram(unsigned int vertexShader, unsigned int fragmentShader) {
    // 1. 創建著色器程式 [cite: 38]
    unsigned int program = glCreateProgram();

    // 2. 附加著色器 [cite: 40]
    glAttachShader(program, vertexShader);
    glAttachShader(program, fragmentShader);

    // 3. 連結程式 [cite: 42]
    glLinkProgram(program);

    // 4. 檢查連結錯誤
    int success;
    char infoLog[512];
    glGetProgramiv(program, GL_LINK_STATUS, &success);
    if (!success) {
        glGetProgramInfoLog(program, 512, NULL, infoLog);
        std::cout << "ERROR::PROGRAM::LINKING_FAILED\n" << infoLog << std::endl;
        return 0;
    }

    // 5. 分離著色器 (連結後即可分離) [cite: 43]
    glDetachShader(program, vertexShader);
    glDetachShader(program, fragmentShader);

    return program;
}

/* TODO#3: modelVAO
 * input:
 * model: Object you want to render
 * output: VAO
 * Hint:
 * glGenVertexArrays [cite: 165], glBindVertexArray [cite: 169], glGenBuffers [cite: 60], glBindBuffer [cite: 63], glBufferData[cite: 72],
 * glVertexAttribPointer [cite: 119], glEnableVertexAttribArray[cite: 131],
 */
unsigned int modelVAO(Object &model) {
    // 把vertex丟到GPU，就可以不用一個一個跑
    // 利用 Vertex Buffer Objects 管理
    unsigned int VAO, VBO[2];  // 需要兩個buffer，儲存position and texcoords

    // 1. 生成並綁定 VAO
    glGenVertexArrays(1, &VAO);
    glBindVertexArray(VAO);

    // 2. 生成 VBO
    // *** 修改 ***
    glGenBuffers(2, VBO); // 生成 2 個 VBO

    // 3. 綁定 VBO[0] (Vertex Positions)
    // 假設 layout(location = 0) in vec3 position;
    glBindBuffer(GL_ARRAY_BUFFER, VBO[0]);
    glBufferData(GL_ARRAY_BUFFER, sizeof(glm::vec3) * model.positions.size(), &model.positions[0], GL_STATIC_DRAW);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, (void*)0);
    glEnableVertexAttribArray(0);

    // 4. 綁定 VBO[1] (Texture Coordinates)
    // *** 修改 *** (移除了 VBO[1] (Normals) 的部分)
    // 假設 layout(location = 2) in vec2 texcoord;
    if (!model.texcoords.empty()) {
        glBindBuffer(GL_ARRAY_BUFFER, VBO[1]); // 使用 VBO[1]
        glBufferData(GL_ARRAY_BUFFER, sizeof(glm::vec2) * model.texcoords.size(), &model.texcoords[0], GL_STATIC_DRAW);
        glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 0, (void*)0); // 指向 location 2
        glEnableVertexAttribArray(2); // 啟用 location 2
    }

    // 5. 解除綁定
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);

    return VAO;
}

/* TODO#4: loadTexture
 * input:
 * filename: texture file name
 * output: texture object
 * Hint:
 * glEnable, glGenTextures [cite: 250], glBindTexture [cite: 252], glTexParameteri [cite: 258], glTexImage2D [cite: 270]
 */
unsigned int loadTexture(const string &filename) {
    unsigned int textureID;
    
    // 1. 生成紋理物件 [cite: 250, 255]
    glGenTextures(1, &textureID);
    
    // 2. 綁定紋理 [cite: 252, 256]
    glBindTexture(GL_TEXTURE_2D, textureID);

    // 3. 設定紋理環繞 (Wrapping) 參數 [cite: 258, 260, 261]
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);

    // 4. 設定紋理過濾 (Filtering) 參數 [cite: 258, 264]
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    // 5. 使用 stb_image 載入圖片
    int width, height, nrChannels;
    stbi_set_flip_vertically_on_load(true); // OpenGL 的紋理 Y 軸是反的
    unsigned char *data = stbi_load(filename.c_str(), &width, &height, &nrChannels, 0);

    if (data) {
        GLenum format;
        if (nrChannels == 1)
            format = GL_RED;
        else if (nrChannels == 3)
            format = GL_RGB;
        else if (nrChannels == 4)
            format = GL_RGBA;
        else {
            std::cout << "ERROR::TEXTURE::UNSUPPORTED_FORMAT: " << filename << std::endl;
            stbi_image_free(data);
            return 0;
        }

        // 6. 生成紋理 [cite: 270, 273, 282]
        glTexImage2D(GL_TEXTURE_2D, 0, format, width, height, 0, format, GL_UNSIGNED_BYTE, data);
        // (可選) glGenerateMipmap(GL_TEXTURE_2D);
    } else {
        std::cout << "Failed to load texture: " << filename << std::endl;
    }

    // 7. 釋放圖片記憶體
    stbi_image_free(data);

    return textureID;
}

void init() {
#if defined(__linux__) || defined(__APPLE__)
    string dirShader = "../../src/shaders/";
    string dirAsset = "../../src/asset/obj/";
    string dirTexture = "../../src/asset/texture/";
#else
    // 根據骨架檔案，假設 Windows 路徑
    string dirShader = "..\\..\\src\\shaders\\";
    string dirAsset = "..\\..\\src\\asset\\obj\\";
    string dirTexture = "..\\..\\src\\asset\\texture\\";
#endif

    // Object
    fishObject = new Object(dirAsset + "fish.obj");
    columnObject = new Object(dirAsset + "column.obj");
    groundObject = new Object(dirAsset + "cube.obj"); // 地面使用 cube.obj

    // Shader
    vertexShader = createShader(dirShader + "vertexShader.vert", "vert");
    fragmentShader = createShader(dirShader + "fragmentShader.frag", "frag");
    shaderProgram = createProgram(vertexShader, fragmentShader);
    // glUseProgram(shaderProgram); // 移至 TODO#5 之前，以確保 glGetUniformLocation 能正確運作

    // Texture
    fishTexture = loadTexture(dirTexture + "fish.jpg"); [cite: 283]
    columnTexture = loadTexture(dirTexture + "column.jpg"); [cite: 283]
    groundTexture = loadTexture(dirTexture + "ground.jpg"); [cite: 284]

    // VAO, VBO
    fishVAO = modelVAO(*fishObject);
    columnVAO = modelVAO(*columnObject);
    groundVAO = modelVAO(*groundObject);
}