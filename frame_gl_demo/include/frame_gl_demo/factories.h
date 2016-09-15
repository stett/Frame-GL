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
#include "glm/gtx/quaternion.hpp"
#include "frame_gl_demo/components/Rotisserie.h"
#include "frame_gl_demo/components/Metabox.h"
#include "frame_gl_demo/systems/RotisserieOperator.h"
#include "frame_gl_demo/systems/FireflyOperator.h"
using namespace frame;

Entity* make_axes(Frame* frame, float scale=1.0f) {
    auto mesh_x = Mesh::Factory::arrow(vec3(0.0f), scale * vec3(1.0f, 0.0f, 0.0f), vec4(1.0f, 0.25f, 0.0f, 1.0f), scale * 0.2f);
    auto mesh_y = Mesh::Factory::arrow(vec3(0.0f), scale * vec3(0.0f, 1.0f, 0.0f), vec4(0.5f, 1.0f, 0.0f, 1.0f), scale * 0.2f);
    auto mesh_z = Mesh::Factory::arrow(vec3(0.0f), scale * vec3(0.0f, 0.0f, 1.0f), vec4(0.25f, 0.75f, 1.0f, 1.0f), scale * 0.2f);
    auto mesh = Mesh::Factory::combine({ mesh_x, mesh_y, mesh_z });
    auto texture = Resource<Texture>(ivec2(1));
    auto shader = Shader::Preset::model_colors();
    auto obj = frame->entities().add<Transform>();
    obj->add<MeshRenderer>(mesh, texture, shader, MeshRenderer::PolyMode::Line, false);
    return obj;
}

Entity* make_metabox(Frame* frame, std::size_t slots=1) {

    // Make box mesh
    auto mesh = Mesh::Factory::cube(1.0f);
    auto texture = Resource<Texture>(ivec2(1));
    auto shader = Shader::Preset::model_colors();
    auto obj = frame->entities().add<Transform>();
    obj->add<MeshRenderer>(mesh, texture, shader, MeshRenderer::PolyMode::Line, false);
    return obj;
}