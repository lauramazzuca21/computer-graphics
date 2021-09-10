#include "raytracer.h"
#include "material.h"
#include "vectors.h"
#include "argparser.h"
#include "raytree.h"
#include "utils.h"
#include "mesh.h"
#include "face.h"
#include "sphere.h"
#include <glm/glm.hpp>

// casts a single ray through the scene geometry and finds the closest hit
bool
RayTracer::CastRay (Ray & ray, Hit & h, bool use_sphere_patches) const
{
  bool answer = false;
  Hit nearest;
  nearest = Hit();

  // intersect each of the quads
  for (int i = 0; i < mesh->numQuadFaces (); i++)
  {
	Face *f = mesh->getFace (i);
	if (f->intersect (ray, h, args->intersect_backfacing))
	{
		if( h.getT() < nearest.getT() )
		{
			answer = true;
			nearest = h;
		}
	}
  }

  // intersect each of the spheres (either the patches, or the original spheres)
  if (use_sphere_patches)
  {
	for (int i = mesh->numQuadFaces (); i < mesh->numFaces (); i++)
	{
	  Face *f = mesh->getFace (i);
	  if (f->intersect (ray, h, args->intersect_backfacing))
	  {
		if( h.getT() < nearest.getT() )
		{
			answer = true;
			nearest = h;
		}
	  }
	}
  }
  else
  {
	const vector < Sphere * >&spheres = mesh->getSpheres ();
	for (unsigned int i = 0; i < spheres.size (); i++)
	{
	  if (spheres[i]->intersect (ray, h))
	  {
		if( h.getT() < nearest.getT() )
		{
			answer = true;
			nearest = h;
		}
	  }
	}
  }

  h = nearest;
  return answer;
}

Ray 
reflectionRay(Ray & ray, Hit & hit, Vec3f point) 
{
	//     calcolare ReflectionRay  R=2<n,l>n -l (usando la formula opposta per avere la giusta direzione del raggio)
	Vec3f direction = ray.getDirection() - 2.0f * hit.getNormal().Dot3(ray.getDirection()) * hit.getNormal();
	direction.Normalize();
	return Ray(point, direction);
}

Vec3f
RayTracer::TraceRay (Ray & ray, Hit & hit, int bounce_count) const
{

  hit = Hit ();
  bool intersect = CastRay (ray, hit, false);

  if( bounce_count == args->num_bounces )
  	RayTree::SetMainSegment (ray, 0, hit.getT () );
  else
	RayTree::AddReflectedSegment(ray, 0, hit.getT() );

  Vec3f answer = args->background_color;

  Material *m = hit.getMaterial ();
  if (intersect == true)
  {

	assert (m != NULL);
	Vec3f normal = hit.getNormal ();
	//get point with which the ray intersected the object
	Vec3f point = ray.pointAtParameter (hit.getT ());

	// ----------------------------------------------
	// ambient light
	answer = args->ambient_light * m->getDiffuseColor ();


	// ----------------------------------------------
	// if the surface is shiny...
	Vec3f reflectiveColor = m->getReflectiveColor ();

	// ==========================================
	// REFLECTIVE LOGIC
	// ==========================================
	
	// se (il punto sulla superficie e' riflettente & bounce_count>0)
	//calcolo la norma per vedere quanto è lungo quel vettore. Sarà riflettente solo se length > 0
    if (reflectiveColor.Length() != 0 && bounce_count > 0)
	{
		Ray reflection = reflectionRay(ray, hit, point);
		answer += TraceRay(reflection, hit, bounce_count - 1) * reflectiveColor;
	}
	// ----------------------------------------------
	// add each light
	int num_lights = mesh->getLights ().size ();
	for (int i = 0; i < num_lights; i++)
	{
		Face *f = mesh->getLights ()[i];
	  if(args->softShadow)
	  {
		// ==========================================
		// SOFT SHADOW LOGIC
		// ==========================================
		for (int k = 0; k < args->num_shadow_samples; k++) {
			Vec3f pointOnLight = f->RandomPoint();
			Vec3f dirToLight = pointOnLight - point;
			dirToLight.Normalize ();

			// creare shadow ray verso il punto luce
			Ray shadow = Ray(point, dirToLight);
			Hit hitShadowRay = Hit();
			// controllare il primo oggetto colpito da tale raggio
			bool hitObj = CastRay(shadow, hitShadowRay, false);
			if (hitObj)
			{
				//get point with which the ray intersected the object
				Vec3f p = shadow.pointAtParameter (hitShadowRay.getT ());
				Vec3f dist = Vec3f();
				Vec3f::Sub(dist, p, pointOnLight);
				float intensity = normal.Dot3(dirToLight);
			// se e' la sorgente luminosa i-esima e il dot prod tra normale ed l è > 0 allora
				if ( dist.Length() < 0.01f &&  intensity > 0.0f)
				{
					//	calcolare e aggiungere ad answer il contributo luminoso
					Vec3f lightColor =  0.2f * f->getMaterial()->getEmittedColor() * f->getArea();
					// add to answer the contribution of the light found - it will return black (so a shadow) if the light does not contribute
					answer += (1.0f/args->num_shadow_samples) * m->Shade(ray, hit, dirToLight, lightColor, args);
				}
			}
		}
	  }
	  else
	  {
		// ==========================================
		// HARD SHADOW LOGIC
		// ==========================================
		Vec3f pointOnLight = f->computeCentroid ();
		Vec3f dirToLight = pointOnLight - point;
		dirToLight.Normalize ();

		// creare shadow ray verso il punto luce
		Ray shadow = Ray(point, dirToLight);
		Hit hitShadowRay = Hit();
		// controllare il primo oggetto colpito da tale raggio
		bool hitObj = CastRay(shadow, hitShadowRay, false);
		if (hitObj)
		{
			//get point with which the ray intersected the object
			Vec3f p = shadow.pointAtParameter (hitShadowRay.getT ());
			Vec3f dist = Vec3f();
			Vec3f::Sub(dist, p, pointOnLight);

		// se e' la sorgente luminosa i-esima e il dot prod tra normale ed l è > 0 allora
			if ( dist.Length() < 0.01f && normal.Dot3(dirToLight) > 0)
			{
				//	calcolare e aggiungere ad answer il contributo luminoso
				Vec3f lightColor = 0.2f * f->getMaterial()->getEmittedColor() * f->getArea();
				// add to answer the contribution of the light found - it will return black (so a shadow) if the light does not contribute
				answer += m->Shade(ray, hit, dirToLight, lightColor, args);
			}
		}
	  }
	}
    
  }

  return answer;
}
