//
//  Slingshot.cpp
//  slingshot
//
//  Created by Roee Kremer on 5/19/13.
//
//

#include "Slingshot.h"

#include "BulletSoftBody/btSoftBody.h"
#include "BulletSoftBody/btSoftBodyRigidBodyCollisionConfiguration.h"
#include "BulletSoftBody/btSoftRigidDynamicsWorld.h"
#include "BulletSoftBody/btSoftBodyHelpers.h"
#include "BulletDynamics/ConstraintSolver/btGeneric6DofConstraint.h"//picking

#include "Helper.h"
#include "Render.h"

#define BOX_DEPTH 0.5f
#define SLING_RADIUS 20.0f



btRigidBody* Slingshot::addRigidBody(btCollisionShape* shape, const ofVec3f& pos, const ofVec3f& rot, float mass)
{
	btTransform t(btQuaternion(rot.x * DEG_TO_RAD, rot.y * DEG_TO_RAD, rot.z * DEG_TO_RAD), toBt(pos));
	btDefaultMotionState* ms = new btDefaultMotionState(t);
	
	btVector3 inertia(0, 0, 0);
	shape->calculateLocalInertia(mass, inertia);
	shape->setMargin(0);
	
	btRigidBody::btRigidBodyConstructionInfo info(mass, ms, shape, inertia);
	btRigidBody *rigid = new btRigidBody(info);
    
    
	rigid->setMassProps(mass, inertia);
    rigid->updateInertiaTensor();
    
	assert(rigid);
    
//    rigid->setRestitution(0.4);
//    rigid->setFriction(0.75);
//    rigid->setDamping(0.25, 0.25);
    
    rigid->setRestitution(0.5);
    rigid->setFriction(0.3);
    rigid->setDamping(0.5, 0.5);
    
	
	m_world->addRigidBody(rigid);
	rigidBodies.push_back(rigid);
	
	return rigid;
}

btSoftBody* Slingshot::createPatch(ofRectangle &rect,float mass) {
    
    
    int resx = 2;
    int resy = 20;
    
    btSoftBody *body = btSoftBodyHelpers::CreatePatch(m_softBodyWorldInfo, toBt(rect.getBottomLeft()),  toBt(rect.getTopLeft()), toBt(rect.getBottomRight()), toBt(rect.getTopRight()), resx,  resy,   0, true);
    
    
    m_world->addSoftBody(body);
	softBodies.push_back(body);
    
    btVector3 inertia(0,0,0);
    body->getCollisionShape()->calculateLocalInertia(mass, inertia);
    body->setTotalMass(mass);
    
    body->m_materials[0]->m_kLST = 1;
    body->m_materials[0]->m_kAST = 1;
    body->m_materials[0]->m_kVST = 1;
    
	body->m_cfg.piterations = 4;
	
	body->getCollisionShape()->setMargin(0.04 * m_worldScale * 2);
    
    
    
    
    
    
    return body;
}



//--------------------------------------------------------------
void Slingshot::setup(){
    
    
    m_collisionConfiguration = new btSoftBodyRigidBodyCollisionConfiguration;
    m_dispatcher = new btCollisionDispatcher(m_collisionConfiguration);
    m_broadphase = new btDbvtBroadphase;
    m_solver = new btSequentialImpulseConstraintSolver;
    m_world = new btSoftRigidDynamicsWorld(m_dispatcher,m_broadphase,m_solver,m_collisionConfiguration,0);
    
    
    m_world->getDispatchInfo().m_enableSPU = true;
    m_world->setGravity(toBt(ofVec3f(0,-980,0)));
    
    m_worldScale = 100;
    m_world->setDebugDrawer(new Render(m_worldScale));
    m_world->getDebugDrawer()->setDebugMode(btIDebugDraw:: DBG_MAX_DEBUG_DRAW_MODE);
    
    m_softBodyWorldInfo.air_density = 1.2;
    m_softBodyWorldInfo.water_density = 0;
    m_softBodyWorldInfo.water_offset = 0;
    m_softBodyWorldInfo.water_normal = toBt(ofVec3f(0,0,0));
    m_softBodyWorldInfo.m_dispatcher = m_dispatcher;
    m_softBodyWorldInfo.m_broadphase = m_broadphase;
    m_softBodyWorldInfo.m_sparsesdf.Initialize();
    m_softBodyWorldInfo.m_gravity = m_world->getGravity();
    
    btCollisionShape *plane_shape = new btStaticPlaneShape(toBt(ofVec3f(0,1,0)), 0);
	walls.push_back(addRigidBody(plane_shape, ofVec3f(0,0,0), ofVec3f(0,0,0),0));
    walls.push_back(addRigidBody(plane_shape, ofVec3f(0,0,-750), ofVec3f(0,90,0),0));
    
    
    center_pos = ofVec3f(0,50,0);
    float box_width = 50;
    float band_width = 100;
    float height = 25;
    
    
    ofRectangle rect;
    rect.setFromCenter(center_pos, box_width, height);
    btCollisionShape *shape = new btBoxShape(toBt(ofVec3f(box_width/2,height/2,BOX_DEPTH)));
    btRigidBody *center_box;
    center_box = addRigidBody(shape, rect.getCenter(), ofVec3f(0,0,0), 10);
    
    //center_box->setActivationState(DISABLE_DEACTIVATION);
    btTransform frameB;
    frameB.setIdentity();
    btGeneric6DofConstraint* pGen6Dof = new btGeneric6DofConstraint( *center_box, frameB, false );
    m_world->addConstraint(pGen6Dof);
    pGen6Dof->setDbgDrawSize(btScalar(5.f));
    
    pGen6Dof->setAngularLowerLimit(btVector3(0,0,0));
    pGen6Dof->setAngularUpperLimit(btVector3(0,0,0));
    pGen6Dof->setLinearLowerLimit(btVector3(0, 0, 0.));
    pGen6Dof->setLinearUpperLimit(btVector3(0, 0, 500.));
    //pGen6Dof->setAxis(toBt(ofVec3f(1,0,0)), toBt(ofVec3f(0,1,0)));
    
    
    
    //    center_box->setCollisionFlags(center_box->getCollisionFlags() | btCollisionObject::CF_KINEMATIC_OBJECT);
    //    center_box->setActivationState(DISABLE_DEACTIVATION);
    
    
    rect.setFromCenter(center_pos+ofVec3f(-box_width/2-band_width/2,0,0), band_width, height);
    btSoftBody *left_band;
    left_band = createPatch(rect, 10);
    left_band->setMass(0, 0);
    left_band->setMass(1, 0);
    left_band->appendAnchor(left_band->m_nodes.size()-1, center_box);
    left_band->appendAnchor(left_band->m_nodes.size()-2, center_box);
    
    
    rect.setFromCenter(center_pos+ofVec3f(box_width/2+band_width/2,0,0), band_width, height);
    btSoftBody *right_band;
    right_band = createPatch(rect, 10);
    right_band->setMass(left_band->m_nodes.size()-1, 0);
    right_band->setMass(left_band->m_nodes.size()-2, 0);
    right_band->appendAnchor(0, center_box);
    right_band->appendAnchor(1, center_box);
    
    
    
    btCylinderShape *can_shape = new btCylinderShape(toBt(ofVec3f(20,30,0)));
    for (int i=0;i<5;i++) {
        cans.push_back(addRigidBody(can_shape, ofVec3f(-50+40*i,0,-250-75*i), ofVec3f(ofRandom(360),0,0), 10));
    }
 
//    cans.push_back(addRigidBody(can_shape, ofVec3f(0,0,-250), ofVec3f(ofRandom(360),0,0), 10));
//    cans.push_back(addRigidBody(can_shape, ofVec3f(40,0,-250), ofVec3f(ofRandom(360),0,0), 10));
//    cans.push_back(addRigidBody(can_shape, ofVec3f(-40,0,-250), ofVec3f(ofRandom(360),0,0), 10));
//    
//    cans.push_back(addRigidBody(can_shape, ofVec3f(0,0,-1000), ofVec3f(ofRandom(360),0,0), 10));
//    cans.push_back(addRigidBody(can_shape, ofVec3f(40,0,-1000), ofVec3f(ofRandom(360),0,0), 10));
//    cans.push_back(addRigidBody(can_shape, ofVec3f(-40,0,-1000), ofVec3f(ofRandom(360),0,0), 10));
//    
//    cans.push_back(addRigidBody(can_shape, ofVec3f(0,0,-350), ofVec3f(ofRandom(360),0,0), 10));
//    cans.push_back(addRigidBody(can_shape, ofVec3f(0,0,-450), ofVec3f(ofRandom(360),0,0), 10));
    
    
    bGrab = false;
    sling = 0;
    
    can.loadModel("can.dae");
//    pebble.loadModel("pebble.dae");
    ball.loadModel("tenbal.dae");
    
    
    light.enable();
    wall.loadImage("auz09u.jpg");
    wall.setAnchorPercent(0.5, 0.5);
    
    //cam.disableMouseInput();
    //cam.disableOrtho();
    
    cam.setPosition(0, 100, 250);
    cam.lookAt(ofVec3f(0,100,-200));
    
    retinaScale = 1.0f;
}

//--------------------------------------------------------------
void Slingshot::update(){
    m_world->stepSimulation(1.f/30,20,1./120.);
    
}

//--------------------------------------------------------------
void Slingshot::draw(){
    
    cam.begin();
       
    m_world->debugDrawWorld();
    
//    cam.end();
    
//    ofSetColor(255);
    
//    for (vector<btRigidBody*>::iterator iter=walls.begin();iter!=walls.end();iter++) {
//        ofPushMatrix();
//        //ofTranslate(toOF((*iter)->getWorldTransform().getOrigin()));
//        ofMultMatrix(toOF((*iter)->getWorldTransform()));
//        ofPushMatrix();
//        ofRotateX(90);
//        ofScale(4.0, 4.0,4.0);
//        ofScale(0.5, 0.5,0.5);
//        
//        wall.draw(0, 0);
//       
//        ofPopMatrix();
//        ofPopMatrix();
//    }
    
    ofSetColor(100,100,255);
    for (vector<btRigidBody*>::iterator iter=cans.begin();iter!=cans.end();iter++) {
        ofPushMatrix();
        //ofTranslate(toOF((*iter)->getWorldTransform().getOrigin()));
        ofMultMatrix(toOF((*iter)->getWorldTransform()));
        ofScale(0.2,0.2,0.2);
        ofScale(retinaScale, retinaScale,retinaScale);
        can.draw(OF_MESH_FILL);
        ofPopMatrix();
    }
    
    ofSetColor(100,100,0);
    if (sling) {
        ofPushMatrix();
        //ofTranslate(toOF((*iter)->getWorldTransform().getOrigin()));
        ofMultMatrix(toOF(sling->getWorldTransform()));
        ofScale(0.073,0.073,0.073);
        ofScale(retinaScale, retinaScale,retinaScale);
        ball.draw(OF_MESH_FILL);
        ofPopMatrix();

    }
    
    
    
//    const btScalar	scl=(btScalar)0.8;
//    const btScalar	alp=(btScalar)1;
//    const btVector3	col(0,(btScalar)0.7,0);
//    const ofVec3f color(toOF(col));
//    ofSetColor(255*color.x, 255*color.y, 255*color.z);
//    
//    for (vector<btSoftBody*>::iterator iter=softBodies.begin(); iter!=softBodies.end(); iter++) {
//        btSoftBody *psb = *iter;
//        
//        for(int i=0;i<psb->m_faces.size();++i)
//        {
//            const btSoftBody::Face&	f=psb->m_faces[i];
//            if(0==(f.m_material->m_flags&btSoftBody::fMaterial::DebugDraw)) continue;
//            const btVector3			x[]={f.m_n[0]->m_x,f.m_n[1]->m_x,f.m_n[2]->m_x};
//            const btVector3			c=(x[0]+x[1]+x[2])/3;
//            ofBeginShape();
//            for (int i=0;i<3;i++) {
//                ofVec3f v(toOF((x[i]-c)*scl+c));
//                ofVertex(v);
//            }
//           ofEndShape();
//            
//            
//        }
//        
//    }
    
    cam.end();

}

void Slingshot::exit() {
    for (vector<btRigidBody*>::iterator iter=rigidBodies.begin();iter!=rigidBodies.end();iter++) {
        if ((*iter)->getCollisionShape())
        {
            delete (*iter)->getCollisionShape();
            (*iter)->setCollisionShape(NULL);
        }
        
        if ((*iter)->getMotionState())
        {
            delete (*iter)->getMotionState();
            (*iter)->setMotionState(NULL);
        }
        
        m_world->removeRigidBody(*iter);
        
        delete *iter;
    }
    
    rigidBodies.clear();
    
    for (vector<btSoftBody*>::iterator iter=softBodies.begin();iter!=softBodies.end();iter++) {
        m_world->removeSoftBody(*iter);
        delete *iter;
    }
    
    softBodies.clear();
    
    delete m_world;
    delete m_broadphase;
    delete m_solver;
    delete m_dispatcher;
    delete m_collisionConfiguration;
}

//--------------------------------------------------------------

void Slingshot::load() {
    if (!sling) {
        
    
        btSphereShape *sphere = new btSphereShape(SLING_RADIUS);
        sling = addRigidBody(sphere, center_pos+ofVec3f(0,0,-BOX_DEPTH-SLING_RADIUS), ofVec3f(), 10);
       
        btTransform frameB;
        frameB.setIdentity();
        btGeneric6DofConstraint* pGen6Dof = new btGeneric6DofConstraint( *sling, frameB, false );
        m_world->addConstraint(pGen6Dof);
        pGen6Dof->setDbgDrawSize(btScalar(5.f));
        
        pGen6Dof->setAngularLowerLimit(btVector3(0,0,0));
        pGen6Dof->setAngularUpperLimit(btVector3(0,0,0));
        pGen6Dof->setLinearLowerLimit(btVector3(0, 0, -100.));
        pGen6Dof->setLinearUpperLimit(btVector3(0, 0, 500.));
        
        m_slingConstraint = pGen6Dof;
    }
    
    
}

void Slingshot::unload() {
    if (sling) {
        if (m_slingConstraint) {
            m_world->removeConstraint(m_slingConstraint);
            delete m_pickConstraint;
            //printf("removed constraint %i",gPickingConstraintId);
            m_slingConstraint = 0;

        }
        
        if (sling->getCollisionShape())
        {
            delete sling->getCollisionShape();
            sling->setCollisionShape(NULL);
        }
        
        if (sling->getMotionState())
        {
            delete sling->getMotionState();
            sling->setMotionState(NULL);
        }
        
        m_world->removeRigidBody(sling);
        
        rigidBodies.erase(std::remove(rigidBodies.begin(),rigidBodies.end(),sling),rigidBodies.end());
        
        sling = 0;
    }
    
}

void Slingshot::grab() {
    if (!bGrab && sling) {
        if (!m_slingConstraint) {
            unload();
            load();
        }
        
        bGrab = true;
        
        pickedBody = sling;
        pickedBody->setActivationState(DISABLE_DEACTIVATION);
        
        
        btVector3 pickPos = sling->getCenterOfMassPosition();
        //printf("pickPos=%f,%f,%f\n",pickPos.getX(),pickPos.getY(),pickPos.getZ());
        
        
        btVector3 localPivot = sling->getCenterOfMassTransform().inverse() * pickPos;
        
        btTransform tr;
        tr.setIdentity();
        tr.setOrigin(localPivot);
        btGeneric6DofConstraint* dof6 = new btGeneric6DofConstraint(*sling, tr,false);
        dof6->setLinearLowerLimit(btVector3(0,0,0));
        dof6->setLinearUpperLimit(btVector3(0,0,0));
        dof6->setAngularLowerLimit(btVector3(0,0,0));
        dof6->setAngularUpperLimit(btVector3(0,0,0));
        
        m_world->addConstraint(dof6,true);
        m_pickConstraint = dof6;
        
        dof6->setParam(BT_CONSTRAINT_STOP_CFM,0.8,0);
        dof6->setParam(BT_CONSTRAINT_STOP_CFM,0.8,1);
        dof6->setParam(BT_CONSTRAINT_STOP_CFM,0.8,2);
        dof6->setParam(BT_CONSTRAINT_STOP_CFM,0.8,3);
        dof6->setParam(BT_CONSTRAINT_STOP_CFM,0.8,4);
        dof6->setParam(BT_CONSTRAINT_STOP_CFM,0.8,5);
        
        dof6->setParam(BT_CONSTRAINT_STOP_ERP,0.1,0);
        dof6->setParam(BT_CONSTRAINT_STOP_ERP,0.1,1);
        dof6->setParam(BT_CONSTRAINT_STOP_ERP,0.1,2);
        dof6->setParam(BT_CONSTRAINT_STOP_ERP,0.1,3);
        dof6->setParam(BT_CONSTRAINT_STOP_ERP,0.1,4);
        dof6->setParam(BT_CONSTRAINT_STOP_ERP,0.1,5);
    }

}

void Slingshot::release(){
    
    if (bGrab) {
        bGrab = false;
        if (m_pickConstraint && m_world)
        {
            m_world->removeConstraint(m_pickConstraint);
            delete m_pickConstraint;
            //printf("removed constraint %i",gPickingConstraintId);
            m_pickConstraint = 0;
            pickedBody->forceActivationState(ACTIVE_TAG);
            pickedBody->setDeactivationTime( 0.f );
            pickedBody = 0;
            
            m_world->removeConstraint(m_slingConstraint);
            delete m_pickConstraint;
            //printf("removed constraint %i",gPickingConstraintId);
            m_slingConstraint = 0;
            sling->forceActivationState(ACTIVE_TAG);
            sling->setDeactivationTime( 0.f );
            
            
            
            
        }
    }
}



//--------------------------------------------------------------
void Slingshot::updateScale(float scale){
    
    if (bGrab && m_pickConstraint->getConstraintType() == D6_CONSTRAINT_TYPE)
    {
        btGeneric6DofConstraint* pickCon = static_cast<btGeneric6DofConstraint*>(m_pickConstraint);
        if (pickCon)
        {
            //keep it at the same picking distance
            
            
            pickCon->getFrameOffsetA().setOrigin(toBt(ofVec3f(0,0,scale*500.0)));
        }
        
    }
    
}
