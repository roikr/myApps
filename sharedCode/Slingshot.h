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
#include "BulletDynamics/ConstraintSolver/btGeneric6DofConstraint.h"
#include "ofxAssimpModelLoader.h"

struct band {
    btSoftBody *body;
    ofVboMesh mesh;
};

class Slingshot{
public:
    void setup();
    void update(float timeStep,int maxSubSteps=1, float fixedTimeStep=1./60.);
    void draw();
    void exit();
    
    void initPhysics();
    void exitPhysics();
    
    void load();
//    void unload();
    void shoot();
  
    
    void slingshotRoated(float x,float y); // normalized to [0,1]
    void slingshotStreched(float z); // normalized to [0,1]

    
    btRigidBody*	localCreateRigidBody(float mass, const btTransform& startTransform,btCollisionShape* shape);
//    btRigidBody* addRigidBody(btCollisionShape* shape, const ofVec3f& pos, const ofVec3f& rot, float mass);
    band createPatch(ofRectangle &rect,float mass);
    
    btBroadphaseInterface *m_broadphase;
	btCollisionConfiguration *m_collisionConfiguration;
	btCollisionDispatcher *m_dispatcher;
	btSequentialImpulseConstraintSolver *m_solver;
	btSoftRigidDynamicsWorld *m_dynamicsWorld;
    
    btSoftBodyWorldInfo m_softBodyWorldInfo;
    
    float m_worldScale;
    
    ofVec3f center_pos;
    
    btRigidBody *m_pocket;
    btGeneric6DofConstraint *m_slider;
    btGeneric6DofConstraint *m_gen6Dof;
    
    btRigidBody *m_sling;
    btPoint2PointConstraint *m_slingConstraint;
    
    
    vector<btCollisionShape*> m_collisionShapes;
    
    vector<btRigidBody*> cans;
    vector<btRigidBody*> walls;
    vector<btRigidBody*> slings;
    
    vector<band> bands;
    
    ofxAssimpModelLoader can;
    ofxAssimpModelLoader ball;
    ofLight light;
    
    ofImage wall;
    ofImage band_material;
    ofImage pocket_material;
    ofCamera cam;
    
    
    float retinaScale;
       
};
