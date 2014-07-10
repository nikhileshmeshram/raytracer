/* 
 * File:   surface.h
 * Author: nikhilesh
 *
 * Created on 27 June, 2014, 7:45 PM
 */

#ifndef SURFACE_H
#define	SURFACE_H
#include <iostream>
#include <algorithm>
#include <vector>
#include <list>
#include <limits>
#include "CImg.h"
//should be in some geometry.h
class CDirection;
class CPoint{
public:
    CPoint(){
        x = 0.0;
        y = 0.0;
        z = 0.0;
    }
    CPoint(double xi, double yi, double zi){
        x = xi;
        y = yi;
        z = zi;
    }
    
    CPoint(CDirection d);
    
    double getX(){
        return x;
    }
    double getY(){
        return y;
    }
    double getZ(){
        return z;
    }
    
private:
    double x;
    double y;
    double z;
};

class CDirection{
    double xhat;
    double yhat;
    double zhat;
public:
    CDirection(){
        xhat = 0.0;
        yhat = 0.0;
        zhat = 0.0;
    }
    
    CDirection(CPoint p){
        xhat = p.getX();
        yhat = p.getY();
        zhat = p.getZ();
    }
    
    CDirection(double a, double b, double c){
        xhat = a;
        yhat = b;
        zhat = c;
    }
    
    void normalize(){
        double mag = sqrt(xhat*xhat + yhat*yhat + zhat*zhat); // make this into function later
        xhat /= mag;
        yhat /= mag;
        zhat /= mag;
    }
    
    double dotProduct(CDirection d){
        return d.xhat*xhat + d.yhat*yhat + d.zhat*zhat;
    }
    
    CDirection crossProduct(CDirection d){
        CDirection temp;
        temp.xhat = yhat * d.zhat - zhat * d.yhat;
        temp.yhat = zhat * d.xhat - xhat * d.zhat;
        temp.zhat = xhat * d.yhat - yhat * d.xhat;
        return temp;
    }
    
    CDirection subtract(CDirection d){
        CDirection temp;
        temp.xhat = xhat - d.xhat;
        temp.yhat = yhat - d.yhat;
        temp.zhat = zhat - d.zhat;
        return temp;
    }
    
    CDirection add(CDirection d){
        CDirection temp;
        temp.xhat = xhat + d.xhat;
        temp.yhat = yhat + d.yhat;
        temp.zhat = zhat + d.zhat;
        return temp;
    }
    
    CDirection Sproduct(double k){
        CDirection temp;
        temp.xhat = xhat * k;
        temp.yhat = yhat * k;
        temp.zhat = zhat * k;
        return temp;
    }
    
    double getX(){
        return xhat;
    }
    
    double getY(){
        return yhat;
    }
    
    double getZ(){
        return zhat;
    }
    
    void print(){
        std::cout<<"CDirection:"<<std::endl;
        std::cout<<"xhat:"<<xhat<<std::endl;
        std::cout<<"yhat:"<<yhat<<std::endl;
        std::cout<<"zhat:"<<zhat<<std::endl;
              
    }
};

CPoint::CPoint(CDirection d){
        x = d.getX();
        y = d.getY();
        z = d.getZ();
}

class CRay{
    CPoint p;
    CDirection d;
public:
    
    CRay(){
        
    }
    
    CRay(CDirection o, CDirection dir){
        p = CPoint(o);
        d = dir;
    }
    
    CPoint getOrigin(){
        return p;
    }
    
    CDirection getDirection(){
        return d;
    }
    
    CPoint getPoint(double t){
        CPoint retPoint(p.getX() + t * d.getX(), p.getY() + t * d.getY(), p.getZ() + t * d.getZ());
        return retPoint;
    }
    
};
class CMaterial{
    
public:
    //Later move to private and provide api's
    CDirection ka;
    CDirection kd;
    CDirection ks;
    double rho;
    CMaterial(){
        ka = CDirection(0.0,0.0,0.0);
        kd = CDirection(0.0,0.0,0.0);
        ks = CDirection(0.0,0.0,0.0);
        rho = 0.0;
    }
    CMaterial(CDirection kai, CDirection kdi, CDirection ksi, double rhoi){
        ka = kai;
        kd = kdi;
        ks = ksi;
        rho = rhoi;
    }
};
class CHitRecord{
public:
    CPoint f;
    double t;
    CDirection norm;
    CMaterial m;
};

class CSurface{
public:
    CSurface(){
        CMaterial defaultmat;
        m = defaultmat;
    }
    CSurface(CMaterial mat){
        m = mat;
    }
    CMaterial getMaterial(){
        return m;
    }
    virtual bool isHit(CRay r, double t0, double t1,CHitRecord& rec) = 0;
private:
    CMaterial m;
};

class CSphere: public CSurface{
public:
    CSphere(){
        CPoint c;
        center = c;
        radius = 1;
    }
    CSphere(double rad, CPoint p, CMaterial m):CSurface(m){
        center = p;
        radius = rad;
    }
    
    bool isHit(CRay r, double t0, double t1,CHitRecord& rec);
private:
    double radius;
    CPoint center;
};

class CTriangle: public CSurface{
public:
    CTriangle(){
        a_vert = CDirection(0.0,0.0,0.0);
        b_vert = CDirection(0.0,1.0,0.0);
        c_vert = CDirection(0.0,0.0,1.0);
    }
    CTriangle(CDirection a1, CDirection b1, CDirection c1, CMaterial m):CSurface(m){
        a_vert = a1;
        b_vert = b1;
        c_vert = c1;//This can be done in the function definition itself
    }
    
    bool isHit(CRay r, double t0, double t1,CHitRecord& rec);
private:
    CDirection a_vert;
    CDirection b_vert;
    CDirection c_vert;
    
};

//should be in environment.h or something
class CReference{
    CPoint origin;
    //should be set such that they are orthonormal
    CDirection x_1;
    CDirection y_1;
    CDirection z_1;
public:
    CReference(){
        
    }
    CReference(CPoint org, CDirection a, CDirection b){
        origin = org;
        x_1 = a;
        y_1 = b;
        z_1 = a.crossProduct(b);
        
        x_1.normalize();
        y_1.normalize();
        z_1.normalize();
    }
    
    CDirection getzDirection(){
        return z_1;
    }
    
    CDirection getxDirection(){
        return x_1;
    }
    
    CDirection getyDirection(){
        return y_1;
    }
    
    CPoint getOrigin(){
        return origin;
    }
};

class CColor{
    int r;
    int g;
    int b;
public:
    CColor(){
        r = 0;
        g = 0;
        b = 0;
    }
    
    CColor(int x, int y, int z){
        r = x;
        g = y;
        b = z;
    }
    
    int getColor(int a){
        if (a == 0){
            return r;
        }
        else if ( a == 1){
            return g;
        }
        else if (a == 2){
            return b;
        }
    }
    
    CColor Sproduct(double k){
        //add boundary checks and k should be > 0
        CColor temp;
        temp.r = r * k;
        temp.g = g * k;
        temp.b = b * k;
        return temp;
    }
    
    CColor add(CColor c){
        CColor temp;
        temp.r = std::min(255,r + c.r);
        temp.g = std::min(255,g + c.g);
        temp.b = std::min(255,b + c.b);
        return temp;
    }
    
    CColor dimmul(CDirection d){
        CColor temp;
        temp.r = r * d.getX();
        temp.g = g * d.getY();
        temp.b = b * d.getZ();
        return temp;
    }
};

class CScreen{
    int width;
    int height;
    CColor* array;
public:
    
    CScreen(){
        width = 0;
        height = 0;
    }
    CScreen(int w, int h){
        width = w;
        height = h;
        array = new CColor[w*h];
    }
    int getWidth(){
        return width;
    }
    int getHeight(){
        return height;
    }
    
    CColor* getArray(){
        return array;
    }
};

class CLight{
    CPoint orig;
    CColor l;
    double intensity; //must be greater than 0 less than 1
public:
    CLight(CPoint lcent,CColor lcol,double inten){
        orig = lcent;
        l = lcol;
        intensity = inten;      //check for boundary condition
    }
    
    double getIntensity(){
        return intensity;
    }
    
    CColor getAmbColor(CMaterial m){
        CColor t = l.Sproduct(intensity);
        CColor s = t.dimmul(m.ka);
        return s;
    }
    
    CColor getDifColor(CMaterial m){
        CColor t = l.Sproduct(intensity);
        CColor s = t.dimmul(m.kd);
        return s;
    }
    
    CColor getSpeColor(CMaterial m){
        CColor t = l.Sproduct(intensity);
        CColor s = t.dimmul(m.ks);
        return s;
    }
    
    CPoint getOrigin(){
        return orig;
    }
    
};

class CEnvironment;
class CCamera{
    double focal;
    CReference ref;
    CScreen c;
public:
    CCamera(CReference ref_c, CScreen Cam_sc, double foc){
        ref = ref_c;
        c = Cam_sc;
        focal = foc;
    }
    
    double getFocal(){
        return focal;
    }
    
    double getScreenWidth(){
        return c.getWidth();
    }
    
    double getScreenHeight(){
        return c.getHeight();
    }
    
    CColor* getArray(){
        return c.getArray();
    }
    
    void click(CEnvironment& env);
    
};

class CEnvironment{
    CCamera* activeCamera;
    double Ia; //ambient light
    
public:
    std::list<CSurface*> S_list;
    std::list<CLight*> L_list;
    CEnvironment(){
        activeCamera = NULL;
    }
    CEnvironment(CCamera* cc){
        activeCamera = cc;
    }
    void render();
    
    void addSurface(CSurface* s1){
        S_list.push_back(s1);
    }
    
    void addLight(CLight* l1){
        L_list.push_back(l1);
    }
    
    void isHit(CRay r, CColor& c);
    
    bool rayHit(CRay r,double t0, double t1,CHitRecord& hiter,CColor& c);
};

#endif	/* SURFACE_H */
