//#include <vld.h>
#include "frame/Frame.h"
#include "frame_gl/systems/Window.h"
#include "frame_gl/systems/Render.h"
#include "frame_gl/systems/Input.h"
#include "frame_gl/components/Transform.h"
#include "frame_gl/components/RenderTarget.h"
#include "frame_gl/components/Camera.h"
#include "frame_gl/components/MeshRenderer.h"
#include "frame_gl/components/ClickableArea.h"
#include "frame_gl/data/Mesh.h"
#include "frame_gl/math.h"
#include "glm/gtx/quaternion.hpp"
#include "frame_gl_demo/components/Rotisserie.h"
#include "frame_gl_demo/components/Manipulate3D.h"
#include "frame_gl_demo/components/FollowMouse.h"
#include "frame_gl_demo/systems/RotisserieOperator.h"
#include "frame_gl_demo/systems/FireflyOperator.h"
#include "frame_gl_demo/systems/Manipulate3DOperator.h"
#include "frame_gl_demo/systems/DisplayFPS.h"
#include "frame_gl_demo/systems/DiagramVisualization.h"
#include "frame_gl_demo/systems/Planes.h"
#include "frame_gl_demo/factories.h"
using namespace frame;
using namespace frame_demo;

float rand_range(float min=-1.0f, float max=1.0f) {
    return min + (max - min) * (float)rand() / (float)RAND_MAX;
}

int main(int argc, char** argv) {
    const ivec2 resolution(600, 600);

    // Set up a diagram
    Diagram diagram;
    {
        DiagramNode* node0 = diagram.add_node();
        DiagramNode* node1 = diagram.add_node();
        DiagramNode* node2 = diagram.add_node();
        DiagramNode* node3 = diagram.add_node();
        DiagramNode* node4 = diagram.add_node();
        DiagramNode* node5 = diagram.add_node();
        DiagramNode* node6 = diagram.add_node();
        DiagramNode* node7 = diagram.add_node();
        DiagramNode* node8 = diagram.add_node();
        DiagramNode* node9 = diagram.add_node();
        DiagramNode* node10 = diagram.add_node();
        DiagramNode* node11 = diagram.add_node();

        Diagram::connect(node0->add_output(), node9->add_input());
        Diagram::connect(node9->add_output(), node2->add_input());
        Diagram::connect(node2->add_output(), node11->add_input());
    }

    // Set up a frame to visualize it
    Frame root;
    {
        root.systems().add<Window>(resolution, false, Window::FitMode::Fit);
        root.systems().add_many<Input, Render, Manipulate3DOperator>();
        //root.systems().add<DiagramVisualization>(&diagram);
        root.systems().add<Planes>();
    }

    // Add a camera
    {
        auto camera = root.entities().add();
        camera->add<Transform>()
            ->set_translation(vec3(10.0f, 10.0f, 10.0f))
            ->look(vec3(0.0f));
        camera->add<RenderTarget>(resolution, true);
        camera->add<Camera>();
        camera->add<Manipulate3D>(Input::MouseButton::Right, 10);
    }

    /*
    // Add some cubes
    {
        auto cube = root.entities().add();
        cube->add<Transform>();
        cube->add<MeshRenderer>(
            Mesh::Factory::cube(),
            Resource<Texture>(ivec2(1)),
            Shader::Preset::model_normals());
    }
    */

    // Add coordinate axes arrows at the origin
    make_axes(&root);

    // Start the frame's execution
    root.run();

    return 0;
}