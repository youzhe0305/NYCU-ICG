#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>

#include <iostream>
#include <fstream>
#include <sstream>
#include <string>

using namespace std; 
using glm::radians;
using glm::clamp;
using std::sin;

#include "./header/stb_image.h"
#include "./header/Object.h"


void framebufferSizeCallback(GLFWwindow *window, int width, int height);
void keyCallback(GLFWwindow *window, int key, int scancode, int action, int mods);
unsigned int createShader(const string &filename, const string &type);
unsigned int createProgram(unsigned int vertexShader, unsigned int fragmentShader);
unsigned int modelVAO(Object &model);
unsigned int loadTexture(const char *tFileName);
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

// Constants you may need
const int rotateColumnSpeed = 10;
const int revolveFishSpeed = 20;
const int rotateFishSpeed = 180;
const float fishColumnDist = 3;
const float heightSpeed = 0.4;
const int squeezeSpeed = 90;

// Variables you can use
float rotateColumnDegree = 0;
float revolveFishDegree = 0;
float rotateFishDegree = 0; 
float fishColumnOffset = 0;
int heightDir = -1; 
float fishHeight = 0;
const float fishHeightMax = 5.0f;
const float fishHeightMin = -1.0f;

bool useSelfRotation = false;
bool useSqueeze = false;
float squeezeFactor = 0;
bool useBreathing = false; 
float intensity = 1.0;
glm::vec3 breathingColor = glm::vec3(1.0f, 1.0f, 1.0f);

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
     *        Ex. HW2-112550000
     */

    // glfw window creation
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

    // TODO#1: Finish function createShader

    // TODO#2: Finish function createProgram
    // TODO#3: Finish function modelVAO
    // TODO#4: Finish function loadTexture
    // You can find the above functions right below the main function

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
     *    1. Retrieve locations for model, view, and projection matrices.
     *    2. Retrieve locations for squeezeFactor, breathingColor, intensity, and other parameters.
     * Hint:
     *    glGetUniformLocation
     */
    
    // 取得GLSL linked program中各個變數的位置 (link完之後，位置有各種不可能，要去實際access才知道)
    GLint modelLoc, viewLoc, projLoc;
    GLint squeezeFactorLoc, breathingColorLoc, intensityLoc, texLoc;
    GLint stripeFrequencyLoc, useStripesLoc;
    modelLoc = glGetUniformLocation(shaderProgram, "model");
    viewLoc = glGetUniformLocation(shaderProgram, "view");
    projLoc = glGetUniformLocation(shaderProgram, "projection");
    squeezeFactorLoc = glGetUniformLocation(shaderProgram, "squeezeFactor");
    breathingColorLoc = glGetUniformLocation(shaderProgram, "breathingColor");
    intensityLoc = glGetUniformLocation(shaderProgram, "intensity");
    texLoc = glGetUniformLocation(shaderProgram, "ourTexture");
    stripeFrequencyLoc = glGetUniformLocation(shaderProgram, "stripeFrequency");
    useStripesLoc = glGetUniformLocation(shaderProgram, "useStripes");

    // render loop
    while (!glfwWindowShouldClose(window)) {
        // render color of water
        glClearColor(0.15, 0.50, 0.65, 1.0);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT); // clean the color & depth of pixels

        glUseProgram(shaderProgram); // following draw should utilize the shader program

        glm::mat4 view = glm::lookAt(glm::vec3(0.0f, 8.5f, 13.0f), glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, 1.0f, 0.0f));

        glm::mat4 projection = glm::perspective(glm::radians(45.0f), (float)SCR_WIDTH / (float)SCR_HEIGHT, 0.1f, 1000.0f);

        glm::mat4 groundModel(1.0f), fishModel(1.0f), columnModel(1.0f);

        /* TODO#6-1: Render Ground
         *    1. Set up ground model matrix.
         *    2. Send model, view, and projection matrices to the program.
         *    3. Send squeezeFactor, breathingColor, intensity, or other parameters to the program.
         *    4. Apply the texture, and render the ground.
         * Hint:
         *	  rotate, translate, scale
         *    glUniformMatrix4fv, glUniform1f, glUniform3fv
         *    glActiveTexture, glBindTexture, glBindVertexArray, glDrawArrays
         */
        
        // 把view, projection這些所有物件共通的矩陣送到shader program的對應位置，用來後面render
        glUniformMatrix4fv(viewLoc, 1, GL_FALSE, glm::value_ptr(view));
        glUniformMatrix4fv(projLoc, 1, GL_FALSE, glm::value_ptr(projection));

        groundModel = glm::mat4(1.0f);
        groundModel = glm::translate(groundModel, glm::vec3(0.0f, -5.0f, 8.0f));
        groundModel = glm::scale(groundModel, glm::vec3(35.0f, 1.0f, 25.0f));
        glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(groundModel)); // 送進shader program的model矩陣
        
        // 上傳其他的地面的GLSL變數，用來讓shder program知道該怎麼去render
        glUniform1f(squeezeFactorLoc, 0.0f); // 不使用squeeze effect
        glUniform3f(breathingColorLoc, 1.0f, 1.0f, 1.0f); // 呼吸燈的顏色固定白色 (保持原色)
        glUniform1f(intensityLoc, 1.0f); // 不使用呼吸燈 (強度設為time-independent的1.0)
        glUniform1i(useStripesLoc, 0); // 不要條文
        
        glActiveTexture(GL_TEXTURE0); // 啟用texture 0 status unit
        glBindTexture(GL_TEXTURE_2D, groundTexture); // 把地面的texture綁到目前的texture unit 0
        glUniform1i(texLoc, 0); // 告訴shader ground texture在texture unit 0 (texture location的第0個)
        
	    glBindVertexArray(groundVAO); // bind vao, 載入vbo設定，就不用重新設定vbo的load
	    glDrawArrays(GL_TRIANGLES, 0, static_cast<GLsizei>(groundObject->positions.size() / 3)); // 把shader內的vertex跑一遍，用三角形polygon畫出來
        


        /* TODO#6-2: Render Column
         *    1. Set up column model matrix.
         *    2. Send model, view, and projection matrices to the program.
         *    3. Send squeezeFactor, breathingColor, intensity, or other parameters to the program.
         *    4. Apply the texture, and render the column.
         * Hint:
         *	  rotate, translate, scale
         *    glUniformMatrix4fv, glUniform1f, glUniform3fv
         *    glActiveTexture, glBindTexture, glBindVertexArray, glDrawArrays
         */

        columnModel = glm::mat4(1.0f);
        columnModel = glm::translate(columnModel, glm::vec3(0.0f, -4.0f, 0.0f));
        columnModel = glm::rotate(columnModel, radians(rotateColumnDegree), glm::vec3(0.0f, 1.0f, 0.0f)); // counter-clockwise
        columnModel = glm::rotate(columnModel, radians(-90.0f), glm::vec3(1.0f, 0.0f, 0.0f));
        columnModel = glm::scale(columnModel, glm::vec3(0.05f, 0.05f, 0.05f));
        glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(columnModel));
        
        glUniform1f(squeezeFactorLoc, 0.0f);
        glUniform3f(breathingColorLoc, 1.0f, 1.0f, 1.0f);
        glUniform1f(intensityLoc, 1.0f);
        glUniform1i(useStripesLoc, 1); // 對柱子上條紋
        glUniform1f(stripeFrequencyLoc, 20.0f);
        
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, columnTexture);
        glUniform1i(texLoc, 0);
        
	    glBindVertexArray(columnVAO);
	    glDrawArrays(GL_TRIANGLES, 0, static_cast<GLsizei>(columnObject->positions.size() / 3));

        /* TODO#6-3: Render Fish
         *    1. Set up fish model matrix.
         *    2. Send model, view, and projection matrices to the program.
         *    3. Send squeezeFactor, breathingColor, intensity, or other parameters to the program.
         *    4. Apply the texture, and render the fish.
         * Hint:
         *	  rotate, translate, scale
         *    glUniformMatrix4fv, glUniform1f, glUniform3fv
         *    glActiveTexture, glBindTexture, glBindVertexArray, glDrawArrays
         */

        fishModel = glm::mat4(1.0f);
        fishModel = glm::rotate(fishModel, radians(revolveFishDegree), glm::vec3(0.0f, 1.0f, 0.0f)); // 公轉，counter-clockwise by column
        fishModel = glm::translate(fishModel, glm::vec3(0.0f, fishHeight, fishColumnDist));
        fishModel = glm::rotate(fishModel, radians(rotateFishDegree), glm::vec3(-1.0f, 0.0f, 0.0f)); // 自轉
        fishModel = glm::rotate(fishModel, radians(-90.0f), glm::vec3(1.0f, 0.0f, 0.0f));
        fishModel = glm::scale(fishModel, glm::vec3(0.05f, 0.05f, 0.05f));
        glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(fishModel));
        
        glUniform1f(squeezeFactorLoc, radians(squeezeFactor)); // 隨時間擠壓
        glUniform3fv(breathingColorLoc, 1, glm::value_ptr(breathingColor)); // 呼吸燈顏色，把breathingColor陣列傳進去，更新1個變數(取3個值)
        glUniform1f(intensityLoc, intensity); // 隨時間改變呼吸燈強度
        glUniform1i(useStripesLoc, 0);
        
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, fishTexture);
        glUniform1i(texLoc, 0);
        
	    glBindVertexArray(fishVAO);
	    glDrawArrays(GL_TRIANGLES, 0, static_cast<GLsizei>(fishObject->positions.size() / 3));


        // Status update
        currentTime = glfwGetTime();
        dt = currentTime - lastTime;
        lastTime = currentTime;

        /* TODO#7: Update "revolveFishDegree", "rotateColumnDegree", "rotateFishDegree", 
         *          "fishHeight", "heightDir", 
         *          "squeezeFactor", "breathingColor", "intensity"
         */

        
        rotateColumnDegree += rotateColumnSpeed * dt;
        revolveFishDegree += -revolveFishSpeed * dt;

        if (useSelfRotation) {
            rotateFishDegree += rotateFishSpeed * dt;
            if (rotateFishDegree >= 360.0f) {
                rotateFishDegree = 0.0f;
                useSelfRotation = false; // 停止自轉 (只在這裡觸發，避免在其他情況被中斷自轉)
            }
        }

        // 慢慢上下游
        fishHeight += heightDir * heightSpeed * dt;
        if (fishHeight > fishHeightMax || fishHeight < fishHeightMin) {
            heightDir *= -1; // 換方向
            fishHeight = clamp(fishHeight, fishHeightMin, fishHeightMax);
        }

        if (useSqueeze) {
            squeezeFactor += squeezeSpeed * dt;
        }

        if (useBreathing) {
            breathingColor = glm::vec3(1.0f, 1.0f, 0.0f);
            intensity = (sin(currentTime * 3.0f) * 0.25f) + 0.75f; // 用sin來跑呼吸燈強度變化
        } else {
            breathingColor = glm::vec3(1.0f, 1.0f, 1.0f); // 不使用呼吸燈: 用白燈 保持顏色不變
            intensity = 1.0f;
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

    // The action is one of GLFW_PRESS, GLFW_REPEAT or GLFW_RELEASE.
    // Events with GLFW_PRESS and GLFW_RELEASE actions are emitted for every key press.
    // Most keys will also emit events with GLFW_REPEAT actions while a key is held down.
    // https://www.glfw.org/docs/3.3/input_guide.html

    /* TODO#8: Key callback 
    *    1. Press 'r' to rotate the fish.
    *    2. Press 's' to squeeze the fish.
    *    3. Press 'b' to make the breathing light.
    * Hint:
    *      GLFW_KEY_R, GLFW_KEY_S, GLFW_KEY_B
    *      GLFW_PRESS, GLFW_REPEAT
    */    
    if (action == GLFW_PRESS) {
            
            if (key == GLFW_KEY_R) {
                if (!useSelfRotation) { // 避免重置進行中的旋轉
                    useSelfRotation = true;
                    rotateFishDegree = 0.0f;
                }
            } 
            else if (key == GLFW_KEY_S) {
                useSqueeze = !useSqueeze;
            } 
            else if (key == GLFW_KEY_B) {
                useBreathing = !useBreathing;
            } 
            else if (key == GLFW_KEY_ESCAPE) {
                glfwSetWindowShouldClose(window, true);
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
 *      filename: shader file name
 *      type: shader type, "vert" means vertex shader, "frag" means fragment shader
 * output: shader object
 * Hint:
 *      glCreateShader, glShaderSource, glCompileShader
 */
unsigned int createShader(const string &filename, const string &type) {
    string shaderCode;
    ifstream shaderFile;
    shaderFile.exceptions(ifstream::failbit | ifstream::badbit);
    try { // 把shadder code讀進來
        shaderFile.open(filename);
        stringstream shaderStream;
        shaderStream << shaderFile.rdbuf();
        shaderFile.close();
        shaderCode = shaderStream.str();
    } catch (ifstream::failure &e) {
        std::cout << "[ERROR] shader file is not successfully read: " << filename << std::endl;
        return 0;
    }
    const char *shaderSource = shaderCode.c_str(); // source code

    unsigned int shader;
    GLenum shaderType;
    if (type == "vert")
        shaderType = GL_VERTEX_SHADER; // 決定shader type
    else if (type == "frag")
        shaderType = GL_FRAGMENT_SHADER;
    else {
        std::cout << "[ERROR] shader unknown type: " << type << std::endl;
        return 0;
    }
    shader = glCreateShader(shaderType); // 創建shader object

    glShaderSource(shader, 1, &shaderSource, NULL); // set source code into shader
    glCompileShader(shader); // compile with shader source code

    // 確認是否compile成功
    int success;
    char infoLog[512];
    glGetShaderiv(shader, GL_COMPILE_STATUS, &success); // get compile status information
    if (!success) {
        glGetShaderInfoLog(shader, 512, NULL, infoLog);
        std::cout << "[ERROR] shader " << type << " compilation failed\n" << infoLog << std::endl;
        return 0;
    }
    return shader;
}

/* TODO#2: createProgram
 * input:
 *      vertexShader: vertex shader object
 *      fragmentShader: fragment shader object
 * output: shader program
 * Hint:
 *      glCreateProgram, glAttachShader, glLinkProgram, glDetachShader
 */
unsigned int createProgram(unsigned int vertexShader, unsigned int fragmentShader) {
    // 把不同的shader串成一個pipeline program
    unsigned int program = glCreateProgram();

    glAttachShader(program, vertexShader);
    glAttachShader(program, fragmentShader);

    glLinkProgram(program); // 串起來檢查in, out並輸出儲存執行檔

    int success;
    char infoLog[512];
    glGetProgramiv(program, GL_LINK_STATUS, &success);
    if (!success) {
        glGetProgramInfoLog(program, 512, NULL, infoLog);
        std::cout << "[ERROR] program linking failed\n" << infoLog << std::endl;
        return 0;
    }

    // link完，program object已經儲存linked後的執行檔，可以把shader detach掉，節省記憶體
    glDetachShader(program, vertexShader);
    glDetachShader(program, fragmentShader);

    return program;
}

/* TODO#3: modelVAO
 * input:
 *      model: Object you want to render
 * output: VAO
 * Hint:
 *      glGenVertexArrays, glBindVertexArray, glGenBuffers, glBindBuffer, glBufferData,
 *      glVertexAttribPointer, glEnableVertexAttribArray,
 */
unsigned int modelVAO(Object &model) {
    // 把vertex丟到GPU，就可以不用一個一個跑
    // 利用 Vertex Buffer Objects 管理
    unsigned int VAO;
    unsigned int VBO[2];  // 需要兩個buffer，儲存position and texcoords

    glGenVertexArrays(1, &VAO); // 生成Vertex Array Object，用來指向VBO的pointer，用來管理大量物件
    glBindVertexArray(VAO);

    glGenBuffers(2, VBO); // 生成 2 個 VBO, 把id存在VBO[]裡

    glBindBuffer(GL_ARRAY_BUFFER, VBO[0]); // 把 VBO[0] bind 到 GL_ARRAY_BUFFER status，用來標示OpenGL struct中儲存vertex attr的buffer指標
    // 把position資料丟到GPU
    // GL_STATIC_DRAW表示CPU只會設定一次資料位置(static)，之後只會被GPU頻繁讀取(draw)，讓GPU知道應該一次就放在一個比較快速能取用的位置
	glBufferData(GL_ARRAY_BUFFER, sizeof(float) * model.positions.size(), &model.positions[0], GL_STATIC_DRAW);
    // 告訴OpenGL怎麼解讀buffer裡的資料
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, (void*)0); // 設定location為0的讀取規則，每次讀3個float (xyz)，然後跨0的stride (在這個buffer內，只有position資料)，offset設為0表從0開始
    glEnableVertexAttribArray(0);

    // 跟前面的position類似
    if (!model.texcoords.empty()) {
        glBindBuffer(GL_ARRAY_BUFFER, VBO[1]);
		glBufferData(GL_ARRAY_BUFFER, sizeof(float) * model.texcoords.size(), &model.texcoords[0], GL_STATIC_DRAW);
        glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 0, (void*)0); // (uv)2個一組
        glEnableVertexAttribArray(1);
    }

    // 解綁
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);

    return VAO;

}

/* TODO#4: loadTexture
 * input:
 *      filename: texture file name
 * output: texture object
 * Hint:
 *      glEnable, glGenTextures, glBindTexture, glTexParameteri, glTexImage2D
 */
unsigned int loadTexture(const string &filename) {
    
    unsigned int textureID;
    glGenTextures(1, &textureID);
    glBindTexture(GL_TEXTURE_2D, textureID); // 跟Bind VBO的概念一樣

    // 設定texture超出給定u,v範圍時，變成重複環繞    
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);

    // 處理texture的放大縮小
    // ex: 在相機靠近物體時，texture會被放大
    // 這裡用線性插值來處理
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    int width, height, nrChannels;
    stbi_set_flip_vertically_on_load(true); // 把jpg的座標改成OpenGL的座標 (原點從左上改到左下)
    unsigned char *data = stbi_load(filename.c_str(), &width, &height, &nrChannels, 0); // load texture image

    if (data) {
        GLenum format;
        if (nrChannels == 1)
            format = GL_RED;
        else if (nrChannels == 3)
            format = GL_RGB;
        else if (nrChannels == 4)
            format = GL_RGBA;
        else {
            std::cout << "[ERROR] texture unsupported format: " << filename << std::endl;
            stbi_image_free(data);
            return 0;
        }
        glTexImage2D(GL_TEXTURE_2D, 0, format, width, height, 0, format, GL_UNSIGNED_BYTE, data); // 把CPU上的texture (data)丟進GPU
    } else {
        std::cout << "Failed to load texture: " << filename << std::endl;
    }
    // free image memory
    stbi_image_free(data);

    return textureID;
}

void init() {
#if defined(__linux__) || defined(__APPLE__)
    string dirShader = "../../src/shaders/";
    string dirAsset = "../../src/asset/obj/";
    string dirTexture = "../../src/asset/texture/";
#else
    string dirShader = "..\\..\\src\\shaders\\";
    string dirAsset = "..\\..\\src\\asset\\obj\\";
    string dirTexture = "..\\..\\src\\asset\\texture\\";
#endif

    // Object
    fishObject = new Object(dirAsset + "fish.obj");
    columnObject = new Object(dirAsset + "column.obj");
    groundObject = new Object(dirAsset + "cube.obj");

    // Shader
    vertexShader = createShader(dirShader + "vertexShader.vert", "vert");
    fragmentShader = createShader(dirShader + "fragmentShader.frag", "frag");
    shaderProgram = createProgram(vertexShader, fragmentShader);
    glUseProgram(shaderProgram);

    // Texture
    fishTexture = loadTexture(dirTexture + "fish.jpg");
    columnTexture = loadTexture(dirTexture + "column.jpg");
    groundTexture = loadTexture(dirTexture + "ground.jpg");

    // VAO, VBO
    fishVAO = modelVAO(*fishObject);
    columnVAO = modelVAO(*columnObject);
    groundVAO = modelVAO(*groundObject);
}

