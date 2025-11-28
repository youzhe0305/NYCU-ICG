#version 330 core
out vec4 FragColor;

in vec3 TexCoords;

// 注意：Cubemap 使用 samplerCube 型別
uniform samplerCube skybox;

void main()
{
    // texture 函式會根據 TexCoords (3D 向量) 自動去 6 張圖裡面找對應的顏色
    FragColor = texture(skybox, TexCoords);
}