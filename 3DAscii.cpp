#include <iostream>
#include <vector>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <chrono>
#include <cstdio>

class Screen
{

public:

    int width, height;
    std::vector<char> coord;

    Screen(int width, int height) 
    {
        this->width = width;
        this->height = height;
        coord.resize(width * height);
        clearBuff();
    }

    void clearBuff()
    {
        for (int i = 0; i < size(coord); i++)
            coord[i] = background;
    }

    void setNormalized(float x, float y, char val) 
    {
        set((width / 2) + round(x * (width / 2)), (height / 2) + round(y * (height / 2)), val);
    }
    
    // essentially a vertex shader, although this is all done in software
    glm::vec4 transform(glm::vec3 vertices, glm::mat4 model, glm::mat4 view, glm::mat4 proj)
    {
        glm::vec4 transvert = glm::vec4(vertices, 1.0);
        transvert = proj * view * model * transvert;
        return transvert;
    }
    // main draw function which is techincally also apart of the vertex shader 
    // takes normalized vector of 3D vertices and applies the transformation matrix to them
    // then uses the indices vector array to connect them together using connect()
    void draw(std::vector<glm::vec3> vertices, std::vector<int> indices, glm::mat4 model, glm::mat4 view, glm::mat4 proj)
    {
        glm::vec2 prev = transform(vertices[indices[indices.size()-1]], model, view, proj);

        for (auto i : indices)
        {
            glm::vec2 curr = transform(vertices[i], model, view, proj);
            connect(prev.x, prev.y, curr.x, curr.y);
            prev = curr;
        }

        std::cout << std::endl;

        for (int y = height-1; y >= 0; y--)
        {
            std::cout << "\t\t\t\t\t";

            for (int x = 0; x < width; x++)
            {
                std::cout << this->get(x, y);
            }
            std::cout << std::endl;
        }
    }

    void injectfps(int fps)
    {
        char cfps[8];
        sprintf_s(cfps, "fps: %d", fps);
        for (int i = 0; i < sizeof(cfps); i++)
            set(i, 0, cfps[i]);


    }

    // taken from http://members.chello.at/easyfilter/bresenham.html
    // an algorithm to draw a line between two 2D points
    // takes 2 nomalized xy vertices and unnormalizes them based on our screen resolution defined by height and width
    void connect(float x0f, float y0f, float x1f, float y1f)
    {
        int x0 = unNorm_x(x0f);
        int y0 = unNorm_y(y0f);
        int x1 = unNorm_x(x1f);
        int y1 = unNorm_y(y1f);

        int dx = abs(x1 - x0), sx = x0 < x1 ? 1 : -1;
        int dy = -abs(y1 - y0), sy = y0 < y1 ? 1 : -1;
        int err = dx + dy, e2; /* error value e_xy */

        for (;;) {  /* loop */
            set(x0, y0, foreground);
            if (x0 == x1 && y0 == y1) break;
            e2 = 2 * err;
            if (e2 >= dy) { err += dy; x0 += sx; } /* e_xy+e_x > 0 */
            if (e2 <= dx) { err += dx; y0 += sy; } /* e_xy+e_y < 0 */
        }
    }

private:
    
    char background = ' '; // background pixel "color/shape"
    char foreground = '*'; // foreground pixel "color/shape"

    void set(int x, int y, char val)
    {
        coord[y * width + x] = val;
    }

    char get(int x, int y)
    {
        return coord[y * width + x];
    }
    
    // unormalizes an x coordinate
    int unNorm_x(float x)
    {
        return (width / 2) + round(x * (width / 2));
    }
    
    // unormalizes a y coordinate
    int unNorm_y(float y)
    {
        return (height / 2) + round(y * (height / 2));
    }
};

int main()
{
    // camera position vectors
    glm::vec3 cameraPos = glm::vec3(0.0f, 0.0f, 3.0f);
    glm::vec3 cameraFront = glm::vec3(0.0f, 0.0f, -1.0f);
    glm::vec3 cameraUp = glm::vec3(0.0f, 1.0f, 0.0f);

    // view matrix 
    glm::mat4 view;
    view = glm::lookAt(cameraPos, cameraPos + cameraFront, cameraUp);

    // projection matrix
    glm::mat4 projection;
    projection = glm::perspective(glm::radians(80.0f), 40.0f / 25.0f, 0.1f, 100.0f);

    Screen screen = Screen(40, 25);

    // 3D Vectors of the normalized coordinates of the cube
    std::vector<glm::vec3> vertices = 
{
        {-0.5f,  0.5f,  0.5f}, // 0
        { 0.5f,  0.5f,  0.5f}, // 1
        { 0.5f, -0.5f,  0.5f}, // 2
        {-0.5f, -0.5f,  0.5f}, // 3
        {-0.5f,  0.5f, -0.5f}, // 4
        { 0.5f,  0.5f, -0.5f}, // 5
        { 0.5f, -0.5f, -0.5f}, // 6
        {-0.5f, -0.5f, -0.5f}  // 7
    };

    // essentially a element array buffer (EBO), gives the order of vertices to connect using Screen.connect()
    std::vector<int> indices =
    {
        0, 1, 2, 3,
        0, 4, 5, 1,
        0, 4, 7, 3, 0,
        4, 5, 6, 7, 4,
        5, 1, 2, 6, 7, 4
    };



    float rot = 0.0f;
    int frames = 0;
    int fps = 0;
    auto start = std::chrono::steady_clock::now();

    while (true) {

        // model matrix
        // using rot variable to continuosly rotate the model
        // this must remain in the draw loop since it needs to be update on every draw call
        glm::mat4 model = glm::mat4(1.0f);
        model = glm::rotate(model, glm::radians(rot), glm::vec3(0.2f, 0.5f, 0.7f));
        
        // clear the buffer to get ready to draw the next frame
        screen.clearBuff();


        // an interesting method i found to remove flickering and jumping cursor
        // simply "presses" the home key and hides cursor with two escape sequences, refer to linked github for more details
        std::cout << "\x1b[?25l" << "\x1b[H"; // thank you https://gist.github.com/fnky/458719343aabd01cfb17a3a4f7296797
        screen.injectfps(fps);

        // draw to the screen using our vertices, indicies and matrices
        screen.draw(vertices, indices, model, view, projection);
        // increment the rotation angle

        rot += 2.0f;
        frames += 1;

        if (std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::steady_clock::now() - start).count() >= 1000) {
            start = std::chrono::steady_clock::now();
            fps = frames;
            frames = 0;
        }


 
    }
}

