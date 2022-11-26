#include <iostream>
#include <vector>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

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
            coord[i] = ' ';
    }

    void setNormalized(float x, float y, char val) 
    {
        set((width / 2) + round(x * (width / 2)), (height / 2) + round(y * (height / 2)), val);
    }

    glm::vec4 transform(glm::vec3 vertices, glm::mat4 model, glm::mat4 view, glm::mat4 proj)
    {
        glm::vec4 transvert = glm::vec4(vertices, 1.0);
        transvert = proj * view * model * transvert;
        return transvert;
    }

    void draw(std::vector<glm::vec3> vertices, glm::mat4 model, glm::mat4 view, glm::mat4 proj)
    {
        glm::vec4 transvert;
        for (auto vertex : vertices) 
        {
            transvert = transform(vertex, model, view, proj);
            setNormalized(transvert.x, transvert.y, '#');
        }

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
    
    // TODO
    void connect(float xx, float yy, float xxx, float yyy) // ty paolo
    { 
        int x1 = unNorm_x(xx);
        int y1 = unNorm_y(yy);
        int x2 = unNorm_x(xxx);
        int y2 = unNorm_y(yyy);
        int dx = abs(x2 - x1);
        int dy = abs(y2 - y1);
        int decide;
        if (dx > dy)
            decide = 1;
        else
            decide = 0;
        int pk = 2 * dy - dx;
        for (int i = 0; i <= dx; i++) {
            set(x1, y1, '*');
            x1 < x2 ? x1++ : x1--;
            if (pk < 0) {
                if (decide == 0)
                    pk = pk + 2 * dy;
                else
                    pk = pk + 2 * dy;
            }
            else {
                y1 < y2 ? y1++ : y1--;
                pk = pk + 2 * dy - 2 * dx;
            }
        }
    }



private:

    void set(int x, int y, char val)
    {
        coord[y * width + x] = val;
    }

    char get(int x, int y)
    {
        return coord[y * width + x];
    }

    int unNorm_x(float x)
    {
        return (width / 2) + round(x * (width / 2));
    }

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
    projection = glm::perspective(glm::radians(90.0f), 40.0f / 25.0f, 0.1f, 100.0f);

    Screen screen = Screen(40, 25);

    std::vector<glm::vec3>  vertices = {
        {-0.5f,  0.5f, 0.5f},
        { 0.5f,  0.5f, 0.5f},
        { 0.5f, -0.5f, 0.5f},
        {-0.5f, -0.5f, 0.5f},
        {-0.5f,  0.5f, -0.5f},
        { 0.5f,  0.5f, -0.5f},
        { 0.5f, -0.5f, -0.5f},
        {-0.5f, -0.5f, -0.5f}
    };

    float rot = 0.0f;

    while (true) {
        // model matrix
        glm::mat4 model = glm::mat4(1.0f);
        model = glm::rotate(model, glm::radians(rot), glm::vec3(0.5f, 0.3f, 0.6f));

        screen.clearBuff();
        std::cout << "\x1b[?25l"; // thank you https://gist.github.com/fnky/458719343aabd01cfb17a3a4f7296797
        std::cout << "\x1b[H"; // this too
        screen.draw(vertices, model, view, projection);
        rot += 1.0f;
    }

    //screen.connect(0.5, 0.5, 0.5, 0.0);
    //screen.connect(0.5, 0.5, 0.5, 0.0);
    //screen.connect(0.5, 0.0, -0.5, 0.0);
    //screen.connect(-0.5, 0.0, -0.5, 0.5);
}

