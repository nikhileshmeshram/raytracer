/* 
 * File:   main.cpp
 * Author: nikhilesh
 *
 * Created on 27 June, 2014, 7:38 PM
 */

#include <cstdlib>
#include "surface.h"
#include <iostream>
#include <algorithm>
#include <math.h>
#include "CImg.h"
using namespace cimg_library;

using namespace std;
/*
 * 
 * */
 
bool CSphere::isHit(CRay r, double t0, double t1,CHitRecord& rec){
    bool retVal = false;
    CDirection d = r.getDirection();
    CDirection e = CDirection(r.getOrigin());
    CDirection eminusc = e.subtract(CDirection(center));
    double b = d.dotProduct(eminusc);
    double a = d.dotProduct(d);
    double c = eminusc.dotProduct(eminusc) - (radius*radius);
    
    double discriminant = b*b -a*c;
    if (discriminant > 0){
        rec.t = (-1*b - sqrt(discriminant))/a;
        if (rec.t>t0 && rec.t<t1){
                rec.f = r.getPoint(rec.t);
                CDirection contactPoint = CDirection(rec.f);
                rec.norm = contactPoint.subtract(CDirection(center));
                rec.norm.normalize();
                rec.m = getMaterial();
                retVal = true;
        }
    }
    
    return retVal;
    
}

bool CTriangle::isHit(CRay r, double t0, double t1,CHitRecord& rec){
   
    CDirection d_ray = r.getDirection();
    CDirection e_ray = CDirection(r.getOrigin());
    CDirection AminusB = a_vert.subtract(b_vert);
    CDirection AminusC = a_vert.subtract(c_vert);
    CDirection AminusE = a_vert.subtract(e_ray);
    
    double a = AminusB.getX();
    double b = AminusB.getY();
    double c = AminusB.getZ();
    
    double d = AminusC.getX();
    double e = AminusC.getY();
    double f = AminusC.getZ();
    
    double g = d_ray.getX();
    double h = d_ray.getY();
    double i = d_ray.getZ();
    
    double j = AminusE.getX();
    double k = AminusE.getY();
    double l = AminusE.getZ();
    
    double EiminusHf = e*i - h*f;
    double GfminusDi = g*f - d*i;
    double DhminusEg = d*h - e*g;
    
    double M = a*EiminusHf + b*GfminusDi + c*DhminusEg;
    
    double t = (f*(a*k-j*b) + e*(j*c-a*l) + d*(b*l-k*c))/(-1*M);
    
    if ((t<t0)||(t>t1))
        return false;
    
    double Gamma = (i*(a*k-j*b) + h*(j*c-a*l) + g*(b*l-k*c))/M;
    
    if ((Gamma<0)||(Gamma>1))
        return false;
    
    double Beta = (j*EiminusHf + k*GfminusDi + l*DhminusEg)/M;
    
    if ((Beta<0)||(Beta>(1-Gamma)))
        return false;
        
    rec.t = t;
    rec.f = r.getPoint(t);
    
    rec.norm = AminusC.crossProduct(AminusB);
    (rec.norm).normalize();
    rec.m = getMaterial();
    return true;
}

void CCamera::click(CEnvironment& env){//std::list<CSurface*> &s, std::list<CLight*> &lishould give list of objects as argument
    CDirection cameradir = ref.getzDirection();
    CDirection camOrigin = ref.getOrigin();
    int screenWidth = getScreenWidth();
    int screenHeight = getScreenHeight();
    double dWidth = 1.0;
    double dHeight = 1.0;
    
    CDirection screencenter = camOrigin.add(cameradir.Sproduct(focal));
    CDirection l = (screencenter.add((ref.getxDirection()).Sproduct(dHeight/2))).subtract((ref.getyDirection()).Sproduct(dWidth/2));
    CDirection r = (screencenter.add((ref.getxDirection()).Sproduct(dHeight/2))).add((ref.getyDirection()).Sproduct(dWidth/2));
    CDirection t = (screencenter.subtract((ref.getxDirection()).Sproduct(dHeight/2))).subtract((ref.getyDirection()).Sproduct(dWidth/2));
    int i,j;
    for (i = 0; i < screenHeight; i++){
        for (j = 0; j < screenWidth; j++){
            CDirection u = (l.add((t.subtract(l)).Sproduct((i+0.5)/screenHeight))).add((r.subtract(l)).Sproduct((j+0.5)/screenWidth));
            CDirection d = u.subtract(camOrigin);
            CRay r(u,d);
            CColor* col = getArray();
                    
            env.isHit(r,col[j+i*screenHeight]);
            /*
            CHitRecord hiter;
            double max_t = std::numeric_limits<double>::max();
            
            //for each object
            for(std::list<CSurface*>::iterator list_iter1 = s.begin();list_iter1 != s.end(); list_iter1++){
                if ((*list_iter1)->isHit(r,0.001,max_t,hiter)){
                    //write to i,j pixel
                    CColor* col;
                    if (hiter.t<max_t){
                        max_t = hiter.t;
                    }
                    CColor Ia = CColor(10,10,10);
                    col = getArray();
                    col[j+i*screenHeight] = CColor();//reset color if new object //let it be hardcoded for now
                    CColor rc = Ia.dimmul(hiter.m.ka);//Ia Later make it environment variable
                    //for each light
                    for(std::list<CLight*>::iterator list_iter2 = li.begin();list_iter2 != li.end(); list_iter2++){
                        CDirection lvec = CDirection((*list_iter2)->getOrigin()).subtract(CDirection(hiter.f));
                        lvec.normalize();
                        
                        CRay sw(hiter.f,lvec);
                        CHitRecord drec;
                        if (!((*list_iter1)->isHit(sw,0.01,12345678.0,drec))){
                            CDirection dTemp = d.Sproduct(-1.0);
                            dTemp.normalize();
                            CDirection h = lvec.add(dTemp);
                            h.normalize();
                            CColor dc = rc.add(((*list_iter2)->getDifColor(hiter.m)).Sproduct(std::max(0.0,(hiter.norm).dotProduct(lvec))));
                            CColor fc = dc.add(((*list_iter2)->getSpeColor(hiter.m)).Sproduct(pow(std::max(0.0,(hiter.norm).dotProduct(h)),hiter.m.rho)));
                            col = getArray();
                            CColor temp = col[j+i*screenHeight];
                            col[j+i*screenHeight] = temp.add(fc);
                        }
                        else{
                            col = getArray();
                            CColor temp = col[j+i*screenHeight];
                            col[j+i*screenHeight] = temp.add(rc);
                        }

                        //col = getArray();
                        //col[j+i*screenHeight] = rc;
                    }//for each light
                }
             
            }
            */
            
        }
    }
}
void CEnvironment::isHit(CRay r, CColor& c){
    CHitRecord hiter;
    double max_t = std::numeric_limits<double>::max();
    if (rayHit(r,0.01,max_t,hiter,c)){
        c = CColor();//reset color if new object //let it be hardcoded for now
        CColor Ia = CColor(100,100,100);
        CColor rc = Ia.dimmul(hiter.m.ka);//Ia Later make it environment variable
        for(std::list<CLight*>::iterator list_iter2 = L_list.begin();list_iter2 != L_list.end(); list_iter2++){
            CDirection lvec = CDirection((*list_iter2)->getOrigin()).subtract(CDirection(hiter.f));
            lvec.normalize();

            CRay sw(hiter.f,lvec);
            CHitRecord drec;
            CColor t;
            if (!(rayHit(sw,0.01,max_t,drec,t))){
		CDirection d = r.getDirection();
                CDirection dTemp = d.Sproduct(-1.0);
                dTemp.normalize();
                CDirection h = lvec.add(dTemp);
                h.normalize();
                CColor dc = rc.add(((*list_iter2)->getDifColor(hiter.m)).Sproduct(std::max(0.0,(hiter.norm).dotProduct(lvec))));
                CColor fc = dc.add(((*list_iter2)->getSpeColor(hiter.m)).Sproduct(pow(std::max(0.0,(hiter.norm).dotProduct(h)),hiter.m.rho)));
		CDirection rvec = d.subtract((hiter.norm).Sproduct(2.0*d.dotProduct(hiter.norm)));
		CRay m(hiter.f,rvec);
		CColor ec = CColor();
		isHit(m,ec);
		CColor temp1 = c;
		c = temp1.add(ec.Sproduct(0.3));
		CColor temp = c;
                c = temp.add(fc);
            }else{
                CColor temp = c;
                c = temp.add(rc);
            }
        }
    }
}

bool CEnvironment::rayHit(CRay r, double t0, double t1, CHitRecord& hiter, CColor& c){
    bool retVal = false;
    double mint = t1;
    for(std::list<CSurface*>::iterator list_iter1 = S_list.begin();list_iter1 != S_list.end(); list_iter1++){
        if ((*list_iter1)->isHit(r,0.001,mint,hiter)){
            mint = hiter.t;
            retVal = true;
        }
    }
    return retVal;
}

void CEnvironment::render(){
    
    activeCamera->click(*this);
}

int main(int argc, char** argv) {
    /*
    CDirection a(1.0,2.0,4.0);
    CDirection b(1234.0,2.0,7.0);
    CDirection c = a.subtract(b);
    
    a.print();
    
    b.print();
    
    c.print();
     */ 
    
    //define the environment
    CPoint pcent(0.0,3.0,1.0);
    CMaterial mat_sphere(CDirection(0.1,0.0,0.0),CDirection(0.7,0.0,0.0),CDirection(1.0,1.0,1.0),50.0);
    CSphere sphere1(1.0,pcent,mat_sphere);
    
    CPoint pcent1(2.5,3.0,1.0);
    CMaterial mat_sphere1(CDirection(0.0,0.0,0.1),CDirection(0.0,0.0,0.7),CDirection(0.0,0.0,0.0),100.0);
    CSphere sphere2(1.0,pcent1,mat_sphere1);
    
    //the lights
    CPoint lcent(-1.0,0.0,3.0);
    CColor lcol(255,255,255);
    CLight light1(lcent,lcol,0.5);
    
    CPoint lcent1(0.0,0.0,3.0);
    CColor lcol1(255,255,255);
    CLight light2(lcent1,lcol1,1.0);
    
    
    //set the camera
    CPoint cam_orig(0.0,-10.0,3.0);
    CDirection cam_x(0,0,1);
    CDirection cam_y(1,0,0);//should be perpendicular to cam_x
    CReference ref_cam(cam_orig,cam_x,cam_y);
    CScreen cam_scr(1000,1000);
    CCamera cam(ref_cam,cam_scr,1);
    
    
    CDirection c_tri(-100.1,0.1,0.1);
    CDirection b_tri(100.1,0.1,0.1);
    CDirection a_tri(0.1,100.1,0.1);
    CMaterial mat_triangle(CDirection(0.0,0.2,0.0),CDirection(0.0,0.7,0.0),CDirection(0.0,0.0,0.0),100.0);
    CTriangle triangle1(a_tri,b_tri,c_tri,mat_triangle);
    //main render loop
    //just one object so..
    //cam.click(&triangle1,light1);
    //cam.click(&sphere1,light1);
    CEnvironment Env(&cam);
    Env.addLight(&light1);
    Env.addLight(&light2);
    Env.addSurface(&sphere1);
    Env.addSurface(&triangle1);
    Env.addSurface(&sphere2);
    
    Env.render();
    
    
    CImg<unsigned char> img(1000,1000,1,3);
    int l = 0;
    int j = 1;
    cimg_for(img,ptr,unsigned char) {
        *ptr=cam.getArray()[l%(1000*1000)].getColor(l/(1000*1000));
        l++;
    }
    img.display("JustWorkMan");
    return 0;
}

