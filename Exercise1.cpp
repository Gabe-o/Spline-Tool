#include <iostream>
#include <vector>
#include <cmath>
#include <GLFW/glfw3.h>
#include <GL/glext.h>

using namespace std;

struct Point
{
    float x;
    float y;
    Point(float x, float y) : x(x), y(y){};
    Point(){};
};

struct Node : Point
{
    bool hasHandle1, hasHandle2;
    Point handle1;
    Point handle2;
};

// Various variables for maintaining state
vector<Node> nodes;
double mouseX = 0, mouseY = 0;
float selectRadius = 10.0f;
bool leftMouse = false;
int selectedNode = 0, selectedControl = 0;
bool draggingNode = false, draggingControl = false;

// Calculates the distance between 2 points in 2D
float getDist(float x1, float y1, float x2, float y2)
{
    return sqrt(pow((x2 - x1), 2) + pow((y2 - y1), 2));
}

// Draws a bezier curve using the nodes and control points provided
void drawBezier(Point n1, Point c1, Point n2, Point c2)
{
    glColor3f(0, 0, 0);
    glBegin(GL_LINE_STRIP);
    for (int i = 0; i < 200; i++)
    {
        float t = i / 199.0f;
        float x = pow((1 - t), 3) * n1.x + 3 * pow((1 - t), 2) * t * c1.x + 3 * (1 - t) * pow(t, 2) * c2.x + pow(t, 3) * n2.x;
        float y = pow((1 - t), 3) * n1.y + 3 * pow((1 - t), 2) * t * c1.y + 3 * (1 - t) * pow(t, 2) * c2.y + pow(t, 3) * n2.y;
        glVertex2f(x, y);
    }
    glEnd();
}

int main(int argc, char *argv[])
{
    int width = atoi(argv[1]);
    int height = atoi(argv[2]);

    GLFWwindow *window;

    /* Initialize the library */
    if (!glfwInit())
        return -1;

    /* Create a windowed mode window and its OpenGL context */
    window = glfwCreateWindow(width, height, "Spline Tool", NULL, NULL);
    if (!window)
    {
        glfwTerminate();
        return -1;
    }

    /* Make the window's context current */
    glfwMakeContextCurrent(window);

    // Setting up viewport
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    glOrtho(0, width, 0, height, -1, 1);

    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
    glViewport(0, 0, width, height);

    // 4x MSAA
    glfwWindowHint(GLFW_SAMPLES, 4);
    glEnable(GL_MULTISAMPLE);

    glLineWidth(2.0f);

    /* Loop until the user closes the window */
    while (!glfwWindowShouldClose(window))
    {
        /* Poll for and process events */
        glfwPollEvents();

        // Update cursor pos
        double mX, mY;
        glfwGetCursorPos(window, &mX, &mY);
        mouseX = mX;
        mouseY = height - mY;

        // Getting E key state for clearing nodes
        if (glfwGetKey(window, GLFW_KEY_E) == GLFW_PRESS)
        {
            nodes.clear();
        }

        // get click state
        int leftMouseState = glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_LEFT);
        if (leftMouseState == GLFW_PRESS && leftMouse) // Holding mouse
        {
            if (draggingControl) // Dragging control nodes
            {
                if (selectedControl == 1) // Clicked control 1
                {
                    nodes[selectedNode].handle1.x = mouseX - nodes[selectedNode].x;
                    nodes[selectedNode].handle1.y = mouseY - nodes[selectedNode].y;
                    nodes[selectedNode].handle2.x = -nodes[selectedNode].handle1.x;
                    nodes[selectedNode].handle2.y = -nodes[selectedNode].handle1.y;
                }
                else // Clicked control 2
                {
                    nodes[selectedNode].handle2.x = mouseX - nodes[selectedNode].x;
                    nodes[selectedNode].handle2.y = mouseY - nodes[selectedNode].y;
                    nodes[selectedNode].handle1.x = -nodes[selectedNode].handle2.x;
                    nodes[selectedNode].handle1.y = -nodes[selectedNode].handle2.y;
                }
            }
            else if (draggingNode) // dragging a node
            {
                nodes[selectedNode].x = mouseX;
                nodes[selectedNode].y = mouseY;
            }
        }
        else if (leftMouseState == GLFW_PRESS && !leftMouse) // Initial Click
        {
            leftMouse = true;

            // Check if we clicked on an existing node
            for (int i = 0; i < nodes.size(); i++)
            {
                // Check if we clicked control 1
                if (nodes[i].hasHandle1 && getDist(mouseX, mouseY, nodes[i].x + nodes[i].handle1.x, nodes[i].y + nodes[i].handle1.y) < selectRadius)
                {
                    selectedNode = i;
                    selectedControl = 1;
                    draggingControl = true;
                    break;
                }
                // check if we clicked control 2
                else if (nodes[i].hasHandle2 && getDist(mouseX, mouseY, nodes[i].x + nodes[i].handle2.x, nodes[i].y + nodes[i].handle2.y) < selectRadius)
                {
                    selectedNode = i;
                    selectedControl = 2;
                    draggingControl = true;
                    break;
                }
                else if (abs(mouseX - nodes[i].x) < selectRadius && abs(mouseY - nodes[i].y) < selectRadius) // Clicked Node
                {
                    selectedNode = i;
                    draggingNode = true;
                    break;
                }
            }

            // Creates a new node if we aren't dragging something
            if (!draggingNode && !draggingControl)
            {
                // Create a node
                Node n;
                n.x = mouseX;
                n.y = mouseY;
                n.hasHandle1 = true;
                n.hasHandle2 = false;
                // handle coordinates are relative to node
                n.handle1.x = 0;
                n.handle1.y = 50;

                if (nodes.size() >= 2) // handling nodes other than the first 2 we need to changed an endpoint to an intermediary
                {
                    // Endpoints are stored at the start and end of the list
                    // Start Endpoint is closest
                    if (getDist(mouseX, mouseY, nodes[0].x, nodes[0].y) < getDist(mouseX, mouseY, nodes[nodes.size() - 1].x, nodes[nodes.size() - 1].y))
                    {
                        nodes[0].hasHandle2 = true;
                        nodes[0].handle2.x = -nodes[0].handle1.x;
                        nodes[0].handle2.y = -nodes[0].handle1.y;
                        nodes.insert(nodes.begin(), n);
                    }
                    else // End endpoint is closest
                    {
                        nodes[nodes.size() - 1].hasHandle2 = true;
                        nodes[nodes.size() - 1].handle2.x = -nodes[nodes.size() - 1].handle1.x;
                        nodes[nodes.size() - 1].handle2.y = -nodes[nodes.size() - 1].handle1.y;
                        nodes.push_back(n);
                    }
                }
                else // Handle first 2 nodes
                {
                    nodes.push_back(n);
                }
            }
        }
        else if (leftMouseState == GLFW_RELEASE && leftMouse) // Initial Mouse release
        {
            leftMouse = false;
            draggingNode = false;
            draggingControl = false;
        }

        /* Render here */
        glClear(GL_COLOR_BUFFER_BIT);

        glClearColor(1, 1, 1, 1);
        glColor3f(0, 0, 0);

        // Draw nodes
        for (Node &n : nodes)
        {
            // Draw dotted line
            glEnable(GL_LINE_SMOOTH);
            glEnable(GL_BLEND);
            glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
            glEnable(GL_LINE_STIPPLE);
            glLineStipple(1, 0xF0F0);
            glColor3f(0, 0.7, 0.7);
            glBegin(GL_LINES);
            glVertex2f(n.x, n.y);
            glVertex2f(n.x + n.handle1.x, n.y + n.handle1.y);
            if (n.hasHandle2)
            {
                glVertex2f(n.x, n.y);
                glVertex2f(n.x + n.handle2.x, n.y + n.handle2.y);
            }
            glEnd();
            glDisable(GL_LINE_STIPPLE);
            glDisable(GL_LINE_SMOOTH);
            glDisable(GL_BLEND);

            // Draw handle
            glEnable(GL_POINT_SMOOTH);
            glEnable(GL_BLEND);
            glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
            glColor3f(0, 0, 0);
            glBegin(GL_POINTS);
            if (n.hasHandle1)
            {
                glVertex2f(n.x + n.handle1.x, n.y + n.handle1.y);
            }
            if (n.hasHandle2)
            {
                glVertex2f(n.x + n.handle2.x, n.y + n.handle2.y);
            }
            glEnd();
            glDisable(GL_POINT_SMOOTH);
            glDisable(GL_BLEND);

            // Draw node
            glColor3f(0, 0, 1);
            glPointSize(10.0f);
            glBegin(GL_POINTS);
            glVertex2f(n.x, n.y);
            glEnd();
        }

        // Draw Bezier
        glEnable(GL_LINE_SMOOTH);
        glEnable(GL_BLEND);
        glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
        if (nodes.size() >= 2)
        {
            for (int i = 0; i < nodes.size() - 1; i++)
            {
                Point n1 = nodes[i];
                Point c1 = Point(nodes[i].x + nodes[i].handle1.x, nodes[i].y + nodes[i].handle1.y);
                Point n2 = nodes[i + 1];
                // Since endpoints only use handle 1 we need to adjust the last control point to use handle 1 instaed of handle 2
                Point c2 = nodes[i + 1].hasHandle2 ? Point(nodes[i + 1].x + nodes[i + 1].handle2.x, nodes[i + 1].y + nodes[i + 1].handle2.y) : Point(nodes[i + 1].x + nodes[i + 1].handle1.x, nodes[i + 1].y + nodes[i + 1].handle1.y);
                drawBezier(n1, c1, n2, c2);
            }
        }
        glDisable(GL_LINE_SMOOTH);
        glDisable(GL_BLEND);

        /* Swap front and back buffers */
        glfwSwapBuffers(window);
    }

    glfwTerminate();
    return 0;
};
