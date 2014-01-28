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

#define BOX_DEPTH 1.f
#define BOX_WIDTH 40.0f
#define BOX_HEIGHT 40.0f
#define SLING_RADIUS 20.0f
#define BAND_WIDTH 100.0f
#define SLINGSHOT_HEIGHT 40.f

#define RES_X 2
#define RES_Y 8

btRigidBody*	Slingshot::localCreateRigidBody(float mass, const btTransform& startTransform,btCollisionShape* shape)
{
	btAssert((!shape || shape->getShapeType() != INVALID_SHAPE_PROXYTYPE));
    
	//rigidbody is dynamic if and only if mass is non zero, otherwise static
	bool isDynamic = (mass != 0.f);
    
	btVector3 localInertia(0,0,0);
	if (isDynamic)
		shape->calculateLocalInertia(mass,localInertia);
    
	//using motionstate is recommended, it provides interpolation capabilities, and only synchronizes 'active' objects
    
#define USE_MOTIONSTATE 1
#ifdef USE_MOTIONSTATE
	btDefaultMotionState* myMotionState = new btDefaultMotionState(startTransform);
    
	btRigidBody::btRigidBodyConstructionInfo cInfo(mass,myMotionState,shape,localInertia);
    
	btRigidBody* body = new btRigidBody(cInfo);
	body->setContactProcessingThreshold(BT_LARGE_FLOAT);
    
#else
	btRigidBody* body = new btRigidBody(mass,0,shape,localInertia);
	body->setWorldTransform(startTransform);
#endif//
    
	m_dynamicsWorld->addRigidBody(body);
   
    
	return body;
}


band Slingshot::createPatch(ofRectangle &rect,float mass) {
    
    
    
    vector<float> tex_coords; 
    tex_coords.resize((RES_X-1)*(RES_Y-1)*12);
    
    
    btSoftBody *body = btSoftBodyHelpers::CreatePatchUV(m_softBodyWorldInfo, toBt(rect.getBottomLeft()),  toBt(rect.getTopLeft()), toBt(rect.getBottomRight()), toBt(rect.getTopRight()), RES_X,  RES_Y,   0, true,tex_coords.data());
//    for (int i=0;i<b.tex_coords.size();i+=2) {
//        cout << "(" << tex_coords[i] << "," << tex_coords[i+1] << ")" << endl;
//    }
    
    
    m_dynamicsWorld->addSoftBody(body);
    
    btVector3 inertia(0,0,0);
    body->getCollisionShape()->calculateLocalInertia(mass, inertia);
    body->setTotalMass(mass);
    
    body->m_materials[0]->m_kLST = 1;
    body->m_materials[0]->m_kAST = 0;
    body->m_materials[0]->m_kVST = 0;
    
	body->m_cfg.piterations = 1;
	
	//body->getCollisionShape()->setMargin(0.04 * m_worldScale * 2);
    
    band b;
    b.body = body;
    b.mesh.setUsage(GL_DYNAMIC_DRAW);
    b.mesh.setMode(OF_PRIMITIVE_TRIANGLES);
    b.mesh.getVertices().resize((RES_X-1)*(RES_Y-1)*2*3);
    b.mesh.getTexCoords().resize((RES_X-1)*(RES_Y-1)*6);
    b.mesh.enableTextures();
    for (int i=0;i<tex_coords.size()/2;i++) {
        b.mesh.getTexCoords()[i] = ofVec2f(tex_coords[i*2],tex_coords[i*2+1]);
        //        cout << "(" << tex_coords[i] << "," << tex_coords[i+1] << ")" << endl;
        //    }
    }

    
    return b;
}



//--------------------------------------------------------------
void Slingshot::initPhysics() {


    
    m_sling=0;
    m_slingConstraint=0;
    
    m_collisionConfiguration = new btSoftBodyRigidBodyCollisionConfiguration;
    m_dispatcher = new btCollisionDispatcher(m_collisionConfiguration);
    m_broadphase = new btDbvtBroadphase;
    m_solver = new btSequentialImpulseConstraintSolver;
    m_dynamicsWorld = new btSoftRigidDynamicsWorld(m_dispatcher,m_broadphase,m_solver,m_collisionConfiguration,0);
    
    
    m_dynamicsWorld->getDispatchInfo().m_enableSPU = true;
    m_dynamicsWorld->setGravity(toBt(ofVec3f(0,-980,0)));
    
    m_worldScale = 100;
    m_dynamicsWorld->setDebugDrawer(new Render(m_worldScale));
    m_dynamicsWorld->getDebugDrawer()->setDebugMode(btIDebugDraw:: DBG_MAX_DEBUG_DRAW_MODE);
    
    m_softBodyWorldInfo.air_density = 1.2;
    m_softBodyWorldInfo.water_density = 0;
    m_softBodyWorldInfo.water_offset = 0;
    m_softBodyWorldInfo.water_normal = toBt(ofVec3f(0,0,0));
    m_softBodyWorldInfo.m_dispatcher = m_dispatcher;
    m_softBodyWorldInfo.m_broadphase = m_broadphase;
    m_softBodyWorldInfo.m_sparsesdf.Initialize();
    m_softBodyWorldInfo.m_gravity = m_dynamicsWorld->getGravity();
    
    btTransform tr;
    tr.setIdentity();
    
    btCollisionShape *plane_shape = new btStaticPlaneShape(toBt(ofVec3f(0,1,0)), 0);
	walls.push_back(localCreateRigidBody(0, tr, plane_shape));
    tr.setOrigin(btVector3(btScalar(0), btScalar(0.), btScalar(-500.)));
    tr.getBasis().setEulerZYX(SIMD_HALF_PI, 0,0 );
//    walls.push_back(addRigidBody(plane_shape, ofVec3f(0,0,-750), ofVec3f(0,90,0),0));
    walls.push_back(localCreateRigidBody(0, tr, plane_shape));
    
//    center_pos = ofVec3f(0,50,0);
//    float box_width = 50;
//    float band_width = 100;
//    float height = 25;
    
    btCompoundShape *compound = new btCompoundShape();
    
    btCollisionShape *shape = new btBoxShape(btVector3(BOX_WIDTH/2,BOX_HEIGHT/2,BOX_DEPTH));
    
    tr.setIdentity();
    tr.setOrigin(btVector3(btScalar(-BAND_WIDTH-BOX_WIDTH), btScalar(0.), btScalar(0.)));
    //tr.getBasis().setEulerZYX(0,0,0);
    compound->addChildShape(tr, shape);
    tr.setOrigin(btVector3(btScalar(BAND_WIDTH+BOX_WIDTH), btScalar(0.), btScalar(0.)));
    compound->addChildShape(tr, shape);
    
    m_collisionShapes.push_back(compound);
    
    tr.setOrigin(btVector3(btScalar(0.), btScalar(SLINGSHOT_HEIGHT), btScalar(0.)));
    btRigidBody* pBodyA = localCreateRigidBody( 1.0, tr, compound);
    pBodyA->setActivationState(DISABLE_DEACTIVATION);
    
    btTransform frameInA;
    frameInA = btTransform::getIdentity();
    frameInA.setOrigin(btVector3(btScalar(0.), btScalar(0.), btScalar(0.)));
    frameInA.getBasis().setEulerZYX(0,0,0);
    
    //        btPoint2PointConstraint* p2p = new btPoint2PointConstraint(*pBodyA, btVector3(btScalar(0.), btScalar(0.), btScalar(0.)));
    //        m_dynamicsWorld->addConstraint(p2p, false);
    //        p2p->setDbgDrawSize(btScalar(5.f));
    
    m_gen6Dof = new btGeneric6DofConstraint(*pBodyA, frameInA, true);
    m_gen6Dof->setLinearUpperLimit(btVector3(0., 0., 0.));
    m_gen6Dof->setLinearLowerLimit(btVector3(0., 0., 0.));
    
    //		m_gen6Dof->setAngularLowerLimit(btVector3( -45*SIMD_RADS_PER_DEG,-45*SIMD_RADS_PER_DEG,0.f));
    //		m_gen6Dof->setAngularUpperLimit(btVector3( 45*SIMD_RADS_PER_DEG, 45*SIMD_RADS_PER_DEG,0.f));
    m_gen6Dof->setAngularLowerLimit(btVector3( 0,0,0.f));
    m_gen6Dof->setAngularUpperLimit(btVector3( 0, 0,0.f));
    
    m_gen6Dof->getRotationalLimitMotor(0)->m_enableMotor = true;
    m_gen6Dof->getRotationalLimitMotor(0)->m_targetVelocity=0;
    m_gen6Dof->getRotationalLimitMotor(0)->m_maxMotorForce = 10.0f;
    
    m_gen6Dof->getRotationalLimitMotor(1)->m_enableMotor = true;
    m_gen6Dof->getRotationalLimitMotor(1)->m_targetVelocity=0;
    m_gen6Dof->getRotationalLimitMotor(1)->m_maxMotorForce = 10.0f;
    
    m_dynamicsWorld->addConstraint(m_gen6Dof, false);
    m_gen6Dof->setDbgDrawSize(btScalar(5.f));
    
    tr.setOrigin(btVector3(btScalar(0.f),btScalar(SLINGSHOT_HEIGHT),btScalar(0.)));
    //tr.getBasis().setEulerZYX(0, 0, 0);
    m_pocket = localCreateRigidBody(1.0, tr, shape);
    m_pocket->setActivationState(DISABLE_DEACTIVATION);
    
    btTransform localA,localB;
    localA.setIdentity();
    //        localA.getBasis().setEulerZYX(0,SIMD_HALF_PI, 0);
    localB.setIdentity();
    //localA.getBasis().setEulerZYX(0, SIMD_HALF_PI, 0);
    m_slider = new btGeneric6DofConstraint(*pBodyA,*m_pocket,localA,localB,true);
    m_slider->setAngularLowerLimit(btVector3(0.,0.,0.));
    m_slider->setAngularUpperLimit(btVector3(0.,0.,0.));
    m_slider->setLinearLowerLimit(btVector3(0.,0.,0.));
    m_slider->setLinearUpperLimit(btVector3(0.,0.,0.));
    m_slider->getTranslationalLimitMotor()->m_enableMotor[2]=true;
    m_slider->getTranslationalLimitMotor()->m_targetVelocity[2]=0;
    m_slider->getTranslationalLimitMotor()->m_maxMotorForce[2] = 10.0f;
    m_dynamicsWorld->addConstraint(m_slider,true);
    m_slider->setDbgDrawSize(5.f);
    
     ofRectangle rect;
    rect.setFromCenter(ofVec3f(0,SLINGSHOT_HEIGHT,0)+ofVec3f(-BOX_WIDTH/2-BAND_WIDTH/2,0,0), BAND_WIDTH, BOX_HEIGHT);
    
    band left_band = createPatch(rect, .1);
//    left_band->setMass(0, 0);
//    left_band->setMass(1, 0);
    left_band.body->appendAnchor(0, pBodyA);
    left_band.body->appendAnchor(1, pBodyA);
    left_band.body->appendAnchor(left_band.body->m_nodes.size()-1, m_pocket);
    left_band.body->appendAnchor(left_band.body->m_nodes.size()-2, m_pocket);
    bands.push_back(left_band);
    
    rect.setFromCenter(ofVec3f(0,SLINGSHOT_HEIGHT,0)+ofVec3f(BOX_WIDTH/2+BAND_WIDTH/2,0,0), BAND_WIDTH, BOX_HEIGHT);
   
    band right_band;
    right_band = createPatch(rect, .1);
//    right_band->setMass(left_band->m_nodes.size()-1, 0);
//    right_band->setMass(left_band->m_nodes.size()-2, 0);
    right_band.body->appendAnchor(right_band.body->m_nodes.size()-1, pBodyA);
    right_band.body->appendAnchor(right_band.body->m_nodes.size()-2, pBodyA);
    right_band.body->appendAnchor(0, m_pocket);
    right_band.body->appendAnchor(1, m_pocket);
    bands.push_back(right_band);
    
    int canColors[]={0x6464ff,0xa2a24e,0x9854b6,0x389e40,0xcc3937};
    btCylinderShape *can_shape = new btCylinderShape(toBt(ofVec3f(20,30,0)));
    for (int i=0;i<5;i++) {
        btTransform tr;
        tr.setIdentity();
        tr.setOrigin(btVector3(-70+50*i,0,-200-60*i));
        tr.getBasis().setEulerZYX(0, ofRandom(SIMD_2_PI), 0);
        can c;
        c.body = localCreateRigidBody(10, tr, can_shape);
        c.color = canColors[i];
        
        cans.push_back(c);
    }

}

void Slingshot::setup(){
    initPhysics();
 
    canModel.loadModel("can.dae");
    ball.loadModel("tenbal.dae");
    
    
    light.enable();
    wall.loadImage("auz09u.jpg");
    band_material.loadImage("elastic_material_3.png");
    pocket_material.loadImage("pocket_material_1.png");
    wall.setAnchorPercent(0.5, 0.5);
    
    //cam.disableMouseInput();
    //cam.disableOrtho();
    
    cam.setPosition(0, 100, 250);
    cam.lookAt(ofVec3f(0,100,-200));
    
    ofEnableAlphaBlending();
//    ofEnableArbTex();
//    ofEnableNormalizedTexCoords();
//    ofDisableArbTex();
    ofSetSmoothLighting(true);
      
}

//--------------------------------------------------------------
void Slingshot::update(float timeStep,int maxSubSteps, float fixedTimeStep){
    
    m_dynamicsWorld->stepSimulation(btScalar(timeStep),maxSubSteps,btScalar(fixedTimeStep));
    
    for (vector<band>::iterator iter=bands.begin();iter!=bands.end();iter++) {
       
		for(int i=0;i<iter->body->m_faces.size();++i) {
    
        
            const btSoftBody::Face&	f=iter->body->m_faces[i];
            for (int j=0;j<3;j++) {
                iter->mesh.getVertices()[i*3+j] = toOF(f.m_n[j]->m_x);
            }
        }
    }
    
    
}

//--------------------------------------------------------------
void Slingshot::draw(){
    
    cam.begin();
       
//    m_dynamicsWorld->debugDrawWorld();
    
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
    
    
    
    for (vector<can>::iterator iter=cans.begin();iter!=cans.end();iter++) {
        ofSetHexColor(iter->color);
        ofPushMatrix();
        //ofTranslate(toOF((*iter)->getWorldTransform().getOrigin()));
        ofMultMatrix(toOF(iter->body->getWorldTransform()));
        ofScale(0.2,0.2,0.2);
        ofScale(retinaScale, retinaScale,retinaScale);
        canModel.draw(OF_MESH_FILL);
        ofPopMatrix();
    }
    
    ofSetColor(100,100,0);
    for (vector<btRigidBody*>::iterator iter=slings.begin();iter!=slings.end();iter++) {
        ofPushMatrix();
        //ofTranslate(toOF((*iter)->getWorldTransform().getOrigin()));
        ofMultMatrix(toOF((*iter)->getWorldTransform()));
        ofScale(0.073,0.073,0.073);
        ofScale(retinaScale, retinaScale,retinaScale);
        ball.draw(OF_MESH_FILL);
        ofPopMatrix();

    }
    
    /*
    for (vector<band>::iterator iter=bands.begin();iter!=bands.end();iter++) {
        const btScalar	scl=(btScalar)0.8;
		const btScalar	alp=(btScalar)1;
		const btVector3	col(0,(btScalar)0.7,0);
		for(int i=0;i<iter->body->m_faces.size();++i)
		{
			const btSoftBody::Face&	f=iter->body->m_faces[i];
            ofTriangle(toOF(f.m_n[0]->m_x), toOF(f.m_n[1]->m_x), toOF(f.m_n[2]->m_x));
        }
    }
     */
//            ofBeginShape();
//            ofVec3f a = toOF(f.m_n[0]->m_x);
//            ofVec3f b = toOF(f.m_n[1]->m_x);
//            ofVec3f c = toOF(f.m_n[2]->m_x);
//            ofVertex(a);
//            ofVertex(b);
//            ofVertex(c);
//            
//            ofEndShape(true);
            /*
			if(0==(f.m_material->m_flags&btSoftBody::fMaterial::DebugDraw)) continue;
			const btVector3			x[]={f.m_n[0]->m_x,f.m_n[1]->m_x,f.m_n[2]->m_x};
			const btVector3			d=(x[0]+x[1]+x[2])/3;
            
            btVector3 a= (x[0]-d)*scl+d;
            btVector3 b= (x[1]-d)*scl+d;
            btVector3 c= (x[2]-d)*scl+d;
            
//            ofSetColor(255*col.x(), 255*col.y(), 255*col.z());
            const btVector3	n=btCross(b-a,c-a).normalized();
            glBegin(GL_TRIANGLES);
            glColor4f(col.getX(), col.getY(), col.getZ(),alp);
            glNormal3d(n.getX(),n.getY(),n.getZ());
            glVertex3d(a.getX(),a.getY(),a.getZ());
            glVertex3d(b.getX(),b.getY(),b.getZ());
            glVertex3d(c.getX(),c.getY(),c.getZ());
            glEnd();
             */
	
    
    ofSetColor(255,255,255,bTrackable ? 190 : 100);
   
    band_material.bind();
    for (vector<band>::iterator iter=bands.begin();iter!=bands.end();iter++) {
        iter->mesh.draw();
    }
    band_material.unbind();
    
    
    
    ofPushMatrix();
    //ofTranslate(toOF((*iter)->getWorldTransform().getOrigin()));
    ofMultMatrix(toOF(m_pocket->getWorldTransform()));
    ofRectangle rect;
    rect.setFromCenter(ofPoint(), BOX_WIDTH , BOX_HEIGHT);
    pocket_material.draw(rect);
    ofPopMatrix();
    
    
    
    
        
    cam.end();

}

void Slingshot::exit() {
    exitPhysics();
}

void Slingshot::exitPhysics(){
    
    cans.clear();
    walls.clear();
    slings.clear();
    bands.clear();
    int i;
    
	//removed/delete constraints
	for (i=m_dynamicsWorld->getNumConstraints()-1; i>=0 ;i--)
	{
		btTypedConstraint* constraint = m_dynamicsWorld->getConstraint(i);
		m_dynamicsWorld->removeConstraint(constraint);
		delete constraint;
	}
	
    
	//remove the rigidbodies from the dynamics world and delete them
	for (i=m_dynamicsWorld->getNumCollisionObjects()-1; i>=0 ;i--)
	{
		btCollisionObject* obj = m_dynamicsWorld->getCollisionObjectArray()[i];
		btRigidBody* body = btRigidBody::upcast(obj);
		if (body && body->getMotionState())
		{
			delete body->getMotionState();
		}
		m_dynamicsWorld->removeCollisionObject( obj );
		delete obj;
	}
    
    
    
    
	//delete collision shapes
	for (int j=0;j<m_collisionShapes.size();j++)
	{
		btCollisionShape* shape = m_collisionShapes[j];
		delete shape;
	}
    
	m_collisionShapes.clear();
    
	//delete dynamics world
	delete m_dynamicsWorld;
    
	//delete solver
	delete m_solver;
    
	//delete broadphase
	delete m_broadphase;
    
	//delete dispatcher
	delete m_dispatcher;
    
	delete m_collisionConfiguration;
    

}

void Slingshot::slingshotRoated(ofPoint rot) {
//    btScalar currentDepth = m_gen6Dof->getRotationalLimitMotor(0)->m_currentPosition;
    btScalar depth = ofMap(rot.y, -1, 1, -45*SIMD_RADS_PER_DEG, 45*SIMD_RADS_PER_DEG, true);
    m_gen6Dof->setLimit(3, depth, depth);
//    m_gen6Dof->getRotationalLimitMotor(0)->m_targetVelocity=(currentDepth>depth ? -10.0f : 10.0f);
    
//    currentDepth = m_gen6Dof->getRotationalLimitMotor(1)->m_currentPosition;
    depth = ofMap(rot.x, -1, 1, -45*SIMD_RADS_PER_DEG, 45*SIMD_RADS_PER_DEG, true);
    m_gen6Dof->setLimit(4, depth, depth);
//    m_gen6Dof->getRotationalLimitMotor(1)->m_targetVelocity=(currentDepth>depth ? -10.0f : 10.0f);
    
}

void Slingshot::slingshotStreched(float z) {
//    btScalar currentDepth = m_gen6Dof->getTranslationalLimitMotor()->m_currentLinearDiff[2];
    btScalar depth = ofMap(z, 0, 1, 0, 200, true);
    m_slider->setLimit(2, depth, depth);
//    m_slider->getTranslationalLimitMotor()->m_targetVelocity[2]=;
}

void Slingshot::setTrackable(bool bTrackable) {
    this->bTrackable = bTrackable;
}

//--------------------------------------------------------------

void Slingshot::load() {
    if (!m_sling) {
        
        
        btSphereShape *sphere = new btSphereShape(SLING_RADIUS);
        btTransform tr;
        tr = m_pocket->getCenterOfMassTransform();
        tr.setOrigin(tr.getOrigin()+btVector3(0,0,-BOX_DEPTH-SLING_RADIUS));
        m_sling=localCreateRigidBody(10, tr, sphere);
        slings.push_back(m_sling);
        
        btVector3 pivotInA(0,0,-BOX_DEPTH);
        btVector3 pivotInB(0,0,SLING_RADIUS);
        
        m_slingConstraint = new btPoint2PointConstraint(*m_pocket, *m_sling, pivotInA, pivotInB);
        
        m_dynamicsWorld->addConstraint(m_slingConstraint);
//        p2p->setBreakingImpulseThreshold(1000);
        m_slingConstraint->setDbgDrawSize(btScalar(5.f));
        
    }
    
    
}

/*
void Slingshot::unload() {
    if (m_sling) {
        if (m_slingConstraint) {
            m_dynamicsWorld->removeConstraint(m_slingConstraint);
            m_slingConstraint = 0;

        }
        
        if (m_sling->getCollisionShape())
        {
            delete m_sling->getCollisionShape();
            m_sling->setCollisionShape(NULL);
        }
        
        if (m_sling->getMotionState())
        {
            delete m_sling->getMotionState();
            m_sling->setMotionState(NULL);
        }
        
        m_dynamicsWorld->removeRigidBody(m_sling);
        
        m_sling = 0;
    }
    
}
*/

void Slingshot::shoot(){
    if (m_slingConstraint) {
        m_dynamicsWorld->removeConstraint(m_slingConstraint);
        m_slider->setLimit(2, 0, 0);
        delete m_slingConstraint;
        m_slingConstraint = 0;
        m_sling = 0;

    }
}




