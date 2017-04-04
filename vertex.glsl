// ==========================================================================
// Vertex program for barebones GLFW boilerplate
//
// Author:  Sonny Chan, University of Calgary
// Date:    December 2015
// ==========================================================================
#version 410

// location indices for these attributes correspond to those specified in the
// InitializeGeometry() function of the main program
layout(location = 0) in vec3 VertexPosition;

uniform mat4 cameraMatrix;
uniform mat4 perspectiveMatrix;
uniform mat4 modelviewMatrix = mat4(1.f);
// output to be interpolated between vertices and passed to the fragment stage

void main()
{
	gl_Position = perspectiveMatrix*cameraMatrix*modelviewMatrix*vec4(VertexPosition, 1.0);
}
