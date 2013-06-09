//
//  Slingshot.h
//  slingshot
//
//  Created by Roee Kremer on 5/19/13.
//
//

#pragma once
#include "ofMain.h"

#include "btBulletDynamicsCommon.h"
#include "BulletSoftBody/btSoftBodyRigidBodyCollisionConfiguration.h"
#include "BulletSoftBody/btSoftRigidDynamicsWorld.h"
#include "BulletSoftBody/btSoftBodyHelpers.h"
#include "ofxAssimpModelLoader.h"



class Slingshot{
public:
    void setup();
    void update();
    void draw();
    void exit();
    
    void load();
    void unload();
    void grab();
    void release();
    void updateScale(float scale);
    
    btRigidBody* addRigidBody(btCollisionShape* shape, const ofVec3f& pos, const ofVec3f& rot, float mass);
    btSoftBody *createPatch(ofRectangle &rect,float mass);
    
    btBroadphaseInterface *m_broadphase;
	btCollisionConfiguration *m_collisionConfiguration;
	btCollisionDispatcher *m_dispatcher;
	btSequentialImpulseConstraintSolver *m_solver;
	btSoftRigidDynamicsWorld *m_world;
    
    btSoftBodyWorldInfo m_softBodyWorldInfo;
    
    float m_worldScale;
    
    ofVec3f center_pos;
    
    btTypedConstraint *m_pickConstraint;
    btRigidBody * pickedBody;
    bool bGrab;
    
    btRigidBody *sling;
    btTypedConstraint *m_slingConstraint;
    
   
    vector<btRigidBody*> rigidBodies;
    vector<btSoftBody*> softBodies;
    
    vector<btRigidBody*> cans;
    vector<btRigidBody*> walls;
    
    ofxAssimpModelLoader can;
    ofxAssimpModelLoader ball;
    ofLight light;
    
    ofImage wall;
    ofCamera cam;
    
    float retinaScale;
    
};
