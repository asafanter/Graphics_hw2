#include "Scene.h"



void Camera::LookAt(Vec3d & eye, Vec3d & at, Vec3d & up)
{
	Vec3d n = (eye - at).normalize();
	Vec3d u = up.cross(n).normalize();
	Vec3d v = n.cross(u).normalize();

	Tmatd c = Tmatd( u(0), v(0), n(0), 0.0, u(1), v(1), n(1), 0.0 , u(2), v(2), n(2), 0.0 , 0, 0, 0, 1);//c = (u, v, n, t) in colums

	Vec3d i = eye;
	i *= -1;	

	cTranform = c.translate(i);
}

void Camera::Ortho(double left, double right, double top, double bottom, double, double)
{

	//TODO: set projection = ....
}

Vec4d Camera::toProjectionView(Vec4d vertex)
{
	return projection * (cTranform * vertex);
}




Camera::Camera()
{
	
	cTranform.translate(0, 0, -5);//default is I

	Ortho(-5, 5, 5, -5, -5, 5);
}

void Camera::setTransformation(const Tmatd & T)
{
	cTranform = T;
}

void Camera::setProjection(const Tmatd & T)
{
	projection = T;
}



















Scene::Scene() :
	_meshes()
{

}

void Scene::addMesh(const Mesh &model)
{
	_meshes.push_back(model);
}

Vec2u Scene::coordsToPixels(const double &x, const double &y, const uint &width, const uint &height)
{
	double width_d = static_cast<double>(width);
	double height_d = static_cast<double>(height);

	uint x_res = static_cast<uint>((width_d / 2.0) * (x + 1.0) );
	uint y_res = static_cast<uint>((height_d / 2.0) * (1.0 - y));

	return Vec2u(x_res, y_res);
}


void Scene::draw(CDC * pDC, int width, int height, bool showFaceNormals, bool showVerNormals)
{

	for (auto &mesh : _meshes) {
		for (const auto &polygon : mesh.getPolygons())
		{
			auto& vertexes = polygon.getVertices();

			for (unsigned i = 0; i < vertexes.size() - 1; i++)
			{
				Vec4d p1 = mesh.getModel() * vertexes[i];
				Vec4d p2 = mesh.getModel() * vertexes[i + 1];

				auto px1 = coordsToPixels(p1(0), p1(1), width, height);
				auto px2 = coordsToPixels(p2(0), p2(1), width, height);

				MidPointDraw(px1(0), px1(1), px2(0), px2(1), pDC, mesh.getColor());
				if (showVerNormals) {
					//TODO: show vertices normals code
				}
			}
			if (showFaceNormals) {
				Vec4d p1 = mesh.getModel() * polygon.getAveragePosition();
				Vec4d p2 = mesh.getModel() * polygon.getFaceNormal();

				auto px1 = coordsToPixels(p1(0), p1(1), width, height);
				auto px2 = coordsToPixels(p2(0), p2(1), width, height);

				MidPointDraw(px1(0), px1(1), px2(0), px2(1), pDC, mesh.getColor());
			}
		}
	}
}

