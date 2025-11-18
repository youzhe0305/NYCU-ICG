ㄏ_ptr(columnModel));
        
        glUniform1f(squeezeFactorLoc, 0.0f);
        glUniform3f(breathingColorLoc, 1.0f, 1.0f, 1.0f);
        glUniform1f(intensityLoc, 1.0f);
        
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, columnTexture);
        glUniform1i(texLoc, 0);
        
        glBindVertexArray(columnVAO);
        glDrawArrays(GL_TRIANGLES, 0, columnObject->positions.size());

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
        fishModel = glm::rotate(fishModel, radians(revolveFishDegree), glm::vec3(0.0f, 1.0f, 0.0f)); // 公轉
        fishModel = glm::translate(fishModel, glm::vec3(0.0f, fishHeight, fishColumnDist));
        fishModel = glm::rotate(fishModel, radians(rotateFishDegree), glm::vec3(-1.0f, 0.0f, 0.0f)); // 自轉
        fishModel = glm::rotate(fishModel, radians(-90.0f), glm::vec3(1.0f, 0.0f, 0.0f));
        fishModel = glm::scale(fishModel, glm::vec3(0.05f, 0.05f, 0.05f));
        glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(fishModel));
        
        glUniform1f(squeezeFactorLoc, radians(squeezeFactor)); // 隨時間擠壓
        glUniform3fv(breathingColorLoc, 1, glm::value_ptr(breathingColor)); // 呼吸燈顏色
        glUniform1f(intensityLoc, intensity); // 隨時間改變呼吸燈強度
        
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, fishTexture);
        glUniform1i(texLoc, 0);
        
        glBindVertexArray(fishVAO);
        glDrawArrays(GL_TRIANGLES, 0, fishObject->positions.size());


        // Status update
        currentTime = glfwGetTime();
        dt = currentTime - lastTime;
        lastTime = currentTime;

        /* TODO#7: Update "revolveFishDegree", "rotateColumnDegree", "rotateFishDegree", 
         *          "fishHeight", "heightDir", 
         *          "squeezeFactor", "breathingColor", "intensity"
         */
        rotateColumnDegree += rotateColumnSpeed * dt;
        if (rotateColumnDegree >= 360.0f) rotateColumnDegree -= 360.0f;

        revolveFishDegree += -revolveFishSpeed * dt;
        if (revolveFishDegree <= -360.0f) revolveFishDegree += 360.0f;

        if (useSelfRotation) {
            rotateFishDegree += rotateFishSpeed * dt;
            if (rotateFishDegree >= 360.0f) {
                rotateFishDegree = 0.0f;
                useSelfRotation = false; // 停止自轉
            }
        }

        // 慢慢上下游
        fishHeight += heightDir * heightSpeed * dt;
        if (fishHeight > 0.5f || fishHeight < -0.5f) {
            heightDir *= -1; // 換方向
            fishHeight = clamp(fishHeight, -0.5f, 0.5f);
        }

        if (useSqueeze) {
            squeezeFactor += squeezeSpeed * dt;
            if (squeezeFactor >= 360.0f) squeezeFactor -= 360.0f;
        }

        if (useBreathing) {
            breathingColor = glm::vec3(1.0f, 1.0f, 0.0f);
            intensity = (sin(currentTime * 3.0f) * 0.25f) + 0.75f; // 用sin來跑呼吸燈強度變化
        } else {
            breathingColor = glm::vec3(1.0f, 1.0f, 1.0f); // 不使用呼吸燈: 用白燈 保持顏色不變
            intensity = 1.0f;
        }

        // glfw: swap buffers and poll IO events
        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    // glfw: terminate
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
                if (!useSqueeze) {
                    squeezeFactor = 0.0f;
                }
            } 
            else if (key == GLFW_KEY_B) {
                useBreathing = !useBreathing;
            } 
            else if (key == GLFW_KEY_ESCAPE) {
                glfwSetWindowShouldClose(window, true);
            }
        }
}

// glfw: whenever the window size changed
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
    unsigned int VAO;
    unsigned int VBO[2];  // 需要兩個buffer，儲存position and texcoords

    glGenVertexArrays(1, &VAO); // 生成Vertex Array Object
    glBindVertexArray(VAO);

    glGenBuffers(2, VBO); // 生成 2 個 VBO, 把id存在VBO[]裡

    // VBO[0]: 頂點位置 (Vertex Positions)
    glBindBuffer(GL_ARRAY_BUFFER, VBO[0]); // 把 VBO[0] bind 到 GL_ARRAY_BUFFER status
    glBufferData(GL_ARRAY_BUFFER, sizeof(glm::vec3) * model.positions.size(), &model.positions[0], GL_STATIC_DRAW);
    // 告訴OpenGL怎麼解讀buffer裡的資料
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, (void*)0); // location = 0
    glEnableVertexAttribArray(0);

    // VBO[1]: 紋理座標 (Texture Coordinates)
    if (!model.texcoords.empty()) {
        glBindBuffer(GL_ARRAY_BUFFER, VBO[1]);
        glBufferData(GL_ARRAY_BUFFER, sizeof(glm::vec2) * model.texcoords.size(), &model.texcoords[0], GL_STATIC_DRAW);
        glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 0, (void*)0); // (uv)2個一組，綁定到 location 1
        glEnableVertexAttribArray(1); // 啟用 location 1
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
unsigned int loadTexture(const string &filename) { // 修正 #3：修正定義以匹配原型
    unsigned int textureID;
    
    glGenTextures(1, &textureID);
    
    glBindTexture(GL_TEXTURE_2D, textureID); // 跟Bind VBO的概念一樣

    // 設定texture超出給定u,v範圍時，變成重複環繞    
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);

    // 處理texture的放大縮小
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