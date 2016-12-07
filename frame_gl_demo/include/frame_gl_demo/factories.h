#pragma once
#include "frame/Frame.h"
#include "frame_gl/systems/Window.h"
#include "frame_gl/systems/Render.h"
#include "frame_gl/systems/Input.h"
#include "frame_gl/components/Transform.h"
#include "frame_gl/components/RenderTarget.h"
#include "frame_gl/components/Camera.h"
#include "frame_gl/components/MeshRenderer.h"
#include "frame_gl/components/Manipulate3D.h"
#include "frame_gl/systems/Manipulate3DOperator.h"
#include "frame_gl/systems/DisplayFPS.h"
#include "frame_gl/data/Mesh.h"
#include "frame_gl/math.h"
#include "frame_gl/colors.h"
#include "glm/gtx/quaternion.hpp"
#include "frame_gl_demo/components/Rotisserie.h"
#include "frame_gl_demo/components/Metabox.h"
#include "frame_gl_demo/systems/RotisserieOperator.h"
#include "frame_gl_demo/systems/FireflyOperator.h"
using namespace frame;

Entity* make_axes(Frame* frame, float scale=1.0f) {
    auto mesh_x = MeshFactory::arrow(vec3(0.0f), scale * vec3(1.0f, 0.0f, 0.0f), vec4(axis_color_x, 1.0f), scale * 0.2f);
    auto mesh_y = MeshFactory::arrow(vec3(0.0f), scale * vec3(0.0f, 1.0f, 0.0f), vec4(axis_color_y, 1.0f), scale * 0.2f);
    auto mesh_z = MeshFactory::arrow(vec3(0.0f), scale * vec3(0.0f, 0.0f, 1.0f), vec4(axis_color_z, 1.0f), scale * 0.2f);
    auto mesh = MeshFactory::combine({ mesh_x, mesh_y, mesh_z });
    auto texture = Resource<Texture>(ivec2(1));
    auto shader = Shader::Preset::model_colors();
    auto obj = frame->entities().add<Transform>();
    obj->add<MeshRenderer>(mesh, texture, shader, MeshRenderer::PolyMode::Line, false);
    return obj;
}

Entity* make_metabox(Frame* frame, std::size_t slots=1) {

    // Make box mesh
    auto mesh = MeshFactory::cube(1.0f);
    auto texture = Resource<Texture>(ivec2(1));
    auto shader = Shader::Preset::model_colors();
    auto obj = frame->entities().add<Transform>();
    obj->add<MeshRenderer>(mesh, texture, shader, MeshRenderer::PolyMode::Line, false);
    return obj;
}