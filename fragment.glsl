// ==========================================================================
// Vertex program for barebones GLFW boilerplate
//
// Author:  Sonny Chan, University of Calgary
// Date:    December 2015
// ==========================================================================
#version 410

// interpolated colour received from vertex stage

in vec3 pos;

// first output is mapped to the framebuffer's colour index by default
out vec4 FragmentColour;

void main(void)
{
	FragmentColour = vec4(pos.z/25.0 + 0.5, 1.0, 1.0, 1.0);
}
