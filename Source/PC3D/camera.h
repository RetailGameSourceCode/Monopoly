#ifndef CAMERA_H
#define CAMERA_H

#ifdef USE_OLD_FRAME
#include "l_Frame.h"
#else
#include "NewFrame.h"
#endif

class CCamera
{
  private:
	  FrameType*  m_Camera;

	  //fields used to calculate new position and orientation for smoth path motion
	  BOOL        m_MovingToTarget;
	  FrameType   m_Source;
	  FrameType   m_Target;
	  int         m_NbFrameToTarget;
	  int         m_CurrentFrameToTarget;
	  
	  FrameType   m_LookAt;//consider the tip of that vector to be the camera target.

  public:
	  CCamera( );
	  void Init();
	  CCamera( FrameType * Master );
	  ~CCamera();
	  void UpdateCameraSmooth();
	  void UpdateCameraSmoothStartAndStop();
	  void SetCameraTarget( FrameType * NewTarget, int NbFrameToReachTarget );
	  BOOL StillGoingTowardTarget() { return m_MovingToTarget; }
	  void Goto(FrameType *LookTo, BOOL LookClose);
	  //___________________________________________________________________
	  //Camera position
	  //these function move everything, even the lookat spot
	  void Foward		( float Distance );
	  void Backward	( float Distance );
	  void Up			( float Distance );
	  void Down		( float Distance );
	  void Left		( float Distance );
	  void Right		( float Distance );

	  //___________________________________________________________________
	  //Camera position around the lookat spot
	  //doesn't change the lookat spot
	  void OrbitFoward();
	  void OrbitBackward();
	  void OrbitUp( float RadAngle );
	  void OrbitDown();
	  void OrbitLeft();
	  void OrbitRight();

	  //___________________________________________________________________
	  //These functions make the camera orbit around a point in 3D space
	  //center is the point of orbit, and angle is the angle in radians
	  void orbitAroundX(vector center, float angle);
	  void orbitAroundY(vector center, float angle);

	  //___________________________________________________________________
	  //Pitch/Yaw/Roll function
	  void Roll(float RadAngle);
	  void Pitch(float RadAngle);
	  void Yaw(float RadAngle);
	  void PitchYawRoll( FrameType* m, float RadPitch, float RadYaw, float RadRoll );

	  //___________________________________________________________________
	  //other functions

	  //Reset functions
	  void ResetAll();
						  
	  void setCamera(FrameType *cam);
	  FrameType *getCamera();
};


#endif
