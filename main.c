
#include <GLFW/glfw3.h>
#include <stdlib.h>
#include <stdio.h>
#include <heman.h>
#include <kazmath\kazmath.h>

void key_callback(GLFWwindow *window, int key, int scancode, int action, int mods)
{
	if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS)
		glfwSetWindowShouldClose(window, GL_TRUE);
}

void drawTexture(int *trianglestrip, heman_image* h, const int* n, int blend)
{
	float minA, maxA;

	// just to find out how many vertices there are
	int cols = *n;
	int rows = *n;
	// int RCvertices = 2 * cols * (rows - 1);
	int TSvertices = 2 * cols * (rows - 1) + 2 * (rows - 2);
	int numVertices = TSvertices;

	if (blend != 0) {
		// This fixes the overlap issue
		glDisable(GL_DEPTH_TEST);

		// Turn on the blend mode
		glEnable(GL_BLEND);

		// Define the blend mode
		glBlendFunc(GL_SRC_ALPHA, GL_ONE);
	}
	else {
		glEnable(GL_DEPTH_TEST);
	}

	glBegin(GL_TRIANGLE_STRIP);
	for (int j = 0; j < numVertices; j++)
	{

		// a number between 0 and n^2 - 1
		int vertexToDraw = trianglestrip[j] - 1;

		//betwenn 0 and n-1
		int jj = (vertexToDraw) % (*n);
		//between 0 and n-1
		int ii = (vertexToDraw - 1 - jj) / (*n);

		// the actual value to be plotted
		float ins = *(heman_image_texel(h, jj, ii));
		float minv = 0.f;
		float maxv = 1.f;
		ins = (ins - minv) * (maxv / minv);
		
		float red = ins;
		float green = ins;
		float blue = ins;
		glColor3f(red, green, blue);
		
		float glx, gly;
		glVertex3f((float) jj / (float) *n, (float) ii / (float) *n, 0.f);
	}
	glEnd();
	return;
}


int* triangle(int n) {

	// generate the triangle sequence
	int cols = n;
	int rows = n;
	int RCvertices = 2 * cols * (rows - 1);
	int TSvertices = 2 * cols * (rows - 1) + 2 * (rows - 2);
	int numVertices = TSvertices;
	int j = 0;
	int i;
	int *trianglestrip = malloc(sizeof(int) * numVertices);

	for (i = 1; i <= RCvertices; i += 2)
	{
		trianglestrip[j] = (1 + i) / 2;
		trianglestrip[j + 1] = (cols * 2 + i + 1) / 2;
		if (trianglestrip[j + 1] % cols == 0)
		{
			if (trianglestrip[j + 1] != cols && trianglestrip[j + 1] != cols * rows)
			{
				trianglestrip[j + 2] = trianglestrip[j + 1];
				trianglestrip[j + 3] = (1 + i + 2) / 2;
				j += 2;
			}
		}
		j += 2;
	}
	return trianglestrip;
}

void main() {
	GLFWwindow* window;
	int width = 640;
	int height = 480;
	float alpha = 1.f;

	if (!glfwInit()) exit(1);
	window = glfwCreateWindow(width, height, "heman demo", NULL, NULL);
	if (!window)
	{
		glfwTerminate();
		exit(1);
	}
	glfwMakeContextCurrent(window);
	glfwSetKeyCallback(window, key_callback);
	glfwGetFramebufferSize(window, &width, &height);
	glViewport(0, 0, width, height);
		glClear(GL_COLOR_BUFFER_BIT);
		glMatrixMode(GL_PROJECTION);
		glLoadIdentity();
		/*float m[] = {
			1., 0., 0., 0.,
			0., cos(alpha), sin(alpha), 0.,
			0., -sin(alpha), cos(alpha), 0.0,
			0., 0., 0., 1.f };
		glMultMatrixf(m);
		*/
		const int seed = 1;
		const int npts = 5;
		const int res = 4096;

		heman_points* pts = heman_image_create(npts, 1, 3);
		kmVec3* coords = (kmVec3*)heman_image_data(pts);
		coords[0] = (kmVec3) { 0.3, 0.4, 0.1 };
		coords[1] = (kmVec3) { 0.2, 0.5, 0.1 };
		coords[2] = (kmVec3) { 0.8, 0.7, 0.1 };
		coords[3] = (kmVec3) { 0.8, 0.5, 0.1 };
		coords[4] = (kmVec3) { 0.5, 0.5, 0.2 };
		heman_color colors[5] = { 0xC8758A, 0xDE935A, 0xE0BB5E, 0xE0BB5E, 0x8EC85D };
		heman_color ocean = 0x83B2B2;
		heman_image* contour = heman_image_create(res, res / 2, 3);
		heman_image_clear(contour, 0);
		heman_draw_contour_from_points(contour, pts, ocean, 0.3, 0.45, 1);
		heman_draw_colored_circles(contour, pts, 20, colors);

		int n = 128;
		int* trianglestrip = triangle(n);

		drawTexture(trianglestrip, contour, &n, 0);

		heman_image* cpcf = heman_distance_create_cpcf(contour);
		heman_image* warped = heman_ops_warp(cpcf, seed, 10);
		heman_image* voronoi = heman_color_from_cpcf(warped, contour);
		heman_image* toon = heman_ops_sobel(voronoi, 0x303030);
		
}