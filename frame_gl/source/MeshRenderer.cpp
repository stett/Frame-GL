#include "frame_gl/components/MeshRenderer.h"

using frame::Resource;
using frame::Mesh;
using frame::Texture;
using frame::Shader;

std::unordered_map< Mesh*, Resource<Mesh> > frame::MeshRenderer::meshes;
std::unordered_map< Texture*, Resource<Texture> > frame::MeshRenderer::textures;
std::unordered_map< Shader*, Resource<Shader> > frame::MeshRenderer::shaders;
